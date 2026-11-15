#include "mesh.h"
#include "tools.h"

#include <vector>
#include <float.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>

#include <concaveman.h>
#include <unordered_set>
#include <algorithm>

#include "cinolib/octree.h"
#include <cinolib/geometry/plane.h>

using namespace MUSE;

std::vector<double> serialized_from_point2d (const std::vector<Point2D> &points)
{
    std::vector<double> serialized_points;
    for (size_t i=0; i < points.size(); i++)
    {
        //serializzazione dei punti da Point3D a xy
        serialized_points.push_back(points.at(i).x);
        serialized_points.push_back(points.at(i).y);
    }
    return serialized_points;
}

std::vector<double> serialized_from_point3d (const std::vector<Point3D> &points)
{
    std::vector<double> serialized_points;
    for (size_t i=0; i < points.size(); i++)
    {
        //serializzazione dei punti da Point3D a xy
        serialized_points.push_back(points.at(i).x);
        serialized_points.push_back(points.at(i).y);
    }
    return serialized_points;
}



std::vector<Point3D> remove_sorted_duplicates (const std::vector<Point3D> &points)
{
    std::vector<Point3D> sorted_points = points;
    std::vector<Point3D> unique_points;

    std::sort(sorted_points.begin(), sorted_points.end(), comparePoint);

    unique_points.push_back(sorted_points.at(0));
    for (uint i=1; i < sorted_points.size(); i++)
    {
        Point2D p0, p1;
        p0.x = sorted_points.at(i-1).x;
        p0.y = sorted_points.at(i-1).y;
        p1.x = sorted_points.at(i).x;
        p1.y = sorted_points.at(i).y;

        if (dist(p0, p1) > 1e-6)
            unique_points.push_back(sorted_points.at(i));
    }
    std::cout << "Removing duplicated points ... COMPLETED. " << unique_points.size() << " left" << std::endl;
    return unique_points;
}


void remove_duplicates_test_opt(const std::vector<Point3D> &points, std::vector<Point3D> &unique_points, const double &tol)
{
    std::cout << "### Starting vector dimension: " << points.size() << std::endl;  
    std::vector<Point3D> sorted_points = points;

    for(uint i=0; i<sorted_points.size(); i++)
        sorted_points.at(i).index = i;

    unique_points = sorted_points;
    std::deque<int> id_dupl;

    std::sort(sorted_points.begin(), sorted_points.end(), comparePoint);

    for (uint i=1; i < sorted_points.size(); i++)
    {
        if (dist3D(sorted_points.at(i-1), sorted_points.at(i)) <= 1e-6)
        {
            id_dupl.push_back(sorted_points.at(i).index);
            std::cout << "### Duplicated point at index: " << sorted_points.at(i).index << std::endl;
            //std::cout << "### Duplicated point: " << sorted_points.at(i).x << "; " << sorted_points.at(i).y << "; " << sorted_points.at(i).z << std::endl;
        }
    }
    sorted_points.clear();
    std::cout << std::endl;

    std::sort(id_dupl.begin(), id_dupl.end(), std::greater<>());
    //std::reverse(id_dupl.begin(), id_dupl.end());

    while (!id_dupl.empty())
    {
        //int curr = (points.size()-1) - id_dupl.front();
        int curr = id_dupl.front();
        std::cout << "### Removing point ID ---> " << curr << std::endl;
        //std::cout << "### Removing point ---> " << unique_points.at(unique_points.begin()+curr). << "; " << unique_points.data()->y << "; " << unique_points.data()->z << "; " << unique_points.data()->index << std::endl;

        id_dupl.pop_front();
        unique_points.erase(unique_points.begin()+curr);
    }

    std::cout << std::endl;
    std::cout << "Removing " << points.size() - unique_points.size() << " duplicated points ... COMPLETED." << std::endl;

}


//Funzione per la rimozione dei vertici isolati
void remove_isolate_vertices (cinolib::Trimesh<> &mesh)
{
    std::vector<uint> tbrem;
    for (uint vid=0; vid < mesh.num_verts(); vid++)
    {
        if (mesh.adj_v2v(vid).size() == 0)
            tbrem.push_back(vid);
    }
    if (tbrem.size() > 0)
    {
        std::reverse(tbrem.begin(), tbrem.end());
        for (uint vid : tbrem)
            mesh.vert_remove_unreferenced(vid);
    }
    std::cout << "Removing " << tbrem.size() << " Isolated Vertices ... COMPLETED" << std::endl;
}


void mesh_summary (cinolib::Trimesh<> &mesh)
{
    std::cout << std::endl;
    std::cout << "Mesh summary ..." <<std::endl;
    std::cout << mesh.num_verts() << "V| " << mesh.num_polys() << "P" << std::endl;
    std::cout << std::endl;

    std::cout << "Edge min lenght = " << mesh.edge_min_length() << std::endl;
    std::cout << "Edge max lenght = " << mesh.edge_max_length() << std::endl;
    std::cout << "Edge average lenght = " << mesh.edge_avg_length() << std::endl;
    std::cout << std::endl;

    //std::cout << "Mean triangles area = " << poly_avg_area(mesh) << std::endl;

    std::cout << "Mesh summary ... COMPLETED." <<std::endl;
    std::cout << std::endl;
}

void rotation_on_trimesh (cinolib::Trimesh<> &trimesh, const MUSE::Rotation &bound_Rotation, bool reverse)
{
    // get_rotation_axis sceglie rotation_axis_vec (rotazioni auto-allineate) o
    // set_rotation_axis(rotation_axis) (rotazioni manuali --rotaxis): usare solo
    // rotation_axis_vec darebbe asse nullo per le rotazioni manuali.
    cinolib::vec3d rot_axis_vec = get_rotation_axis(bound_Rotation);
    cinolib::vec3d c (bound_Rotation.rotation_center_x, bound_Rotation.rotation_center_y, bound_Rotation.rotation_center_z);
 
    double sign = 1.0;
    if(reverse)
        sign = -1.0;
 
    for(size_t i=0; i< trimesh.num_verts(); i++)
    {
        cinolib::vec3d sample (trimesh.vert(i).x(), trimesh.vert(i).y(), trimesh.vert(i).z());
        sample = point_rotation(sample, rot_axis_vec, sign * bound_Rotation.rotation_angle, c);
 
        trimesh.vert(i).x() = sample.x();
        trimesh.vert(i).y() = sample.y();
        trimesh.vert(i).z() = sample.z();
    }
}

void rotation_on_trimesh (cinolib::Polygonmesh<> &mesh, const MUSE::Rotation &bound_Rotation, bool reverse)
{
    // same as the Trimesh version, for general polygon meshes
    cinolib::vec3d rot_axis_vec = get_rotation_axis(bound_Rotation);
    cinolib::vec3d c (bound_Rotation.rotation_center_x, bound_Rotation.rotation_center_y, bound_Rotation.rotation_center_z);

    double sign = 1.0;
    if(reverse)
        sign = -1.0;

    for(size_t i=0; i< mesh.num_verts(); i++)
    {
        cinolib::vec3d sample (mesh.vert(i).x(), mesh.vert(i).y(), mesh.vert(i).z());
        sample = point_rotation(sample, rot_axis_vec, sign * bound_Rotation.rotation_angle, c);

        mesh.vert(i).x() = sample.x();
        mesh.vert(i).y() = sample.y();
        mesh.vert(i).z() = sample.z();
    }
}

MUSE::Rotation rotation_to_xyplane (const std::vector<cinolib::vec3d> &verts)
{
    // align_points_to_xyplane works on copies of the coordinates: only the rotation is kept
    std::vector<double> x, y, z;
    for(const cinolib::vec3d &v : verts)
    {
        x.push_back(v.x());
        y.push_back(v.y());
        z.push_back(v.z());
    }

    MUSE::Rotation rotation;
    align_points_to_xyplane(x, y, z, rotation);
    return rotation;
}

