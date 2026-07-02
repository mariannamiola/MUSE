#include "tools.h"

#include <iostream>
#include <float.h>
#include <math.h>
#include <vector>

#include <deque>
#include <tuple>

#include <cinolib/geometry/aabb.h>
#include <cinolib/geometry/plane.h>

#include "muselib/colors.h"

bool check_index (const std::vector<int> &id_dupl, int index)
{
    bool id_found = false;
    for(int i : id_dupl)
    {
        if(i == index)
        {
            //std::cout << "indici uguali" << std::endl;
            id_found = true;
            break;
        }
    }
    return id_found;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double point_to_line_distance (const double &x0, const double &y0, const double &angle_rad, const double &x, const double &y)
{
    //https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#A_vector_projection_proof
    double dist = abs(cos(angle_rad)*(y0-y) - sin(angle_rad)*(x0-x));
    return dist;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool point_on_segment(const Point2D &p, const Point2D &a, const Point2D &b, const double tol)
{
    const double dx = b.x - a.x;
    const double dy = b.y - a.y;
 
    const double len2 = dx * dx + dy * dy;
 
    if(len2 <= tol * tol)
    {
        const double ddx = p.x - a.x;
        const double ddy = p.y - a.y;
 
        return std::sqrt(ddx * ddx + ddy * ddy) <= tol;
    }
 
    // Distanza del punto dalla retta passante per a-b.
    const double cross = (p.x - a.x) * dy - (p.y - a.y) * dx;
    const double dist = std::fabs(cross) / std::sqrt(len2);
 
    if(dist > tol)
        return false;
 
    // Check che il punto cada tra gli estremi del segmento.
    if(p.x < std::min(a.x, b.x) - tol || p.x > std::max(a.x, b.x) + tol)
        return false;
 
    if(p.y < std::min(a.y, b.y) - tol || p.y > std::max(a.y, b.y) + tol)
        return false;
 
    return true;
}


bool point_in_polygon (const Point2D p, const std::vector<Point2D> &boundaries, const double tol)
{
    double min_x =  DBL_MAX;
    double min_y =  DBL_MAX;
    double max_x = -DBL_MAX;
    double max_y = -DBL_MAX;

    //Trova minimo e massimo x,y per i punti di bordo
    for (const Point2D &bp : boundaries)
    {
        if (bp.x < min_x) min_x = bp.x;
        if (bp.x > max_x) max_x = bp.x;
        if (bp.y < min_y) min_y = bp.y;
        if (bp.y > max_y) max_y = bp.y;
    }

    //Check se i punti sono fuori/dentro i min/max
    if (p.x < min_x - tol || p.x > max_x + tol || p.y < min_y - tol || p.y > max_y + tol )
        return false;


    // https://stackoverflow.com/questions/11716268/point-in-polygon-algorithm
    //The first line of the if control if the point's y-coord is within the edge's scope
    //The second line checks whether the test point is to the left of the line
    // -> If that is true the line drawn rightwards from the test point crosses that edge.
    int i, j, c = 0;
    unsigned int nvert = boundaries.size();

    //Controllo esplicito se il punto è sul bordo
    for(size_t i = 0, j = nvert - 1; i < nvert; j = i++)
    {
        if(point_on_segment(p, boundaries[j], boundaries[i], tol))
            return true;
    }

    for (size_t i = 0, j = nvert-1; i < nvert; j = i++)
    {
        const Point2D &pi = boundaries[i];
        const Point2D &pj = boundaries[j];

        if ( ((pi.y>p.y) != (pj.y>p.y)) &&
                 (p.x < (pj.x-pi.x) * (p.y-pi.y) / (pj.y-pi.y) + pi.x) )
            c = !c;
    }
    return c;
}


void points_in_polygon (const std::vector<Point2D> &points, const std::vector<Point2D> &boundaries, std::vector<unsigned int> &id_in_points, const double tol)
{
    id_in_points.clear();

    unsigned int id = 0;
    for (const Point2D &p : points)
    {
        if (point_in_polygon(p, boundaries, tol)) //se è vero, salva l'indice del punto nel vettore id_is_points
            id_in_points.push_back(id);
        id++;
    }
}



void string_to_double_conversion_vectors (const std::vector<std::string> &values, const std::vector<std::string> &id, const std::vector<double> &xcoord, const std::vector<double> &ycoord, const std::vector<double> &zcoord,
                                          std::vector<double> &conv_values, std::vector<std::string> &corr_id, std::vector<double> &corr_xcoord, std::vector<double> &corr_ycoord, std::vector<double> &corr_zcoord)
{
    for(size_t i = 0 ; i< values.size(); i++)
    {
        std::string val_tmp = values.at(i);

        double val = 0.0;
        if(!val_tmp.empty() && val_tmp.compare("nd")!=0)
        {
            if(val_tmp.compare("*")!=0)
            {
                if(val_tmp.compare("NA")!=0)
                {
                    val = std::stod(val_tmp);
                    conv_values.push_back(val);

                    if(id.size() > 0)
                        corr_id.push_back(id.at(i));
                    if(xcoord.size() > 0)
                        corr_xcoord.push_back(xcoord.at(i));
                    if(ycoord.size() > 0)
                        corr_ycoord.push_back(ycoord.at(i));
                    if(zcoord.size() > 0)
                        corr_zcoord.push_back(zcoord.at(i));
                }
            }
        }
    }
}

void string_to_double_conversion_vectors (const std::vector<uint> &indices,
                                          const std::vector<std::string> &values,
                                          const std::vector<std::string> &id,
                                          const std::vector<double> &xcoord, const std::vector<double> &ycoord, const std::vector<double> &zcoord,
                                          std::vector<double> &conv_values, std::vector<std::string> &corr_id, std::vector<double> &corr_xcoord, std::vector<double> &corr_ycoord, std::vector<double> &corr_zcoord)
{
    for(size_t i:indices)
    {
        std::string val_tmp = values.at(i);

        double val = 0.0;
        if(!val_tmp.empty() && val_tmp.compare("nd")!=0)
        {
            if(val_tmp.compare("*")!=0)
            {
                if(val_tmp.compare("NA")!=0)
                {
                    val = std::stod(val_tmp);
                    conv_values.push_back(val);

                    if(id.size() > 0)
                        corr_id.push_back(id.at(i));
                    if(xcoord.size() > 0)
                        corr_xcoord.push_back(xcoord.at(i));
                    if(ycoord.size() > 0)
                        corr_ycoord.push_back(ycoord.at(i));
                    if(zcoord.size() > 0)
                        corr_zcoord.push_back(zcoord.at(i));
                }
            }
        }
    }
}


cinolib::vec2d segment_segment_intersection_2d (const cinolib::vec2d &p1, const cinolib::vec2d &p2, const cinolib::vec2d &v0, const cinolib::vec2d &v1)
{
    cinolib::vec2d p_inter;

    double A1 = p2.y() - p1.y();
    double B1 = p1.x() - p2.x();
    double C1 = A1 * p1.x() + B1 * p1.y();

    double A2 = v1.y() - v0.y();
    double B2 = v0.x() - v1.x();
    double C2 = A2 * v0.x() + B2 * v0.y();

    double det = A1 * B2 - A2 * B1;

    //std::cout << "determinant = " <<  det << std::endl;

    if(det != 0)
    {
        p_inter.x() = (B2 * C1 - B1 * C2) / det;
        p_inter.y() = (A1 * C2 - A2 * C1) / det;

        //std::cout << "x_intersection = " << p_inter.x() << std::endl;
        //std::cout << "y_intersection = " << p_inter.y() << std::endl;
    }
    else
    {
        p_inter.x() = DBL_MAX;
        p_inter.y() = DBL_MAX;
    }

    return p_inter;
}

//https://stackoverflow.com/questions/42740765/intersection-between-line-and-triangle-in-3d
cinolib::vec3d segment_triangle_intersection_3d (const cinolib::vec3d &q1, const cinolib::vec3d &q2, const cinolib::vec3d &p1, const cinolib::vec3d &p2, const cinolib::vec3d &p3)
{
    cinolib::vec3d p_inter;

    cinolib::vec3d p21 = p2-p1;
    cinolib::vec3d p31 = p3-p1;

    cinolib::vec3d cross = p21.cross(p31);

    cinolib::vec3d q1p1 = q1-p1;
    cinolib::vec3d q2q1 = q2-q1;

    double dot1 = q1p1.dot(cross);
    double dot2 = q2q1.dot(cross);

    double t = -(dot1/dot2);

    p_inter = q1 + t*(q2-q1);

    return p_inter;
}

cinolib::vec3d set_rotation_axis (const std::string &rot_axis)
{
    cinolib::vec3d axis (0,0,0);

    if(rot_axis.compare("X") == 0)
        axis.x() = 1;
    else if (rot_axis.compare("Y") == 0)
        axis.y() = 1;
    else if (rot_axis.compare("Z") == 0)
        axis.z() = 1;

    return axis;
}


void point_rotation (const double &x, const double &y, const double &z, const cinolib::vec3d &rot_axis, const double &rot_angle, const cinolib::vec3d &rot_center, double &x_rot, double &y_rot, double &z_rot) //angle in degree
{
    double rad = (rot_angle * M_PI)/180;

    cinolib::vec3d point (x,y,z);

    cinolib::mat3d R = cinolib::mat3d::ROT_3D(rot_axis, rad);

    point -= rot_center;
    point = R * point;
    point += rot_center;

    x_rot = point.x();
    y_rot = point.y();
    z_rot = point.z();
}


cinolib::vec3d point_rotation (cinolib::vec3d &point, const cinolib::vec3d &rot_axis, const double &rot_angle, const cinolib::vec3d &rot_center) //angle in degree
{
    double rad = (rot_angle * M_PI)/180;

    cinolib::mat3d R = cinolib::mat3d::ROT_3D(rot_axis, rad);

    point -= rot_center;
    point = R * point;
    point += rot_center;

    return point;
}

std::vector<cinolib::vec3d> points_rotation (std::vector<cinolib::vec3d> &points, const cinolib::vec3d &rot_axis, const double &rot_angle, const cinolib::vec3d &rot_center) //angle in degree
{
    double rad = (rot_angle * M_PI)/180;

    cinolib::mat3d R = cinolib::mat3d::ROT_3D(rot_axis, rad);

    for(uint i=0; i<points.size(); i++)
    {
        points.at(i) -= rot_center;
        points.at(i)  = R * points.at(i);
        points.at(i) += rot_center;
    }

    return points;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


///Check plane orientation
///
/// \brief align_points_to_xyplane
/// \param points: points to be aligned on the x-y plane
///
void align_points_to_xyplane (std::vector<double> &points_x, std::vector<double> &points_y, std::vector<double> &points_z, MUSE::Rotation &dataRotation)
{
    std::cout << "=== Automatic rotation of points to X-Y plane ..." << std::endl;

    // ----------------------------------------------------------
    // 1) Costruisci il vettore di punti cinolib dai dati reali
    // ----------------------------------------------------------

    std::vector<cinolib::vec3d> pts;
    pts.reserve(points_x.size());
    for(size_t i = 0; i < points_x.size(); i++)
        pts.push_back(cinolib::vec3d(points_x[i], points_y[i], points_z[i]));

    cinolib::Plane plane(pts);
    cinolib::vec3d normal = plane.n;

    // Orienta la normale sempre verso l'alto (Z > 0) per coerenza
    if(normal.z() < 0.0) normal = -normal;

    std::cout << "=== Estimated plane normal (on real points): [" << normal.x() << ", " << normal.y() << ", " << normal.z() << "]" << std::endl;

    std::cout << "=== Checking if plane normal is aligned with x-y plane normal ..." << std::endl;
    std::cout << std::endl;

    // ----------------------------------------------------------
    // 2) Verifica l'angolo tra la normale stimata e Z
    // ----------------------------------------------------------
    cinolib::vec3d normal_xy (0.0, 0.0, 1.0);
    double dot       = normal.dot(normal_xy);
    dot              = std::min(1.0, std::max(-1.0, dot));   // clamp per acos
    double angle_deg = std::acos(dot) * 180.0 / M_PI;

    std::cout << "=== Angle between plane normal and Z-axis: "
              << angle_deg << " degree" << std::endl;

    // tol viene passato come distanza lineare (es. 1e-2):
    // convertiamo in gradi con una soglia fissa di 1° come minimo sensato
    double threshold_deg = 1.0;
    std::cout << "=== Tolerance set to: " << threshold_deg << std::endl;

    if(angle_deg < 1.0)
    {
        std::cout << "=== Plane is already aligned with XY-plane. No rotation needed."
                  << std::endl;
        dataRotation.autoalign = false;
        dataRotation.rotation  = false;
        return;
    }

    // ----------------------------------------------------------
    // 3) Asse di rotazione = cross(normale_piano, Z)
    //    FIX: caso degenere quando la normale è anti-parallela a Z
    //    (angolo ≈ 180°): il cross product ha norma ≈ 0, quindi
    //    l'asse sarebbe indefinito. Gestiamo esplicitamente.
    // ----------------------------------------------------------
    cinolib::vec3d rot_axis = normal.cross(normal_xy);
    double ax_len = rot_axis.norm();

    if(ax_len < 1e-10)
    {
        // Normale quasi parallela o anti-parallela a Z
        // Se anti-parallela (angle ≈ 180°): ruota 180° attorno a X
        rot_axis  = cinolib::vec3d(1.0, 0.0, 0.0);
        angle_deg = 180.0;
        std::cout << "=== Degenerate case: plane normal is anti-parallel to Z. "
                  << "Rotating 180 deg around X-axis." << std::endl;
    }
    else
    {
        rot_axis /= ax_len;   // normalizza
    }

    // ----------------------------------------------------------
    // 4) Centro di rotazione = centroide dei punti reali
    //    FIX: la versione precedente calcolava il centro sull'AABB
    //    dei vertici del bbox (già sbagliati), non sui punti.
    // ----------------------------------------------------------
    double cx = 0.0, cy = 0.0, cz = 0.0;
    size_t n  = points_x.size();
    for(size_t i = 0; i < n; i++)
    {
        cx += points_x[i];
        cy += points_y[i];
        cz += points_z[i];
    }
    cx /= n; cy /= n; cz /= n;

    std::cout << "=== Rotation axis:   ["
              << rot_axis.x() << ", " << rot_axis.y() << ", " << rot_axis.z() << "]"
              << std::endl;
    std::cout << "=== Rotation angle:  " << angle_deg << " degree" << std::endl;
    std::cout << "=== Rotation center: [" << cx << ", " << cy << ", " << cz << "]"
              << std::endl;

    
    // ----------------------------------------------------------
    // 5) Applica la rotazione a tutti i punti tramite point_rotation
    //    (già disponibile in muselib/geometry/tools.h, usa cinolib)
    // ----------------------------------------------------------
    cinolib::vec3d center(cx, cy, cz);
    constexpr size_t MAX_PRINT = 5;

    for(size_t i = 0; i < n; i++)
    {
        cinolib::vec3d p(points_x[i], points_y[i], points_z[i]);

        if(i < MAX_PRINT)
            std::cout << "   | p[" << i << "] original: ("
                      << p.x() << ", " << p.y() << ", " << p.z() << ")" << std::endl;

        cinolib::vec3d pr = point_rotation(p, rot_axis, angle_deg, center);
        points_x[i] = pr.x();
        points_y[i] = pr.y();
        points_z[i] = pr.z();

        if(i < MAX_PRINT)
            std::cout << "   | p[" << i << "] rotated:  ("
                      << pr.x() << ", " << pr.y() << ", " << pr.z() << ")" << std::endl;
    }

    // ----------------------------------------------------------
    // 6) Verifica post-rotazione: varianza Z deve essere ≈ 0
    // ----------------------------------------------------------
    double mean_z = 0.0;
    for(size_t i = 0; i < n; i++) mean_z += points_z[i];
    mean_z /= n;
    double var_z = 0.0;
    for(size_t i = 0; i < n; i++)
        var_z += (points_z[i] - mean_z) * (points_z[i] - mean_z);
    var_z /= n;
    std::cout << "=== Post-rotation Z variance: " << var_z
              << "  (should be approx 0 if data lie on a flat plane)" << std::endl;

    // ----------------------------------------------------------
    // 7) Salva parametri di rotazione per la back-rotation
    // ----------------------------------------------------------
    dataRotation.autoalign   = true;
    dataRotation.rotation    = true;

    dataRotation.rotation_axis_vec   = { rot_axis.x(), rot_axis.y(), rot_axis.z() };
    dataRotation.rotation_angle      = angle_deg;
    dataRotation.rotation_center_x   = cx;
    dataRotation.rotation_center_y   = cy;
    dataRotation.rotation_center_z   = cz;

    dataRotation.normal_vec_original = { normal.x(),    normal.y(),    normal.z()    };
    dataRotation.normal_vec_rotated  = { normal_xy.x(), normal_xy.y(), normal_xy.z() };

    std::cout << "=== Automatic rotation to XY-plane ... COMPLETED." << std::endl;
}


// ============================================================
// check_align_points_to_xyplane
//
// Ritorna true se i punti NON sono già sul piano XY
// (e quindi la rotazione è necessaria).
// Usa cinolib::Plane sui punti reali, coerente con align_.
// ============================================================
bool check_align_points_to_xyplane(
        const std::vector<double>& points_x,
        const std::vector<double>& points_y,
        const std::vector<double>& points_z)
{
    if(points_x.size() < 3)
    {
        std::cout << "=== check_align: fewer than 3 points, assuming XY-plane." << std::endl;
        return false;
    }

    std::vector<cinolib::vec3d> pts;
    pts.reserve(points_x.size());
    for(size_t i = 0; i < points_x.size(); i++)
        pts.push_back(cinolib::vec3d(points_x[i], points_y[i], points_z[i]));

    cinolib::Plane plane(pts);
    cinolib::vec3d normal = plane.n;
    if(normal.z() < 0.0) normal = -normal;

    cinolib::vec3d normal_xy(0.0, 0.0, 1.0);
    double dot       = std::min(1.0, std::max(-1.0, normal.dot(normal_xy)));
    double angle_deg = std::acos(dot) * 180.0 / M_PI;

    // Ritorna true (rotazione necessaria) se l'angolo supera 1°
    return (angle_deg > 1.0);
}




bool align_points_to_xyplane(
    std::vector<Point3D> &points,
    MUSE::Rotation &dataRotation
)
{
    dataRotation = MUSE::Rotation();
 
    if(points.size() < 3)
    {
        std::cerr << FRED("ERROR: at least 3 points are required to estimate a support plane.") << std::endl;
        return false;
    }
 
    std::vector<double> x, y, z;
    x.reserve(points.size());
    y.reserve(points.size());
    z.reserve(points.size());
 
    for(const Point3D &p : points)
    {
        x.push_back(p.x);
        y.push_back(p.y);
        z.push_back(p.z);
    }
 
    align_points_to_xyplane(x, y, z, dataRotation);
 
    for(size_t i = 0; i < points.size(); ++i)
    {
        points[i].x = x[i];
        points[i].y = y[i];
        points[i].z = z[i];
    }
 
    return dataRotation.rotation;
}


void apply_rotation_to_points(
    std::vector<Point3D> &points,
    const MUSE::Rotation &rotation,
    const bool inverse
)
{
    if(!rotation.rotation)
        return;
 
    cinolib::vec3d axis(
        rotation.rotation_axis_vec[0],
        rotation.rotation_axis_vec[1],
        rotation.rotation_axis_vec[2]
    );
 
    cinolib::vec3d center(
        rotation.rotation_center_x,
        rotation.rotation_center_y,
        rotation.rotation_center_z
    );
 
    const double angle = inverse ? -rotation.rotation_angle
                                 :  rotation.rotation_angle;
 
    for(Point3D &p : points)
    {
        cinolib::vec3d q(p.x, p.y, p.z);
        q = point_rotation(q, axis, angle, center);
 
        p.x = q.x();
        p.y = q.y();
        p.z = q.z();
    }
}











