#ifndef SURFACE_MESH_H
#define SURFACE_MESH_H

#include <cinolib/meshes/polygonmesh.h>

namespace MUSE
{

template<class M = cinolib::Mesh_std_attributes, // default template arguments
         class V = cinolib::Vert_std_attributes,
         class E = cinolib::Edge_std_attributes,
         class P = cinolib::Polygon_std_attributes>
class SurfaceMesh : public cinolib::Polygonmesh<M,V,E,P>
{
    public:

        explicit SurfaceMesh(){}

        explicit SurfaceMesh(const char * filename, const MeshType type);

        ~SurfaceMesh(){}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        MeshType mesh_type() const { return _mesh_type; }

        void save(const char * filename) const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        void load(const char * filename) override
        {
            cinolib::Polygonmesh<M,V,E,P>::load(filename);
        }

        bool check_lateral_closing() const;
        unsigned int first_correspondence_points (const cinolib::vec3d &first_point, const double eps = 1e-06) const;

        MeshType set_meshtype() const;

        void triangles_split_on_centroid();
        void triangles_split_on_edge();
        void quads_split_on_edge();
        //void remove_isolate_vert();

    private:

        MeshType _mesh_type;
};


}

#ifndef STATIC_MUSELIB
#include "surface_mesh.cpp"
#endif

#endif // SURFACE_MESH_H