MUSE::Rotation rotation_to_zaxis (const cinolib::vec3d &dir, const cinolib::vec3d &center)
{
    // same encoding as align_points_to_xyplane (autoalign: axis in rotation_axis_vec, angle in degree),
    // so that rotation_on_trimesh can apply it and undo it
    MUSE::Rotation rotation;

    cinolib::vec3d u = dir;
    u.normalize();
    cinolib::vec3d z (0.0, 0.0, 1.0);

    double dot = std::min(1.0, std::max(-1.0, u.dot(z)));
    double angle_deg = std::acos(dot) * 180.0 / M_PI;
    if(angle_deg < 1e-6)
        return rotation; //already along +Z: no rotation

    cinolib::vec3d axis = u.cross(z);
    if(axis.norm() < 1e-10) //anti-parallel to Z
        axis = cinolib::vec3d(1.0, 0.0, 0.0);
    else
        axis.normalize();

    rotation.autoalign = true;
    rotation.rotation  = true;
    rotation.rotation_axis_vec = { axis.x(), axis.y(), axis.z() };
    rotation.rotation_angle    = angle_deg;
    rotation.rotation_center_x = center.x();
    rotation.rotation_center_y = center.y();
    rotation.rotation_center_z = center.z();
    rotation.normal_vec_original = { u.x(), u.y(), u.z() };
    rotation.normal_vec_rotated  = { 0.0, 0.0, 1.0 };

    return rotation;
}

cinolib::vec3d best_fit_normal (const std::vector<cinolib::vec3d> &verts)
{
    cinolib::Plane plane(verts);
    cinolib::vec3d n = plane.n;
    n.normalize();
    return n;
}

//Routine for points triangulation
cinolib::Trimesh<> points_triangulation (const std::vector<Point3D> &points, std::string opt)
{
    std::vector<double> serialized_points = serialized_from_point3d(points);

    // Run triangulation by exploting Triangle Library in Cinolib and create a triangle mesh (m_tri)
    cinolib::Trimesh<> trimesh;
    triangle_wrap(serialized_points, {}, {}, 0, opt, trimesh);

    // Vertices in the generated mesh have Z=0
    // Restore original Z coordinate
    for (uint vid=0; vid < points.size(); vid++)
        trimesh.vert(vid).z() = points[vid].z;

    //Clean generated mesh by removing possible isolated vertices
    remove_isolate_vertices(trimesh);

    std::cout << "Running PointCloud Triangulation ... COMPLETED." << std::endl;

    mesh_summary(trimesh);

    return trimesh;
}




cinolib::Trimesh<> boundary_triangulation (const std::vector<Point3D> &boundaries, std::string opt)
{
    std::vector<double> triangle_points_in;
    std::vector<double> triangle_holes_in;
    std::vector<unsigned int> segs_in;


    //ciclo sul bordo

    size_t counter = 0;

    //std::vector<bool> duplicates (boundaries.size(), false);

    Point2D last;

    int first_id = -1, last_id = -1 ;

    for (const Point3D &p : boundaries)
    {
        bool far = true;

        if (counter > 0)
        {
            Point2D cp, cl;
            cp.x = p.x;
            cp.y = p.y;

            cl.x = last.x;
            cl.y = last.y;

            if (dist(cp, cl) < 1e-6) //se la distanza è piccola, ci sono dei punti duplicati sul bordo
            {
                std::cout << "\033[0;33mWARNING: Duplicated on boundary - IGNORED\033[0m" << std::endl;
                far = false; //flag lontani = falso!
                //duplicates.at(counter) = true;
            }
        }

//        if (!far) //se far = vero -> se i punti sono lontani, continua
//            continue;

        //aggiungere punti di bordo alla lista serializzata dei punti
        triangle_points_in.push_back(p.x);
        triangle_points_in.push_back(p.y);

        last.x = p.x;
        last.y = p.y;

        //posizione ultimo punto:
        unsigned long id = triangle_points_in.size()/2 -1;

        //aggiorno indici di first e last point
        if (first_id == -1)
            first_id = id;

        last_id = id;

        if (counter > 0)
        {
            segs_in.push_back(id-1);
            segs_in.push_back(id);
        }
        counter++;
    }
    segs_in.push_back(last_id);
    segs_in.push_back(first_id);

    //std::vector<double> triangle_points_out;
    //std::vector<unsigned int> triangle_tris_out;

    //cinolib::triangle_wrap(triangle_points_in, segs_in, triangle_holes_in, "", triangle_points_out, triangle_tris_out);

//    std::vector<double> triangle_points_out_coords;

//    unsigned int vid=0;
//    for(unsigned int i=0; i<triangle_points_out.size(); i+=2)
//    {
//        //deserializzo
//        triangle_points_out_coords.push_back(triangle_points_out.at(i));
//        triangle_points_out_coords.push_back(triangle_points_out.at(i+1));
//        triangle_points_out_coords.push_back(boundaries.at(vid).z);
//        vid++;
//    }

//    cinolib::Trimesh<> trimesh (triangle_points_out_coords, triangle_tris_out);


    cinolib::Trimesh<> trimesh;
    cinolib::triangle_wrap(triangle_points_in, segs_in, triangle_holes_in, 0, opt, trimesh);

    for (uint vid=0; vid < boundaries.size(); vid++)
        trimesh.vert(vid).z() = boundaries[vid].z;

//    for (uint vid=0; vid < trimesh.num_verts(); vid++)
//    {
//        if(duplicates.at(vid) == false)
//            trimesh.vert(vid).z() = boundaries[vid].z;
//    }

    std::cout << "Constrained Delaunay Triangulation ... COMPLETED." << std::endl;

    return trimesh;

}


cinolib::Trimesh<> constrained_triangulation (const std::vector<Point3D> &boundary3d, const std::vector<Point3D> &points, std::string opt)
{
    std::vector<double> triangle_points_in;
    std::vector<double> triangle_holes_in;
    std::vector<unsigned int> segs_in;


    std::vector<Point2D> boundary;
    for (size_t i=0; i < boundary3d.size(); i++)
    {
        Point2D pp;
        pp.x = boundary3d.at(i).x;
        pp.y = boundary3d.at(i).y;

        boundary.push_back(pp);
    }


    for (size_t i=0; i < points.size(); i++)
    {
        //serializzazione dei punti da Point3D a xy
        triangle_points_in.push_back(points.at(i).x);
        triangle_points_in.push_back(points.at(i).y);

        Point2D pp;
        pp.x = points.at(i).x;
        pp.y = points.at(i).y;

        //controllo se il punto pp è interno al poligono
        if (!point_in_polygon(pp, boundary)) //se non è interno, continua
            continue;
    }

    //ciclo sul bordo

    size_t counter = 0;

    Point2D last;

    int first_id = -1, last_id = -1 ;

    for (const Point2D &p : boundary)
    {
        bool far = true;

        if (counter > 0)
        {
            Point2D cp, cl;
            cp.x = p.x;
            cp.y = p.y;

            cl.x = last.x;
            cl.y = last.y;

            if (dist(cp, cl) < 1e-2) //se la distanza è piccola, ci sono dei punti duplicati sul bordo
            {
                std::cout << "\033[0;33mWARNING: Duplicated on boundary - IGNORED\033[0m" << std::endl;
                far = false; //flag lontani = falso!
            }
        }

        if (!far) //se far = vero -> se i punti sono lontani, continua
            continue;

        //aggiungere punti di bordo alla lista serializzata dei punti
        triangle_points_in.push_back(p.x);
        triangle_points_in.push_back(p.y);

        last.x = p.x;
        last.y = p.y;

        //posizione ultimo punto:
        unsigned long id = triangle_points_in.size()/2 -1;

        //aggiorno indici di first e last point
        if (first_id == -1)
            first_id = id;

        last_id = id;

        if (counter > 0)
        {
            segs_in.push_back(id-1);
            segs_in.push_back(id);
        }
        counter++;
    }
    segs_in.push_back(last_id);
    segs_in.push_back(first_id);


    std::vector<double> triangle_points_out;
    std::vector<unsigned int> triangle_tris_out;

    cinolib::triangle_wrap(triangle_points_in, segs_in, triangle_holes_in, "", triangle_points_out, triangle_tris_out);

    std::cout << "Constrained Delaunay ... COMPLETED" << std::endl;

    std::vector<double> triangle_points_out_coords;

    unsigned int vid=0, vid_b =0;
    for (unsigned int i=0; i < triangle_points_out.size(); i+=2)
    {
        triangle_points_out_coords.push_back(triangle_points_out.at(i));
        triangle_points_out_coords.push_back(triangle_points_out.at(i+1));

        if (vid < points.size())
            triangle_points_out_coords.push_back(points.at(vid).z);

        else
        {
            triangle_points_out_coords.push_back(boundary3d.at(vid_b).z);
            vid_b++;
        }

        vid++;
    }

    cinolib::Trimesh<> trimesh (triangle_points_out_coords, triangle_tris_out);

    std::cout << "Constrained Delaunay Triangulation on boundary ... COMPLETED." << std::endl;

    return trimesh;

}



