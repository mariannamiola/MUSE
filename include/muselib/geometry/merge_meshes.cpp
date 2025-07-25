#include "merge_meshes.h"

#include <cinolib/octree.h>

template<class M, class V, class E, class P>
void merge_meshes(const MUSE::SurfaceMesh<M,V,E,P> &mesh0, const MUSE::SurfaceMesh<M,V,E,P> &mesh1, MUSE::SurfaceMesh<M,V,E,P> &mesh_merge)
{
    std::vector<uint> bverts1 = mesh1.get_boundary_vertices();
    std::sort(bverts1.begin(), bverts1.end(), std::greater<uint>());

    mesh_merge = mesh0;
    mesh_merge += mesh1;

    for(uint vid1:bverts1)
    {
        cinolib::vec3d v1 = mesh1.vert(vid1);
        for(uint vid0:mesh0.get_boundary_vertices())
        {
            cinolib::vec3d v0 = mesh0.vert(vid0);
            if(v0 == v1)
            {
                uint remid=mesh0.num_verts()+vid1; //id vertex to remove in mesh_merge

                if(remid >= mesh_merge.num_verts())
                {
                    std::cout << FRED("ERROR: vert index not found!") << std::endl;
                    exit(1);
                }

                // std::cout << mesh0.num_verts() << std::endl;
                // std::cout << vid0 << ";" << std::setprecision(10) << mesh0.vert(vid0) << std::endl;

                // std::cout << mesh1.num_verts() << std::endl;
                // std::cout << vid1 << ";" << std::setprecision(10) << mesh1.vert(vid1) << std::endl;


                // std::cout << mesh_merge.num_verts() << std::endl;
                // std::cout << remid << ";" << std::setprecision(10) << mesh_merge.vert(remid) << std::endl;

                mesh_merge.vert_merge(vid0, remid);
            }
        }
    }
}

// template<class M, class V, class E, class P>
// void merge_meshes_at_coincident_vertices(const MUSE::SurfaceMesh<M,V,E,P> &mesh0, const MUSE::SurfaceMesh<M,V,E,P> &mesh1, MUSE::SurfaceMesh<M,V,E,P> &mesh_merge)
// {
//     cinolib::Octree octree;
//     octree.build_from_mesh_points(mesh0); //reference mesh

//     mesh_merge = mesh0;

//     std::map<uint,uint> vmap;
//     for(uint vid=0; vid<mesh1.num_verts(); ++vid)
//     {
//         cinolib::vec3d p = mesh1.vert(vid);

//         std::unordered_set<uint> ids;
//         if(octree.contains(p, false, ids))
//         {
//             // WARNING: I am assuming that the mapping is one to one at most
//             assert(ids.size()==1);
//             vmap[vid] = *ids.begin();
//         }
//         else
//         {
//             uint fresh_id = mesh_merge.vert_add(p);
//             vmap[vid] = fresh_id;
//         }
//     }

//     for(uint pid=0; pid<mesh1.num_polys(); ++pid)
//     {
//         auto p = mesh1.poly_verts_id(pid);

//         for(auto & vid : p)
//             vid = vmap.at(vid);

//         int test_id = mesh_merge.poly_id(p);
//         if(test_id<0)
//             mesh_merge.poly_add(p);
//     }
// }
