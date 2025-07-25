#include "hexmesh.h"

#include "muselib/data_structures/point.h"
#include "muselib/geometry/tools.h"
#include <cinolib/geometry/vec_mat.h>

#include <igl/cotmatrix.h>
#include <igl/barycenter.h>
#include <igl/boundary_facets.h>
#include <igl/parula.h>
#include <igl/readMESH.h>
#include <igl/slice.h>
#include <igl/marching_tets.h>
#include <igl/winding_number.h>
//#include <igl/opengl/glfw/Viewer.h>
#include <Eigen/Sparse>

#include <cinolib/vector_serialization.h>

namespace MUSE {


template<class M, class V, class E, class F, class P>
Hexmesh<M,V,E,F,P>::Hexmesh (const double &res_x, const double &res_y, const double &res_z, const std::vector<Point3D> &boundary)
{
    // Definizione boundary 3d e calcolo della bounding box
    double min_x =  DBL_MAX;
    double min_y =  DBL_MAX;
    double min_z =  DBL_MAX;

    double max_x = -DBL_MAX;
    double max_y = -DBL_MAX;
    double max_z = -DBL_MAX;

    std::vector<Point2D> boundary2d;
    for (uint i=0; i < boundary.size(); i++)
    {
        Point2D p;
        p.x = boundary.at(i).x;
        p.y = boundary.at(i).y;
        //p.z = boundary.at(i).z;

        if (p.x < min_x)
            min_x = p.x;
        if (p.y < min_y)
            min_y = p.y;
        if (boundary.at(i).z < min_z)
            min_z = boundary.at(i).z;

        if (p.x > max_x)
            max_x = p.x;
        if (p.y > max_y)
            max_y = p.y;
        if (boundary.at(i).z > max_z)
            max_z = boundary.at(i).z;

        boundary2d.push_back(p);
    }
    std::cout << "Computing bounding box ... COMPLETED. " << std::endl;


    // Calcolo delta
    double delta_x = max_x - min_x;
    double delta_y = max_y - min_y;
    double delta_z = max_z - min_z;

    // Calcolo numero di celle in base alla risoluzione in input
    uint npolys_x = static_cast<uint>(delta_x / res_x ); //ncelle su asse x
    uint npolys_y = static_cast<uint>(delta_y / res_y ); //ncelle su asse y
    uint npolys_z = static_cast<uint>(delta_z / res_z ); //ncelle su asse z

    std::cout << "n. polys - x " << npolys_x << std::endl;
    std::cout << "n. polys - y " << npolys_y << std::endl;
    std::cout << "n. polys - z " << npolys_z << std::endl;


    // Vertici cella
    cinolib::vec3d v0 (0.0, 0.0, 0.0);
    cinolib::vec3d v1 (res_x, 0.0, 0.0);
    cinolib::vec3d v2 (res_x, res_y, 0.0);
    cinolib::vec3d v3 (0.0, res_y, 0.0);

    cinolib::vec3d v4 (0.0, 0.0, res_z);
    cinolib::vec3d v5 (res_x, 0.0, res_z);
    cinolib::vec3d v6 (res_x, res_y, res_z);
    cinolib::vec3d v7 (0.0, res_y, res_z);

    // Centro cella
    cinolib::vec3d center (res_x/2, res_y/2, res_z/2);

    std::map<cinolib::vec3d, uint> verts;

    for (uint nx=0; nx < npolys_x; nx++)
    {
        double x = min_x + res_x * nx;

        for (uint ny=0; ny < npolys_y; ny++)
        {
            double y = min_y + res_y * ny;

            for (uint nz=0; nz <npolys_z; nz++)
            {
                double z = min_z + res_z * nz;

                cinolib::vec3d v0_pos (v0);
                cinolib::vec3d v1_pos (v1);
                cinolib::vec3d v2_pos (v2);
                cinolib::vec3d v3_pos (v3);

                cinolib::vec3d v4_pos (v4);
                cinolib::vec3d v5_pos (v5);
                cinolib::vec3d v6_pos (v6);
                cinolib::vec3d v7_pos (v7);

                double dx = x - v0.x();
                double dy = y - v0.y();
                double dz = z - v0.z();
                cinolib::vec3d vdx (dx, dy, dz);

                v0_pos += vdx;
                v1_pos += vdx;
                v2_pos += vdx;
                v3_pos += vdx;

                v4_pos += vdx;
                v5_pos += vdx;
                v6_pos += vdx;
                v7_pos += vdx;

                cinolib::vec3d cp = center + vdx;

                // Verifico se il centro della cella è all'interno del bordo (-> per vincolare la griglia al bordo in input)
                /*Point2D p_center;
                p_center.x = cp.x();
                p_center.y = cp.y();

                bool p_center_is_in = false;
                if (point_in_polygon(p_center, boundary2d))
                    p_center_is_in = true;

                if (!p_center_is_in)
                    continue;*/

                // Definizione dell'iteratore
                auto v0_it = verts.find(v0_pos);
                auto v1_it = verts.find(v1_pos);
                auto v2_it = verts.find(v2_pos);
                auto v3_it = verts.find(v3_pos);

                auto v4_it = verts.find(v4_pos);
                auto v5_it = verts.find(v5_pos);
                auto v6_it = verts.find(v6_pos);
                auto v7_it = verts.find(v7_pos);


                // Definizione indici vertici
                uint v0_id = 0;
                uint v1_id = 0;
                uint v2_id = 0;
                uint v3_id = 0;

                uint v4_id = 0;
                uint v5_id = 0;
                uint v6_id = 0;
                uint v7_id = 0;

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

                if (v4_it == verts.end())
                {
                    v4_id = this->vert_add(v4_pos);
                    verts.insert(std::pair<cinolib::vec3d,uint> (v4_pos, v4_id));
                }
                else
                    v4_id = v4_it->second;

                if (v5_it == verts.end())
                {
                    v5_id = this->vert_add(v5_pos);
                    verts.insert(std::pair<cinolib::vec3d,uint> (v5_pos, v5_id));
                }
                else
                    v5_id = v5_it->second;

                if (v6_it == verts.end())
                {
                    v6_id = this->vert_add(v6_pos);
                    verts.insert(std::pair<cinolib::vec3d,uint> (v6_pos, v6_id));
                }
                else
                    v6_id = v6_it->second;

                if (v7_it == verts.end())
                {
                    v7_id = this->vert_add(v7_pos);
                    verts.insert(std::pair<cinolib::vec3d,uint> (v7_pos, v7_id));
                }
                else
                    v7_id = v7_it->second;


                std::vector<uint> verts;
                verts.push_back(v0_id);
                verts.push_back(v1_id);
                verts.push_back(v2_id);
                verts.push_back(v3_id);

                verts.push_back(v4_id);
                verts.push_back(v5_id);
                verts.push_back(v6_id);
                verts.push_back(v7_id);


                this->poly_add(verts);


            }
        }
    }

    std::cout << "Creation Hexmesh ... COMPLETED." << std::endl;

}



template<class M, class V, class E, class F, class P>
Hexmesh<M,V,E,F,P>::Hexmesh (const double &res_x, const double &res_y, const double &res_z, cinolib::Trimesh<> mesh)
{
    #ifdef MUSE_USES_LIBIGL

    using namespace Eigen;
    using namespace std;

    Eigen::MatrixXd VV   (mesh.num_verts(), 3); //vettore dei vertici
    Eigen::MatrixXi TT   (mesh.num_polys(), 3); //vettore delle facce

    for(uint vid=0; vid<mesh.num_verts(); vid++)
    {
        VV(vid, 0) = mesh.vert(vid).x();
        VV(vid, 1) = mesh.vert(vid).y();
        VV(vid, 2) = mesh.vert(vid).z();
    }

    for(uint pid=0; pid<mesh.num_polys(); pid++)
    {
        TT(pid, 0) = mesh.poly_vert_id(pid, 0);
        TT(pid, 1) = mesh.poly_vert_id(pid, 1);
        TT(pid, 2) = mesh.poly_vert_id(pid, 2);
    }

    // Definizione boundary 3d e calcolo della bounding box
    double min_x =  DBL_MAX;
    double min_y =  DBL_MAX;
    double min_z =  DBL_MAX;

    double max_x = -DBL_MAX;
    double max_y = -DBL_MAX;
    double max_z = -DBL_MAX;

    max_x = mesh.bbox().max.x();
    max_y = mesh.bbox().max.y();
    max_z = mesh.bbox().max.z();

    min_x = mesh.bbox().min.x();
    min_y = mesh.bbox().min.y();
    min_z = mesh.bbox().min.z();

    std::cout << "Computing bounding box ... COMPLETED. " << std::endl;

    // Calcolo delta
    double delta_x = max_x - min_x;
    double delta_y = max_y - min_y;
    double delta_z = max_z - min_z;

    // Calcolo numero di celle in base alla risoluzione in input
    uint npolys_x = static_cast<uint>(delta_x / res_x ); //ncelle su asse x
    uint npolys_y = static_cast<uint>(delta_y / res_y ); //ncelle su asse y
    uint npolys_z = static_cast<uint>(delta_z / res_z ); //ncelle su asse z

    std::cout << "n. polys - x " << npolys_x << std::endl;
    std::cout << "n. polys - y " << npolys_y << std::endl;
    std::cout << "n. polys - z " << npolys_z << std::endl;

    // Vertici cella
    cinolib::vec3d v0 (0.0, 0.0, 0.0);
    cinolib::vec3d v1 (res_x, 0.0, 0.0);
    cinolib::vec3d v2 (res_x, res_y, 0.0);
    cinolib::vec3d v3 (0.0, res_y, 0.0);

    cinolib::vec3d v4 (0.0, 0.0, res_z);
    cinolib::vec3d v5 (res_x, 0.0, res_z);
    cinolib::vec3d v6 (res_x, res_y, res_z);
    cinolib::vec3d v7 (0.0, res_y, res_z);

    // Centro cella
    cinolib::vec3d center (res_x/2, res_y/2, res_z/2);

    std::map<cinolib::vec3d, uint> verts;
    for (uint nz=0; nz < npolys_z; nz++)
    {
        double z = min_z + res_z * nz;

        Eigen::MatrixXd VC  (npolys_x * npolys_y, 3); //campioni
        Eigen::VectorXd W   (npolys_x * npolys_y);

        for (uint ny=0; ny < npolys_y; ny++)
        {
            double y = min_y + res_y * ny;

            for (uint nx=0; nx <npolys_x; nx++)
            {
                double x = min_x + res_x * nx;

                cinolib::vec3d v0_pos (v0);
                cinolib::vec3d v1_pos (v1);
                cinolib::vec3d v2_pos (v2);
                cinolib::vec3d v3_pos (v3);

                cinolib::vec3d v4_pos (v4);
                cinolib::vec3d v5_pos (v5);
                cinolib::vec3d v6_pos (v6);
                cinolib::vec3d v7_pos (v7);

                double dx = x - v0.x();
                double dy = y - v0.y();
                double dz = z - v0.z();
                cinolib::vec3d vdx (dx, dy, dz);

                v0_pos += vdx;
                v1_pos += vdx;
                v2_pos += vdx;
                v3_pos += vdx;

                v4_pos += vdx;
                v5_pos += vdx;
                v6_pos += vdx;
                v7_pos += vdx;

                cinolib::vec3d cp = center + vdx;

                VC(ny*npolys_x + nx, 0) = cp.x();
                VC(ny*npolys_x + nx, 1) = cp.y();
                VC(ny*npolys_x + nx, 2) = cp.z();
            }
        }

        igl::winding_number(VV,TT,VC,W);

        for (uint ny=0; ny < npolys_y; ny++)
        {
            for (uint nx=0; nx <npolys_x; nx++)
            {
                if (W[ny*npolys_x + nx] < 0.5) //il punto è fuori
                    continue;

                double y = min_y + res_y * ny;
                double x = min_x + res_x * nx;

                cinolib::vec3d v0_pos (v0);
                cinolib::vec3d v1_pos (v1);
                cinolib::vec3d v2_pos (v2);
                cinolib::vec3d v3_pos (v3);

                cinolib::vec3d v4_pos (v4);
                cinolib::vec3d v5_pos (v5);
                cinolib::vec3d v6_pos (v6);
                cinolib::vec3d v7_pos (v7);

                double dx = x - v0.x();
                double dy = y - v0.y();
                double dz = z - v0.z();
                cinolib::vec3d vdx (dx, dy, dz);

                v0_pos += vdx;
                v1_pos += vdx;
                v2_pos += vdx;
                v3_pos += vdx;

                v4_pos += vdx;
                v5_pos += vdx;
                v6_pos += vdx;
                v7_pos += vdx;

                // Definizione dell'iteratore
                auto v0_it = verts.find(v0_pos);
                auto v1_it = verts.find(v1_pos);
                auto v2_it = verts.find(v2_pos);
                auto v3_it = verts.find(v3_pos);

                auto v4_it = verts.find(v4_pos);
                auto v5_it = verts.find(v5_pos);
                auto v6_it = verts.find(v6_pos);
                auto v7_it = verts.find(v7_pos);

                // Definizione indici vertici
                uint v0_id = 0;
                uint v1_id = 0;
                uint v2_id = 0;
                uint v3_id = 0;

                uint v4_id = 0;
                uint v5_id = 0;
                uint v6_id = 0;
                uint v7_id = 0;

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

                if (v4_it == verts.end())
                {
                    v4_id = this->vert_add(v4_pos);
                    verts.insert(std::pair<cinolib::vec3d,uint> (v4_pos, v4_id));
                }
                else
                    v4_id = v4_it->second;

                if (v5_it == verts.end())
                {
                    v5_id = this->vert_add(v5_pos);
                    verts.insert(std::pair<cinolib::vec3d,uint> (v5_pos, v5_id));
                }
                else
                    v5_id = v5_it->second;

                if (v6_it == verts.end())
                {
                    v6_id = this->vert_add(v6_pos);
                    verts.insert(std::pair<cinolib::vec3d,uint> (v6_pos, v6_id));
                }
                else
                    v6_id = v6_it->second;

                if (v7_it == verts.end())
                {
                    v7_id = this->vert_add(v7_pos);
                    verts.insert(std::pair<cinolib::vec3d,uint> (v7_pos, v7_id));
                }
                else
                    v7_id = v7_it->second;


                std::vector<uint> verts;
                verts.push_back(v0_id);
                verts.push_back(v1_id);
                verts.push_back(v2_id);
                verts.push_back(v3_id);

                verts.push_back(v4_id);
                verts.push_back(v5_id);
                verts.push_back(v6_id);
                verts.push_back(v7_id);


                this->poly_add(verts);


            }
        }

    }

    std::cout << "Creation Hexmesh ... COMPLETED." << std::endl;


#else

        std::cerr << "LIBIGL is required. Please include the library and use MUSE_USES_LIBIGL." << std::endl;

#endif

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<class M, class V, class E, class F, class P>
void Hexmesh<M,V,E,F,P>::subHexmesh_from_trimesh (MUSE::Hexmesh<> hexmesh, cinolib::Trimesh<> mesh_bound)
{
    #ifdef MUSE_USES_LIBIGL

    using namespace Eigen;
    using namespace std;

    Eigen::MatrixXd VV   (mesh_bound.num_verts(), 3); //vettore dei vertici
    Eigen::MatrixXi TT   (mesh_bound.num_polys(), 3); //vettore delle facce

    for(uint vid=0; vid<mesh_bound.num_verts(); vid++)
    {
        VV(vid, 0) = mesh_bound.vert(vid).x();
        VV(vid, 1) = mesh_bound.vert(vid).y();
        VV(vid, 2) = mesh_bound.vert(vid).z();
    }

    for(uint pid=0; pid<mesh_bound.num_polys(); pid++)
    {
        TT(pid, 0) = mesh_bound.poly_vert_id(pid, 0);
        TT(pid, 1) = mesh_bound.poly_vert_id(pid, 1);
        TT(pid, 2) = mesh_bound.poly_vert_id(pid, 2);
    }

    std::map<cinolib::vec3d, uint> verts;

    Eigen::MatrixXd VC  (hexmesh.num_polys(), 3); //campioni
    Eigen::VectorXd W   (hexmesh.num_polys());

    for(uint pid=0; pid <hexmesh.num_polys(); pid++)
    {
        cinolib::vec3d centr = hexmesh.poly_centroid(pid);

        VC(pid, 0) = centr.x();
        VC(pid, 1) = centr.y();
        VC(pid, 2) = centr.z();
    }

    igl::winding_number(VV,TT,VC,W);

    for(uint pid=0; pid <hexmesh.num_polys(); pid++)
    {
        if (W[pid] < 0.5) //il punto è fuori
            continue;

        cinolib::vec3d v0_pos = hexmesh.poly_vert(pid, 0);
        cinolib::vec3d v1_pos = hexmesh.poly_vert(pid, 1);
        cinolib::vec3d v2_pos = hexmesh.poly_vert(pid, 2);
        cinolib::vec3d v3_pos = hexmesh.poly_vert(pid, 3);

        cinolib::vec3d v4_pos = hexmesh.poly_vert(pid, 4);
        cinolib::vec3d v5_pos = hexmesh.poly_vert(pid, 5);
        cinolib::vec3d v6_pos = hexmesh.poly_vert(pid, 6);
        cinolib::vec3d v7_pos = hexmesh.poly_vert(pid, 7);

        // Definizione dell'iteratore
        auto v0_it = verts.find(v0_pos);
        auto v1_it = verts.find(v1_pos);
        auto v2_it = verts.find(v2_pos);
        auto v3_it = verts.find(v3_pos);

        auto v4_it = verts.find(v4_pos);
        auto v5_it = verts.find(v5_pos);
        auto v6_it = verts.find(v6_pos);
        auto v7_it = verts.find(v7_pos);

        // Definizione indici vertici
        uint v0_id = 0;
        uint v1_id = 0;
        uint v2_id = 0;
        uint v3_id = 0;

        uint v4_id = 0;
        uint v5_id = 0;
        uint v6_id = 0;
        uint v7_id = 0;

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

        if (v4_it == verts.end())
        {
            v4_id = this->vert_add(v4_pos);
            verts.insert(std::pair<cinolib::vec3d,uint> (v4_pos, v4_id));
        }
        else
            v4_id = v4_it->second;

        if (v5_it == verts.end())
        {
            v5_id = this->vert_add(v5_pos);
            verts.insert(std::pair<cinolib::vec3d,uint> (v5_pos, v5_id));
        }
        else
            v5_id = v5_it->second;

        if (v6_it == verts.end())
        {
            v6_id = this->vert_add(v6_pos);
            verts.insert(std::pair<cinolib::vec3d,uint> (v6_pos, v6_id));
        }
        else
            v6_id = v6_it->second;

        if (v7_it == verts.end())
        {
            v7_id = this->vert_add(v7_pos);
            verts.insert(std::pair<cinolib::vec3d,uint> (v7_pos, v7_id));
        }
        else
            v7_id = v7_it->second;

        std::vector<uint> vlist;
        vlist.push_back(v0_id);
        vlist.push_back(v1_id);
        vlist.push_back(v2_id);
        vlist.push_back(v3_id);

        vlist.push_back(v4_id);
        vlist.push_back(v5_id);
        vlist.push_back(v6_id);
        vlist.push_back(v7_id);

        this->poly_add(vlist);

    }

#else

std::cerr << "LIBIGL is required. Please include the library and use MUSE_USES_LIBIGL." << std::endl;

#endif
}

}