std::vector<Point3D> computing_concavehull (const std::vector<Point3D> &points, std::vector<int> &convexhull, std::vector<int> &b_id)
{
    // 1. Creo il vettore di punti: data_points nel template richiesto
    typedef double T;
    typedef std::array<T, 2> point_type;
    std::vector<point_type> data_points;
    for(size_t i=0; i<points.size(); i++)
    {
        point_type p {points.at(i).x, points.at(i).y};
        data_points.push_back(p);
    }

    // 2. Calcolare il concave hull con concaveman_m.h (versione modificata per compilazione)
    std::vector<Point2D> concavehull;
    auto concave = concaveman<T, 16>(data_points, convexhull, 2, 1);
    for (auto &p : concave)
    {
        Point2D ch;
        ch.x = p[0];
        ch.y = p[1];
        concavehull.push_back(ch);
    }

    // 3. Estrazione della coordinata z dei punti del concave hull
    std::vector<Point3D> concavehull3d;
    for(size_t i=0; i<concavehull.size(); i++)
    {
        Point2D point_ch = concavehull.at(i);

        Point3D p3d;
        for(size_t j =0; j< points.size(); j++)
        {
            Point2D p;
            p.x = points.at(j).x;
            p.y = points.at(j).y;

            if(dist(point_ch, p) < 1e-2)
            {
                p3d.x = point_ch.x;
                p3d.y = point_ch.y;
                p3d.z = points.at(j).z;

                b_id.push_back(j); //vettore degli indici dei vertici uguali a quelli del concave hull -> punti da rimuovere da data
                //std::cout << "i punti sono uguali" << std::endl;
                break;
            }
        }
        concavehull3d.push_back(p3d);
    }

    std::cout << "Computing concave hull ... COMPLETED." << std::endl;

    return concavehull3d;
}






cinolib::Trimesh<> concavehull_triangulation (const std::vector<Point3D> &concavehull3d, const std::vector<Point3D> &points, const std::string &opt, const double &tol)
{
    std::vector<double> triangle_points_in;
    std::vector<double> triangle_holes_in;
    std::vector<unsigned int> segs_in;


    std::vector<Point2D> concavehull;
    for (size_t i=0; i < concavehull3d.size(); i++)
    {
        Point2D pp;
        pp.x = concavehull3d.at(i).x;
        pp.y = concavehull3d.at(i).y;
        concavehull.push_back(pp);
    }


    for (size_t i=0; i < points.size(); i++)
    {
        //serializzazione dei punti da Point3D a xy
        triangle_points_in.push_back(points.at(i).x);
        triangle_points_in.push_back(points.at(i).y);

        Point2D pp;
        pp.x = points.at(i).x;
        pp.y = points.at(i).y;
        //controllo se il punto pp è interno al poligono
        if (!point_in_polygon(pp, concavehull)) //se non è interno, continua
            continue;
    }

    //ciclo sul bordo

    size_t counter = 0;

    Point2D last;

    int first_id = -1, last_id = -1 ;

    for (const Point2D &p : concavehull)
    {
        bool far = true;

        if (counter > 0)
        {
            Point2D cp, cl;
            cp.x = p.x;
            cp.y = p.y;

            cl.x = last.x;
            cl.y = last.y;

            if (dist(cp, cl) < tol) //se la distanza è piccola, ci sono dei punti duplicati sul bordo
            {
                std::cout << "\033[0;33mWARNING: Duplicated on boundary - IGNORED\033[0m" << std::endl;
                far = false; //flag lontani = falso!
            }
        }

//        if (!far) //se far = vero -> se i punti sono lontani, continua
//            continue;

        //aggiungere punti di bordo alla lista serializzata dei punti
        triangle_points_in.push_back(p.x);
        triangle_points_in.push_back(p.y);

        last.x = p.x;
        last.y = p.y;

        //posizione ultimo punto:
        unsigned long id = triangle_points_in.size()/2 -1;

        //aggiorno indici di first e last point
        if (first_id == -1)
            first_id = id;

        last_id = id;

        if (counter > 0)
        {
            segs_in.push_back(id-1);
            segs_in.push_back(id);
        }
        counter++;
    }
    segs_in.push_back(last_id);
    segs_in.push_back(first_id);


    std::vector<double> triangle_points_out;
    std::vector<unsigned int> triangle_tris_out;

    cinolib::triangle_wrap(triangle_points_in, segs_in, triangle_holes_in, opt, triangle_points_out, triangle_tris_out);

    std::cout << "Constrained Delaunay ... COMPLETED" << std::endl;

    std::vector<double> triangle_points_out_coords;

    unsigned int vid=0, vid_b=0;
    for (unsigned int i=0; i < triangle_points_out.size(); i+=2)
    {
        triangle_points_out_coords.push_back(triangle_points_out.at(i));
        triangle_points_out_coords.push_back(triangle_points_out.at(i+1));

        if (vid < points.size())
        {
            triangle_points_out_coords.push_back(points.at(vid).z);
        }
        else
        {
            if(vid < points.size()+concavehull3d.size())
            {
                //triangle_points_out_coords.push_back(concavehull3d.at(vid-points.size()).z);
                triangle_points_out_coords.push_back(concavehull3d.at(vid_b).z);
                vid_b++;
            }
            else
                triangle_points_out_coords.push_back(0.0);
        }
        vid++;
    }

    cinolib::Trimesh<> trimesh (triangle_points_out_coords, triangle_tris_out);

    std::cout << "Constrained Delaunay Triangulation - Concave hull ... COMPLETED." << std::endl;

    return trimesh;

}




