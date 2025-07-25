#ifndef POLYGON_MESH_H
#define POLYGON_MESH_H

#include <cinolib/meshes/polygonmesh.h>

#include "muselib/data_structures/point.h"
#include <vector>

namespace MUSE
{

template<class M = cinolib::Mesh_std_attributes, // default template arguments
         class V = cinolib::Vert_std_attributes,
         class E = cinolib::Edge_std_attributes,
         class P = cinolib::Polygon_std_attributes>
class Polygonmesh : public cinolib::Polygonmesh<M,V,E,P>
{
public:

    explicit Polygonmesh(){}

    explicit Polygonmesh (const std::vector<std::vector<Point3D>> &boundaries, Polygonmesh<> &m);

};
}


#ifndef STATIC_MUSELIB
#include "polygon_mesh.cpp"
#endif

#endif // POLYGON_MESH_H
