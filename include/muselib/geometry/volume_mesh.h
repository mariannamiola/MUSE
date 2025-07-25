#ifndef VOLUME_MESH_H
#define VOLUME_MESH_H

#include <cinolib/meshes/polyhedralmesh.h>

namespace MUSE
{

template<class M = cinolib::Mesh_std_attributes, // default template arguments
         class V = cinolib::Vert_std_attributes,
         class E = cinolib::Edge_std_attributes,
         class F = cinolib::Polygon_std_attributes,
         class P = cinolib::Polyhedron_std_attributes>
class VolumeMesh : public cinolib::Polyhedralmesh<M,V,E,F,P>
{
public:
    explicit VolumeMesh(){}

    explicit VolumeMesh(const char * filename);

    explicit VolumeMesh(const char * filename, const MeshType type);

    explicit VolumeMesh(const std::vector<cinolib::vec3d>    & verts,
                        const std::vector<std::vector<uint>> & faces,
                        const std::vector<std::vector<uint>> & polys,
                        const std::vector<std::vector<bool>> & polys_face_winding);

    ~VolumeMesh(){}

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    MeshType mesh_type() const override
    {
        std::set<MeshType> types;

        for (uint pid=0; pid < this->num_polys(); pid++)
        {
            if (this->poly_is_tetrahedron(pid))
                types.insert(TETMESH);
            else
                if (this->poly_is_hexahedron(pid))
                    types.insert(HEXMESH);
            else types.insert(POLYHEDRALMESH);
        }

        if (types.size() == 1)
            return *types.begin();

        return POLYHEDRALMESH;
    }

    void save(const char * filename, const MeshType type) const;

    MeshType set_meshtype() const;

    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    void load(const char * filename) override
    {
        cinolib::Polyhedralmesh<M,V,E,F,P>::load(filename);
    }

    void write_poly_VTK(const char * filename);

private:

    MeshType _mesh_type;


};

}



#ifndef STATIC_MUSELIB
#include "volume_mesh.cpp"
#endif

#endif // VOLUME_MESH_H
