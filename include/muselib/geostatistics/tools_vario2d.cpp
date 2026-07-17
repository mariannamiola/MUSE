#include "tools_vario2d.h"

#include "muselib/geometry/tools.h"
#include "muselib/plot/plots.h"
#include "muselib/geostatistics/vario.h"

// ---------------------------------------------------------------------------
/// @brief Back-project the anisotropy ellipse and range point cloud from the
///        computational XY plane to the original data plane.
///
/// @param ellipse_xy   EllipseParameter fitted on the XY plane (output of
///                     fit_anisotropy_ellipse).  Only semiaxes are meaningful
///                     on input; directions and phi_rad will be recomputed.
/// @param range_pts_x  X-coordinates of the range points on the XY plane
///                     (as passed to biv_plot_leg before back-rotation).
/// @param range_pts_y  Y-coordinates of the range points on the XY plane.
/// @param rotation     Rotation struct saved by align_points_to_xyplane.
///                     Must have autoalign==true if a rotation was applied.
///                     If autoalign==false, returns a copy of the input with
///                     phi_rad recomputed from max_direction (no-op path).
/// @return BackprojectedEllipse with updated directions, phi_rad and the
///         projected point cloud ready for plotting.
// ---------------------------------------------------------------------------
MUSE::BackprojectedEllipse backproject_ellipse_to_original_plane(
    const MUSE::EllipseParameter      &ellipse_xy,
    const std::vector<double>   &range_pts_x,
    const std::vector<double>   &range_pts_y,
    const MUSE::Rotation        &rotation)
{
    MUSE::BackprojectedEllipse result;
    result.ellipse = ellipse_xy; // copy — we will overwrite angles below
 
    // ------------------------------------------------------------------
    // STEP 0: Reconstruct the original plane normal.
    //
    // align_points_to_xyplane stores the normal in normal_vec_original.
    // The normal points "upward" (Z component ≥ 0 after the orientation
    // fix applied in that function).
    // ------------------------------------------------------------------
    cinolib::vec3d plane_normal_orig;
    if(!rotation.normal_vec_original.empty())
    {
        plane_normal_orig = cinolib::vec3d(
            rotation.normal_vec_original[0],
            rotation.normal_vec_original[1],
            rotation.normal_vec_original[2]);
    }
    else
    {
        // Fallback: if normal was not saved, derive it by back-rotating Z.
        // This should not happen with a correct align_points_to_xyplane.
        cinolib::vec3d z_axis(0.0, 0.0, 1.0);
        cinolib::vec3d rot_axis(
            rotation.rotation_axis_vec[0],
            rotation.rotation_axis_vec[1],
            rotation.rotation_axis_vec[2]);
        cinolib::vec3d center(0.0, 0.0, 0.0); // normals are directions, not points
        plane_normal_orig = point_rotation(z_axis, rot_axis,
                                           -rotation.rotation_angle, center);
        std::cout << "[backproject] WARNING: normal_vec_original was empty, "
                     "reconstructed from inverse rotation." << std::endl;
    }
    plane_normal_orig.normalize();
 
    result.plane_normal = { plane_normal_orig.x(),
                            plane_normal_orig.y(),
                            plane_normal_orig.z() };
 
    // ------------------------------------------------------------------
    // STEP 1: Build the local orthonormal in-plane frame of the ORIGINAL plane --
    //         based on the standard structural-geology strike, valid for ANY plane
    //         orientation with no degenerate/special case:
    //
    //   strike_vec  = normalize(Up x normal)         -- horizontal line within the plane
    //   dip_dir_vec = normalize(strike_vec x normal) -- in-plane, points down-dip
    //   up_dip_vec  = normalize(normal x strike_vec) = -dip_dir_vec -- in-plane, up-dip
    //
    // Unlike a "North/East" frame (which degenerates whenever the plane's normal
    // approaches North/South -- a vertical, East-West-striking plane, where the
    // plane contains no North component at all), strike_vec is only degenerate for
    // a perfectly horizontal plane (normal parallel to Up) -- and that case never
    // reaches this function, since align_points_to_xyplane routes dip < 1 deg
    // through the autoalign == false, no-op path instead.
    //
    // PLOT FRAME = (strike_vec, up_dip_vec). We deliberately use up_dip_vec (NOT
    // dip_dir_vec) as the plot's vertical axis so that (strike, up_dip, normal) is
    // RIGHT-handed (strike x up_dip = +normal): the rose diagram then reads as the
    // plane seen from the +normal side, with "up-dip = up on the page" like a natural
    // cross-section. Using dip_dir_vec instead makes (strike, dip_dir, normal) LEFT-
    // handed (strike x dip_dir = -normal), which mirror-flips the diagram left-right.
    // dip_dir_vec is still computed, but only to report the plane's geological dip
    // azimuth (the compass direction the plane dips TOWARD, by definition down-dip).
    // ------------------------------------------------------------------
    cinolib::vec3d world_up(0.0, 0.0, 1.0);

    cinolib::vec3d strike_vec = world_up.cross(plane_normal_orig);
    strike_vec.normalize(); // safe: strike_vec.norm() = sin(dip) >= sin(1 deg), see note above

    cinolib::vec3d dip_dir_vec = strike_vec.cross(plane_normal_orig);
    dip_dir_vec.normalize();

    cinolib::vec3d up_dip_vec = plane_normal_orig.cross(strike_vec); // = -dip_dir_vec, right-handed plot axis
    up_dip_vec.normalize();

    result.strike_vec  = { strike_vec.x(),  strike_vec.y(),  strike_vec.z()  };
    result.up_dip_vec  = { up_dip_vec.x(),  up_dip_vec.y(),  up_dip_vec.z()  };
    result.dip_dir_vec = { dip_dir_vec.x(), dip_dir_vec.y(), dip_dir_vec.z() };

    // Plane orientation, standard geological convention: dip azimuth is the compass
    // direction (deg CW from North) of the down-dip direction's horizontal projection;
    // dip_dir_vec's own x,y components ARE that horizontal projection (up to scale), so
    // atan2 on them directly gives the azimuth without needing to drop dip_dir_vec's z.
    result.dip_azimuth_deg = std::atan2(dip_dir_vec.x(), dip_dir_vec.y()) * 180.0 / M_PI;
    if(result.dip_azimuth_deg < 0.0) result.dip_azimuth_deg += 360.0;
    result.dip_deg = std::acos(std::max(-1.0, std::min(1.0, plane_normal_orig.z()))) * 180.0 / M_PI;

    std::cout << "[backproject] Original-plane frame:" << std::endl;
    std::cout << "  strike_vec (plot x):  [" << strike_vec.x() << ", " << strike_vec.y() << ", " << strike_vec.z() << "]" << std::endl;
    std::cout << "  up_dip_vec (plot y):  [" << up_dip_vec.x() << ", " << up_dip_vec.y() << ", " << up_dip_vec.z() << "]" << std::endl;
    std::cout << "  dip_dir_vec (down):   [" << dip_dir_vec.x() << ", " << dip_dir_vec.y() << ", " << dip_dir_vec.z() << "]" << std::endl;
    std::cout << "  normal:      [" << plane_normal_orig.x() << ", "
                                    << plane_normal_orig.y() << ", "
                                    << plane_normal_orig.z() << "]" << std::endl;
    std::cout << "  dip azimuth: " << result.dip_azimuth_deg << " deg, dip: " << result.dip_deg << " deg" << std::endl;
 
    // ------------------------------------------------------------------
    // STEP 2: Back-rotate the 3D range points from the XY plane to the
    //         original plane.
    //
    // Forward rotation: original → XY  (axis, +angle)
    // Inverse rotation: XY → original  (same axis, -angle)
    //
    // Range points are direction vectors (they encode only geometry, not
    // position), so they must be rotated about the ORIGIN, not about the data
    // centroid: point_rotation(p, axis, angle, center) computes
    // R*(p - center) + center, which for center != origin adds a spurious
    // constant translation to a direction vector (it's only a pure rotation,
    // R*p, when center is the origin). Passing rotation.rotation_center_* here
    // (as an earlier version of this function did) corrupted every
    // backprojected point/axis by that constant offset.
    // ------------------------------------------------------------------
    cinolib::vec3d rot_axis(0.0, 0.0, 1.0); // default (no rotation)
    double inv_angle = 0.0;
    const cinolib::vec3d rot_center(0.0, 0.0, 0.0); // vectors rotate about the origin

    if(rotation.autoalign && !rotation.rotation_axis_vec.empty())
    {
        rot_axis  = cinolib::vec3d(rotation.rotation_axis_vec[0],
                                   rotation.rotation_axis_vec[1],
                                   rotation.rotation_axis_vec[2]);
        inv_angle = -rotation.rotation_angle; // inverse rotation
    }

    size_t n_pts = range_pts_x.size();
    result.x_proj.resize(n_pts);
    result.y_proj.resize(n_pts);

    for(size_t i = 0; i < n_pts; i++)
    {
        // Range points on the XY plane have z = 0
        cinolib::vec3d p_xy(range_pts_x[i], range_pts_y[i], 0.0);

        cinolib::vec3d p_orig = rotation.autoalign
            ? point_rotation(p_xy, rot_axis, inv_angle, rot_center)
            : p_xy;

        // Project onto the right-handed plot frame (strike_vec, up_dip_vec): these
        // are the plot coordinates (up-dip is "up", no left-right mirror).
        result.x_proj[i] = p_orig.dot(strike_vec);
        result.y_proj[i] = p_orig.dot(up_dip_vec);
    }

    // ------------------------------------------------------------------
    // STEP 3: Recompute the ellipse orientation in the original frame.
    //
    // The major axis unit vector in the XY plane is:
    //   major_xy = ( cos(phi_rad_xy),  sin(phi_rad_xy),  0 )
    // where phi_rad_xy is phi_rad from the fit on the XY plane.
    //
    // We back-rotate it to 3D and project onto the plot frame (strike_vec, up_dip_vec)
    // to get the in-plane angle, then recompute:
    //   phi_rad_new = atan2( dot(major_3d, up_dip_vec),  dot(major_3d, strike_vec) )
    //
    // This phi_rad_new IS the pitch (deg, from strike toward up-dip) by construction,
    // in the SAME right-handed frame the range points are projected onto -- so the
    // drawn ellipse overlays the point cloud with no mirror. No further conversion
    // needed, unlike a North/East frame where the azimuth-from-North convention only
    // applies to one specific axis choice (see main.cpp's summary.max_direction for
    // that other convention, used only for the already-horizontal, autoalign==false
    // case).
    //   pitch_deg = phi_rad_new * 180/pi   (+ modulo 180 to stay in [0,180))
    // ------------------------------------------------------------------
    double phi_xy = ellipse_xy.phi_rad;
    cinolib::vec3d major_xy(std::cos(phi_xy), std::sin(phi_xy), 0.0);

    cinolib::vec3d major_3d = rotation.autoalign
        ? point_rotation(major_xy, rot_axis, inv_angle, rot_center)
        : major_xy;

    // Re-project onto the plane (numerical safety: remove any component
    // along the normal that crept in due to floating-point errors)
    major_3d = major_3d - plane_normal_orig * major_3d.dot(plane_normal_orig);
    if(major_3d.norm() > 1e-12)
        major_3d.normalize();
    else
    {
        // Degenerate: major axis collapsed onto the normal — use strike as fallback
        major_3d = strike_vec;
        std::cout << "[backproject] WARNING: major axis degenerate after "
                     "back-rotation, defaulting to strike_vec." << std::endl;
    }

    // phi_rad in the (strike_vec, up_dip_vec) plot frame -- this is what ellipse_plot
    // expects when the x-axis of the plot is strike_vec and the y-axis is up_dip_vec
    double phi_rad_new = std::atan2(major_3d.dot(up_dip_vec), major_3d.dot(strike_vec));

    // Pitch from strike toward up-dip, in [0, 180) because the variogram ellipse has
    // 180-degree symmetry
    double max_dir_deg = phi_rad_new * 180.0 / M_PI;
    // Bring into [0, 180)
    while(max_dir_deg <   0.0) max_dir_deg += 180.0;
    while(max_dir_deg >= 180.0) max_dir_deg -= 180.0;

    double min_dir_deg = max_dir_deg + 90.0;
    if(min_dir_deg >= 180.0) min_dir_deg -= 180.0;

    result.ellipse.phi_rad       = phi_rad_new;
    result.ellipse.max_direction = max_dir_deg;
    result.ellipse.min_direction = min_dir_deg;

    // Semiaxes are invariant under rotation — no change needed.

    std::cout << "[backproject] Back-projected ellipse:" << std::endl;
    std::cout << "  phi_rad (original frame): " << phi_rad_new * 180.0 / M_PI << " deg" << std::endl;
    std::cout << "  max pitch (from strike): " << max_dir_deg << " deg" << std::endl;
    std::cout << "  min pitch (from strike): " << min_dir_deg << " deg" << std::endl;
    std::cout << "  max_semiaxis: " << result.ellipse.max_semiaxis << std::endl;
    std::cout << "  min_semiaxis: " << result.ellipse.min_semiaxis << std::endl;
 
    // ------------------------------------------------------------------
    // STEP 4: Also handle the no-rotation case gracefully.
    //         If the data were already on the XY plane, align_points_to_xyplane
    //         sets autoalign=false and we skip all rotations.  The ellipse
    //         parameters from the fit are already correct except that phi_rad
    //         may differ from what was stored (it was overwritten with an
    //         approximate formula in the main).  We recompute from max_direction.
    // ------------------------------------------------------------------
    if(!rotation.autoalign)
    {
        // phi_rad for the XY case: max_direction is CW from North;
        // atan2 convention is CCW from x-axis (= East here, NOT North).
        // Standard: angle CCW from East = 90 - max_direction_CW_from_North
        double md = ellipse_xy.max_direction;
        phi_rad_new = (90.0 - md) * M_PI / 180.0;
 
        result.ellipse.phi_rad       = phi_rad_new;
        result.ellipse.max_direction = md;
        result.ellipse.min_direction = ellipse_xy.min_direction;
 
        // Points are unchanged
        result.x_proj = range_pts_x;
        result.y_proj = range_pts_y;
 
        std::cout << "[backproject] No rotation was applied (autoalign=false). "
                     "phi_rad recomputed from max_direction." << std::endl;
    }
 
    return result;
}