uint interpolate_unknown_z (cinolib::Trimesh<> &trimesh, const std::vector<bool> &known, bool restrict_boundary)
{
    const uint nv = trimesh.num_verts();
    assert(known.size() == nv);

    // Neighbours (and weights) used in the equation of each unknown vertex
    std::vector<std::vector<std::pair<uint,double>>> nbrs(nv);
    std::vector<uint> unknown;
    for (uint vid=0; vid < nv; vid++)
    {
        if (known.at(vid))
            continue;
        unknown.push_back(vid);

        const bool only_boundary = restrict_boundary && trimesh.vert_is_boundary(vid);
        for (uint eid : trimesh.adj_v2e(vid))
        {
            if (only_boundary && !trimesh.edge_is_boundary(eid))
                continue;
            const uint nid = trimesh.vert_opposite_to(eid, vid);
            const cinolib::vec3d &p = trimesh.vert(vid), &q = trimesh.vert(nid);
            const double len = std::hypot(p.x()-q.x(), p.y()-q.y());
            nbrs.at(vid).emplace_back(nid, 1.0/std::max(len, 1e-9));
        }
    }
    if (unknown.empty())
        return 0;

    // 1. Reachability: an unknown vertex can be solved only if it depends (through its equation) on a known vertex
    std::vector<bool> reached = known;
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (uint vid : unknown)
        {
            if (reached.at(vid))
                continue;
            for (const auto &nw : nbrs.at(vid))
                if (reached.at(nw.first)) { reached.at(vid) = true; changed = true; break; }
        }
    }

    std::vector<int> col(nv, -1);
    std::vector<uint> active;
    uint n_unreached = 0;
    for (uint vid : unknown)
    {
        if (reached.at(vid)) { col.at(vid) = static_cast<int>(active.size()); active.push_back(vid); }
        else                 n_unreached++;
    }
    if (active.empty())
        return n_unreached;

    // 2. Harmonic system: sum_n w_vn (z_v - z_n) = 0, known vertices act as Dirichlet conditions
    std::vector<Eigen::Triplet<double>> entries;
    Eigen::VectorXd rhs = Eigen::VectorXd::Zero(active.size());
    for (size_t r=0; r < active.size(); r++)
    {
        const uint vid = active.at(r);
        double wsum = 0.0;
        for (const auto &nw : nbrs.at(vid))
        {
            if (!reached.at(nw.first))
                continue;
            wsum += nw.second;
            if (known.at(nw.first)) rhs(r) += nw.second * trimesh.vert(nw.first).z();
            else                    entries.emplace_back(r, col.at(nw.first), -nw.second);
        }
        entries.emplace_back(r, r, wsum);
    }
    Eigen::SparseMatrix<double> A(active.size(), active.size());
    A.setFromTriplets(entries.begin(), entries.end());

    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
    solver.compute(A);
    Eigen::VectorXd z;
    if (solver.info() == Eigen::Success)
        z = solver.solve(rhs);
    if (solver.info() != Eigen::Success)
    {
        std::cerr << "\033[0;31mERROR: z interpolation failed (sparse solver) - z left unchanged.\033[0m" << std::endl;
        return static_cast<uint>(unknown.size());
    }

    for (size_t r=0; r < active.size(); r++)
        trimesh.vert(active.at(r)).z() = z(r);

    return n_unreached;
}


