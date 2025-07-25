#ifndef TOOLS_H
#define TOOLS_H

#include <vector>

#include <cinolib/geometry/vec_mat.h>

#include "muselib/data_structures/point.h"

using namespace MUSE;


bool check_index                                (const std::vector<int> &id_dupl, int index);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double point_to_line_distance                   (const double &x0, const double &y0, const double &angle_rad, const double &x, const double &y);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


bool point_in_polygon                           (const Point2D p, const std::vector<Point2D> &boundaries);
void points_in_polygon                          (const std::vector<Point2D> &points, const std::vector<Point2D> &boundaries, std::vector<unsigned int> &id_in_points);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void string_to_double_conversion_vectors        (const std::vector<std::string> &values, const std::vector<std::string> &id, const std::vector<double> &xcoord, const std::vector<double> &ycoord, const std::vector<double> &zcoord,
                                                    std::vector<double> &conv_values, std::vector<std::string> &corr_id, std::vector<double> &corr_xcoord, std::vector<double> &corr_ycoord, std::vector<double> &corr_zcoord);
void string_to_double_conversion_vectors (const std::vector<uint> &indices,
                                          const std::vector<std::string> &values,
                                          const std::vector<std::string> &id,
                                          const std::vector<double> &xcoord, const std::vector<double> &ycoord, const std::vector<double> &zcoord,
                                          std::vector<double> &conv_values, std::vector<std::string> &corr_id, std::vector<double> &corr_xcoord, std::vector<double> &corr_ycoord, std::vector<double> &corr_zcoord);

cinolib::vec2d segment_segment_intersection_2d  (const cinolib::vec2d &p1, const cinolib::vec2d &p2, const cinolib::vec2d &v0, const cinolib::vec2d &v1);
cinolib::vec3d segment_triangle_intersection_3d (const cinolib::vec3d &q1, const cinolib::vec3d &q2, const cinolib::vec3d &p1, const cinolib::vec3d &p2, const cinolib::vec3d &p3);


cinolib::vec3d set_rotation_axis                (const std::string &rot_axis);

void point_rotation                             (const double &x, const double &y, const double &z, const cinolib::vec3d &rot_axis, const double &rot_angle, const cinolib::vec3d &rot_center, double &x_rot, double &y_rot, double &z_rot); //angle in degree

cinolib::vec3d point_rotation                   (cinolib::vec3d &point, const cinolib::vec3d &rot_axis, const double &rot_angle, const cinolib::vec3d &rot_center); //angle in degree
std::vector<cinolib::vec3d> points_rotation     (std::vector<cinolib::vec3d> &points, const cinolib::vec3d &rot_axis, const double &rot_angle, const cinolib::vec3d &rot_center); //angle in degree

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//std::vector<Point3D> create_grid_on_centers (const std::vector<Point3D> &points, const std::vector<Point3D> &boundary);



#ifndef STATIC_MUSELIB
#include "tools.cpp"
#endif

#endif // TOOLS_H
