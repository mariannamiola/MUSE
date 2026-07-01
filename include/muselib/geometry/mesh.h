#ifndef MESH_H
#define MESH_H

#include <iostream>

#include "muselib/data_structures/point.h"
#include "muselib/data_structures/rotation.h"

#include <cinolib/triangle_wrap.h>
#include <cinolib/meshes/drawable_trimesh.h>
#include <cinolib/meshes/quadmesh.h>

using namespace MUSE;


std::vector<Point3D> remove_sorted_duplicates   (const std::vector<Point3D> &points);
void remove_duplicates_test_opt                 (const std::vector<Point3D> &points, std::vector<Point3D> &unique_points, const double &tol = 1e-2);

void remove_isolate_vertices                    (cinolib::Trimesh<> &mesh);
void mesh_summary                               (cinolib::Trimesh<> &mesh);

void rotation_on_trimesh                        (cinolib::Trimesh<> &trimesh, const MUSE::Rotation &bound_Rotation, bool reverse = false);

cinolib::Trimesh<> points_triangulation         (const std::vector<Point3D> &points, std::string opt);

cinolib::Trimesh<> boundary_triangulation       (const std::vector<Point3D> &boundaries, std::string opt);
cinolib::Trimesh<> constrained_triangulation    (const std::vector<Point3D> &boundary3d, const std::vector<Point3D> &points, std::string opt);
cinolib::Trimesh<> concavehull_triangulation    (const std::vector<Point3D> &concavehull3d, const std::vector<Point3D> &points, const std::string &opt, const double &tol = 1e-2);

unsigned int first_boundary_point_corresponding (const cinolib::vec3d &first_point, const std::vector<uint> &or_idch, cinolib::Trimesh<> mesh);
unsigned int first_boundary_point_corresponding (const cinolib::vec3d &first_point, const std::vector<uint> &or_idch, cinolib::Quadmesh<> mesh);

cinolib::Trimesh<> constrained_triangulation2   (const std::vector<Point3D> &boundary3d, const std::vector<Point3D> &points, std::string opt);

bool check_poly_normals                         (cinolib::Trimesh<> mesh);

std::vector<cinolib::vec3d> search_ordered_boundary_points(cinolib::Trimesh<> &mesh);

bool check_closing_mesh                         (cinolib::Trimesh<> closed_mesh);
bool check_closing_mesh                         (cinolib::Quadmesh<> closed_mesh);

cinolib::Trimesh<> closing_2trimeshes           (cinolib::Trimesh<> &mesh_sup, cinolib::Trimesh<> &mesh_inf, const double &step);
cinolib::Quadmesh<> closing_2quadmeshes         (cinolib::Quadmesh<> &mesh_sup, cinolib::Quadmesh<> &mesh_inf, const double &step);

cinolib::Trimesh<> add_lateral_polys            (cinolib::Trimesh<> &closed_m, const std::vector<uint> &or_idch0, const std::vector<uint> &or_idch1);
cinolib::Quadmesh<> add_lateral_polys           (cinolib::Quadmesh<> &closed_m, const std::vector<uint> &or_idch0, const std::vector<uint> &or_idch1);

cinolib::Trimesh<> closing_2trimeshes           (cinolib::Trimesh<> &mesh_sup, const std::vector<cinolib::vec3d> &new_verts, std::vector<uint> id_start_verts, std::vector<uint> id_last_verts);

std::vector<Point3D> computing_concavehull      (const std::vector<Point3D> &points, std::vector<int> &convexhull, std::vector<int> &b_id);


std::vector<double> region_growing_for_projection  (cinolib::Trimesh<> &mesh, cinolib::vec3d &p);
std::vector<double> region_growing_for_projection1 (cinolib::Trimesh<> &mesh, cinolib::vec3d &p);


bool vert_merge (cinolib::Trimesh<> mesh, const uint vid0, const uint vid1);


void triangles_split_on_centroid(cinolib::Trimesh<> &mesh);
//void triangles_split_on_edge    (cinolib::Trimesh<> &mesh);
//void quads_split_on_edge        (cinolib::Quadmesh<> &mesh);


#ifndef STATIC_MUSELIB
#include "mesh.cpp"
#endif

#endif // MESH_H