cinolib::Trimesh<> constrained_triangulation2 (const std::vector<Point3D> &boundary3d, const std::vector<Point3D> &points, std::string opt, BoundaryZ boundary_z)
{
    std::vector<double> triangle_points_in;
    std::vector<double> triangle_holes_in;
    std::vector<unsigned int> segs_in;


    std::vector<Point2D> boundary;
    for (size_t i=0; i < boundary3d.size(); i++)
    {
        Point2D pp;
        pp.x = boundary3d.at(i).x;
        pp.y = boundary3d.at(i).y;

        boundary.push_back(pp);
    }
    std::cout << "### Number of boundary points: " << boundary.size() << std::endl;

    // for (size_t i=0; i < points.size(); i++)
    // {
    //     //serializzazione dei punti da Point3D a xy
    //     triangle_points_in.push_back(points.at(i).x);
    //     triangle_points_in.push_back(points.at(i).y);

    //     Point2D pp;
    //     pp.x = points.at(i).x;
    //     pp.y = points.at(i).y;

    //     //controllo se il punto pp è interno al poligono
    //     if (!point_in_polygon(pp, boundary)) //se non è interno, continua
    //         continue;
    // }

    std::vector<uint> id_inpoints, id_boundary;
    for (size_t i=0; i < points.size(); i++)
    {
        Point2D pp;
        pp.x = points.at(i).x;
        pp.y = points.at(i).y;

        //controllo se il punto pp è interno al poligono
        if (point_in_polygon(pp, boundary)) //se è interno
        {
            //serializzazione dei punti da Point3D a xy
            triangle_points_in.push_back(pp.x);
            triangle_points_in.push_back(pp.y);

            id_inpoints.push_back(i); //indice nel vettore points
        }
    }
    std::cout << "### Number of points: " << points.size() << std::endl;
    std::cout << "### Number of internal points (in boundary): " << triangle_points_in.size() << " | " << triangle_points_in.size()/2 << std::endl;

    //ciclo sul bordo

    size_t counter = 0;

    Point2D last;

    int first_id = -1, last_id = -1 ;

    int counter_dupl = 0;
    //for (const Point2D &p : boundary)
    for (size_t i=0; i < boundary.size(); i++)
    {
        Point2D p;
        p.x = boundary.at(i).x;
        p.y = boundary.at(i).y;

        bool far = true;

        if (counter > 0)
        {
            Point2D cp, cl;
            cp.x = p.x;
            cp.y = p.y;

            cl.x = last.x;
            cl.y = last.y;

            if (dist(cp, cl) < 1e-2) //se la distanza è piccola, ci sono dei punti duplicati sul bordo
            {
                counter_dupl++;
                std::cout << "\033[0;33mWARNING: Duplicated on boundary - IGNORED\033[0m" << std::endl;
                far = false; //flag lontani = falso!
            }
        }

        if (!far) //se far = vero -> se i punti sono lontani, continua
            continue;

        //aggiungere punti di bordo alla lista serializzata dei punti
        triangle_points_in.push_back(p.x);
        triangle_points_in.push_back(p.y);
        id_boundary.push_back(i); //indice nel vettore boundary

        last.x = p.x;
        last.y = p.y;

        //posizione ultimo punto:
        unsigned long id = triangle_points_in.size()/2 -1;

        //aggiorno indici di first e last point
        if (first_id == -1)
            first_id = id;

        last_id = id;

        if (counter > 0)
        {
            segs_in.push_back(id-1);
            segs_in.push_back(id);
        }
        counter++;
    }
    segs_in.push_back(last_id);
    segs_in.push_back(first_id);

    std::cout << "### Number of duplicates: " << counter_dupl << std::endl;
    std::cout << "### Number of internal points (in boundary) - adding boundary points (excluding duplicates): " << triangle_points_in.size() << " | " << triangle_points_in.size()/2 << std::endl;

    std::cout << "### Number of boundary segments: " << segs_in.size() << std::endl;

    std::cout << "### Size - ID points: " << id_inpoints.size() << std::endl;
    std::cout << "### Size - ID boundary points: " << id_boundary.size() << std::endl;

    std::vector<double> triangle_points_out;
    std::vector<unsigned int> triangle_tris_out;

    cinolib::triangle_wrap(triangle_points_in, segs_in, triangle_holes_in, opt, triangle_points_out, triangle_tris_out);

    std::cout << "Constrained Delaunay triangulation on boundary ... COMPLETED." << std::endl;

    std::vector<double> triangle_points_out_coords;


    // for (unsigned int i=0; i < triangle_points_out.size(); i+=2)
    // {
    //     triangle_points_out_coords.push_back(triangle_points_out.at(i));
    //     triangle_points_out_coords.push_back(triangle_points_out.at(i+1));

    //     if (vid < points.size())
    //         triangle_points_out_coords.push_back(points.at(vid).z);

    //     else
    //     {
    //         if(vid < points.size()+boundary3d.size())
    //         {
    //             triangle_points_out_coords.push_back(boundary3d.at(vid_b).z);
    //             vid_b++;
    //         }
    //         else
    //             triangle_points_out_coords.push_back(0.0);
    //     }

    //     vid++;
    // }

    uint vid=0, vid_b=0, counter_null=0;
    for (uint i=0; i < triangle_points_out.size(); i+=2)
    {
        triangle_points_out_coords.push_back(triangle_points_out.at(i));
        triangle_points_out_coords.push_back(triangle_points_out.at(i+1));

        //std::cout << "vid = " << vid << std::endl;
        if (vid < id_inpoints.size())
            triangle_points_out_coords.push_back(points.at(id_inpoints.at(vid)).z);
        else
        {
            if(vid < id_inpoints.size()+id_boundary.size())
            {
                triangle_points_out_coords.push_back(boundary3d.at(id_boundary.at(vid_b)).z);
                vid_b++;
            }
            else
            {
                triangle_points_out_coords.push_back(0.0); //placeholder, interpolated below
                counter_null++;
            }
        }
        vid++;
    }
    std::cout << "### Number of additional points (by Triangle): " << counter_null << std::endl;

    cinolib::Trimesh<> trimesh (triangle_points_out_coords, triangle_tris_out);
    std::cout << "Creation trimesh data structure ... COMPLETED." << std::endl;

    // Vertex order in Triangle output: internal data points, boundary points, additional (Steiner) points
    const size_t n_data = id_inpoints.size();
    const size_t n_data_boundary = n_data + id_boundary.size();
    const uint nv = trimesh.num_verts();

    // Original z values of the boundary vertices
    std::vector<double> z_boundary;
    for (size_t v=n_data; v < n_data_boundary; v++)
        z_boundary.push_back(trimesh.vert(v).z());

    bool project = false;
    std::vector<bool> known (nv, false);
    uint n_unreached = 0;

    // 2D boundary: all its z values are null (e.g. 2D shape)
    size_t n_zero = 0;
    for (double z : z_boundary)
        if (z == 0.0)
            n_zero++;
    const bool boundary_2d = !z_boundary.empty() && (n_zero == z_boundary.size());

    if (n_data == 0)
    {
        if (boundary_2d || boundary_z == BoundaryZ::PROJECT)
            std::cout << "\033[0;33mWARNING: no data points inside the boundary - boundary cannot be projected, its z values are kept.\033[0m" << std::endl;
    }
    else
    {
        // Projection of the boundary on the data surface: boundary (and Steiner) z interpolated from the data points
        for (uint v=0; v < nv; v++)
            known.at(v) = (v < n_data);
        n_unreached = interpolate_unknown_z(trimesh, known, false);

        project = boundary_2d || (boundary_z == BoundaryZ::PROJECT);

        if (project)
        {
            if (boundary_2d)
                std::cout << "\033[0;33m=== Boundary has null z (2D shape).\033[0m" << std::endl;
            else
                std::cout << "\033[0;33m=== Projection of the 3D boundary forced (--projbz).\033[0m" << std::endl;
            std::cout << "\033[0;33m=== Projecting boundary points on the data surface: z interpolated as weighted average of neighbouring vertices (weights = 1/edge length).\033[0m" << std::endl;
        }
        else
        {
            // 3D boundary (z kept): check its z values against the data surface (warnings only)
            double data_zmin = DBL_MAX, data_zmax = -DBL_MAX;
            for (size_t v=0; v < n_data; v++)
            {
                data_zmin = std::min(data_zmin, trimesh.vert(v).z());
                data_zmax = std::max(data_zmax, trimesh.vert(v).z());
            }
            std::vector<double> dz;
            for (size_t i=0; i < z_boundary.size(); i++)
                dz.push_back(std::fabs(z_boundary.at(i) - trimesh.vert(n_data+i).z()));
            std::sort(dz.begin(), dz.end());
            const double dz_median = dz.empty() ? 0.0 : dz.at(dz.size()/2);
            const double dz_max    = dz.empty() ? 0.0 : dz.back();

            const double data_range = data_zmax - data_zmin;
            const double tol = (data_range > 0.0) ? 0.01 * data_range : 1e-6 * (1.0 + std::fabs(data_zmax));

            std::cout << "=== 3D boundary: |dz| between boundary and data surface: median = " << dz_median << ", max = " << dz_max << std::endl;
            if (n_zero > 0)
                std::cout << "\033[0;33mWARNING: " << n_zero << " boundary vertices with z = 0 (missing z values?).\033[0m" << std::endl;
            if (dz_median > tol)
                std::cout << "\033[0;33mWARNING: boundary z far from data (|dz| median = " << dz_median << " > 1% of data z range = " << tol
                          << "): expect a step along the boundary (use --projbz to project the boundary on the data).\033[0m" << std::endl;
        }
    }

    if (!project)
    {
        // Keep the boundary z values: restore them, interpolate the remaining vertices (Steiner points)
        for (size_t i=0; i < z_boundary.size(); i++)
            trimesh.vert(n_data+i).z() = z_boundary.at(i);
        for (uint v=0; v < nv; v++)
            known.at(v) = (v < n_data_boundary);
        n_unreached = interpolate_unknown_z(trimesh, known, true);
        std::cout << "=== Boundary z values are taken from the boundary (3D boundary)." << std::endl;
    }

    const uint n_unknown = static_cast<uint>(std::count(known.begin(), known.end(), false));
    if (n_unknown > n_unreached)
        std::cout << "### Interpolated z on " << n_unknown - n_unreached << " vertices." << std::endl;
    if (n_unreached > 0)
        std::cout << "\033[0;33mWARNING: " << n_unreached << " vertices not connected to any vertex with known z - z left unchanged.\033[0m" << std::endl;
    std::cout << "Assigned z values at vertices ... COMPLETED." << std::endl;

    //std::cout << "Constrained Delaunay Triangulation on boundary ... COMPLETED." << std::endl;

    return trimesh;

}







//Funzione che trova i punti con coordinate uguali, appartenenti a due mesh diverse (ciclando sul ch ordinato)
unsigned int first_boundary_point_corresponding (const cinolib::vec3d &first_point, const std::vector<uint> &or_idch, cinolib::Trimesh<> mesh)
{
    uint index_first_point = 0;
    cinolib::vec2d first_point2d(first_point.x(), first_point.y());
    //std::cout << "stampa first_point = " << first_point2d << std::endl;

    for(uint i:or_idch)
    {
        cinolib::vec3d search_first_point = mesh.vert(i);
        cinolib::vec2d search_first_point2d(search_first_point.x(), search_first_point.y());

        double dist = search_first_point2d.dist(first_point2d);
        //std::cout << "stampa search_first_point = " << search_first_point2d << std::endl;

        if(dist < 1e-06)
            break;
        else
            index_first_point++;
    }
    //std::cout << "stampa indice inizio top = " << index_first_point << std::endl;
    return index_first_point; //mi restituisce la posizione nel vettore dove si ha il primo punto
}

//Funzione che trova i punti con coordinate uguali, appartenenti a due mesh diverse (ciclando sul ch ordinato)
unsigned int first_boundary_point_corresponding (const cinolib::vec3d &first_point, const std::vector<uint> &or_idch, cinolib::Quadmesh<> mesh)
{
    uint index_first_point = 0;
    cinolib::vec2d first_point2d(first_point.x(), first_point.y());
    //std::cout << "stampa first_point = " << first_point2d << std::endl;

    for(uint i:or_idch)
    {
        cinolib::vec3d search_first_point = mesh.vert(i);
        cinolib::vec2d search_first_point2d(search_first_point.x(), search_first_point.y());

        double dist = search_first_point2d.dist(first_point2d);
        //std::cout << "stampa search_first_point = " << search_first_point2d << std::endl;

        if(dist < 1e-06)
            break;
        else
            index_first_point++;
    }
    //std::cout << "stampa indice inizio top = " << index_first_point << std::endl;
    return index_first_point; //mi restituisce la posizione nel vettore dove si ha il primo punto
}

