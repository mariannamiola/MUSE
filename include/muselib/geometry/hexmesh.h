#ifndef HEXMESH_H
#define HEXMESH_H

#include <cinolib/meshes/hexmesh.h>

#include "muselib/data_structures/point.h"

namespace MUSE
{

template<class M = cinolib::Mesh_std_attributes, // default template arguments
         class V = cinolib::Vert_std_attributes,
         class E = cinolib::Edge_std_attributes,
         class F = cinolib::Polygon_std_attributes,
         class P = cinolib::Polyhedron_std_attributes>
class Hexmesh : public cinolib::Hexmesh<M,V,E,F,P>
{
    public:

        explicit Hexmesh(){}

        explicit Hexmesh (const double &res_x, const double &res_y, const double &res_z, const std::vector<Point3D> &boundary);

        explicit Hexmesh (const double &res_x, const double &res_y, const double &res_z, cinolib::Trimesh<> quadmesh);

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void subHexmesh_from_trimesh  (MUSE::Hexmesh<> hexmesh, cinolib::Trimesh<> trimesh);
};

}

#ifndef STATIC_MUSELIB
#include "hexmesh.cpp"
#endif

#endif // HEXMESH_H
