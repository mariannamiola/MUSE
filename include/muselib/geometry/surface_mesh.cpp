#include "surface_mesh.h"

#include <iostream>

#include <cinolib/meshes/quadmesh.h>
#include <cinolib/meshes/trimesh.h>
#include <cinolib/meshes/abstract_mesh.h>

#include "muselib/colors.h"

namespace MUSE
{
template<class M, class V, class E, class P>
SurfaceMesh<M,V,E,P>::SurfaceMesh(const char * filename, const MeshType type)
{
    if (type == MeshType::QUADMESH)
    {
        std::cout << "Loading quadmesh ... " << filename << std::endl;

        cinolib::Quadmesh<> *m = new cinolib::Quadmesh<> ;
        m->load (filename);

        //std::cout << m->num_verts() << " / " << m->num_polys() << std::endl;

        this->init(m->vector_verts(), m->vector_polys());
         _mesh_type = type;

        delete  m;
    }
    else if (type == MeshType::TRIMESH)
    {
        std::cout << "Loading trimesh ... " << filename << std::endl;

        cinolib::Trimesh<> *m = new cinolib::Trimesh<> ;
        m->load (filename);

        this->init(m->vector_verts(), m->vector_polys());
        _mesh_type = type;

        delete  m;
    }
    else
    {
        std::cout << FRED("ERROR. Only trimesh/quadmesh are supported as SurfaceMesh.") << std::endl;
        exit(1);
    }
}


template<class M, class V, class E, class P>
void SurfaceMesh<M,V,E,P>::save(const char * filename) const
{
    const std::string fname = filename;
    const std::string ext = fname.substr(fname.find_last_of("."));

    if (ext.compare(".obj") == 0 || ext.compare(".off") == 0)
    {
        if (_mesh_type == MeshType::QUADMESH)
        {
            cinolib::Quadmesh<> *m = new cinolib::Quadmesh<>(this->vector_verts(), this->vector_polys());
            m->save(filename);
            delete m;
        }
        else if (_mesh_type == MeshType::TRIMESH)
        {
            cinolib::Trimesh<> *m = new cinolib::Trimesh<>(this->vector_verts(), this->vector_polys());
            m->save(filename);
            delete m;
        }
        else
            cinolib::Polygonmesh<M,V,E,P>::save(filename);
    }
    else
    {
        std::cout << FRED("Mesh format extension is not supported (only .obj or .off.)") << std::endl;
        exit(1);
    }

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
MeshType SurfaceMesh<M,V,E,P>::set_meshtype() const
{
    MeshType type;

    if(this->verts_per_poly(0) == 3)
        type = MeshType::TRIMESH;
    else if(this->verts_per_poly(0) == 4)
        type = MeshType::QUADMESH;
    else
        type = MeshType::POLYGONMESH;

    return type;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
bool SurfaceMesh<M,V,E,P>::check_lateral_closing() const
{
    std::vector<uint> vertices;
    vertices = this->get_boundary_vertices();

    std::vector<cinolib::ipair> edge;
    edge = this->get_boundary_edges();

    if(edge.size() == 0 || vertices.size() == 0)
        return true;
    else
        return false;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
unsigned int SurfaceMesh<M,V,E,P>::first_correspondence_points(const cinolib::vec3d &first_point, const double eps) const
{
    uint index_first_point = 0;
    cinolib::vec2d first_point2d(first_point.x(), first_point.y());
    //std::cout << "stampa first_point = " << first_point2d << std::endl;

    for(uint i:this->get_ordered_boundary_vertices())
    {
        cinolib::vec3d search_first_point = this->vert(i);
        cinolib::vec2d search_first_point2d(search_first_point.x(), search_first_point.y());

        double dist = search_first_point2d.dist(first_point2d);
        //std::cout << "stampa search_first_point = " << search_first_point2d << std::endl;

        if(dist < eps)
            break;
        else
            index_first_point++;
    }
    //std::cout << "stampa indice inizio top = " << index_first_point << std::endl;
    return index_first_point;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
void SurfaceMesh<M,V,E,P>::triangles_split_on_centroid()
{
    uint n_polys = this->num_polys();
    for(uint pid=0; pid < n_polys; pid++)
    {
        uint centr = this->vert_add(this->poly_centroid(pid));

        uint vid0 = this->poly_vert_id(pid, 0);
        uint vid1 = this->poly_vert_id(pid, 1);
        uint vid2 = this->poly_vert_id(pid, 2);

        std::vector<uint> list0 {vid0, vid1, centr};
        std::vector<uint> list1 {vid1, vid2, centr};
        std::vector<uint> list2 {vid2, vid0, centr};

        uint new_pid;
        new_pid = this->poly_add(list0);
        this->poly_data(new_pid) = this->poly_data(pid);

        new_pid = this->poly_add(list1);
        this->poly_data(new_pid) = this->poly_data(pid);

        new_pid = this->poly_add(list2);
        this->poly_data(new_pid) = this->poly_data(pid);

        this->poly_remove(pid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
void SurfaceMesh<M,V,E,P>::triangles_split_on_edge()
{
    uint n_verts = this->num_verts();
    for(uint eid=0; eid<this->num_edges();eid++)
    {
        cinolib::vec3d v0 = this->edge_vert(eid, 0);
        cinolib::vec3d v1 = this->edge_vert(eid, 1);
        cinolib::vec3d delta = (v1-v0)/2;

        cinolib::vec3d v_med (v0.x()+delta.x(), v0.y()+delta.y(), v0.z()+delta.z());
        this->vert_add(v_med);
    }

    uint n_polys = this->num_polys();
    for(uint pid=0; pid < n_polys; pid++)
    {
        uint vid0 = this->poly_vert_id(pid, 0);
        uint vid1 = this->poly_vert_id(pid, 1);
        uint vid2 = this->poly_vert_id(pid, 2);

        std::vector<uint> pid_adj_edge = this->adj_p2e(pid);

        uint new0 = pid_adj_edge.at(0) + n_verts;
        uint new1 = pid_adj_edge.at(1) + n_verts;
        uint new2 = pid_adj_edge.at(2) + n_verts;

        std::vector<uint> list0 {vid0, new0, new2};
        std::vector<uint> list1 {new0, vid1, new1};
        std::vector<uint> list2 {new2, new1, vid2};
        std::vector<uint> list3 {new0, new1, new2};

        uint new_pid;
        new_pid = this->poly_add(list0);
        this->poly_data(new_pid) = this->poly_data(pid);

        new_pid = this->poly_add(list1);
        this->poly_data(new_pid) = this->poly_data(pid);

        new_pid = this->poly_add(list2);
        this->poly_data(new_pid) = this->poly_data(pid);

        new_pid = this->poly_add(list3);
        this->poly_data(new_pid) = this->poly_data(pid);

        this->poly_remove(pid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
void SurfaceMesh<M,V,E,P>::quads_split_on_edge()
{
    uint n_verts = this->num_verts();
    for(uint eid=0; eid<this->num_edges();eid++)
    {
        cinolib::vec3d v0 = this->edge_vert(eid, 0);
        cinolib::vec3d v1 = this->edge_vert(eid, 1);
        cinolib::vec3d delta = (v1-v0)/2;

        cinolib::vec3d v_med (v0.x()+delta.x(), v0.y()+delta.y(), v0.z()+delta.z());
        this->vert_add(v_med);
    }

    uint n_polys = this->num_polys();
    for(uint pid=0; pid < n_polys; pid++)
    {
        uint centr = this->vert_add(this->poly_centroid(pid));

        uint vid0 = this->poly_vert_id(pid, 0);
        uint vid1 = this->poly_vert_id(pid, 1);
        uint vid2 = this->poly_vert_id(pid, 2);
        uint vid3 = this->poly_vert_id(pid, 3);

        std::vector<uint> pid_adj_edge = this->adj_p2e(pid);

        uint new0 = pid_adj_edge.at(0) + n_verts;
        uint new1 = pid_adj_edge.at(1) + n_verts;
        uint new2 = pid_adj_edge.at(2) + n_verts;
        uint new3 = pid_adj_edge.at(3) + n_verts;

        std::vector<uint> list0 {vid0, new0, centr, new3};
        std::vector<uint> list1 {new0, vid1, new1, centr};
        std::vector<uint> list2 {centr, new1, vid2, new2};
        std::vector<uint> list3 {new3, centr, new2, vid3};

        uint new_pid;
        new_pid = this->poly_add(list0);
        this->poly_data(new_pid) = this->poly_data(pid);

        new_pid = this->poly_add(list1);
        this->poly_data(new_pid) = this->poly_data(pid);

        new_pid = this->poly_add(list2);
        this->poly_data(new_pid) = this->poly_data(pid);

        new_pid = this->poly_add(list3);
        this->poly_data(new_pid) = this->poly_data(pid);

        this->poly_remove(pid);
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// //Funzione per la rimozione dei vertici isolati
// template<class M, class V, class E, class P>
// void SurfaceMesh<M,V,E,P>::remove_isolate_vert()
// {
//     std::vector<uint> tbrem;
//     for (uint vid=0; vid < this->num_verts(); vid++)
//     {
//         if (this->adj_v2v(vid).size() == 0)
//             tbrem.push_back(vid);
//     }
//     if (tbrem.size() > 0)
//     {
//         std::reverse(tbrem.begin(), tbrem.end());
//         for (uint vid : tbrem)
//             this->vert_remove_unreferenced(vid);
//     }
//     std::cout << "Removing " << tbrem.size() << " Isolated Vertices ... COMPLETED" << std::endl;
// }

}