std::vector<cinolib::vec3d> search_ordered_boundary_points(cinolib::Trimesh<> &mesh)
{
    std::vector<cinolib::vec3d> boundary_points;
    std::vector<uint> id_chpoints;
    id_chpoints = mesh.get_ordered_boundary_vertices();

    for(uint i=0; i<id_chpoints.size(); i++)
        boundary_points.push_back(mesh.vert(id_chpoints.at(i)));

    return boundary_points;
}


bool check_poly_normals (cinolib::Trimesh<> mesh)
{
    // Check on normals
    bool n_polys_cww = true;
    for(uint pid=0; pid<mesh.num_polys(); pid++)
    {
        std::vector<cinolib::vec2d> vec2d;
        for(size_t i=0; i<mesh.poly_verts(pid).size(); i++)
        {
            cinolib::vec2d v;
            v.x() = mesh.poly_verts(pid).at(i).x();
            v.y() = mesh.poly_verts(pid).at(i).y();
            vec2d.push_back(v);
        }
        //std::cout << polygon_is_CCW(vec2d) << std::endl;
        //std::cout << polygon_signed_area(vec2d) << std::endl;
        // se l'area è positiva, quindi ccw = 1 (true) -> normale uscente
        if(polygon_is_CCW(vec2d) == false)
        {
            n_polys_cww = false;
            break;
        }
    }
    return n_polys_cww;
}


bool check_closing_mesh (cinolib::Trimesh<> closed_mesh)
{
    //Check se la superficie è chiusa lateralmente in maniera corretta (riferimento: mesh completa con le tre superfici e la superficie laterale)
    std::vector<uint> vertices;
    vertices = closed_mesh.get_boundary_vertices();
    std::vector<cinolib::ipair> edge;
    edge = closed_mesh.get_boundary_edges();

    if(edge.size() == 0 || vertices.size() == 0)
        return true;
    else
        return false;
}

bool check_closing_mesh (cinolib::Quadmesh<> closed_mesh)
{
    //Check se la superficie è chiusa lateralmente in maniera corretta (riferimento: mesh completa con le tre superfici e la superficie laterale)
    std::vector<uint> vertices;
    vertices = closed_mesh.get_boundary_vertices();
    std::vector<cinolib::ipair> edge;
    edge = closed_mesh.get_boundary_edges();

    if(edge.size() == 0 || vertices.size() == 0)
        return true;
    else
        return false;
}





//cinolib::Trimesh<> closing_2trimeshes (cinolib::Trimesh<> &mesh_sup, cinolib::Trimesh<> &mesh_inf)
/*cinolib::Trimesh<> closing_2trimeshes (cinolib::Trimesh<> &mesh_sup, cinolib::Trimesh<> &mesh_inf)
{
    // 5) Creazione superficie laterale
    // 5.1) Vector indici dei punti sul convex hull nelle rispettive mesh
    std::vector<uint> or_idch0, or_idch1;
    or_idch0 = mesh_sup.get_ordered_boundary_vertices();
    or_idch1 = mesh_inf.get_ordered_boundary_vertices();


    //Trovare il primo punto da cui partire per la corrispondenza: ottengo la posizione che il primo punto ha nel vettore
    uint corr_s = first_boundary_point_corresponding(mesh_sup.vert(or_idch0.at(0)), or_idch1, mesh_inf);

    std::vector<uint> or_idch1_rev;
    or_idch1_rev.push_back(or_idch1.at(corr_s));
    uint delta_f2s = or_idch1.size()-1;
    uint curr_f2s = corr_s;
    while(delta_f2s > 0)
    {
        uint next = (curr_f2s < or_idch1.size()-1) ? curr_f2s+1 : 0;
        or_idch1_rev.push_back(or_idch1.at(next));
        curr_f2s = next;
        delta_f2s --;
    }

    // 5.2) Creazione mappe per le corrispondenze indici tra vertici del convex hull (proprio delle singole superfici)
    std::map <uint, uint> map; //mappa per le corrispondenze tra indici first&second
    for(uint i =0; i< or_idch0.size(); i++)
        map.insert(std::pair<uint, uint> (or_idch0.at(i), or_idch1_rev.at(i)));


    // 5.3) Aggiunta progressiva dei triangoli, cercando i vertici tra di loro corrispondenti tra seabed e topMTD
    cinolib::Trimesh<> closed_m = mesh_sup;
    closed_m += mesh_inf;

    std::map<uint,uint>::iterator it;
    for(uint i=0; i<or_idch0.size()-1; i++)
    {
        uint curr = or_idch0.at(i); //indice nella mesh del vertice in posizione i
        uint prox = or_idch0.at(i+1); //indice nella mesh del vertice in posizione i+1

        //std::map<uint,uint>::iterator it_curr;
        it = map.find(curr);
        uint p_curr = it->second + mesh_sup.num_verts(); //indice del punto corrispondente a curr (i)

        //std::map<uint,uint>::iterator it_prox;
        it = map.find(prox);
        uint p_prox = it->second + mesh_sup.num_verts(); //indice del punto corrispondente a prox (i+1)

        //Aggiunta dei triangoli (con schema antiorario)
        uint pid0 = closed_m.poly_add(curr, p_curr, p_prox);
        uint pid1 = closed_m.poly_add(curr, p_prox, prox);

    }//ciclo fino al penultimo vertice

    //Unione ultimo vertice con primo per chiusura superficie laterale
    uint last = or_idch0.at(or_idch0.size()-1); //indice dell'ultimo elemento
    uint first = or_idch0.at(0); //vertice in posizione i

    //std::map<uint,uint>::iterator it_last;
    it = map.find(last);
    uint p_last = it->second + mesh_sup.num_verts(); //corrispondente a curr

    //std::map<uint,uint>::iterator it_first;
    it = map.find(first);
    uint p_first = it->second + mesh_sup.num_verts(); //corrispondente a curr

    uint pid0 = closed_m.poly_add(last, p_last, p_first);
    uint pid1 = closed_m.poly_add(last, p_first, first);

    return closed_m;

}*/




cinolib::Trimesh<> add_lateral_polys (cinolib::Trimesh<> &closed_m, const std::vector<uint> &or_idch0, const std::vector<uint> &or_idch1)
{
    // 5.2) Creazione mappe per le corrispondenze indici tra vertici del convex hull (proprio delle singole superfici)
    std::map <uint, uint> map; //mappa per le corrispondenze tra indici first&second
    for(uint i =0; i< or_idch0.size(); i++)
        map.insert(std::pair<uint, uint> (or_idch0.at(i), or_idch1.at(i)));


    // 5.3) Aggiunta progressiva dei triangoli, cercando i vertici tra di loro corrispondenti tra seabed e topMTD
    std::map<uint,uint>::iterator it;
    for(uint i=0; i<or_idch0.size()-1; i++)
    {
        uint curr = or_idch0.at(i); //indice nella mesh del vertice in posizione i
        uint prox = or_idch0.at(i+1); //indice nella mesh del vertice in posizione i+1

        //std::map<uint,uint>::iterator it_curr;
        it = map.find(curr);
        uint p_curr = it->second; //indice del punto corrispondente a curr (i)

        //std::map<uint,uint>::iterator it_prox;
        it = map.find(prox);
        uint p_prox = it->second; //indice del punto corrispondente a prox (i+1)

        //Aggiunta dei triangoli (con schema antiorario)
        closed_m.poly_add(curr, p_curr, p_prox);
        closed_m.poly_add(curr, p_prox, prox);

    }//ciclo fino al penultimo vertice

    //Unione ultimo vertice con primo per chiusura superficie laterale
    uint last = or_idch0.at(or_idch0.size()-1); //indice dell'ultimo elemento
    uint first = or_idch0.at(0); //vertice in posizione i

    //std::map<uint,uint>::iterator it_last;
    it = map.find(last);
    uint p_last = it->second; //corrispondente a curr

    //std::map<uint,uint>::iterator it_first;
    it = map.find(first);
    uint p_first = it->second; //corrispondente a curr

    closed_m.poly_add(last, p_last, p_first);
    closed_m.poly_add(last, p_first, first);

    return closed_m;
}



