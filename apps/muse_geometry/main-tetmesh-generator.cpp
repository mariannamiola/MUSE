#include <cinolib/find_intersections.h>
#include <cinolib/geometry/plane.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/triangle_wrap.h>
#include <cinolib/tetgen_wrap.h>

#include <tclap/CmdLine.h>

#include <iostream>

using namespace TCLAP;

bool intersectPlaneSegment(const cinolib::Plane& plane, const cinolib::Segment& segment, cinolib::vec3d& intersection)
{
    cinolib::vec3d dir = segment.v[1] - segment.v[0]; // Segment direction
    double denom = plane.n.dot(dir);

    // Check if segment is parallel to the plane
    if (std::abs(denom) < 1e-6) {
        return false; // No intersection (parallel)
    }

    // Compute intersection parameter t
    double t = plane.n.dot(plane.p - segment.v[0]) / denom;

    // Check if intersection is within segment bounds
    if (t < 0.0 || t > 1.0) {
        return false; // Intersection is outside the segment
    }

    // Compute intersection point
    intersection = segment.v[0] + t * dir;
    return true;
}

int main(int argc, char **argv)
{
    // Print start message
    std::cout << std::endl;
    std::cout << "########### STARTING MUSE-TETMESH-GENERATOR ..." << std::endl;
    std::cout << std::endl;

    try
    {
        // Define command line parser and arguments
        CmdLine cmd("MUSE = Modelling of Uncertainty as a Support of Environment; Export tool", ' ', "version 0.5");

        // Define main functionalities options
        ValueArg<std::string> geomModel("m", "geom", "Geometry model", true, "name_geometry", "string", cmd);
        ValueArg<std::string> xyzPlane("p", "plane", "Plane", false, "plane", "string", cmd);

        ValueArg<double> planeShift("", "plane-shift", "Plane shift", false, 0.0, "double", cmd);
        
        ValueArg<std::string> triangleFlags("t", "triangle", "Triangle flags", false, "p", "string", cmd);
        ValueArg<std::string> tetgenFlags("T", "tetgen", "Tetgen flags", false, "p", "string", cmd);

        ValueArg<std::string> output("o", "out", "Output file", true, "output_file", "path", cmd);

        // Parse the argv array
        cmd.parse(argc, argv);

        std::string s = geomModel.getValue();
        std::string p = xyzPlane.getValue();
        std::string o = output.getValue();

        std::string triangle_flags = "";
        std::string tetgen_flags = "";
        
        if (triangleFlags.isSet())
            triangle_flags = triangleFlags.getValue();

        if (tetgenFlags.isSet())
            tetgen_flags = tetgenFlags.getValue();

        bool shift_plane = false;
        double plane_shift = 0.0;

        if (planeShift.isSet())
        {
            plane_shift = planeShift.getValue();
            shift_plane = true;
        }

        ////////////////////

        cinolib::Trimesh<> m(s.c_str());

        double avg_length = m.edge_avg_length();
    
        std::vector<cinolib::vec3d> ppoints;
        double plane_min_z = DBL_MAX;
    
        std::ifstream pf;
        pf.open(p);
    
        double x,y,z;
        while (pf >> x >> y >> z)
        {
            ppoints.push_back(cinolib::vec3d(x,y,z));
            if (z < plane_min_z) plane_min_z = z;
        }
    
        // GLcanvas gui1;
    
        // gui1.push(&m);
    
        // for (uint i=0; i < ppoints.size(); i++)
        //     gui1.push_marker(ppoints.at(i), "", Color::GREEN(), .5);
        // // gui.push(&menu);
        // // gui.push(&menu_m);
    
    
        // // for (uint i=0; i < 15; i++)
        // // {
        // //     cinolib::DrawableArrow arrow (m.vert(bverts.at(i)), mbot.vert(i));
        // //     gui.push(&arrow);
        // // }
    
        // return gui1.launch();
    
        cinolib::Plane plane (ppoints);
    
        const std::vector<uint> bverts = m.get_ordered_boundary_vertices();
    
        std::vector<cinolib::vec3d> bverts_proj;
    
        for (uint vid : bverts)
        {
            cinolib::vec3d intersection;
            cinolib::vec3d p2 = m.vert(vid);
            p2.z() = plane_min_z;
            cinolib::Segment segm (0, m.vert(vid), p2);
    
            if (!intersectPlaneSegment(plane,segm,intersection)) //.vert(vid));
            {
                std::cerr << "error" << std::endl;
            }
    
            bverts_proj.push_back(intersection);
        }
    
        std::cout << "projected" << std::endl;
    
        std::vector<uint> segms;
    
        for (uint i=0; i < bverts_proj.size(); i++)
        {
            segms.push_back(i);
            segms.push_back(i+1);
        }
    
        segms.push_back(bverts_proj.size()-1);
        segms.push_back(0);

        if (triangle_flags.length() == 0)
        {
            double avg_area = 0.0;
        
            for (uint pid=0; pid < m.num_polys(); pid++)
                avg_area += m.poly_area(pid);
        
            avg_area /= m.num_polys();
        
            avg_area *=2.0;

            triangle_flags = "pYa" + std::to_string(avg_area);
        }
    
        cinolib::Trimesh<> mbot;
    
        triangle_wrap(bverts_proj, segms, std::vector<cinolib::vec3d> (), plane_min_z, triangle_flags.c_str() , mbot);
    
        std::cout << "=======" << std::endl;
    
        for (uint vid=0 ; vid < mbot.num_verts(); vid++)
        {
            // vec3d pv = plane.project_onto(mbot.vert(vid));
    
            cinolib::vec3d intersection;
            cinolib::vec3d p2 = mbot.vert(vid);
            p2.z() = m.bbox().max.z();
            cinolib::Segment segm (0, mbot.vert(vid), p2);
            intersectPlaneSegment(plane,segm,intersection); //.vert(vid));
    
            mbot.vert(vid).z() = intersection.z();
        }
    
        std::cout << "projected" << std::endl;
    
        std::cout << m.get_ordered_boundary_vertices().size() << std::endl;
        std::cout << mbot.get_ordered_boundary_vertices().size() << std::endl;
    
        for (uint pid=0; pid < mbot.num_polys(); pid++)
        {
            const std::vector<uint> vp = mbot.vector_polys().at(pid);
            mbot.vector_polys().at(pid).at(0) = vp.at(2);
            mbot.vector_polys().at(pid).at(2) = vp.at(0);
        }
    
        mbot.update_p_normals();
    
        cinolib::Trimesh<> total = m;
        total += mbot;
    
        for (uint pid=0; pid < total.num_polys(); pid++)
            total.poly_data(pid).label=INT_MAX;
    
        uint n_bverts = bverts.size();
        uint n_edges = total.num_edges();
    
        for (uint i=0; i < n_bverts-1; i++)
        {
            total.poly_add(bverts.at(i), m.num_verts() +i, bverts.at(i+1));
            total.poly_add(bverts.at(i+1), m.num_verts() +i, m.num_verts() +i+1);
            // std::cout << total.num_polys() << std::endl;
        }
    
        total.poly_add(bverts.at(bverts.size()-1), m.num_verts() +bverts.size()-1, bverts.at(0));
        total.poly_add(bverts.at(0), m.num_verts() +bverts.size()-1, m.num_verts() +0);
    
        bool split = false;
        uint n_splits = 0;
    
        do
        {
            split = false;
            for (int eid=total.num_edges()-1; eid >=n_edges; eid--)
                if (total.edge_length(eid) > avg_length)
                {
                    if (total.poly_data(total.adj_e2p(eid).at(0)).label < INT_MAX && total.poly_data(total.adj_e2p(eid).at(1)).label < INT_MAX)
                    {
                        total.edge_split(eid);
                        split=true;
                    }
                }
            n_splits++;
        } while (/*split == true*/n_splits < 2);
        
        // std::set<ipair> intersections;
        // find_intersections(total, intersections);
    
        // std::cout << "self intersections: " << intersections.size() << std::endl;
        
        cinolib::Tetmesh<> tetmesh, tmp;
    
        cinolib::vec3d translate_vec = total.bbox().center();
        total.translate(-translate_vec);
    
        if (tetgen_flags.length() == 0)
        {
            tetgen_wrap(total, "Y", tmp);

            double avg_vol=0;
        
            for (uint pid=0; pid < tmp.num_polys(); pid++)
            {
                // total.vert_add(tmp.poly_centroid(pid));
                avg_vol += tmp.poly_volume(pid);
            }
        
            avg_vol /= tmp.num_polys();
        
            double vol = avg_vol / 1.1;

            tetgen_flags = "Ya" + std::to_string(vol);
        }
    
        tetgen_wrap(total, tetgen_flags, tetmesh);
        // tetgen_wrap(total, "Y" , tetmesh);
    
        tetmesh.translate(translate_vec);
    
        for (cinolib::vec3d &p : ppoints)
            p.z() += plane_shift;
    
        cinolib::Plane shifted_plane (ppoints);
    
        std::vector<uint> tbsplit;
    
        for (int pid = 0; pid <= tetmesh.num_polys()-1; pid++)
        {
            uint v_up=0;
            uint v_down=0;
    
            for (uint v=0; v < 4; v++)
            {
                cinolib::vec3d c = tetmesh.poly_vert(pid, v);
                cinolib::vec3d s0 = c; s0.z() = tetmesh.bbox().min.z();
                cinolib::vec3d s1 = c; s1.z() = tetmesh.bbox().max.z();
    
                cinolib::Segment segm (0, s0, s1);
    
                cinolib::vec3d intersection;
                intersectPlaneSegment(shifted_plane, segm, intersection);
    
                if (intersection.z() > c.z())
                    v_up++;
                else
                    v_down++;
            }
    
            if ( v_up > 0 && v_down > 0 )
                tbsplit.push_back(pid);
        }
    
        std::cout << "Splitting " << tbsplit.size() << " tets... " << std::endl;
    
        tetmesh.polys_split(tbsplit);
    
        std::cout << tetmesh.num_verts() << "V / " << tetmesh.num_polys() << "P" << std::endl;
    
        tetmesh.save(o.c_str());

        return 0;
    }
    catch (ArgException &e) // catch exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(-3);
    }
}
