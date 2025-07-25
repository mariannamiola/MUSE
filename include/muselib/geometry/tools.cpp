#include "tools.h"

#include <float.h>
#include <math.h>
#include <vector>

#include <deque>
#include <tuple>


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

bool point_in_polygon (const Point2D p, const std::vector<Point2D> &boundaries)
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
    if (p.x < min_x || p.x > max_x || p.y < min_y || p.y > max_y )
        return false;


    // https://stackoverflow.com/questions/11716268/point-in-polygon-algorithm
    //The first line of the if control if the point's y-coord is within the edge's scope
    //The second line checks whether the test point is to the left of the line
    // -> If that is true the line drawn rightwards from the test point crosses that edge.
    int i, j, c = 0;
    unsigned int nvert = boundaries.size();

    for (i = 0, j = nvert-1; i < nvert; j = i++)
    {
        if ( ((boundaries[i].y>p.y) != (boundaries[j].y>p.y)) &&
                 (p.x < (boundaries[j].x-boundaries[i].x) * (p.y-boundaries[i].y) / (boundaries[j].y-boundaries[i].y) + boundaries[i].x) )
            c = !c;
    }
    return c;
}


void points_in_polygon (const std::vector<Point2D> &points, const std::vector<Point2D> &boundaries, std::vector<unsigned int> &id_in_points)
{
    id_in_points.clear();

    unsigned int id = 0;
    for (const Point2D &p : points)
    {
        if (point_in_polygon(p, boundaries)) //se è vero, salva l'indice del punto nel vettore id_is_points
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


// std::vector<std::vector<std::tuple<double, double>>> buffered_points(const std::vector<std::vector<std::tuple<double, double>>>& poly_list, double distance = 1000)
// {
//     std::vector<std::vector<std::tuple<double, double>>> buff_vertices_l;
//     std::vector<std::vector<std::tuple<double, double>>> buff_vertices_t;

//     for (const auto& poly : poly_list)
//     {
//         size_t size_of_list = poly.size();
//         std::deque<std::tuple<double, double>> poly_coord;

//         for (size_t index = 0; index < size_of_list; ++index)
//         {
//             auto current_vertex = poly[index];
//             std::vector<std::tuple<double, double>> control_vertices =
//             {
//                 poly[(index - 1 + size_of_list) % size_of_list],
//                 current_vertex,
//                 poly[(index + 1) % size_of_list]
//             };
//         }
//     }
//     return buff_vertices_l; // Adjust return as needed
// }


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::








// Remove duplicates for Point2D, considering a fixed tolerance t = 1e-06
//std::vector<Point3D> remove_duplicates (std::vector<Point3D> &points, const double t)
//{
//    // Fill index
//    for(size_t i=0; i<points.size(); i++)
//        points.at(i).id = i;

//    std::vector<Point3D> sorted_points = points;
//    std::vector<Point3D> unique_points;
//    std::vector<int> id_dupl;

//    std::sort(sorted_points.begin(), sorted_points.end(), comparePoint);

//    for(size_t i=1; i < sorted_points.size(); i++)
//    {
//        Point2D p0, p1;
//        p0.x = sorted_points.at(i-1).x;
//        p0.y = sorted_points.at(i-1).y;
//        p1.x = sorted_points.at(i).x;
//        p1.y = sorted_points.at(i).y;

//        if (dist(p0, p1) <= t)
//            id_dupl.push_back(sorted_points.at(i).id);
//    }

//    if(id_dupl.size() > 0)
//    {
//        std::sort(id_dupl.begin(), id_dupl.end());

//        for(size_t i=0; i< points.size(); i++)
//            if (!check_index(id_dupl, points.at(i).id))
//                unique_points.push_back(points.at(i));
//    }
//    else
//        unique_points = points;

//    std::cout << "Removing duplicated points ... COMPLETED. " << unique_points.size() << " left" << std::endl;
//    return unique_points;
//}

