cinolib::Quadmesh<> add_lateral_polys (cinolib::Quadmesh<> &closed_m, const std::vector<uint> &or_idch0, const std::vector<uint> &or_idch1)
{
    // 5.2) Creazione mappe per le corrispondenze indici tra vertici del convex hull (proprio delle singole superfici)
    std::map <uint, uint> map; //mappa per le corrispondenze tra indici first&second
    for(uint i =0; i< or_idch0.size(); i++)
        map.insert(std::pair<uint, uint> (or_idch0.at(i), or_idch1.at(i)));


    // 5.3) Aggiunta progressiva dei triangoli, cercando i vertici tra di loro corrispondenti tra seabed e topMTD
    std::map<uint,uint>::iterator it;
    for(uint i=0; i<or_idch0.size()-1; i++)
    {
        uint curr = or_idch0.at(i); //indice nella mesh del vertice in posizione i
        uint prox = or_idch0.at(i+1); //indice nella mesh del vertice in posizione i+1

        //std::map<uint,uint>::iterator it_curr;
        it = map.find(curr);
        uint p_curr = it->second; //indice del punto corrispondente a curr (i)

        //std::map<uint,uint>::iterator it_prox;
        it = map.find(prox);
        uint p_prox = it->second; //indice del punto corrispondente a prox (i+1)

        //Aggiunta dei triangoli (con schema antiorario)
        std::vector<uint> vlist;
        vlist.push_back(curr);
        vlist.push_back(p_curr);
        vlist.push_back(p_prox);
        vlist.push_back(prox);

        closed_m.poly_add(vlist);

    }//ciclo fino al penultimo vertice

    //Unione ultimo vertice con primo per chiusura superficie laterale
    uint last = or_idch0.at(or_idch0.size()-1); //indice dell'ultimo elemento
    uint first = or_idch0.at(0); //vertice in posizione i

    //std::map<uint,uint>::iterator it_last;
    it = map.find(last);
    uint p_last = it->second; //corrispondente a curr

    //std::map<uint,uint>::iterator it_first;
    it = map.find(first);
    uint p_first = it->second; //corrispondente a curr

    std::vector<uint> vlist;
    vlist.push_back(last);
    vlist.push_back(p_last);
    vlist.push_back(p_first);
    vlist.push_back(first);

    closed_m.poly_add(vlist);

    return closed_m;
}


///
/// \brief closing_2trimeshes computes the lateral closure of trimeshes (ordered following z axis, from up to down)
/// \param mesh_sup is the first mesh
/// \param mesh_inf is the second (followed) mesh
/// \param step represents the costant step to discretize along z direction
/// \return closed mesh adding triangles as lateral surface
///
cinolib::Trimesh<> closing_2trimeshes (cinolib::Trimesh<> &mesh_sup, cinolib::Trimesh<> &mesh_inf, const double &step)
{
    cinolib::Trimesh<> closed_m;

    double offset = mesh_sup.bbox().center().z() - mesh_inf.bbox().center().z();

    uint nstep_z = static_cast<uint>(offset/step); //ncelle su asse z
    std::cout << "n steps: " << nstep_z << std::endl;
    std::cout << std::endl;

    // 5) Creazione superficie laterale
    // 5.1) Vector indici dei punti sul convex hull nelle rispettive mesh
    std::vector<uint> or_idch0, or_idch1;

    closed_m = mesh_sup;

    if(nstep_z == 1 || nstep_z == 0)
    {
        //unisci sup con inf senza step intermedio
        //closed_m = closing_2trimeshes (mesh_sup, mesh_inf);

        or_idch0 = mesh_sup.get_ordered_boundary_vertices();
        std::vector<uint> or_idch1_rev = mesh_inf.get_ordered_boundary_vertices();

        uint corr_s = first_boundary_point_corresponding(closed_m.vert(or_idch0.at(0)), or_idch1_rev, mesh_inf);

        or_idch1.push_back(or_idch1_rev.at(corr_s) + closed_m.num_verts());
        uint delta_f2s = or_idch1_rev.size()-1;
        uint curr_f2s = corr_s;
        while(delta_f2s > 0)
        {
            uint next = (curr_f2s < or_idch1_rev.size()-1) ? curr_f2s+1 : 0;
            or_idch1.push_back(or_idch1_rev.at(next) + closed_m.num_verts());
            curr_f2s = next;
            delta_f2s --;
        }
        closed_m += mesh_inf;

        closed_m = add_lateral_polys(closed_m, or_idch0, or_idch1);
    }
    else
    {
        std::vector<uint> id_last_verts;

        for(uint s=0; s<nstep_z; s++)
        {
            if(s==0) //sono sul primo step
            {
                or_idch0 = mesh_sup.get_ordered_boundary_vertices();
                or_idch1.clear();

                for(uint i:or_idch0)
                {
                    cinolib::vec3d v (mesh_sup.vert(i).x(), mesh_sup.vert(i).y(), mesh_sup.vert(i).z() - step);
                    uint id_v = closed_m.vert_add(v);
                    or_idch1.push_back(id_v);
                }

            }
            else if(s == nstep_z-1)
            {
                or_idch0 = id_last_verts;
                or_idch1.clear();

                std::vector<uint> or_idch1_rev = mesh_inf.get_ordered_boundary_vertices();

                uint corr_s = first_boundary_point_corresponding(closed_m.vert(or_idch0.at(0)), or_idch1_rev, mesh_inf);

                or_idch1.push_back(or_idch1_rev.at(corr_s) + closed_m.num_verts());
                uint delta_f2s = or_idch1_rev.size()-1;
                uint curr_f2s = corr_s;
                while(delta_f2s > 0)
                {
                    uint next = (curr_f2s < or_idch1_rev.size()-1) ? curr_f2s+1 : 0;
                    or_idch1.push_back(or_idch1_rev.at(next) + closed_m.num_verts());
                    curr_f2s = next;
                    delta_f2s --;
                }
                closed_m += mesh_inf;

            }
            else
            {
                or_idch0 = id_last_verts;
                or_idch1.clear();

                for(uint i:or_idch0)
                {
                    cinolib::vec3d v (closed_m.vert(i).x(), closed_m.vert(i).y(), closed_m.vert(i).z() - step);
                    uint id_v = closed_m.vert_add(v);
                    or_idch1.push_back(id_v);
                }
            }
            id_last_verts = or_idch1;

            closed_m = add_lateral_polys(closed_m, or_idch0, or_idch1);
        }
    }

    return closed_m;
}



