// MUSE = Modelling of Uncertainty as a Support of Environment
// Export raster tool
// This tool exports the results of the MUSE SIS simulation to a raster file.
//
// Example of command line:
// ./muse_export_raster -m tetmesh_4.5M.mesh -s sisim.csv -o ratser.xyz -x 1488500 -y 4929000 -X 1494500 -Y 4916500 -r 378 -c 629 -l 5 --clean_from_bot

// LICENSE STATEMENT
// This file is part of MUSE.

// DISCLAIMER
// This software is provided "as is" and any expressed or implied warranties, including,
// but not limited to, the implied warranties of merchantability and fitness for a particular
// purpose are disclaimed. In no event shall the authors and the MUSE members be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services; loss of use,
// data, or profits; or business interruption) however caused and on any theory of liability,
// whether in contract, strict liability, or tort (including negligence or otherwise) arising
// in any way out of the use of this software, even if advised of the possibility of such damage.


#include <cinolib/meshes/meshes.h>
#include <cinolib/export_cluster.h>
#include <cinolib/grid_mesh.h>
#include <cinolib/octree.h>

#include <tclap/CmdLine.h>

using namespace TCLAP;

int sign(const double x)
{
    if (std::abs(x) < 1e-9) return 0;
    return (x > 0) - std::signbit(x);  // Returns 1 for positive, -1 for negative, 0 for zero
}

