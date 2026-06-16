#include "tools_vario2d.h"

#include "muselib/geometry/tools.h"
#include "muselib/plot/plots.h"
#include "muselib/geostatistics/vario.h"

// ============================================================
// back_rotate_ellipse_to_original_plane
//
// Prende l'EllipseParameter calcolata sul piano XY e la
// riporta nel piano 3D originale usando la rotazione INVERSA
// salvata in dataRotation (prodotta da align_points_to_xyplane).
//
// CONVENZIONE ANGOLARE usata nel codice esistente:
//   max_direction è in gradi da NORD (Y+), misurato CW sul piano XY.
//   Il versore 3D corrispondente nel piano XY è:
//       u = (sin(dir_deg), cos(dir_deg), 0)
//
// Se dataRotation.autoalign == false il piano era già XY:
//   la funzione copia i valori senza trasformare nulla.
// ============================================================
MUSE::EllipseOriented3D back_rotate_ellipse_to_original_plane(
        const EllipseParameter&  ellipse_xy,
        const PlotStruct&        h_plot_xy,      // punti di range sul piano XY
        const MUSE::Rotation&    dataRotation,
        EllipseParameter&        summary_updated) // ellisse rifittata, aggiorna quella esistente
{
    EllipseOriented3D result;
    result.max_semiaxis = ellipse_xy.max_semiaxis;
    result.min_semiaxis = ellipse_xy.min_semiaxis;
    result.valid        = true;
 
    if(!dataRotation.autoalign)
    {
        // Piano già XY: nessuna trasformazione
        result.max_direction_deg = ellipse_xy.max_direction;
        result.min_direction_deg = ellipse_xy.min_direction;
        result.plane_normal      = {0, 0, 1};
        summary_updated          = ellipse_xy;
        return result;
    }
 
    // ----------------------------------------------------------
    // 1) Riproietta i punti di range dal piano XY al piano originale
    //    usando point_rotation INVERSA (stesso asse, angolo negato)
    //    Centro = centroide originale dei dati (salvato in dataRotation)
    //    MA i punti h_plot sono vettori (distanze dal centro del
    //    variogramma, non coordinate assolute) → centro = origine
    // ----------------------------------------------------------
    const cinolib::vec3d rot_axis(
        dataRotation.rotation_axis_vec.at(0),
        dataRotation.rotation_axis_vec.at(1),
        dataRotation.rotation_axis_vec.at(2));
    const double         inv_angle = -dataRotation.rotation_angle;
    const cinolib::vec3d origin(0.0, 0.0, 0.0);
 
    PlotStruct h_plot_orig;
    for(size_t i = 0; i < h_plot_xy.x.size(); i++)
    {
        cinolib::vec3d p(h_plot_xy.x.at(i), h_plot_xy.y.at(i), 0.0);
        cinolib::vec3d pr = point_rotation(p, rot_axis, inv_angle, origin);
        h_plot_orig.x.push_back(pr.x());
        h_plot_orig.y.push_back(pr.y());
        h_plot_orig.z.push_back(pr.z());
    }
 
    // ----------------------------------------------------------
    // 2) Rifitto l'ellisse sui punti riproiettati
    //    NB: fit_anisotropy_ellipse lavora su x,y 2D.
    //    Sul piano originale inclinato, i punti hanno anche z ≠ 0.
    //    Devo proiettare sul piano originale usando le coordinate
    //    locali del piano (u, v) — est locale e nord locale.
    // ----------------------------------------------------------
 
    // Normale al piano originale = back-rotate di Z
    cinolib::vec3d z_vec(0.0, 0.0, 1.0);
    cinolib::vec3d rotatedpoint = point_rotation(z_vec, rot_axis, inv_angle, origin);
    result.plane_normal = {rotatedpoint.x(), rotatedpoint.y(), rotatedpoint.z()};
    cinolib::vec3d pn (result.plane_normal.at(0), result.plane_normal.at(1), result.plane_normal.at(2));
 
    // Nord locale = proiezione di Y-mondiale sul piano originale
    cinolib::vec3d world_y(0.0, 1.0, 0.0);
    cinolib::vec3d north_local = world_y - pn * world_y.dot(pn);
    double nl_len = north_local.norm();
    if(nl_len < 1e-8)
    {
        // Fallback: piano quasi verticale con strike N-S → uso Z
        cinolib::vec3d world_z(0.0, 0.0, 1.0);
        north_local = world_z - pn * world_z.dot(pn);
        nl_len = north_local.norm();
    }
    north_local /= nl_len;
 
    // Est locale = Nord × normale
    cinolib::vec3d east_local = north_local.cross(pn);
    east_local /= east_local.norm();
 
    // Proietta i punti 3D sul sistema di riferimento locale (u=est, v=nord)
    // per passarli a fit_anisotropy_ellipse che vuole coordinate 2D
    std::vector<double> h_local_u, h_local_v;
    for(size_t i = 0; i < h_plot_orig.x.size(); i++)
    {
        cinolib::vec3d p(h_plot_orig.x.at(i),
                         h_plot_orig.y.at(i),
                         h_plot_orig.z.at(i));
        h_local_u.push_back(p.dot(east_local));   // coordinata est locale
        h_local_v.push_back(p.dot(north_local));  // coordinata nord locale
    }
 
    // ----------------------------------------------------------
    // 3) Rifitto l'ellisse nelle coordinate locali del piano originale
    // ----------------------------------------------------------
    EllipseParameter summary_local;
    fit_anisotropy_ellipse(h_local_u, h_local_v, summary_local);
 
    // Converti phi_rad → direzione CW da Nord locale
    summary_local.max_direction = 90.0 - get_degrees(summary_local.phi_rad);
    if(summary_local.max_direction < 0.0)
        summary_local.max_direction = 180.0 + summary_local.max_direction;
    summary_local.min_direction = summary_local.max_direction - 90.0;
    if(summary_local.min_direction < 0.0)
        summary_local.min_direction = 180.0 + summary_local.min_direction;
 
    result.max_semiaxis      = summary_local.max_semiaxis;
    result.min_semiaxis      = summary_local.min_semiaxis;
    result.max_direction_deg = summary_local.max_direction;
    result.min_direction_deg = summary_local.min_direction;
 
    // Versori 3D degli assi nel sistema originale
    auto local_deg_to_vec3d = [&](double dir_deg) -> cinolib::vec3d {
        double rad = dir_deg * M_PI / 180.0;
        return east_local * std::sin(rad) + north_local * std::cos(rad);
    };
    result.max_axis_vec3d = {local_deg_to_vec3d(result.max_direction_deg).x(), local_deg_to_vec3d(result.max_direction_deg).y(), local_deg_to_vec3d(result.max_direction_deg).z()};
    result.min_axis_vec3d = {local_deg_to_vec3d(result.min_direction_deg).x(), local_deg_to_vec3d(result.min_direction_deg).y(), local_deg_to_vec3d(result.min_direction_deg).z()};
 
    // Aggiorna summary per chi lo usa dopo (metavario.setSummary ecc.)
    summary_updated = summary_local;
 
    std::cout << "=== Ellipse re-fitted on original plane:" << std::endl;
    std::cout << "    max_semiaxis = " << result.max_semiaxis
<< "  direction = " << result.max_direction_deg
<< " deg (from local North, CW)" << std::endl;
    std::cout << "    min_semiaxis = " << result.min_semiaxis
<< "  direction = " << result.min_direction_deg
<< " deg (from local North, CW)" << std::endl;
    std::cout << "    plane normal (original) = ["
<< result.plane_normal.at(0) << ", "
<< result.plane_normal.at(1) << ", "
<< result.plane_normal.at(2) << "]" << std::endl;
 
    return result;
}

 
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
    // STEP 1: Build the local orthonormal frame (e1, e2) of the ORIGINAL
    //         plane. e1 is the projection of world-Y (North) onto the plane;
    //         e2 = plane_normal x e1 gives the "East" direction.
    //
    // This frame is independent of the rotation stored in 'rotation' and
    // is the natural frame an analyst expects: e1 points toward North
    // (as much as the plane allows), e2 points toward East.
    // ------------------------------------------------------------------
    cinolib::vec3d world_y(0.0, 1.0, 0.0); // North
    cinolib::vec3d world_z(0.0, 0.0, 1.0); // Up
 
    // Project world-Y onto the plane: subtract its component along the normal
    cinolib::vec3d e1 = world_y - plane_normal_orig * world_y.dot(plane_normal_orig);
 
    if(e1.norm() < 1e-8)
    {
        // Plane is nearly horizontal → world-Y lies in the plane but is
        // poorly defined; fall back to world-Z projected onto the plane.
        e1 = world_z - plane_normal_orig * world_z.dot(plane_normal_orig);
        std::cout << "[backproject] INFO: plane is nearly horizontal, "
                     "using world-Z as e1 reference." << std::endl;
    }
    e1.normalize();
 
    // e2 = n x e1: right-hand East for a North-pointing e1
    cinolib::vec3d e2 = plane_normal_orig.cross(e1);
    e2.normalize();
 
    result.e1 = { e1.x(), e1.y(), e1.z() };
    result.e2 = { e2.x(), e2.y(), e2.z() };
 
    std::cout << "[backproject] Original-plane frame:" << std::endl;
    std::cout << "  e1 (North): [" << e1.x() << ", " << e1.y() << ", " << e1.z() << "]" << std::endl;
    std::cout << "  e2 (East):  [" << e2.x() << ", " << e2.y() << ", " << e2.z() << "]" << std::endl;
    std::cout << "  normal:     [" << plane_normal_orig.x() << ", "
                                   << plane_normal_orig.y() << ", "
                                   << plane_normal_orig.z() << "]" << std::endl;
 
    // ------------------------------------------------------------------
    // STEP 2: Back-rotate the 3D range points from the XY plane to the
    //         original plane.
    //
    // Forward rotation: original → XY  (axis, +angle)
    // Inverse rotation: XY → original  (same axis, -angle)
    //
    // Range points are direction vectors (they encode only geometry, not
    // position), so the rotation center does not matter for their direction.
    // We still pass the stored center so that point_rotation is consistent
    // with the forward call in the main.
    // ------------------------------------------------------------------
    cinolib::vec3d rot_axis(0.0, 0.0, 1.0); // default (no rotation)
    double inv_angle = 0.0;
    cinolib::vec3d rot_center(0.0, 0.0, 0.0);
 
    if(rotation.autoalign && !rotation.rotation_axis_vec.empty())
    {
        rot_axis  = cinolib::vec3d(rotation.rotation_axis_vec[0],
                                   rotation.rotation_axis_vec[1],
                                   rotation.rotation_axis_vec[2]);
        inv_angle = -rotation.rotation_angle; // inverse rotation
        rot_center = cinolib::vec3d(rotation.rotation_center_x,
                                    rotation.rotation_center_y,
                                    rotation.rotation_center_z);
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
 
        // Project onto (e1, e2): these are the plot coordinates
        // x-axis of the rose diagram = e1 (North)
        // y-axis of the rose diagram = e2 (East)
        result.x_proj[i] = p_orig.dot(e1);
        result.y_proj[i] = p_orig.dot(e2);
    }
 
    // ------------------------------------------------------------------
    // STEP 3: Recompute the ellipse orientation in the original frame.
    //
    // The major axis unit vector in the XY plane is:
    //   major_xy = ( cos(phi_rad_xy),  sin(phi_rad_xy),  0 )
    // where phi_rad_xy is phi_rad from the fit on the XY plane.
    //
    // We back-rotate it to 3D and project onto (e1, e2) to get the
    // in-plane angle, then recompute:
    //   phi_rad_new = atan2( dot(major_3d, e2),  dot(major_3d, e1) )
    //
    // max_direction_deg is the azimuth from North = atan2(e2, e1) in degrees,
    // converted to the convention "degrees clockwise from North":
    //   max_direction_deg = 90 - phi_rad_new * 180/pi
    //                       (+ modulo 180 to stay in [0,180))
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
        // Degenerate: major axis collapsed onto the normal — use e1 as fallback
        major_3d = e1;
        std::cout << "[backproject] WARNING: major axis degenerate after "
                     "back-rotation, defaulting to e1." << std::endl;
    }
 
    // phi_rad in the (e1, e2) frame — this is what ellipse_plot expects
    // when the x-axis of the plot is e1 and the y-axis is e2
    double phi_rad_new = std::atan2(major_3d.dot(e2), major_3d.dot(e1));
 
    // Azimuth from North (clockwise, in [0, 180) because the variogram
    // ellipse has 180-degree symmetry)
    double max_dir_deg = 90.0 - phi_rad_new * 180.0 / M_PI;
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
    std::cout << "  max_direction (from North, CW): " << max_dir_deg << " deg" << std::endl;
    std::cout << "  min_direction (from North, CW): " << min_dir_deg << " deg" << std::endl;
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