#include "mesh.h"
#include "tools.h"

#include <vector>
#include <float.h>

#include <concaveman.h>
#include <unordered_set>
#include <algorithm>

#include "cinolib/octree.h"

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




//std::vector<Point3D> remove_duplicates (const std::vector<Point3D> &points)
//{
//    //std::vector<Point3D> points_ex;
//    for(size_t i=0; i<points.size(); i++)
//    {
//        Point3DID pex;
//        pex.x = points.at(i).x;
//        pex.y = points.at(i).y;
//        pex.z = points.at(i).z;
//        pex.id = i;

//        points_ex.push_back(pex);
//    }

//    std::vector<Point3DID> sorted_points = points_ex;
//    std::vector<Point3D> unique_points;
//    std::vector<int> id_dupl;

//    std::sort(sorted_points.begin(), sorted_points.end(), comparePoint2);

//    for (uint i=1; i < sorted_points.size(); i++)
//    {
//        Point2D p0, p1;
//        p0.x = sorted_points.at(i-1).x;
//        p0.y = sorted_points.at(i-1).y;
//        p1.x = sorted_points.at(i).x;
//        p1.y = sorted_points.at(i).y;

//        if (dist(p0, p1) <= 1e-6)
//            id_dupl.push_back(sorted_points.at(i).id);
//    }

//    if(id_dupl.size() > 0)
//    {
//        std::sort(id_dupl.begin(), id_dupl.end());

//        for(size_t i=0; i< points_ex.size(); i++)
//        {
//            if (!check_index(id_dupl, points_ex.at(i).id))
//            {
//                Point3D unique_p;
//                unique_p.x = points_ex.at(i).x;
//                unique_p.y = points_ex.at(i).y;
//                unique_p.z = points_ex.at(i).z;

//                unique_points.push_back(unique_p);
//            }
//        }
//    }
//    else
//        unique_points = points;

//    std::cout << "Removing duplicated points ... COMPLETED. " << unique_points.size() << " left" << std::endl;
//    return unique_points;\
//}

//namespace std
//{
//    template<>
//    struct hash<Point3D>
//    {
//        size_t operator()(Point3D const& pt) const
//        {
//            return (size_t)(pt.x*100 + pt.y);
//        }
//    };
//}
//bool operator==(const Point3D& pt1, const Point3D& pt2)
//{
//    return ((pt1.x == pt2.x) && (pt1.y == pt2.y));
//}

//std::vector<Point3D> remove_points_duplicates (std::vector<Point3D> &vec)
//{

//    std::unordered_set<Point3D> pointset;  // unordered_set is a hash table implementation

//    auto itor = vec.begin();
//    while (itor != vec.end())
//    {
//        if (pointset.find(*itor) != pointset.end())   // O(1) lookup time for unordered_set
//        {
//            itor = vec.erase(itor); // vec.erase returns the next valid iterator
//        }
//        else
//        {
//            pointset.insert(*itor);
//            itor++;
//        }
//    }

////    std::sort(points.begin(), points.end(), comparePoint);
////    auto unique_end = std::unique(points.begin(), points.end(), equalPoint);
////    points.erase(unique_end, points.end());


//}


void remove_duplicates_test(const std::vector<Point3D> &points, std::vector<Point3D> &unique_points, const double &tol)
{
    unique_points.push_back(points.at(0));

    for(uint i=1; i<points.size(); i++)
    {
        bool is_unique = true;
        for(uint j=0; j< unique_points.size(); j++)
        {
            double d = dist3D(points.at(i), unique_points.at(j));

            if(d < tol)
            {
                is_unique = false;
                break;
            }

        }
        if(is_unique)
            unique_points.push_back(points.at(i));
    }
    std::cout << "Removing " << points.size()-unique_points.size() << " duplicated points ... COMPLETED." << std::endl;
}


void remove_duplicates_test_opt(const std::vector<Point3D> &points, std::vector<Point3D> &unique_points)
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




cinolib::Trimesh<> constrained_triangulation2 (const std::vector<Point3D> &boundary3d, const std::vector<Point3D> &points, std::string opt)
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
                std::cout << "z value is set on 0.0" << std::endl;
                triangle_points_out_coords.push_back(0.0);
                counter_null++;
            }
        }
        vid++;
    }
    std::cout << "### Number of additional points (by Triangle) with null z-coordinate: " << counter_null << std::endl;
    std::cout << "Assigned z values at vertices ... COMPLETED." << std::endl;

    cinolib::Trimesh<> trimesh (triangle_points_out_coords, triangle_tris_out);
    std::cout << "Creation trimesh data structure ... COMPLETED." << std::endl;

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