int main(int argc, char **argv)
{

    // Print start message
    std::cout << std::endl;
    std::cout << "########### STARTING MUSE-EXPORT-RASTER ..." << std::endl;
    std::cout << std::endl;

    try
    {
        // Define command line parser and arguments
        CmdLine cmd("MUSE = Modelling of Uncertainty as a Support of Environment; Export tool", ' ', "version 0.5");

        // Define main functionalities options
        /**

         * @brief Geometry model

         * @param geom geometry model

         */

        ValueArg<std::string> geomModel("m", "geom", "Geometry model", true, "name_geometry", "string", cmd);
        /**

         * @brief Label

         * @param label label

         */

        ValueArg<int> label_keep("l", "label", "Label", true, 0, "int", cmd);
        /**

         * @brief Sis simulation

         * @param sis sis simulation

         */

        ValueArg<std::string> sisCSV("s", "sis", "Sis simulation", false, "name_sis", "string", cmd);

        /**


         * @brief Raster number of rows


         * @param nrows Number of raster number of rows


         */


        ValueArg<int> nrows("r", "nrows", "Raster number of rows", true, 0, "int", cmd);
        /**

         * @brief Raster number of cols

         * @param ncols Number of raster number of cols

         */

        ValueArg<int> ncols("c", "ncols", "Raster number of cols", true, 0, "int", cmd);

        /**


         * @brief Raster min x


         * @param min_x raster min x


         */


        ValueArg<double> minx("x", "min_x", "Raster min x", true, 0.0, "double", cmd);
        /**

         * @brief Raster min y

         * @param min_y raster min y

         */

        ValueArg<double> miny("y", "min_y", "Raster min y", true, 0.0, "double", cmd);
        /**

         * @brief Raster max x

         * @param max_x raster max x

         */

        ValueArg<double> maxx("X", "max_x", "Raster max x", true, 0.0, "double", cmd);
        /**

         * @brief Raster max y

         * @param max_y raster max y

         */

        ValueArg<double> maxy("Y", "max_y", "Raster max y", true, 0.0, "double", cmd);

        SwitchArg cl_bot("", "clean_from_bot", "Clean from bottom", cmd);
        SwitchArg cl_top("", "clean_from_top", "Clean from top", cmd);

        /**


         * @brief Output file


         * @param out Path to output file


         */


        ValueArg<std::string> output("o", "out", "Output file", true, "output_file", "path", cmd);

        // Parse the argv array
        cmd.parse(argc, argv);

        std::string s = geomModel.getValue();
        std::string c = sisCSV.getValue();
        std::string o = output.getValue();

        uint label = label_keep.getValue();
        uint res_x = nrows.getValue(), res_y = ncols.getValue();

        double min_x = minx.getValue();
        double min_y = miny.getValue();
        double max_x = maxx.getValue();
        double max_y = maxy.getValue();

        bool clean_from_bottom = cl_bot.isSet();
        bool clean_from_top = cl_top.isSet();

        ////

        std::cout << "Reading mesh ..." << std::endl;
        cinolib::Polyhedralmesh<> m_total(s.c_str());

        std::cout << std::endl << "Reading SIS results ..." << std::endl<< std::endl;
        std::ifstream csv_file;
        csv_file.open(c);

        int l, pid_counter=0;
        while (csv_file >> l)
            m_total.poly_data(pid_counter++).label = l;

        // translate to recompute normals - to guarantee numerical stability
        cinolib::vec3d bbox_center = m_total.bbox().center();

        m_total.translate(-bbox_center);
        m_total.update_f_normals();
        m_total.translate(bbox_center);

        // extract submesh aving only polys with label = input label

        std::cout << "Creating submesh with label " << label << " ..." << std::endl;
        cinolib::Polyhedralmesh<> m;
        std::unordered_map<uint,uint>      m2subm_vmap;
        std::unordered_map<uint,uint>      subm2m_vmap;

        cinolib::export_cluster (m_total, label, m, m2subm_vmap, subm2m_vmap);

        if (clean_from_bottom || clean_from_top)
        {
            std::cout << std::endl << "Cleaning submesh ..." << std::endl << std::endl;
            
            // get the starting faces to recompute top & bottom
            uint id_max, id_min;
            double max_normal = -DBL_MAX, min_normal = DBL_MAX;

            for (uint fid=0; fid < m_total.num_faces(); fid++)
            {
                if (!m_total.face_is_on_srf(fid))
                    continue;

                if (m_total.face_data(fid).normal.z() > max_normal) //// top
                {
                    max_normal = m_total.face_data(fid).normal.z();
                    id_max = fid;
                }

                if (m_total.face_data(fid).normal.z() < min_normal) //// bot
                {
                    min_normal = m_total.face_data(fid).normal.z();
                    id_min = fid;
                }
            }

            std::queue<uint> top, bottom;
            std::vector<uint> starting_faces;

            if (clean_from_top)
            {
                top.push(id_max);
                m_total.face_data(id_max).label = 100;

                while (!top.empty())
                {
                    uint curr_fid = top.front();
                    top.pop();

                    if (sign(m_total.face_data(curr_fid).normal.z()) != 1)
                        continue;

                    starting_faces.push_back(curr_fid);

                    for (uint adj_fid : m_total.adj_f2f(curr_fid))
                    {
                        if (!m_total.face_is_on_srf(adj_fid))
                            continue;

                        if (m_total.face_data(adj_fid).label == 100)
                            continue;

                        top.push(adj_fid);
                        m_total.face_data(adj_fid).label = 100;
                    }
                }
            }

            if (clean_from_bottom)
            {
                bottom.push(id_min);
                m_total.face_data(id_min).label = 100;

                while (!bottom.empty())
                {
                    uint curr_fid = bottom.front();
                    bottom.pop();

                    if (sign(m_total.face_data(curr_fid).normal.z()) != -1)
                        continue;

                    starting_faces.push_back(curr_fid);

                    for (uint adj_fid : m_total.adj_f2f(curr_fid))
                    {
                        if (!m_total.face_is_on_srf(adj_fid))
                            continue;

                        if (m_total.face_data(adj_fid).label == 100)
                            continue;

                        bottom.push(adj_fid);
                        m_total.face_data(adj_fid).label = 100;
                    }
                }
            }

            for (uint fid : starting_faces)
            {
                uint v0, v1, v2;
                v0 = m_total.adj_f2v(fid).at(0);
                v1 = m_total.adj_f2v(fid).at(1);
                v2 = m_total.adj_f2v(fid).at(2);
        
                uint v0_m, v1_m, v2_m;
        
                auto query = m2subm_vmap.find(v0);
                if (query == m2subm_vmap.end()) continue;
        
                query = m2subm_vmap.find(v1);
                if (query == m2subm_vmap.end()) continue;
        
                query = m2subm_vmap.find(v2);
                if (query == m2subm_vmap.end()) continue;
        
                v0_m = m2subm_vmap.at(v0);
                v1_m = m2subm_vmap.at(v1);
                v2_m = m2subm_vmap.at(v2);
        
                std::vector<uint> vlist {v0_m, v1_m, v2_m};
        
                uint id_min_m = m.face_id(vlist);
        
                if (id_min_m == -1)
                    continue;

                uint starting_pid = m.adj_f2p(id_min_m).at(0);
        
                if (m.poly_data(starting_pid).label == INT_MAX)
                    continue;
        
                //std::cout << "starting fid : " << id_min_m << std::endl;
                //std::cout << "starting pid : " << starting_pid << std::endl;
        
                ////
        
                //std::vector<bool> visited (m.num_polys(), false);
        
                std::queue<uint> pids_oi;
                pids_oi.push(starting_pid);
                m.poly_data(starting_pid).label = INT_MAX;
        
                while (!pids_oi.empty())
                {
                    uint curr_pid = pids_oi.front();
                    pids_oi.pop();
        
                    for (uint adj_pid : m.adj_p2p(curr_pid))
                    {
                        if (m.poly_data(adj_pid).label == INT_MAX)
                            continue;
        
                        m.poly_data(adj_pid).label = INT_MAX;
                        pids_oi.push(adj_pid);
                    }
                }
            }    

            //// Remove all the unlabelled pids
            std::vector<uint> tbrem;
            for (int pid = m.num_polys()-1; pid >=0; pid--)
                if(m.poly_data(pid).label < INT_MAX)
                    tbrem.push_back(pid);

            std::cout << "Removing " << tbrem.size() << " polys ..." << std::endl;

            for (uint pid : tbrem)
                m.poly_remove(pid);

            std::cout << "mesh\t " << m.num_verts() << "V / " << m.num_polys() << " P" << std::endl << std::endl;
        }

        // put the mesh elements into an octree for efficiency

        cinolib::Octree octree;

        for (uint fid=0; fid < m.num_faces(); fid++)
            octree.push_triangle(fid, m.face_vert(fid,0), m.face_vert(fid,1), m.face_vert(fid,2));

        octree.build();

        // create the raster structure and put the raster into the user-defined position

        std::cout << "Creating raster structure ..." << std::endl;
        cinolib::Quadmesh<> grid;
        cinolib::grid_mesh(res_x, res_y, grid);

        cinolib::AABB bbox_raster (cinolib::vec3d(min_x,min_y,0), cinolib::vec3d(max_x,max_y,0));
        grid.translate(bbox_raster.min);

        double scale_x, scale_y;

        if (bbox_raster.delta().x() > res_x)
            scale_x = bbox_raster.delta().x() / (double) res_x;
        else
            scale_x = (double) res_x / bbox_raster.delta().x();

        if (bbox_raster.delta().y() > res_y)
            scale_y = bbox_raster.delta().y() / (double) res_y;
        else
            scale_y = (double) res_y / bbox_raster.delta().y();

        grid.scale(scale_x, scale_y, 1);
        grid.translate(bbox_raster.min - grid.bbox().min);
        cinolib::vec3d vec_trasl (0,0, m.bbox().max.z()+10);

        grid.translate(vec_trasl);

        /// set raster pixel values

        std::cout << std::endl << "Setting raster values per pixel ..." << std::endl << std::endl;

        #pragma omp parallel for
        for (uint pid = 0; pid < grid.num_polys(); pid++)
        {
            cinolib::vec3d c = grid.poly_centroid(pid);

            std::set<std::pair<double,uint>> intersections;
            octree.intersects_ray(c, cinolib::vec3d(0,0,-1), intersections);

            if (intersections.empty())
                grid.poly_data(pid).quality = FLT_MAX;
            else
            {
                grid.poly_data(pid).quality = c.z() - intersections.begin()->first;
            }
        }

        // write raster file

        std::cout << "Saving raster ..." << std::endl;

        std::ofstream raster_file;
        raster_file.open(o);

        for (uint pid = 0; pid < grid.num_polys(); pid++)
        {
            cinolib::vec3d cc = grid.poly_centroid(pid);

            if (grid.poly_data(pid).quality == FLT_MAX)
            {
                raster_file << std::setprecision(10) << cc.x() << " " << cc.y() << " " << cc.z() << " NAN" << std::endl;
            }
            else
            {
                raster_file << std::setprecision(10) << cc.x() << " " << cc.y() << " " << cc.z() << " " << grid.poly_data(pid).quality << std::endl;
            }
        }

        raster_file.close();

        std::cout << "Saved raster: " << o << std::endl;

        return 0;
    }
    catch (ArgException &e) // catch exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(-3);
    }

}
