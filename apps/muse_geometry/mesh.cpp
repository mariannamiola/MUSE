#include "mesh.h"

//std::vector<Point3D> remove_duplicates (const std::vector<Point3D> &points)
//{
//    std::vector<Point3D> sorted_points = points;
//    std::vector<Point3D> unique_points;

//    cinolib::SORT_VEC(sorted_points);   //Questa funzione ordina i punti in base alla x (punti duplicati saranno vicini di x)
//                                        //I punti ordinati in base alla x, saranno ordinati in base alla y
//                                        //I punti ordinati in base alla y, saranno ordinati in base alla z

//    unique_points.push_back(sorted_points.at(0));

//    for (uint i=1; i < sorted_points.size(); i++)
//    {
//        Point2D p0, p1;
//        p0.x = sorted_points.at(i-1).x;
//        p0.y = sorted_points.at(i-1).y;

//        p1.x = sorted_points.at(i).x;
//        p1.y = sorted_points.at(i).y;

//        double dist = pow(pow((p1.x-p0.x),2) + pow((p1.y-p0.y),2), 0.5);

//        if (dist > 1e-6)
//            unique_points.push_back(sorted_points.at(i));
//    }
//    std::cout << "Removing duplicated points ... COMPLETED. " << unique_points.size() << " left" << std::endl;
//    return unique_points;
//}


//Funzione per la rimozione dei vertici isolati
//void remove_isolate_vertices (cinolib::DrawableTrimesh<> &mesh)
//{
//    //std::cout << "Removing Isolated Vertices ..." << std::endl;
//    std::vector<uint> tbrem;
//    for (uint vid=0; vid < mesh.num_verts(); vid++)
//    {
//        if (mesh.adj_v2v(vid).size() == 0)
//            tbrem.push_back(vid);
//    }
//    if (tbrem.size() > 0)
//    {
//        std::reverse(tbrem.begin(), tbrem.end());
//        for (uint vid : tbrem)
//            mesh.vert_remove_unreferenced(vid);
//    }
//    std::cout << "Removing Isolated Vertices ... COMPLETED" << std::endl;
//}

//Triangolazione punti con rimozione dei punti isolati + colorazione vertici in base alle z
//cinolib::DrawableTrimesh<> points_triangulation (const std::vector<cinolib::vec3d> &points, char opt[100]) //OK
//{
//    std::cout << "Running points Triangulation ..." << std::endl;

//    // Run triangulation by exploting Triangle Library in Cinolib and create a triangle mesh (m_tri)
//    cinolib::DrawableTrimesh<> m_tri;
//    triangle_wrap(points, {}, {}, 0, opt, m_tri);

//    // Vertices in the generated mesh have Z=0
//    // Restore original Z coordinate
//    //std::cout << "Restore z coordinates ..." << std::endl;
//    for (uint vid=0; vid < points.size(); vid++)
//        m_tri.vert(vid).z() = points[vid].z();

//    std::cout << "Restore z coordinates ... DONE" << std::endl;

//    //Clean generated mesh by removing possible isolated vertices
//    remove_isolate_vertices(m_tri);

//    std::cout << "Running PointCloud Triangulation ... COMPLETED." << std::endl;

//    //mesh_summary(m_tri);

//    // Mesh coloring - For each vertex, set a color depending on its Z coordinate
//    //vertex_coloring_on_z(m_tri); //for visualization

//    std::cout << "----------------------------------------------------------------------------" << std::endl;
//    std::cout << "Triangulation of points and saving mesh ... COMPLETED." << std::endl;
//    std::cout << std::endl;

//    return m_tri;
//}
