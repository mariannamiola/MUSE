#include "volume_mesh.h"

#include <cinolib/meshes/tetmesh.h>
#include <cinolib/meshes/hexmesh.h>

#include <cinolib/meshes/abstract_mesh.h>
#include <muselib/input/load_vtk.h>
#include <muselib/output/save_vtk.h>
#include "muselib/geometry/tools.h"

namespace MUSE
{
template<class M, class V, class E, class F, class P>
VolumeMesh<M,V,E,F,P>::VolumeMesh(const char * filename, const MeshType type)
{
    const std::string fname = filename;
    const std::string ext = fname.substr(fname.find_last_of('.'));

    if (ext.compare(".vtk") == 0 || ext.compare(".VTK") == 0)
    {
        std::vector<cinolib::vec3d> verts;
        std::vector<std::vector<uint>> polys;

        if(load_vtk(filename, verts, polys) != 0)
        {
            std::cerr << "ERROR while loading vtk file: " << filename << std::endl;
            exit(1);
        }

        for(const auto &poly : polys)
        {
            if(type == MeshType::TETMESH && poly.size() != 4)
            {
                std::cerr << "ERROR. Requested TETMESH but file contains non-tetra cells." << std::endl;
                exit(1);
            }
            if(type == MeshType::HEXMESH && poly.size() != 8)
            {
                std::cerr << "ERROR. Requested HEXMESH but file contains non-hexa cells." << std::endl;
                exit(1);
            }
        }

        this->init(verts, polys);
        _mesh_type = type;
        return;
    }

    if (type == MeshType::TETMESH)
    {
        std::cout << "Loading tetmesh ... " << filename << std::endl;

        cinolib::Tetmesh<> *m = new cinolib::Tetmesh<> ;
        m->load (filename);

        std::cout << m->num_verts() << " / " << m->num_polys() << std::endl;

        this->init(m->vector_verts(), m->vector_polys());
         _mesh_type = type;

        delete  m;
    }
    else if (type == MeshType::HEXMESH)
    {
        std::cout << "Loading hexmesh ... " << filename << std::endl;

        cinolib::Hexmesh<> *m = new cinolib::Hexmesh<> ;
        m->load (filename);

        this->init(m->vector_verts(), m->vector_polys());
        _mesh_type = type;

        delete  m;
    }
    else
    {
        std::cout << "ERROR. Only tetmesh/hexmesh are supported as VolumeMesh." << std::endl;
        exit(1);
    }
}

template<class M, class V, class E, class F, class P>
void VolumeMesh<M,V,E,F,P>::load(const char * filename)
{
    const std::string fname = filename;
    const std::string ext = fname.substr(fname.find_last_of('.'));

    if (ext.compare(".vtk") == 0 || ext.compare(".VTK") == 0)
    {
        std::vector<cinolib::vec3d> verts;
        std::vector<std::vector<uint>> polys;

        if(load_vtk(filename, verts, polys) != 0)
        {
            std::cerr << "ERROR while loading vtk file: " << filename << std::endl;
            exit(1);
        }

        this->clear();
        this->init(verts, polys);
        return;
    }

    cinolib::Polyhedralmesh<M,V,E,F,P>::load(filename);
}


template<class M, class V, class E, class F, class P>
void VolumeMesh<M,V,E,F,P>::save(const char * filename, const MeshType type) const
{
    const std::string fname = filename;
    const std::string ext = fname.substr(fname.find_last_of("."));

    std::vector<std::vector<uint>> poly;
    for(uint pid=0; pid < this->num_polys(); pid++)
        poly.push_back(this->poly_verts_id(pid));

    if (ext.compare(".vtk") == 0 || ext.compare(".VTK") == 0)
    {
        if(save_vtk(filename, this->vector_verts(), poly) != 0)
        {
            std::cerr << "ERROR while writing vtk file: " << filename << std::endl;
            exit(1);
        }
        return;
    }

    if (ext.compare(".mesh") == 0 || ext.compare(".MESH") == 0)
    {
        if (type == MeshType::HEXMESH)
        {
            //cinolib::Hexmesh<> *m = new cinolib::Hexmesh<>(this->vector_verts(), this->vector_polys());
            cinolib::Hexmesh<> *m = new cinolib::Hexmesh<>(this->vector_verts(), poly);
            m->save(filename);
            delete m;
        }
        else if (type == MeshType::TETMESH)
        {
            //cinolib::Tetmesh<> *m = new cinolib::Tetmesh<>(this->vector_verts(), this->vector_polys());
            cinolib::Tetmesh<> *m = new cinolib::Tetmesh<>(this->vector_verts(), poly);
            m->save(filename);
            delete m;
        }
    }
    else
    {
        std::cout << "ERROR. Only tetmesh/hexmesh are supported as VolumeMesh." << std::endl;
        exit(1);
    }
}

template<class M, class V, class E, class F, class P>
MeshType VolumeMesh<M,V,E,F,P>::set_meshtype() const
{
    MeshType type;
    for (uint pid=0; pid < this->num_polys(); pid++)
    {
        if(this->poly_is_tetrahedron(pid))
            type = MeshType::TETMESH;
        else if(this->poly_is_hexahedron(pid))
            type = MeshType::HEXMESH;
        else
            type = MeshType::POLYHEDRALMESH;
    }
    return type;
}


template<class M, class V, class E, class F, class P>
void VolumeMesh<M,V,E,F,P>::write_poly_VTK(const char * filename)
{
    std::vector<std::vector<uint>> polys;
    polys.reserve(this->num_polys());
    for(uint pid = 0; pid < this->num_polys(); ++pid)
        polys.push_back(this->adj_p2v(pid));

    if(save_vtk(filename, this->vector_verts(), polys) != 0)
    {
        std::cerr << "ERROR while writing vtk file: " << filename << std::endl;
        exit(1);
    }
}

template<class M, class V, class E, class F, class P>
void VolumeMesh<M,V,E,F,P>::rotate(const MUSE::Rotation &bound_Rotation, bool reverse)
{
    // Stesso pattern di SurfaceMesh::rotate / rotation_on_trimesh: get_rotation_axis
    // sceglie rotation_axis_vec (rotazioni auto-allineate) o set_rotation_axis(rotation_axis)
    // (rotazioni manuali --rotaxis), cosicché entrambe le sorgenti di rotazione siano gestite.
    cinolib::vec3d rot_axis_vec = get_rotation_axis(bound_Rotation);
    cinolib::vec3d c (bound_Rotation.rotation_center_x, bound_Rotation.rotation_center_y, bound_Rotation.rotation_center_z);

    double sign = 1.0;
    if(reverse)
        sign = -1.0;

    for(size_t i=0; i< this->num_verts(); i++)
    {
        cinolib::vec3d sample (this->vert(i).x(), this->vert(i).y(), this->vert(i).z());
        sample = point_rotation(sample, rot_axis_vec, sign * bound_Rotation.rotation_angle, c);

        this->vert(i).x() = sample.x();
        this->vert(i).y() = sample.y();
        this->vert(i).z() = sample.z();
    }
}

}