cinolib::Quadmesh<> closing_2quadmeshes (cinolib::Quadmesh<> &mesh_sup, cinolib::Quadmesh<> &mesh_inf, const double &step)
{
    cinolib::Quadmesh<> closed_m;

    double offset = mesh_sup.bbox().center().z() - mesh_inf.bbox().center().z();

    uint nstep_z = static_cast<uint>(offset/step); //ncelle su asse z
    std::cout << "n steps: " << nstep_z << std::endl;
    std::cout << std::endl;

    // 5) Creazione superficie laterale
    // 5.1) Vector indici dei punti sul convex hull nelle rispettive mesh
    std::vector<uint> or_idch0, or_idch1;

    closed_m = mesh_sup;

    if(nstep_z == 1 || nstep_z == 0)
    {
        //unisci sup con inf senza step intermedio
        //closed_m = closing_2trimeshes (mesh_sup, mesh_inf);

        or_idch0 = mesh_sup.get_ordered_boundary_vertices();
        std::vector<uint> or_idch1_rev = mesh_inf.get_ordered_boundary_vertices();

        uint corr_s = first_boundary_point_corresponding(closed_m.vert(or_idch0.at(0)), or_idch1_rev, mesh_inf);

        or_idch1.push_back(or_idch1_rev.at(corr_s) + closed_m.num_verts());
        uint delta_f2s = or_idch1_rev.size()-1;
        uint curr_f2s = corr_s;
        while(delta_f2s > 0)
        {
            uint next = (curr_f2s < or_idch1_rev.size()-1) ? curr_f2s+1 : 0;
            or_idch1.push_back(or_idch1_rev.at(next) + closed_m.num_verts());
            curr_f2s = next;
            delta_f2s --;
        }
        closed_m += mesh_inf;

        closed_m = add_lateral_polys(closed_m, or_idch0, or_idch1);
    }
    else
    {
        std::vector<uint> id_last_verts;

        for(uint s=0; s<nstep_z; s++)
        {
            if(s==0) //sono sul primo step
            {
                or_idch0 = mesh_sup.get_ordered_boundary_vertices();
                or_idch1.clear();

                for(uint i:or_idch0)
                {
                    cinolib::vec3d v (mesh_sup.vert(i).x(), mesh_sup.vert(i).y(), mesh_sup.vert(i).z() - step);
                    uint id_v = closed_m.vert_add(v);
                    or_idch1.push_back(id_v);
                }

            }
            else if(s == nstep_z-1)
            {
                or_idch0 = id_last_verts;
                or_idch1.clear();

                std::vector<uint> or_idch1_rev = mesh_inf.get_ordered_boundary_vertices();

                uint corr_s = first_boundary_point_corresponding(closed_m.vert(or_idch0.at(0)), or_idch1_rev, mesh_inf);

                or_idch1.push_back(or_idch1_rev.at(corr_s) + closed_m.num_verts());
                uint delta_f2s = or_idch1_rev.size()-1;
                uint curr_f2s = corr_s;
                while(delta_f2s > 0)
                {
                    uint next = (curr_f2s < or_idch1_rev.size()-1) ? curr_f2s+1 : 0;
                    or_idch1.push_back(or_idch1_rev.at(next) + closed_m.num_verts());
                    curr_f2s = next;
                    delta_f2s --;
                }
                closed_m += mesh_inf;

            }
            else
            {
                or_idch0 = id_last_verts;
                or_idch1.clear();

                for(uint i:or_idch0)
                {
                    cinolib::vec3d v (closed_m.vert(i).x(), closed_m.vert(i).y(), closed_m.vert(i).z() - step);
                    uint id_v = closed_m.vert_add(v);
                    or_idch1.push_back(id_v);
                }
            }
            id_last_verts = or_idch1;

            closed_m = add_lateral_polys(closed_m, or_idch0, or_idch1);
        }
    }

    return closed_m;
}





std::vector<double> region_growing_for_projection (cinolib::Trimesh<> &mesh, cinolib::vec3d &p)
{
    //double proj_p = 0.0;
    std::vector<double> proj;

    cinolib::Octree octree;
    octree.build_from_mesh_polys(mesh);

    cinolib::vec3d cp = octree.closest_point(p); //cerca il punto più vicino nell'octree (mesh)
    uint i_cp = mesh.pick_vert(cp); //indice del punto più vicino nella mesh di partenza

    std::vector<bool> visited (mesh.num_polys(), false);

    std::queue<uint> queue; //inizializzazione coda con la lista di triangoli adiacenti al vertice di indice i_cp? Ora solo con il primo triangolo della lista delle adiacenze al vertice, per evitare ripetizioni negli step successivi
//    for (uint pid : mesh.adj_v2p(i_cp))
//        queue.push(pid);
    queue.push(mesh.adj_v2p(i_cp).at(0));

    int count_p = 0;
    while (!queue.empty())
    {
        uint curr = queue.front();
        //std::cout << curr << std::endl;
        queue.pop();

        visited.at(curr) = true;


        cinolib::vec3d v1 = mesh.poly_vert(curr, 0);
        cinolib::vec3d v2 = mesh.poly_vert(curr, 1);
        cinolib::vec3d v3 = mesh.poly_vert(curr, 2);

        cinolib::vec2d p2d(p.x(), p.y());

        cinolib::vec2d v12d(v1.x(), v1.y());
        cinolib::vec2d v22d(v2.x(), v2.y());
        cinolib::vec2d v32d(v3.x(), v3.y());

        if (cinolib::point_in_triangle_2d(p2d, v12d, v22d, v32d)!=0) //se trovo la proiezione
        {

            cinolib::vec3d tn = cinolib::triangle_normal(v1, v2, v3);

            double d = -(v1.dot(tn));
            double proj_p = -(d+tn.x()*p2d.x()+tn.y()*p2d.y())/tn.z();

            proj.push_back(proj_p);
        }

        for (uint pid : mesh.adj_p2p(curr))
            if (!visited.at(pid))
                queue.push(pid);

//        if(count_p >= 2)
//            break;
    }

    return proj;
}


std::vector<double> region_growing_for_projection1 (cinolib::Trimesh<> &mesh, cinolib::vec3d &p)
{
    //double proj_p = 0.0;
    std::vector<double> proj;

    cinolib::Octree octree;
    octree.build_from_mesh_polys(mesh);

    cinolib::vec3d cp = octree.closest_point(p); //cerca il punto più vicino nell'octree (mesh)
    uint i_cp = mesh.pick_vert(cp); //indice del punto più vicino nella mesh di partenza

    std::vector<bool> visited (mesh.num_polys(), false);

    std::queue<uint> queue; //inizializzazione coda con la lista di triangoli adiacenti al vertice di indice i_cp? Ora solo con il primo triangolo della lista delle adiacenze al vertice, per evitare ripetizioni negli step successivi
//    for (uint pid : mesh.adj_v2p(i_cp))
//        queue.push(pid);
    queue.push(mesh.adj_v2p(i_cp).at(0));


    while (!queue.empty())
    {
        uint curr = queue.front();
        //std::cout << curr << std::endl;
        queue.pop();

        visited.at(curr) = true;


        cinolib::vec3d v1 = mesh.poly_vert(curr, 0);
        cinolib::vec3d v2 = mesh.poly_vert(curr, 1);
        cinolib::vec3d v3 = mesh.poly_vert(curr, 2);

        cinolib::vec2d p2d(p.x(), p.y());

        cinolib::vec2d v12d(v1.x(), v1.y());
        cinolib::vec2d v22d(v2.x(), v2.y());
        cinolib::vec2d v32d(v3.x(), v3.y());

        if (cinolib::point_in_triangle_2d(p2d, v12d, v22d, v32d)!=0) //se trovo la proiezione
        {
            cinolib::vec3d tn = cinolib::triangle_normal(v1, v2, v3);

            double d = -(v1.dot(tn));
            double proj_p = -(d+tn.x()*p2d.x()+tn.y()*p2d.y())/tn.z();

            proj.push_back(proj_p);
        }

        for (uint pid : mesh.adj_p2p(curr))
            if (!visited.at(pid))
                queue.push(pid);
    }

    return proj;
}



bool vert_merge (cinolib::Trimesh<> mesh, const uint vid0, const uint vid1)
{
    std::vector<uint> old_polys = mesh.adj_v2p(vid1);
    std::vector<std::vector<uint>> new_polys;
    for(uint pid : old_polys)
    {
        // if there is a polygon containing both vid0 and vid1 abort the operation
        if(mesh.poly_contains_vert(pid,vid0)) return false;

        std::vector<uint> p = mesh.poly_verts_id(pid);
        std::replace(p.begin(), p.end(), vid1, vid0);
        new_polys.push_back(p);
    }

    //for(auto p : new_polys) poly_add(p);
    mesh.vert_remove(vid1);

    return true;
}