//void triangles_split_on_centroid (cinolib::Trimesh<> &mesh)
//{
//    uint n_polys = mesh.num_polys();
//    for(uint pid=0; pid < n_polys; pid++)
//        mesh.poly_split(pid);
//}

//void triangles_split_on_edge (cinolib::Trimesh<> &mesh)
//{
//    uint n_verts = mesh.num_verts();
//    for(uint eid=0; eid<mesh.num_edges();eid++)
//    {
//        cinolib::vec3d v0 = mesh.edge_vert(eid, 0);
//        cinolib::vec3d v1 = mesh.edge_vert(eid, 1);
//        cinolib::vec3d delta = (v1-v0)/2;

//        cinolib::vec3d v_med (v0.x()+delta.x(), v0.y()+delta.y(), v0.z()+delta.z());
//        mesh.vert_add(v_med);
//    }
//    //std::cout << "N. ORIGINALE VERTICI: " << n_verts << std::endl;
//    //std::cout << "N. VERTICI POST: " << mesh.num_verts() << std::endl;

//    uint n_polys = mesh.num_polys();
//    for(uint pid=0; pid < n_polys; pid++)
//    {
//        uint vid0 = mesh.poly_vert_id(pid, 0);
//        uint vid1 = mesh.poly_vert_id(pid, 1);
//        uint vid2 = mesh.poly_vert_id(pid, 2);

//        uint new0 = mesh.poly_edge_id(pid, 0) + n_verts;
//        uint new1 = mesh.poly_edge_id(pid, 1) + n_verts;
//        uint new2 = mesh.poly_edge_id(pid, 2) + n_verts;

//        uint new_pid;
//        new_pid = mesh.poly_add(vid0, new0, new2);
//        mesh.poly_data(new_pid) = mesh.poly_data(pid);

//        new_pid = mesh.poly_add(new0, vid1, new1);
//        mesh.poly_data(new_pid) = mesh.poly_data(pid);

//        new_pid = mesh.poly_add(new2, new1, vid2);
//        mesh.poly_data(new_pid) = mesh.poly_data(pid);

//        new_pid = mesh.poly_add(new0, new1, new2);
//        mesh.poly_data(new_pid) = mesh.poly_data(pid);

//        mesh.poly_remove(pid);
//    }
//}


//void quads_split_on_edge (cinolib::Quadmesh<> &mesh)
//{
//    uint n_verts = mesh.num_verts();
//    for(uint eid=0; eid<mesh.num_edges();eid++)
//    {
//        cinolib::vec3d v0 = mesh.edge_vert(eid, 0);
//        cinolib::vec3d v1 = mesh.edge_vert(eid, 1);
//        cinolib::vec3d delta = (v1-v0)/2;

//        cinolib::vec3d v_med (v0.x()+delta.x(), v0.y()+delta.y(), v0.z()+delta.z());
//        mesh.vert_add(v_med);
//    }
//    //std::cout << "N. ORIGINALE VERTICI: " << n_verts << std::endl;
//    //std::cout << "N. VERTICI POST: " << mesh.num_verts() << std::endl;

//    uint n_polys = mesh.num_polys();
//    for(uint pid=0; pid < n_polys; pid++)
//    {
//        uint centr = mesh.vert_add(mesh.poly_centroid(pid));

//        uint vid0 = mesh.poly_vert_id(pid, 0);
//        uint vid1 = mesh.poly_vert_id(pid, 1);
//        uint vid2 = mesh.poly_vert_id(pid, 2);
//        uint vid3 = mesh.poly_vert_id(pid, 3);

//        std::vector<uint> pid_adj_edge = mesh.adj_p2e(pid);

//        uint new0 = pid_adj_edge.at(0) + n_verts;
//        uint new1 = pid_adj_edge.at(1) + n_verts;
//        uint new2 = pid_adj_edge.at(2) + n_verts;
//        uint new3 = pid_adj_edge.at(3) + n_verts;

//        std::vector<uint> list0 {vid0, new0, centr, new3};
//        std::vector<uint> list1 {new0, vid1, new1, centr};
//        std::vector<uint> list2 {centr, new1, vid2, new2};
//        std::vector<uint> list3 {new3, centr, new2, vid3};


//        uint new_pid;
//        new_pid = mesh.poly_add(list0);
//        mesh.poly_data(new_pid) = mesh.poly_data(pid);

//        new_pid = mesh.poly_add(list1);
//        mesh.poly_data(new_pid) = mesh.poly_data(pid);

//        new_pid = mesh.poly_add(list2);
//        mesh.poly_data(new_pid) = mesh.poly_data(pid);

//        new_pid = mesh.poly_add(list3);
//        mesh.poly_data(new_pid) = mesh.poly_data(pid);

//        mesh.poly_remove(pid);
//    }
//}









