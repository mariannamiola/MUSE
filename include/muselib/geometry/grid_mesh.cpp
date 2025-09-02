#include "grid_mesh.h"

#include "muselib/colors.h"

#include "muselib/data_structures/point.h"
#include "muselib/geometry/tools.h"
#include <cinolib/geometry/vec_mat.h>

#include <cinolib/vector_serialization.h>

namespace MUSE {


template<class M, class V, class E, class P>
Quadmesh<M,V,E,P>::Quadmesh (const uint polys_per_row, const uint polys_per_col)
{
    std::cout << "nx = " << polys_per_row << "; ny = " << polys_per_col << std::endl;

    std::vector<cinolib::vec3d> verts;
    std::vector<uint> polys;
    for(uint r=0; r<=polys_per_row; ++r)
    {
        for(uint c=0; c<=polys_per_col; ++c)
        {
            cinolib::vec3d v (c,r,0);
            verts.push_back(v);

            if (r<polys_per_row && c<polys_per_col)
            {
                int p0 = r * (polys_per_col+1) + c;
                polys.push_back(p0);

                int p1 = r * (polys_per_col+1) + (c+1);
                polys.push_back(p1);

                int p2 = (r+1) * (polys_per_col+1) + (c+1);
                polys.push_back(p2);

                int p3 = (r+1) * (polys_per_col+1) + c;
                polys.push_back(p3);
            }
        }
    }

    this->init(verts, cinolib::polys_from_serialized_vids(polys, 4));

    std::cout << "Creation Quadmesh ... COMPLETED." << std::endl;
}


///
/// \brief Quadmesh::Quadmesh constructor
/// \param polys_per_row: number of polygons per row
/// \param polys_per_col: number of polygons per column
/// \param pixel_size_x
/// \param pixel_size_y
/// \param XOrigin
/// \param YOrigin
///
template<class M, class V, class E, class P>
Quadmesh<M,V,E,P>::Quadmesh (const uint polys_per_row, const uint polys_per_col,
                               const float pixel_size_x, const float pixel_size_y,
                               const float XOrigin, const float YOrigin)
{
    std::vector<cinolib::vec3d> verts;
    std::vector<uint> polys;

    for (uint r = 0; r <= polys_per_row; ++r)
    {
        for (uint c = 0; c <= polys_per_col; ++c)
        {
            double x = XOrigin + c * pixel_size_x;
            double y = YOrigin + r * pixel_size_y;

            verts.emplace_back(x, y, 0.0);

            if (r < polys_per_row && c < polys_per_col)
            {
                int p0 = r * (polys_per_col + 1) + c;
                int p1 = p0 + 1;
                int p2 = p0 + (polys_per_col + 1) + 1;
                int p3 = p0 + (polys_per_col + 1);

                polys.push_back(p0);
                polys.push_back(p1);
                polys.push_back(p2);
                polys.push_back(p3);
            }
        }
    }

    this->init(verts, cinolib::polys_from_serialized_vids(polys, 4));
    std::cout << "Creation Quadmesh ... COMPLETED." << std::endl;
}


///
/// \brief Quadmesh::Quadmesh constructor
/// \param polys_per_row: number of polygons per row
/// \param polys_per_col: number of polygons per column
/// \param pixel_size_x
/// \param pixel_size_y
/// \param XOrigin
/// \param YOrigin
/// \param elevation: assign z value for 2.5D representation
///
template<class M, class V, class E, class P>
Quadmesh<M,V,E,P>::Quadmesh (const uint polys_per_row, const uint polys_per_col,
                               const float pixel_size_x, const float pixel_size_y,
                               const float XOrigin, const float YOrigin, const std::vector<std::vector<float>> &elevation)
{
    std::vector<cinolib::vec3d> verts;
    std::vector<uint> polys;

    // Controllo sicurezza: elevazione coerente con dimensioni input
    assert(elevation.size() == polys_per_row + 1);
    assert(elevation[0].size() == polys_per_col + 1);

    for (uint r = 0; r <= polys_per_row; ++r)
    {
        for (uint c = 0; c <= polys_per_col; ++c)
        {
            double x = XOrigin + c * pixel_size_x;
            double y = YOrigin + r * pixel_size_y;
            double z = static_cast<double>(elevation[r][c]);

            verts.emplace_back(x, y, z);

            if (r < polys_per_row && c < polys_per_col)
            {
                int p0 = r * (polys_per_col + 1) + c;
                int p1 = p0 + 1;
                int p2 = p0 + (polys_per_col + 1) + 1;
                int p3 = p0 + (polys_per_col + 1);

                polys.push_back(p0);
                polys.push_back(p1);
                polys.push_back(p2);
                polys.push_back(p3);
            }
        }
    }

    this->init(verts, cinolib::polys_from_serialized_vids(polys, 4));
    std::cout << "Creation Quadmesh ... COMPLETED." << std::endl;
}

template<class M, class V, class E, class P>
Quadmesh<M,V,E,P>::Quadmesh (const double &res_x, const double &res_y, const double &z, const std::vector<Point3D> &boundary)
{
    // Definizione boundary 2d e calcolo della bounding box
    double min_x =  DBL_MAX;
    double min_y =  DBL_MAX;

    double max_x = -DBL_MAX;
    double max_y = -DBL_MAX;

    std::vector<Point2D> boundary2d;
    for (uint i=0; i < boundary.size(); i++)
    {
        Point2D p;
        p.x = boundary.at(i).x;
        p.y = boundary.at(i).y;

        if (p.x < min_x)
            min_x = p.x;
        if (p.y < min_y)
            min_y = p.y;

        if (p.x > max_x)
            max_x = p.x;
        if (p.y > max_y)
            max_y = p.y;

        boundary2d.push_back(p);
    }
    //std::cout << "Computing bounding box ... COMPLETED. " << std::endl;


    // Calcolo delta
    double delta_x = max_x - min_x;
    double delta_y = max_y - min_y;
    //std::cout << "deltax = " << delta_x << "; deltay = " << delta_y << std::endl;

    // Calcolo numero di celle in base alla risoluzione in input
    uint npolys_x = static_cast<uint>(delta_x / res_x ); //ncelle su asse x
    uint npolys_y = static_cast<uint>(delta_y / res_y ); //ncelle su asse y

    std::cout << "n. polys per row " << npolys_x << std::endl;
    std::cout << "n. polys per column " << npolys_y << std::endl;

    // Vertici cella
    cinolib::vec3d v0 (0.0, 0.0, z);
    cinolib::vec3d v1 (res_x, 0.0, z);
    cinolib::vec3d v2 (res_x, res_y, z);
    cinolib::vec3d v3 (0.0, res_y, z);

    // Centro cella
    cinolib::vec3d center (res_x/2, res_y/2, z);


    std::map<cinolib::vec3d, uint> verts;
    for (uint nx=0; nx < npolys_x; nx++)
    {
        double x = min_x + res_x * nx;

        for (uint ny=0; ny < npolys_y; ny++)
        {
            double y = min_y + res_y * ny;

            cinolib::vec3d v0_pos (v0);
            cinolib::vec3d v1_pos (v1);
            cinolib::vec3d v2_pos (v2);
            cinolib::vec3d v3_pos (v3);

            double dx = x - v0.x();
            double dy = y - v0.y();
            cinolib::vec3d vdx (dx, dy, 0);


            v0_pos += vdx;
            v1_pos += vdx;
            v2_pos += vdx;
            v3_pos += vdx;
            cinolib::vec3d cp = center + vdx;

            // Verifico se il centro della cella è all'interno del bordo (-> per vincolare la griglia al bordo in input)
            Point2D p_center;
            p_center.x = cp.x();
            p_center.y = cp.y();

            bool p_center_is_in = false;
            if (point_in_polygon(p_center, boundary2d))
                p_center_is_in = true;

            if (!p_center_is_in)
                continue;


            // Definizione dell'iteratore
            auto v0_it = verts.find(v0_pos);
            auto v1_it = verts.find(v1_pos);
            auto v2_it = verts.find(v2_pos);
            auto v3_it = verts.find(v3_pos);


            // Definizione indici vertici
            uint v0_id = 0;
            uint v1_id = 0;
            uint v2_id = 0;
            uint v3_id = 0;

            if (v0_it == verts.end()) //se non lo trovo, quindi il vertice non è stato ancora aggiunto
            {
                v0_id = this->vert_add(v0_pos);
                verts.insert(std::pair<cinolib::vec3d,uint> (v0_pos, v0_id));
            }
            else
                v0_id = v0_it->second;

            if (v1_it == verts.end())
            {
                v1_id = this->vert_add(v1_pos);
                verts.insert(std::pair<cinolib::vec3d,uint> (v1_pos, v1_id));
            }
            else
                v1_id = v1_it->second;

            if (v2_it == verts.end())
            {
                v2_id = this->vert_add(v2_pos);
                verts.insert(std::pair<cinolib::vec3d,uint> (v2_pos, v2_id));
            }
            else
                v2_id = v2_it->second;

            if (v3_it == verts.end())
            {
                v3_id = this->vert_add(v3_pos);
                verts.insert(std::pair<cinolib::vec3d,uint> (v3_pos, v3_id));
            }
            else
                v3_id = v3_it->second;


            std::vector<uint> verts;
            verts.push_back(v0_id);
            verts.push_back(v1_id);
            verts.push_back(v2_id);
            verts.push_back(v3_id);

            this->poly_add(verts);
        }
    }

    std::cout << FGRN("Creation Quadmesh ... COMPLETED.") << std::endl;
}

//Funzione per la rimozione dei vertici isolati
template<class M, class V, class E, class P>
void Quadmesh<M,V,E,P>::Quadmesh::remove_isolate_vert()
{
    std::vector<uint> tbrem;
    for (uint vid=0; vid < this->num_verts(); vid++)
    {
        if (this->adj_v2v(vid).size() == 0)
            tbrem.push_back(vid);
    }
    if (tbrem.size() > 0)
    {
        std::reverse(tbrem.begin(), tbrem.end());
        for (uint vid : tbrem)
            this->vert_remove_unreferenced(vid);
    }
    std::cout << "Removing " << tbrem.size() << " Isolated Vertices ... COMPLETED" << std::endl;
}

//Funzione per la rimozione dei quad isolati
template<class M, class V, class E, class P>
void Quadmesh<M,V,E,P>::Quadmesh::remove_isolate_poly()
{
    std::vector<uint> tbrem;
    for (uint pid=0; pid < this->num_polys(); pid++)
    {
        if (this->adj_p2p(pid).size() == 0)
            tbrem.push_back(pid);
    }
    if (tbrem.size() > 0)
    {
        std::reverse(tbrem.begin(), tbrem.end());
        for (uint pid : tbrem)
            this->poly_remove_unreferenced(pid);
    }
    std::cout << "Removing " << tbrem.size() << " Isolated Polys ... COMPLETED" << std::endl;
}

}
