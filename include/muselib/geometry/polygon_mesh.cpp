#include "polygon_mesh.h"

#include "muselib/colors.h"

#include "muselib/data_structures/point.h"
#include "muselib/geometry/tools.h"
#include "muselib/geometry/mesh.h"
#include <cinolib/geometry/vec_mat.h>

#include <cinolib/vector_serialization.h>
#include <cinolib/merge_meshes_at_coincident_vertices.h>

namespace MUSE {

template<class M, class V, class E, class P>
Polygonmesh<M,V,E,P>::Polygonmesh (const std::vector<std::vector<Point3D>> &boundaries, Polygonmesh<> &m)
{
    std::cout << "### Size: " << boundaries.size() << std::endl;

    //Adding first polygon
    std::vector<uint> id_vert_boundary;

    std::vector<Point3D> boundaries_unique;
    remove_duplicates_test_opt(boundaries.at(0), boundaries_unique);
    std::cout << "### #V polygon: " << boundaries_unique.size() << std::endl;
    for(uint poly_vid=0; poly_vid< boundaries_unique.size(); poly_vid++)
    {
        cinolib::vec3d vert_pos(boundaries_unique.at(poly_vid).x, boundaries_unique.at(poly_vid).y, boundaries_unique.at(poly_vid).z);
        uint vert_id = m.vert_add(vert_pos);
        id_vert_boundary.push_back(vert_id);
    }
    uint poly_id = m.poly_add(id_vert_boundary);
    std::cout << "### Add poly ID: 0" << std::endl;

    for(uint poly=1;poly<boundaries.size(); poly++)
    {
        Polygonmesh<> m_tmp;

        boundaries_unique.clear();
        remove_duplicates_test_opt(boundaries.at(poly), boundaries_unique);
        std::cout << "### #V polygon: " << boundaries_unique.size() << std::endl;

        std::vector<uint> id_vert_boundary1;
        for(uint poly_vid=0; poly_vid< boundaries_unique.size(); poly_vid++)
        {
            cinolib::vec3d vert_pos(boundaries_unique.at(poly_vid).x, boundaries_unique.at(poly_vid).y, boundaries_unique.at(poly_vid).z);
            uint vert_id = m_tmp.vert_add(vert_pos);
            id_vert_boundary1.push_back(vert_id);
        }
        uint poly_id1 = m_tmp.poly_add(id_vert_boundary1);
        std::cout << "### Add poly ID: " << poly << std::endl;

        cinolib::merge_meshes_at_coincident_vertices(m, m_tmp, m);
    }
    std::cout << "#V: " << m.num_verts() << " -- #P: " << m.num_polys() << std::endl;

    std::cout << FGRN("Creation Polygonmesh ... COMPLETED.") << std::endl;
}

}
