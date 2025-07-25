#ifndef GRID_MESH_H
#define GRID_MESH_H

#include <cinolib/meshes/polygonmesh.h>
#include <cinolib/meshes/quadmesh.h>

#include "muselib/data_structures/point.h"

namespace MUSE
{

template<class M = cinolib::Mesh_std_attributes, // default template arguments
         class V = cinolib::Vert_std_attributes,
         class E = cinolib::Edge_std_attributes,
         class P = cinolib::Polygon_std_attributes>
class Quadmesh : public cinolib::Polygonmesh<M,V,E,P>
//class Quadmesh : public cinolib::Quadmesh<M,V,E,P>
{
    public:

        explicit Quadmesh(){}

        explicit Quadmesh(const char * filename);

        explicit Quadmesh (const uint polys_per_row, const uint polys_per_col);

        explicit Quadmesh (const double &res_x, const double &res_y, const double &z, const std::vector<Point3D> &boundary);

        void remove_isolate_vert();
        void remove_isolate_poly();
};
}

#ifndef STATIC_MUSELIB
#include "grid_mesh.cpp"
#endif

#endif // GRID_MESH_H
