/********************************************************************************
*  This file is part of MUSE                                                    *
*  Copyright(C) 2025: Marianna Miola                                            *
*                                                                               *
*  Author:                                                                      *
*                                                                               *
*     Marianna Miola (marianna.miola@cnr.it)                                    *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*                                                                               *
*  This program is free software: you can redistribute it and/or modify it      *
*  under the terms of the GNU General Public License as published by the        *
*  Free Software Foundation, either version 3 of the License, or (at your       *
*  option) any later version.                                                   *
*                                                                               *
*  This program is distributed in the hope that it will be useful, but          *
*  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
*  more details.                                                                *
*                                                                               *
*  You should have received a copy of the GNU General Public License along      *
*  with this program. If not, see <https://www.gnu.org/licenses/>.              *
*                                                                               *
*********************************************************************************/

#include <iostream>
#include <fstream>
#include <iomanip>

#include <filesystem>

#include <tclap/CmdLine.h>
//#include <json.hpp>

#include <igl/cotmatrix.h>
#include <igl/barycenter.h>
#include <igl/boundary_facets.h>
#include <igl/parula.h>
#include <igl/readMESH.h>
#include <igl/slice.h>
#include <igl/marching_tets.h>
#include <igl/winding_number.h>
#include <Eigen/Sparse>


#include "cinolib/predicates.h"
#include "cinolib/octree.h"

#include "muselib/input/load_xyz.h"
#include "muselib/input/load_vtk.h"

#include "muselib/colors.h"

//for filesystem
#ifdef __APPLE__
    using namespace std::__fs;
#else
    using namespace std;
#endif

#include "muselib/data_structures/project.h"
#include "muselib/data_structures/data.h"

#include "muselib/metadata/manipulate_meta.h"
#include "muselib/metadata/data_meta.h"

#include "muselib/utils.h"
#include "muselib/geometry/tools.h"
#include "muselib/geometry/mesh.h"
#include "muselib/input/load_xyz.h"

#include <cinolib/meshes/drawable_trimesh.h>
#include <cinolib/meshes/drawable_tetmesh.h>

#include "muselib/geometry/surface_mesh.h"
#include "muselib/geometry/volume_mesh.h"

#include "muselib/stratigraphic_trasformation/coord_transf.h"


using namespace MUSE;
using namespace TCLAP;

int sign(const double x) 
{
    if (std::abs(x) < 1e-9) return 0;
    return (x > 0) - std::signbit(x);  // Returns 1 for positive, -1 for negative, 0 for zero
}

int main(int argc, char** argv)
{
    std::cout << std::endl;
    std::cout << "########### STARTING MUSE-MANIPULATE ..." << std::endl;
    std::cout << std::endl;

    std::string app_name = "manipulate"; //app name
    std::string app_data = "data"; //app data name
    std::string app_geometry = "geometry"; //app geometry name


    try {
    CmdLine cmd("MUSE = Modelling of Uncertainty as a Support of Environment; Manipulate tool", ' ', "version 0.0");

    // ---------------------------------------------------------------------------------------------------------
    // MAIN FUNCTIONALITIES:

    // Option 0. Index extraction from geometry model
    /**
     * @brief Extraction data basing on geometry model. This option allows to extract data from a geometry model, such as a surface or volume mesh, based on specified criteria. The extracted data can be used for further analysis or visualization.
     * @note When using -E/--extract, requires:
     * - --geom: Geometry model to extract from (mandatory)
     * - --zcoord: Coordinate Z to use for extraction (optional, used for 3D models)
     * @example -E -p /path/to/project --geom model.obj
     */
    SwitchArg setExtract                    ("E", "extract", "Set extraction data based on geometry model", cmd, false); //booleano

    /**
     * @brief Set project directory. This is the main directory where the project files are located. It is a required argument for running the application, as it specifies the context in which the manipulation will occur. The project directory should contain all necessary data and geometry files for the operations to be performed.
     * @note This argument is mandatory for running the application. The project directory should be organized according to the expected structure of the MUSE project, with subdirectories for data, geometry, and output as needed.
     * @example -p /path/to/project
     */
    ValueArg<std::string> projectFolder     ("p", "pdir", "Set project directory", true, "path/to/project", "string", cmd);

    /**
     * @brief Set geometry model. This argument specifies the geometry model to be used for data extraction when the -E/--extract flag is enabled. The geometry model can be a surface mesh, volume mesh, or any other supported geometric representation. The specified model must exist within the project directory and will be used as the basis for extracting data according to the defined criteria.
     * @example -E --geom model.obj
     */
    ValueArg<std::string> geomModel         ("", "geom", "Set geometry model", false, "geometry-name", "string", cmd);

    /**
     * @brief Set coordinate Z. This argument specifies the coordinate Z to be used for data extraction when the -E/--extract flag is enabled. It is optional and used for 3D models.
     * @example -E --geom model.obj --zcoord elevation
     */
    ValueArg<std::string> setZcoord         ("z", "zcoord", "Set coordinate Z", false, "z_name", "string", cmd);

    // Option 1. Index extraction from interval
    /**
     * @brief Set extraction data from interval
     * @param intextr Enable extraction data from interval
     * @note When using -I/--intextr, requires:
     * - --sup: Superior interval limit (mandatory)
     * - --inf: Inferior interval limit (mandatory)
     * - --nvar: Variable name to check (mandatory)
     * @example -I --sup 100 --inf 0 --nvar temperature
     */
    SwitchArg setIntervalExtraction         ("I", "intextr", "Set extraction data from interval", cmd, false); //booleano

    /**
     * @brief Set sup interval
     * @param sup sup interval
     * @note Used with -I/--intextr flag. Required for interval extraction
     * Must be used together with --inf and --nvar
     */
    ValueArg<int> supInterval               ("", "sup", "Set sup interval", false, 0, "int", cmd);

    /**
     * @brief Set inf interval
     * @param inf inf interval
     * @note Used with -I/--intextr flag. Required for interval extraction
     * Must be used together with --sup and --nvar
     */

    ValueArg<int> infInterval               ("", "inf", "Set inf interval", false, 0, "int", cmd);

    /**
     * @brief Set variable to check
     * @param nvar Name of set variable to check
     * @note Used with -I/--intextr flag. Required for interval extraction
     * Must be used together with --sup and --inf
     */
    ValueArg<std::string> nameVar           ("", "nvar", "Set variable to check", false, "var_name", "string", cmd);

    /**
     * @brief Set sub dataset extraction based on geometry
     * @param sub extraction sub dataset basing on geometry
     */
    ValueArg<std::string> subDataset        ("", "sub", "Set sub dataset extraction based on geometry", false, "name", "path", cmd);



    /**
     * @brief Set rotation axis
     * @param rotaxis rotation axis
     * @note When using rotation, these flags work together:
     * - --rotaxis: Rotation axis (X, Y, Z)
     * - --rotangle: Rotation angle (required if rotaxis != NO)
     * - --rotcx, --rotcy, --rotcz: Rotation center coordinates
     * @example For Z-axis rotation: --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0
     */
    ValueArg<std::string> setRotAxis        ("", "rotaxis", "Set rotation axis", false, "NO", "rot_axis", cmd);

    /**
     * @brief Set rotation angle (clockwise)
     * @param rotangle rotation angle (clockwise)
     * @note Used together with --rotaxis flag. Required when rotaxis != NO
     */
    ValueArg<double> setRotAngle            ("", "rotangle", "Set rotation angle (clockwise)", false, 0.0, "double", cmd);
    
    /**
     * @brief Set rotation center x
     * @param rotcx rotation center x
     */
    ValueArg<double> setRotCenterX          ("", "rotcx", "Set rotation center x", false, 0.0, "double", cmd);
    
    /**
     * @brief Set rotation center y
     * @param rotcy rotation center y
     */
    ValueArg<double> setRotCenterY          ("", "rotcy", "Set rotation center y", false, 0.0, "double", cmd);
    
    /**
     * @brief Set rotation center z
     * @param rotcz rotation center z
     */
    ValueArg<double> setRotCenterZ          ("", "rotcz", "Set rotation center z", false, 0.0, "double", cmd);


    // Option 2. Point projection on surfaces
    /**
     * @brief Points projection on surfaces
     * @param prsurf Enable points projection on surfaces
     * @note Projection mode selection (mutually exclusive):
     * - -P/--prsurf: Surface projection
     * - -S/--prsect: Section projection (2D)
     * - -R/--prqsect: Quad section projection
     * Choose only ONE projection mode per operation
     */
    SwitchArg setProjectionOnSurface        ("P", "prsurf", "Points projection on surfaces", cmd, false);
    
    /**
     * @brief Compute points projection on boundary (2D section case).
     * @param prsect Flag to compute points projection on boundary (2d section case).
     * @note Mutually exclusive with -P/--prsurf and -R/--prqsect
     * Use for 2D section projection operations
     */
    SwitchArg setProjectionOnSection        ("S", "prsect", "Compute points projection on boundary (2D section case).", cmd, false);
    
    /**
     * @brief Points projection on quads sections
     * @param prqsect Enable points projection on quads sections
     * @note Mutually exclusive with -P/--prsurf and -S/--prsect
     * Use for quad-based section projection operations
     */
    SwitchArg setProjectionOnQSection       ("R", "prqsect", "Points projection on quads sections", cmd, false);

    /**
     * @brief Compute points projection on boundary (3D volumetric case).
     * @param prvol Flag to compute points projection on boundary (3d volumetric case).
     */
    SwitchArg setProjectionOnVolume         ("V", "prvol", "Compute points projection on boundary (3D volumetric case).", cmd, false);

    /**
     * @brief Set number of steps for geometry model
     * @param step Number of set number of steps for geometry model
     */
    ValueArg<double> setStepGeometry        ("", "step", "Set number of steps for geometry model", false, 0.0, "double", cmd);
    
    /**
     * @brief Set tolerance to enlarge bounding box
     * @param epsilon tolerance to enlarge bounding box
     */
    ValueArg<double> setBBEpsilon           ("", "epsilon", "Set tolerance to enlarge bounding box", false, 1.0, "double", cmd);

    //SwitchArg setProjectionOnVolume        ("Q", "prqvol", "Points qprojection on volumes", cmd, false);
    //SwitchArg setProjectionOnVolume2        ("R", "prvol2", "Points projection on volumes2", cmd, false);
    MultiArg<std::string> meshFiles         ("m", "mgeom", "Multi-geometry to pass", false, "string", cmd );

    /**
     * @brief Set direction of projection
     * @param prdir Path to set direction of projection
     */
    ValueArg<std::string> setProjDir        ("", "prdir", "Set direction of projection", false, "Y", "string", cmd);

    std::vector<std::string> allowedType = {"SAMPLES","TET","HEX","VOLUME","GEOMETRY","QUADMESH"};
    ValuesConstraint<std::string> allowedValsT(allowedType);
    /**
     * @brief Set type
     * @param type type
     */
    ValueArg<std::string> setType           ("", "type", "Set type", false, "SAMPLES", &allowedValsT, cmd);
    allowedType.clear();


    // Option 3. Stratigraphic coordinate transformation
    std::vector<std::string> allowedStratigraphicCondition = {"PROPORTIONAL","TRUNCATION","ONLAP","COMBINATION"};
    ValuesConstraint<std::string> allowedValsSC(allowedStratigraphicCondition);

    /**
     * @brief Points projection on surfaces
     * @param strat Enable points projection on surfaces
     */
    SwitchArg setStratigraphicTransf        ("T", "strat", "Points projection on surfaces", cmd, false);

    /**
     * @brief Name of geometry model
     * @param name Name of name of geometry model
     */
    ValueArg<std::string> geomName          ("", "name", "Name of geometry model", false, "name", "string", cmd);
    
    /**
     * @brief Set type of stratigraphic transformation
     * @param sttype type of stratigraphic transformation
     */
    ValueArg<std::string> stratCondition    ("", "sttype", "Set type of stratigraphic transformation", false, "NO", &allowedValsSC, cmd);
    
    /**
     * @brief Top geometry model
     * @param top top geometry model
     */
    ValueArg<std::string> topSurface        ("", "top", "Top geometry model", false, "name top geometry", "string", cmd);
    
    /**
     * @brief Bottom geometry model
     * @param bot bottom geometry model
     */
    ValueArg<std::string> botSurface        ("", "bot", "Bottom geometry model", false, "name bottom geometry", "string", cmd);
    
    /**
     * @brief Set region growing
     * @param reggrow Enable set region growing
     */
    SwitchArg setRegionGrowing              ("", "reggrow", "Set region growing", cmd, false); //booleano


    // ---------------------------------------------------------------------------------------------------------
    // ADDITIONAL FUNCTIONALITIES:

    /**
     * @brief Saving trimesh in obj format
     * @param obj Enable saving trimesh in obj format
     */
    SwitchArg objConversion                 ("", "obj", "Saving trimesh in obj format", cmd, false); //booleano
    
    /**
     * @brief Saving tetmesh in vtk format
     * @param vtk Enable saving tetmesh in vtk format
     */
    SwitchArg vtkConversion                 ("", "vtk", "Saving tetmesh in vtk format", cmd, false); //booleano
    
    /**
     * @brief Saving extraction as set of points
     * @param save Enable saving extraction as set of points
     */
    SwitchArg saveExtraction                ("", "save", "Saving extraction as set of points", cmd, false); //booleano
    
    /**
     * @brief Variable
     * @param var Name of variable
     */
    ValueArg<std::string> Variable          ("v", "var", "Variable", false, "variable to analyse", "name", cmd);

    /**
     * @brief Path file
     * @param file Path to path file
     */
    ValueArg<std::string> setFileData       ("", "file", "Path file", false, "path", "string", cmd);


    // ---------------------------------------------------------------------------------------------------------
    // PARSING:

    // Parse the argv array.
    cmd.parse(argc, argv);


    // ---------------------------------------------------------------------------------------------------------
    // SETTINGS:

    MUSE::Project Project;
    Project.folder = projectFolder.getValue(); //percorso progetto
    Project.name = Project.folder.substr(Project.folder.find_last_of("/")+1, Project.folder.length()); //nome progetto

    // 0) Commands
    std::cout << FCYN("###### Execution command ...") << std::endl;
    std::string command;
    std::cout << "Number of command arguments: " << argc << std::endl;

    filesystem::path abspath = argv[3];
    std::cout << "Absolute path: " << abspath << std::endl;

    for(int i=1; i< argc; i++)
    {
        std::string string = argv[i];
        if(string.find(abspath) != std::string::npos)
        {
            //std::cout << "Path: " << argv[i] << std::endl;

            filesystem::path path = argv[i];
            filesystem::path relpath = filesystem::relative(path, abspath);
            //std::cout << "Relative path: " << relpath << std::endl;

            if(relpath.string().length() > 1)
                command += "./" + relpath.string();
            else
                command += relpath;
            command += " ";
        }
        else
        {
            command += argv[i];
            command += " ";
        }
    }
    std::cout << command << std::endl;
    std::cout << FCYN("###### ###### ###### ######") << std::endl;
    std::cout << std::endl;

    // Output folder
    std::string out_folder = Project.folder + "/out";

    std::string metadata_folder = out_folder + "/data/metadata";
    std::string data_folder = out_folder + "/data/data";
    std::string surf_folder = out_folder + "/" + app_geometry + "/surf";

    std::string app_folder = out_folder + "/" + app_name;
    if(!filesystem::exists(app_folder))
        filesystem::create_directory(app_folder);



    // ---------------------------------------------------------------------------------------------------------
    // STARTS:

    if(setExtract.isSet() && !setFileData.isSet())
    {
        std::string abs_datadir = out_folder + "/" + app_data;
        std::vector<std::string> list_dir = get_directories(abs_datadir);
        if(list_dir.empty())
            list_dir.push_back(abs_datadir);

        if((get_filename(list_dir.at(0)).compare("data") == 0 && get_filename(list_dir.at(1)).compare("metadata") == 0)
            || (get_filename(list_dir.at(1)).compare("data") == 0 && get_filename(list_dir.at(0)).compare("metadata") == 0))
        {
            list_dir.clear();
            list_dir.resize(1, abs_datadir);
        }

        int count_frame = 0;
        for(const std::string &l:list_dir)
        {
            count_frame++;

            filesystem::path dir = l;
            filesystem::path rel_datadir = filesystem::relative(dir, abs_datadir);
            std::cout << rel_datadir.string() << std::endl;

            app_folder.clear();
            app_folder = out_folder + "/" + app_name;
            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);

            if(rel_datadir.string().compare(".") != 0)
            {
                app_folder += "/" + rel_datadir.string();
                filesystem::create_directory(app_folder);

                std::cout << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << "### NUMBER OF TIME FRAMES: " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME N° " << count_frame << " ON " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME NAME: " << rel_datadir.string() << std::endl;
                std::cout << std::endl;
            }


            std::vector<std::string> list_json = get_files(l, ".json");
            if(list_json.size() > 1)
            {
                std::cerr << "ERROR. Only a file JSON is expected!" << std::endl;
                exit(1);
            }


            MUSE::DataMeta datameta;
            datameta.read(list_json.at(0));

            ExtractionMeta extrmeta;
            extrmeta.setProject(Project);

            std::vector<std::string> excommands;
            excommands.push_back(command);
            extrmeta.setCommands(excommands);


            // 0) Define meta for dependencies
            std::vector<std::string> depsextr;

            std::vector<std::string> id;
            std::vector<double> xCoord, yCoord, zCoord;
            if(!setFileData.isSet())
            {
                if(datameta.getInfoData().id_name.compare("Unknown") != 0)
                {
                    readTextValues(l + "/data/" + datameta.getInfoData().id_name + ".dat", id);
                    depsextr.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().id_name + ".json", Project.folder));
                }
                else
                    std::cerr << "ERROR reading ID: " << l + "/data/" + datameta.getInfoData().id_name + ".dat" << " NOT found." << std::endl;

                if(datameta.getInfoData().x_name.compare("Unknown") != 0)
                {
                    readCoordinate(l + "/data/" + datameta.getInfoData().x_name + ".dat", xCoord);
                    depsextr.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().x_name + ".json", Project.folder));
                }
                else
                    std::cerr << "ERROR reading X coordinate: " << l + "/data/" + datameta.getInfoData().x_name + ".dat" << " NOT found." << std::endl;

                if(datameta.getInfoData().y_name.compare("Unknown") != 0)
                {
                    readCoordinate(l + "/data/" + datameta.getInfoData().y_name + ".dat", yCoord);
                    depsextr.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().y_name + ".json", Project.folder));
                }
                else
                    std::cerr << "ERROR reading Y coordinate: " << l + "/data/" + datameta.getInfoData().y_name + ".dat" << " NOT found." << std::endl;

                if(datameta.getInfoData().z_name.compare("Unknown") != 0)
                {
                    readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                    depsextr.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().z_name + ".json", Project.folder));
                }
                else
                {
                    if(setZcoord.isSet())
                    {
                        readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                        depsextr.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().z_name + ".json", Project.folder));
                    }
                    else
                        std::cout << "\033[0;33mWARNING: Z coordinate is Unknown. Set -z --name <variable> for setting the variable.\033[0;0m" << std::endl;
                }
            }
            else
                load_xyzfile(setFileData.getValue(), xCoord, yCoord, zCoord);


            // 2) Load polygonal mesh and check on mesh type

            MUSE::SurfaceMesh<> mesh;
            mesh.load(geomModel.getValue().c_str());

            std::cout << "\033[0;32mLoading mesh file: " << geomModel.getValue() << " ... COMPLETED.\033[0m" << std::endl;
            std::cout << std::endl;

            depsextr.push_back(filesystem::relative(get_basename(geomModel.getValue()) + ".json", Project.folder));
            extrmeta.setDependencies(depsextr); //added dependencies


            std::string geom_name = geomModel.getValue().substr(geomModel.getValue().find_last_of("/")+1, geomModel.getValue().length()); //nome progetto

            MUSE::ExtractionMeta::DataExtraction infoextr;
            infoextr.geometry = get_basename(geom_name);


            std::vector<uint> id_points_in;
            if(!mesh.check_lateral_closing()) //se la mesh non è chiusa -> allora è una superficie
            {
                std::vector<uint> bv = mesh.get_ordered_boundary_vertices();

                MUSE::Rotation otfRotation, otfRotation_data; //relativa alla geometria -> definisco un piano locale x-y di riferimento per l'allineamento dei dati

                // ------------------------------------------------------------
                // 1. Uso il boundary della mesh come geometria master.
                // La mesh definisce il piano locale su cui fare point-in-polygon.
                // ------------------------------------------------------------
                std::vector<Point3D> boundary_xy;
                boundary_xy.reserve(bv.size());
                for(uint vid : bv)
                {
                    Point3D p;
                    p.x = mesh.vert(vid).x();
                    p.y = mesh.vert(vid).y();
                    p.z = mesh.vert(vid).z();
                    boundary_xy.push_back(p);
                }

                bool auto_aligned_geom = align_points_to_xyplane(boundary_xy, otfRotation);

                std::cout << "=== Point-in-polygon is performed on local XY plane." << std::endl;
                if(auto_aligned_geom)
                    std::cout << "=== Mesh boundary automatically aligned to XY plane." << std::endl;
                else
                    std::cout << "=== Mesh boundary already aligned to XY plane." << std::endl;
                
                // ------------------------------------------------------------
                // 2. Porto i dati nello stesso sistema locale XY della mesh.
                // Se la mesh era già XY, i dati restano invariati.
                // ------------------------------------------------------------
                std::vector<Point3D> coords_xy;
                coords_xy.reserve(xCoord.size());
                
                bool auto_aligned_data = false;
                if(auto_aligned_geom)
                {
                    for(size_t i = 0; i < xCoord.size(); i++)
                    {
                        cinolib::vec3d axis (otfRotation.rotation_axis_vec.at(0), otfRotation.rotation_axis_vec.at(1), otfRotation.rotation_axis_vec.at(2));
                        cinolib::vec3d center (otfRotation.rotation_center_x, otfRotation.rotation_center_y, otfRotation.rotation_center_z);

                        Point3D p_xy;
                        point_rotation(xCoord.at(i), yCoord.at(i), zCoord.at(i), axis, otfRotation.rotation_angle, center, p_xy.x, p_xy.y, p_xy.z);

                        coords_xy.push_back(p_xy);
                    }    
                }
                else 
                {
                    for(size_t i = 0; i < xCoord.size(); i++)
                    {
                        Point3D p;
                        p.x = xCoord.at(i);
                        p.y = yCoord.at(i);
                        p.z = zCoord.at(i);
                        p.index = i;
                        coords_xy.push_back(p);
                    }
                    auto_aligned_data = align_points_to_xyplane(coords_xy, otfRotation_data);
                }
                

                std::vector<Point2D> polygon;
                for(size_t i=0; i<boundary_xy.size(); i++)
                {
                    Point2D v;
                    v.x = boundary_xy.at(i).x;
                    v.y = boundary_xy.at(i).y;

                    polygon.push_back(v);
                }

                std::vector<Point2D> coords;
                for(size_t i=0; i<coords_xy.size(); i++)
                {
                    Point2D point;
                    point.x = coords_xy.at(i).x;
                    point.y = coords_xy.at(i).y;

                    coords.push_back(point);
                }

                //Il check sulla bounding box viene fatto all'interno della funzione dei points_in_polygon
                points_in_polygon(coords, polygon, id_points_in);
            }

            else
            {
                // Check on bbox
                double bbx_max = mesh.bbox().max.x();
                double bbx_min = mesh.bbox().min.x();
                double bby_max = mesh.bbox().max.y();
                double bby_min = mesh.bbox().min.y();
                double bbz_max = mesh.bbox().max.z();
                double bbz_min = mesh.bbox().min.z();

                std::vector<Point3D> coords_in;
                for(size_t i=0; i<xCoord.size(); i++)
                {
                    if (xCoord.at(i) < bbx_min || xCoord.at(i) > bbx_max || yCoord.at(i) < bby_min || yCoord.at(i) > bby_max || zCoord.at(i) < bbz_min || zCoord.at(i) > bbz_max )
                    {
                        continue;
                        std::cout << "Point is out from the bbox" << std::endl;
                        std::cout << "ID = " << i << " - " << xCoord.at(i) << "; " << yCoord.at(i) << "; " << zCoord.at(i) << std::endl;
                    }
                    else
                    {
                        Point3D point;
                        point.x = xCoord.at(i);
                        point.y = yCoord.at(i);
                        point.z = zCoord.at(i);
                        point.index = i;

                        coords_in.push_back(point);
                    }
                }
                std::cout << coords_in.size() << " points in bbox." << std::endl;


    #ifdef MUSE_USES_LIBIGL

                using namespace Eigen;
                using namespace std;

                Eigen::MatrixXd V   (mesh.num_verts(), 3); //vettore dei vertici
                Eigen::MatrixXi T   (mesh.num_polys(), 3); //vettore delle facce

                for(uint vid=0; vid<mesh.num_verts(); vid++)
                {
                    V(vid, 0) = mesh.vert(vid).x();
                    V(vid, 1) = mesh.vert(vid).y();
                    V(vid, 2) = mesh.vert(vid).z();
                }

                for(uint pid=0; pid<mesh.num_polys(); pid++)
                {
                    T(pid, 0) = mesh.poly_vert_id(pid, 0);
                    T(pid, 1) = mesh.poly_vert_id(pid, 1);
                    T(pid, 2) = mesh.poly_vert_id(pid, 2);
                }

                Eigen::MatrixXd VC  (coords_in.size(), 3); //campioni
                Eigen::VectorXd W   (coords_in.size());

                int j = 0;
                for(size_t i=0; i<coords_in.size(); i++)
                {
                    VC(j, 0) = coords_in.at(i).x;
                    VC(j, 1) = coords_in.at(i).y;
                    VC(j, 2) = coords_in.at(i).z;

                    j++;
                }

                // Compute generalized winding number at all barycenters
                //cout<<"Computing winding number over all "<<T.rows()<<" tets..." << T.cols() <<endl;
                igl::winding_number(V,T,VC,W);

                std::vector<int> id_points_out;
                for(size_t i=0; i<coords_in.size(); i++)
                {
                    if(W[i] < 0.5)
                    {
                        id_points_out.push_back(coords_in.at(i).index);
                        std::cout << "### Point #" << coords_in.at(i).index << " is out from the bbox: [" << std::setprecision(12) << coords_in.at(i).x << "; " << coords_in.at(i).y << "; " << coords_in.at(i).z << "]" << std::endl;
                        //continue;
                    }
                    else
                    {
                        //std::cout << "the point is in the tet mesh!" << std::endl;
                        id_points_in.push_back(coords_in.at(i).index);
                    }
                }
                std::cout << "Computing winding numbers... COMPLETED." << std::endl;


    #else

                std::cerr << "LIBIGL is required. Please include the library and use MUSE_USES_LIBIGL." << std::endl;

    #endif
            }

            std::cout << std::endl;
            if(xCoord.size() == id_points_in.size())
                std::cout << "All points are included in mesh: " << geomModel.getValue() << std::endl;
            else
                std::cout << id_points_in.size() << " / " << xCoord.size() << " in mesh: " << geomModel.getValue() << std::endl;


            infoextr.n_points = id_points_in.size();
            infoextr.id_points = id_points_in;
            extrmeta.setDataExtraction(infoextr);

            extrmeta.write(app_folder + "/" + get_basename(geom_name) + ".json");


            if(saveExtraction.isSet() && Variable.isSet())
            {
                std::vector<double> in_values; //, out_values; //, out_x, out_y, out_z;
                load1d_xyzfile(l + "/data/" + Variable.getValue() + ".dat", in_values);

                std::ofstream file_out;
                file_out.open(app_folder + "/" + Variable.getValue() + "_" + get_basename(geom_name) + ".dat", std::fstream::out);
                if(!file_out.is_open())
                {
                    std::cerr << "\033[0;31mError in file opening: " << app_folder + "/" + Variable.getValue() + "_" + get_basename(geom_name) + ".dat" << "\033[0m" << std::endl;
                    exit(1);
                }
                else
                {
                    for(uint idin:id_points_in)
                        file_out << std::setprecision(9) << xCoord.at(idin) << " " << yCoord.at(idin) << " " << in_values.at(idin) << std::endl;
                    file_out.close();
                }

                // for(uint i:id_points_in)
                // {
                //     out_x.push_back(xCoord.at(i));
                //     out_y.push_back(yCoord.at(i));
                //     out_z.push_back(zCoord.at(i));
                //     out_values.push_back(in_values.at(i));
                // }
                // export_idxyzv(app_folder + "/" + Variable.getValue() + "_" + get_basename(geom_name) + ".dat" , out_x, out_y, out_z, out_values);
            }
            else if(saveExtraction.isSet() && !Variable.isSet())
            {
                std::ofstream file_out;
                file_out.open(get_basename(setFileData.getValue()) + "__.dat", std::fstream::out);
                if(!file_out.is_open())
                {
                    std::cerr << "\033[0;31mError in file opening: " << app_folder + "/" + Variable.getValue() + "_" + get_basename(geom_name) + ".dat" << "\033[0m" << std::endl;
                    exit(1);
                }
                else
                {
                    for(uint idin:id_points_in)
                        file_out << std::setprecision(9) << xCoord.at(idin) << " " << yCoord.at(idin) << " " << zCoord.at(idin) << std::endl;
                    file_out.close();
                }
            }

            std::cout << std::endl;
            std::cout << "Points extrapolation... COMPLETED." << std::endl;

        }

    }


    if(setExtract.isSet() && setFileData.isSet())
    {
        std::vector<double> xCoord, yCoord, zCoord;
        load_xyzfile(setFileData.getValue(), xCoord, yCoord, zCoord);

        // 2) Load polygonal mesh and check on mesh type
        MUSE::SurfaceMesh<> mesh;
        mesh.load(geomModel.getValue().c_str());

        std::cout << "\033[0;32mLoading mesh file: " << geomModel.getValue() << " ... COMPLETED.\033[0m" << std::endl;
        std::cout << std::endl;


        std::string geom_name = geomModel.getValue().substr(geomModel.getValue().find_last_of("/")+1, geomModel.getValue().length()); //nome progetto



        std::vector<uint> id_points_in;

        //if(!check_closing_mesh(mesh)) //se la mesh non è chiusa -> allora è una superficie
        if(!mesh.check_lateral_closing()) //se la mesh non è chiusa -> allora è una superficie
        {
            std::vector<uint> bv = mesh.get_ordered_boundary_vertices();

            std::vector<Point2D> polygon;
            for(uint vid:bv)
            {
                Point2D v;
                v.x = mesh.vert(vid).x();
                v.y = mesh.vert(vid).y();

                polygon.push_back(v);
            }

            std::vector<Point2D> coords;
            for(size_t i=0; i<xCoord.size(); i++)
            {
                Point2D point;
                point.x = xCoord.at(i);
                point.y = yCoord.at(i);

                coords.push_back(point);
            }

            //Il check sulla bounding box viene fatto all'interno della funzione dei points_in_polygon
            points_in_polygon(coords, polygon, id_points_in);
        }

        else
        {
            // Check on bbox
            double bbx_max = mesh.bbox().max.x();
            double bbx_min = mesh.bbox().min.x();
            double bby_max = mesh.bbox().max.y();
            double bby_min = mesh.bbox().min.y();
            double bbz_max = mesh.bbox().max.z();
            double bbz_min = mesh.bbox().min.z();

            std::vector<Point3D> coords_in;
            for(size_t i=0; i<xCoord.size(); i++)
            {
                if (xCoord.at(i) < bbx_min || xCoord.at(i) > bbx_max || yCoord.at(i) < bby_min || yCoord.at(i) > bby_max || zCoord.at(i) < bbz_min || zCoord.at(i) > bbz_max )
                {
                    continue;
                    std::cout << "Point is out from the bbox" << std::endl;
                    std::cout << "ID = " << i << " - " << xCoord.at(i) << "; " << yCoord.at(i) << "; " << zCoord.at(i) << std::endl;
                }
                else
                {
                    Point3D point;
                    point.x = xCoord.at(i);
                    point.y = yCoord.at(i);
                    point.z = zCoord.at(i);
                    point.index = i;

                    coords_in.push_back(point);
                }
            }
            std::cout << coords_in.size() << " points in bbox." << std::endl;


#ifdef MUSE_USES_LIBIGL

            using namespace Eigen;
            using namespace std;

            Eigen::MatrixXd V   (mesh.num_verts(), 3); //vettore dei vertici
            Eigen::MatrixXi T   (mesh.num_polys(), 3); //vettore delle facce

            for(uint vid=0; vid<mesh.num_verts(); vid++)
            {
                V(vid, 0) = mesh.vert(vid).x();
                V(vid, 1) = mesh.vert(vid).y();
                V(vid, 2) = mesh.vert(vid).z();
            }

            for(uint pid=0; pid<mesh.num_polys(); pid++)
            {
                T(pid, 0) = mesh.poly_vert_id(pid, 0);
                T(pid, 1) = mesh.poly_vert_id(pid, 1);
                T(pid, 2) = mesh.poly_vert_id(pid, 2);
            }

            Eigen::MatrixXd VC  (coords_in.size(), 3); //campioni
            Eigen::VectorXd W   (coords_in.size());

            int j = 0;
            for(size_t i=0; i<coords_in.size(); i++)
            {
                VC(j, 0) = coords_in.at(i).x;
                VC(j, 1) = coords_in.at(i).y;
                VC(j, 2) = coords_in.at(i).z;

                j++;
            }

            // Compute generalized winding number at all barycenters
            //cout<<"Computing winding number over all "<<T.rows()<<" tets..." << T.cols() <<endl;
            igl::winding_number(V,T,VC,W);

            std::vector<int> id_points_out;
            for(size_t i=0; i<coords_in.size(); i++)
            {
                if(W[i] < 0.5)
                {
                    id_points_out.push_back(coords_in.at(i).index);
                    std::cout << "### Point #" << coords_in.at(i).index << " is out from the bbox: [" << std::setprecision(12) << coords_in.at(i).x << "; " << coords_in.at(i).y << "; " << coords_in.at(i).z << "]" << std::endl;
                    //continue;
                }
                else
                {
                    //std::cout << "the point is in the tet mesh!" << std::endl;
                    id_points_in.push_back(coords_in.at(i).index);
                }
            }
            std::cout << "Computing winding numbers... COMPLETED." << std::endl;


#else

                std::cerr << "LIBIGL is required. Please include the library and use MUSE_USES_LIBIGL." << std::endl;

#endif
        }

        std::cout << std::endl;
        if(xCoord.size() == id_points_in.size())
            std::cout << "All points are included in mesh: " << geomModel.getValue() << std::endl;
        else
            std::cout << id_points_in.size() << " points in mesh: " << geomModel.getValue() << std::endl;

        if(saveExtraction.isSet())
        {
            std::ofstream file_out;
            file_out.open(app_folder + "/"+ get_basename(get_filename(setFileData.getValue())) + "__.dat", std::fstream::out);
            if(!file_out.is_open())
            {
                std::cerr << "\033[0;31mError in file opening: " << app_folder + "/"+ get_basename(get_filename(setFileData.getValue())) + "__.dat" << "\033[0m" << std::endl;
                exit(1);
            }
            else
            {
                for(uint idin:id_points_in)
                    file_out << std::setprecision(9) << xCoord.at(idin) << " " << yCoord.at(idin) << " " << zCoord.at(idin) << std::endl;
                file_out.close();
            }
        }

        std::cout << std::endl;
        std::cout << "Points extrapolation... COMPLETED." << std::endl;

    }




    //JSON DA ALLINEARE!!!
    if(setProjectionOnSurface.isSet())
    {
        std::string abs_datadir = out_folder + "/" + app_data;
        std::vector<std::string> list_dir = get_directories(abs_datadir);
        if(list_dir.empty())
            list_dir.push_back(abs_datadir);

        if((get_filename(list_dir.at(0)).compare("data") == 0 && get_filename(list_dir.at(1)).compare("metadata") == 0)
            || (get_filename(list_dir.at(1)).compare("data") == 0 && get_filename(list_dir.at(0)).compare("metadata") == 0))
        {
            list_dir.clear();
            list_dir.resize(1, abs_datadir);
        }

        int count_frame = 0;
        for(const std::string &l:list_dir)
        {
            count_frame++;

            filesystem::path dir = l;
            filesystem::path rel_datadir = filesystem::relative(dir, abs_datadir);
            std::cout << rel_datadir.string() << std::endl;

            app_folder.clear();
            app_folder = out_folder + "/" + app_name;
            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);

            if(rel_datadir.string().compare(".") != 0)
            {
                app_folder += "/" + rel_datadir.string();
                filesystem::create_directory(app_folder);

                std::cout << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << "### NUMBER OF TIME FRAMES: " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME N° " << count_frame << " ON " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME NAME: " << rel_datadir.string() << std::endl;
                std::cout << std::endl;
            }

            std::vector<std::string> list_json = get_files(l, ".json");
            if(list_json.size() > 1)
            {
                std::cerr << "ERROR. Only a file JSON is expected!" << std::endl;
                exit(1);
            }

            MUSE::ManipulateMeta manmeta;
            manmeta.setProject(Project);

            ManipulateMeta::DataProjection dataProjection;
            dataProjection.proj_is_set = true;
            dataProjection.data_type = setType.getValue();

            if(setType.getValue().compare("SAMPLES") == 0) //leggo in automatico le coordinate del set di dati (da json data)
            {
                std::vector<double> xCoord, yCoord, zCoord;
                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR. Set mesh files -m <filename>" << std::endl;
                    exit(1);
                }

                MUSE::DataMeta datameta;
                datameta.read(list_json.at(0));

                if(datameta.getInfoData().x_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().x_name + ".dat", xCoord);
                else
                {
                    std::cerr << "ERROR reading X coordinate: " << l + "/data/" + datameta.getInfoData().x_name + ".dat" << " NOT found." << std::endl;
                    exit(1);
                }

                if(datameta.getInfoData().y_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().y_name + ".dat", yCoord);
                else
                {
                    std::cerr << "ERROR reading Y coordinate: " << l + "/data/" + datameta.getInfoData().y_name + ".dat" << " NOT found." << std::endl;
                    exit(1);
                }

                if(datameta.getInfoData().z_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                else
                {
                    if(setZcoord.isSet())
                        readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                    else
                    {
                        std::cerr << "\033[0;32mWARNING: Z coordinate is Unknown. Set -z --name <variable> for setting the variable.\033[0;0m" << std::endl;
                        exit(1);
                    }
                }



                MUSE::Rotation dataRotation;

                std::vector<cinolib::vec3d> coord_samples;
                if(setRotAxis.isSet())
                {
                    std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                    std::cout << "Rotation axis: " << setRotAxis.getValue() << std::endl;
                    std::cout << "Rotation center: [" << setRotCenterX.getValue() << "; "<< setRotCenterY.getValue() << "; " << setRotCenterZ.getValue() << "]" << std::endl;
                    std::cout << "Rotation angle (degree): " << setRotAngle.getValue() << std::endl;
                    std::cout << std::endl;

                    dataRotation.rotation = true;
                    dataRotation.rotation_axis = setRotAxis.getValue();
                    dataRotation.rotation_center_x = setRotCenterX.getValue();
                    dataRotation.rotation_center_y = setRotCenterY.getValue();
                    dataRotation.rotation_center_z = setRotCenterZ.getValue();
                    dataRotation.rotation_angle = setRotAngle.getValue();

                    manmeta.setRotation(dataRotation);
                }

                std::vector<uint> sub_index;
                if(subDataset.isSet())
                {
                    MUSE::ExtractionMeta extrmeta;
                    extrmeta.read(app_folder + "/" + subDataset.getValue() + ".json");
                    sub_index = extrmeta.getDataExtraction().id_points;

                    dataRotation = extrmeta.getRotation();

                    std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                    std::cout << "Rotation axis: " << dataRotation.rotation_axis << std::endl;
                    std::cout << "Rotation center: [" << dataRotation.rotation_center_x << "; " << dataRotation.rotation_center_y << "; " << dataRotation.rotation_center_z << "]" <<  std::endl;
                    std::cout << "Rotation angle (degree): " << dataRotation.rotation_angle << std::endl;
                    std::cout << std::endl;

                    manmeta.setRotation(dataRotation);

                    dataProjection.subdataset_is_set = true;
                }
                else
                {
                    for(uint i=0; i< xCoord.size(); i++)
                        sub_index.push_back(i);
                }



                for(uint i:sub_index)
                {
                    cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));

                    if(dataRotation.rotation == true)
                    {
                        cinolib::vec3d axis = set_rotation_axis(dataRotation.rotation_axis);
                        cinolib::vec3d c (dataRotation.rotation_center_x, dataRotation.rotation_center_y, dataRotation.rotation_center_z);
                        sample = point_rotation(sample, axis, dataRotation.rotation_angle, c);
                    }
                    coord_samples.push_back(sample);
                }





                // 1) Lista dei file mesh in input su cui proiettare i punti

                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR set trimesh file to compute projections: --mgeom <file>" << std::endl;
                    exit(1);
                }

                std::vector<std::string> files = meshFiles.getValue();

                // Ciclo sui file mesh e procedo con le proiezioni
                for(uint i=0; i<files.size(); i++)
                {
                    cinolib::Trimesh<> mesh;
                    mesh.load(files.at(i).c_str());
                    std::cout << "\033[0;32mLoading mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;

                    std::string geom_name = files.at(i).substr(files.at(i).find_last_of("/")+1, files.at(i).length());
                    geom_name = get_basename(geom_name);

                    //std::vector<double> proj; //vettore delle proiezioni

                    std::vector<Point3D> proj_top, proj_bot; //vettore delle proiezioni

                    if(setProjDir.getValue().compare("Y") == 0)
                    {
                        for(uint j=0; j<coord_samples.size(); j++)
                        {
                            Point3D proj_max;
                            proj_max.x = coord_samples.at(j).x();
                            proj_max.y = -DBL_MAX;
                            proj_max.z = coord_samples.at(j).z();

                            Point3D proj_min;
                            proj_min.x = coord_samples.at(j).x();
                            proj_min.y = -DBL_MAX;
                            proj_min.z = coord_samples.at(j).z();

                            cinolib::vec3d p (coord_samples.at(j).x(), coord_samples.at(j).y(), coord_samples.at(j).z());
                            std::vector<double> proj = region_growing_for_projection(mesh, p);

                            std::cout << proj.at(0) << std::endl;
                            std::cout << proj.at(1) << std::endl;
                            std::cout << "the size is = " << proj.size() << std::endl;

                            for(uint ii=0; ii<proj.size(); ii++)
                            {
                                if(proj.at(ii) < proj_min.y)
                                    proj_min.y = proj.at(ii);

                                if(proj.at(ii) > proj_max.y)
                                    proj_max.y = proj.at(ii);
                            }

                            proj_top.push_back(proj_max);
                            proj_bot.push_back(proj_min);
                        }
                    }
                    else if (setProjDir.getValue().compare("Z") == 0)
                    {
                        exit(1);
                    }


                    dataProjection.mesh = geom_name;
                    dataProjection.proj_direction = setProjDir.getValue();
                    dataProjection.top_name = geom_name + "_top";
                    dataProjection.bottom_name = geom_name + "_bot";
                    manmeta.setDataProjection(dataProjection);
                    manmeta.write(app_folder + "/samples_" + geom_name + ".json");

                    export3d_xyz(app_folder + "/samples_" + dataProjection.top_name + ".dat", proj_top);
                    export3d_xyz(app_folder + "/samples_" + dataProjection.bottom_name + ".dat", proj_bot);

                    std::cout << "n top projections : " << proj_top.size() << std::endl;
                    std::cout << "n bot projections : " << proj_bot.size() << std::endl;

                    std::cout << "\033[0;32mComputing points projection on mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;
                    std::cout << std::endl;

                }
            }
        }

        std::cout << "Points manipulation... COMPLETED." << std::endl;

    }


    if(setProjectionOnSection.isSet())
    {
        std::string abs_datadir = out_folder + "/" + app_data;
        std::vector<std::string> list_dir = get_directories(abs_datadir);
        if(list_dir.empty())
            list_dir.push_back(abs_datadir);

        if((get_filename(list_dir.at(0)).compare("data") == 0 && get_filename(list_dir.at(1)).compare("metadata") == 0)
            || (get_filename(list_dir.at(1)).compare("data") == 0 && get_filename(list_dir.at(0)).compare("metadata") == 0))
        {
            list_dir.clear();
            list_dir.resize(1, abs_datadir);
        }

        int count_frame = 0;
        for(const std::string &l:list_dir)
        {
            count_frame++;

            filesystem::path dir = l;
            filesystem::path rel_datadir = filesystem::relative(dir, abs_datadir);
            std::cout << rel_datadir.string() << std::endl;

            app_folder.clear();
            app_folder = out_folder + "/" + app_name;
            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);

            if(rel_datadir.string().compare(".") != 0)
            {
                app_folder += "/" + rel_datadir.string();
                filesystem::create_directory(app_folder);

                std::cout << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << "### NUMBER OF TIME FRAMES: " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME N° " << count_frame << " ON " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME NAME: " << rel_datadir.string() << std::endl;
                std::cout << std::endl;
            }

            std::vector<std::string> list_json = get_files(l, ".json");
            if(list_json.size() > 1)
            {
                std::cerr << "ERROR. Only a file JSON is expected!" << std::endl;
                exit(1);
            }

            MUSE::ManipulateMeta manmeta;
            manmeta.setProject(Project);

            std::vector<std::string> excommands;
            excommands.push_back(command);
            manmeta.setCommands(excommands);

            // 0) Define meta for dependencies
            std::vector<std::string> depsextr;

            ManipulateMeta::DataProjection dataProjection;
            dataProjection.proj_is_set = true;
            dataProjection.data_type = setType.getValue();


            if(setType.getValue().compare("SAMPLES") == 0) //leggo in automatico le coordinate del set di dati (da json data)
            {
                std::vector<double> xCoord, yCoord, zCoord;
                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR. Set mesh files -m <filename>" << std::endl;
                    exit(1);
                }

                MUSE::DataMeta datameta;
                datameta.read(list_json.at(0));

                if(datameta.getInfoData().x_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().x_name + ".dat", xCoord);
                else
                {
                    std::cerr << "ERROR reading X coordinate: " << l + "/data/" + datameta.getInfoData().x_name + ".dat" << " NOT found." << std::endl;
                    exit(1);
                }

                if(datameta.getInfoData().y_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().y_name + ".dat", yCoord);
                else
                {
                    std::cerr << "ERROR reading Y coordinate: " << l + "/data/" + datameta.getInfoData().y_name + ".dat" << " NOT found." << std::endl;
                    exit(1);
                }

                if(datameta.getInfoData().z_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                else
                {
                    if(setZcoord.isSet())
                    {
                        readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                        depsextr.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().z_name + ".json", Project.folder));
                    }
                    else
                    {
                        std::cerr << "\033[0;32mWARNING: Z coordinate is Unknown. Set -z --name <variable> for setting the variable.\033[0;0m" << std::endl;
                        exit(1);
                    }
                }



                MUSE::Rotation dataRotation;

                std::vector<cinolib::vec3d> coord_samples;
                if(setRotAxis.isSet())
                {
                    dataRotation.rotation = true;
                    dataRotation.rotation_axis = setRotAxis.getValue();
                    dataRotation.rotation_center_x = setRotCenterX.getValue();
                    dataRotation.rotation_center_y = setRotCenterY.getValue();
                    dataRotation.rotation_center_z = setRotCenterZ.getValue();
                    dataRotation.rotation_angle = setRotAngle.getValue();

                    std::cout << std::endl;
                    std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                    std::cout << "Rotation axis: " << dataRotation.rotation_axis << std::endl;
                    std::cout << "Rotation center: [" << dataRotation.rotation_center_x << "; " << dataRotation.rotation_center_y << "; " << dataRotation.rotation_center_z << "]" <<  std::endl;
                    std::cout << "Rotation angle (degree): " << dataRotation.rotation_angle << std::endl;
                    std::cout << std::endl;


                    for(uint i=0; i< xCoord.size(); i++)
                    {
                        //rotazione coordinate all'inidice i
                        cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));
                        cinolib::vec3d axis = set_rotation_axis(dataRotation.rotation_axis);
                        cinolib::vec3d c (dataRotation.rotation_center_x, dataRotation.rotation_center_y, dataRotation.rotation_center_z);
                        sample = point_rotation(sample, axis, dataRotation.rotation_angle, c);

                        coord_samples.push_back(sample);
                    }

                    std::cout << FGRN("Rotation on data ... COMPLETED.") << std::endl;

                    manmeta.setRotation(dataRotation);
                }

                if(subDataset.isSet())
                {
                    MUSE::ExtractionMeta extrmeta;
                    extrmeta.read(app_folder + "/" + subDataset.getValue() + ".json");
                    std::cout << "Extraction sub-dataset is set. Reading ... " << app_folder + "/" + subDataset.getValue() + ".json" << std::endl;


                    //1) VERIFICARE ROTAZIONE DATI
                    MUSE::Rotation dataRotation = extrmeta.getRotation();
                    if(dataRotation.rotation == true)
                    {
                        std::cout << std::endl;
                        std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                        std::cout << "Rotation axis: " << dataRotation.rotation_axis << std::endl;
                        std::cout << "Rotation center: [" << dataRotation.rotation_center_x << "; " << dataRotation.rotation_center_y << "; " << dataRotation.rotation_center_z << "]" <<  std::endl;
                        std::cout << "Rotation angle (degree): " << dataRotation.rotation_angle << std::endl;
                        std::cout << std::endl;

                        //2) ESTRARRE SOTTODATASET DA INDICI
                        if(extrmeta.getDataExtraction().id_points.size() == 0)
                        {
                            std::cout << FRED("Vector of index is empty.") << std::endl;
                            exit(1);
                        }

                        for(uint i:extrmeta.getDataExtraction().id_points)
                        {
                            //rotazione coordinate all'inidice i
                            cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));
                            cinolib::vec3d axis = set_rotation_axis(dataRotation.rotation_axis);
                            cinolib::vec3d c (dataRotation.rotation_center_x, dataRotation.rotation_center_y, dataRotation.rotation_center_z);
                            sample = point_rotation(sample, axis, dataRotation.rotation_angle, c);

                            coord_samples.push_back(sample);
                        }
                        std::cout << FGRN("Rotation on data ... COMPLETED.") << std::endl;
                    }

                    manmeta.setRotation(dataRotation);

                }

                if(!subDataset.isSet() && !setRotAxis.isSet())
                {
                    for(uint i=0; i< xCoord.size(); i++)
                    {
                        //rotazione coordinate all'inidice i
                        cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));
                        coord_samples.push_back(sample);
                    }
                }


                if(subDataset.isSet())
                    depsextr.push_back(filesystem::relative(app_folder + "/" + subDataset.getValue() + ".json", Project.folder));
                else
                {
                    depsextr.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().x_name + ".json", Project.folder));
                    depsextr.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().y_name + ".json", Project.folder));
                    depsextr.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().z_name + ".json", Project.folder));
                }



                // 1) Lista dei file mesh in input su cui proiettare i punti

                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR set trimesh file to compute projections: --mgeom <file>" << std::endl;
                    exit(1);
                }
                std::vector<std::string> files = meshFiles.getValue();

                // Ciclo sui file mesh e procedo con le proiezioni
                for(uint i=0; i<files.size(); i++)
                {
                    // Carico la mesh in input
                    cinolib::Trimesh<> mesh;
                    mesh.load(files.at(i).c_str());
                    std::cout << "\033[0;32mLoading mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;

                    std::string geom_name = files.at(i).substr(files.at(i).find_last_of("/")+1, files.at(i).length());
                    geom_name = get_basename(geom_name);

                    std::vector<Point3D> proj_top, proj_bot; //vettore delle proiezioni

                    std::vector<uint> vert_bound = mesh.get_ordered_boundary_vertices();


                    if(setProjDir.getValue().compare("Y") == 0)
                    {
                        double bby_max = mesh.bbox().max.y();
                        double bby_min = mesh.bbox().min.y();

                        for(uint j=0; j<coord_samples.size(); j++)
                        {
                            //std::cout << "ycoord = " <<  coord_samples.at(j).y() << std::endl;

                            Point3D proj_max;
                            proj_max.y = -DBL_MAX;

                            Point3D proj_min;
                            proj_min.y = DBL_MAX;


                            cinolib::vec2d p1 (coord_samples.at(j).x(), bby_min); //bottom
                            cinolib::vec2d p2 (coord_samples.at(j).x(), bby_max); //top

                            //std::cout << "p1 = " <<  p1 << std::endl;
                            //std::cout << "p2 = " <<  p2 << std::endl;

                            for(uint vidb = 0; vidb < vert_bound.size(); vidb ++)
                            {
                                uint curr = vert_bound.at(vidb);

                                uint next;
                                if(vidb < vert_bound.size()-1)
                                    next = vert_bound.at(vidb + 1);
                                else
                                    next = vert_bound.at(0);

                                cinolib::vec3d v0 = mesh.vert(curr);
                                cinolib::vec3d v1 = mesh.vert(next);

                                cinolib::vec2d v02d(v0.x(), v0.y());
                                cinolib::vec2d v12d(v1.x(), v1.y());


                                if(cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 2 || cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 1)
                                {
                                    //std::cout << "v02d = " <<  v02d << std::endl;
                                    //std::cout << "v12d = " <<  v12d << std::endl;

                                    cinolib::vec2d proj = segment_segment_intersection_2d(p1, p2, v02d, v12d);

                                    Point3D p_proj;
                                    p_proj.z = coord_samples.at(j).z();

                                    if(proj.y() != DBL_MAX)
                                    {
                                        p_proj.x = proj.x();
                                        p_proj.y = proj.y();
                                    }
                                    else
                                    {
                                        cinolib::vec2d p (coord_samples.at(j).x(), coord_samples.at(j).y());
                                        if(p.dist(v02d) <= p.dist(v12d))
                                        {
                                            p_proj.y = v02d.y();
                                            p_proj.x = coord_samples.at(j).x();
                                        }
                                        else
                                        {
                                            p_proj.y = v12d.y();
                                            p_proj.x = coord_samples.at(j).x();
                                        }
                                    }


                                    if(p_proj.y < proj_min.y)
                                        proj_min = p_proj;

                                    if(p_proj.y > proj_max.y)
                                        proj_max = p_proj;
                                }
                            }

                            proj_bot.push_back(proj_min);
                            proj_top.push_back(proj_max);

                        }
                    }
                    else if(setProjDir.getValue().compare("X") == 0)
                    {
                        double bbx_max = mesh.bbox().max.x();
                        double bbx_min = mesh.bbox().min.x();

                        for(uint j=0; j<coord_samples.size(); j++)
                        {
                            //std::cout << "ycoord = " <<  coord_samples.at(j).y() << std::endl;

                            Point3D proj_max;
                            proj_max.x = -DBL_MAX;

                            Point3D proj_min;
                            proj_min.x = DBL_MAX;


                            cinolib::vec2d p1 (bbx_min, coord_samples.at(j).y()); //bottom
                            cinolib::vec2d p2 (bbx_max, coord_samples.at(j).y()); //top

                            for(uint vidb = 0; vidb < vert_bound.size(); vidb ++)
                            {
                                uint curr = vert_bound.at(vidb);

                                uint next;
                                if(vidb < vert_bound.size()-1)
                                    next = vert_bound.at(vidb + 1);
                                else
                                    next = vert_bound.at(0);

                                cinolib::vec3d v0 = mesh.vert(curr);
                                cinolib::vec3d v1 = mesh.vert(next);

                                cinolib::vec2d v02d(v0.x(), v0.y());
                                cinolib::vec2d v12d(v1.x(), v1.y());

                                if(cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 2 || cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 1)
                                {

                                    cinolib::vec2d proj = segment_segment_intersection_2d(p1, p2, v02d, v12d);

                                    Point3D p_proj;
                                    p_proj.z = coord_samples.at(j).z();

                                    if(proj.x() != DBL_MAX)
                                    {
                                        p_proj.x = proj.x();
                                        p_proj.y = proj.y();
                                    }
                                    else
                                    {
                                        cinolib::vec2d p (coord_samples.at(j).x(), coord_samples.at(j).y());
                                        if(p.dist(v02d) <= p.dist(v12d))
                                        {
                                            p_proj.x = v02d.x();
                                            p_proj.y = coord_samples.at(j).y();
                                        }
                                        else
                                        {
                                            p_proj.x = v12d.x();
                                            p_proj.y = coord_samples.at(j).y();
                                        }
                                    }


                                    if(p_proj.x < proj_min.x)
                                        proj_min = p_proj;

                                    if(p_proj.x > proj_max.x)
                                        proj_max = p_proj;
                                }
                            }

                            proj_bot.push_back(proj_min);
                            proj_top.push_back(proj_max);
                        }

                    }

                    dataProjection.mesh = geom_name;
                    dataProjection.proj_direction = setProjDir.getValue();
                    dataProjection.top_name = geom_name + "_top";
                    dataProjection.bottom_name = geom_name + "_bot";

                    manmeta.setDependencies(depsextr); //added dependencies
                    manmeta.setDataProjection(dataProjection);
                    manmeta.write(app_folder + "/samples_" + geom_name + ".json");


                    export3d_xyz(app_folder + "/samples_" + dataProjection.top_name + ".dat", proj_top);
                    export3d_xyz(app_folder + "/samples_" + dataProjection.bottom_name + ".dat", proj_bot);

                    std::cout << "n top projections : " << proj_top.size() << std::endl;
                    std::cout << "n bot projections : " << proj_bot.size() << std::endl;

                    std::cout << "\033[0;32mComputing points projection on mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;
                    std::cout << std::endl;
                }

            }

            if(setType.getValue().compare("GEOMETRY") == 0)
            {
                if(!geomModel.isSet())
                {
                    std::cout << "ERROR set trimesh --geom <file>"<< std::endl;
                    exit(1);
                }

                cinolib::Trimesh<> section;
                section.load(geomModel.getValue().c_str());
                std::cout << "\033[0;32mLoading mesh file: " << geomModel.getValue() << " ... COMPLETED.\033[0m" << std::endl;
                std::cout << std::endl;

                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR set trimesh file to compute projections: --mgeom <file>" << std::endl;
                    exit(1);
                }

                std::vector<std::string> files = meshFiles.getValue();
                for(uint i=0; i<files.size(); i++)
                {
                    cinolib::Trimesh<> mesh;
                    mesh.load(files.at(i).c_str());
                    std::cout << "\033[0;32mLoading mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;

                    depsextr.push_back(filesystem::relative(get_basename(files.at(i)) + ".json", Project.folder));

                    std::string geom_name = files.at(i).substr(files.at(i).find_last_of("/")+1, files.at(i).length());
                    geom_name = get_basename(geom_name);

                    std::vector<Point3D> proj_top, proj_bot; //vettore delle proiezioni

                    std::vector<uint> vert_bound = mesh.get_ordered_boundary_vertices();

                    if(setProjDir.getValue().compare("Y") == 0)
                    {
                        double bby_max = mesh.bbox().max.y();
                        double bby_min = mesh.bbox().min.y();

                        //In questo caso i punti da proiettare sono i vertici interni della mesh

                        for(uint vid=0; vid<section.num_verts(); vid++)
                        {
                            //cinolib::vec3d p (model.vert(vid).x(), model.vert(vid).y(), model.vert(vid).z());

                            //std::vector<cinolib::vec2d> proiezioni;

                            Point3D proj_max;
                            proj_max.y = -DBL_MAX;

                            Point3D proj_min;
                            proj_min.y = DBL_MAX;


                            cinolib::vec2d p1 (section.vert(vid).x(), bby_min); //bottom
                            cinolib::vec2d p2 (section.vert(vid).x(), bby_max); //top

        //                    std::cout << "p1 = " <<  p1 << std::endl;
        //                    std::cout << "p2 = " <<  p2 << std::endl;

                            for(uint vidb = 0; vidb < vert_bound.size(); vidb ++)
                            {
                                uint curr = vert_bound.at(vidb);

                                uint next;
                                if(vidb < vert_bound.size()-1)
                                    next = vert_bound.at(vidb + 1);
                                else
                                    next = vert_bound.at(0);

                                cinolib::vec3d v0 = mesh.vert(curr);
                                cinolib::vec3d v1 = mesh.vert(next);

                                cinolib::vec2d v02d(v0.x(), v0.y());
                                cinolib::vec2d v12d(v1.x(), v1.y());


                                if(cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 2 || cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 1)
                                {
                                    //std::cout << "v02d = " <<  v02d << std::endl;
                                    //std::cout << "v12d = " <<  v12d << std::endl;

                                    cinolib::vec2d proj = segment_segment_intersection_2d(p1, p2, v02d, v12d);

                                    Point3D p_proj;
                                    p_proj.z = section.vert(vid).z();
                                    //p_proj.y = proj.y();

                                    if(proj.y() != DBL_MAX)
                                    {
                                        p_proj.x = proj.x();
                                        p_proj.y = proj.y();
                                    }
                                    else
                                    {
                                        cinolib::vec2d p (section.vert(vid).x(), section.vert(vid).y());
                                        if(p.dist(v02d) <= p.dist(v12d))
                                        {
                                            p_proj.y = v02d.y();
                                            p_proj.x = section.vert(vid).x();
                                        }
                                        else
                                        {
                                            p_proj.y = v12d.y();
                                            p_proj.x = section.vert(vid).x();
                                        }
                                    }


                                    if(p_proj.y < proj_min.y)
                                        proj_min = p_proj;

                                    if(p_proj.y > proj_max.y)
                                        proj_max = p_proj;
                                }
                            }
                            proj_top.push_back(proj_max);
                            proj_bot.push_back(proj_min);

                        }
                    }
                    else if(setProjDir.getValue().compare("X") == 0)
                    {
                        double bbx_max = mesh.bbox().max.x();
                        double bbx_min = mesh.bbox().min.x();

                        //In questo caso i punti da proiettare sono i vertici interni della mesh

                        for(uint vid=0; vid<section.num_verts(); vid++)
                        {
                            //cinolib::vec3d p (model.vert(vid).x(), model.vert(vid).y(), model.vert(vid).z());

                            //std::vector<cinolib::vec2d> proiezioni;

                            Point3D proj_max;
                            proj_max.y = -DBL_MAX;

                            Point3D proj_min;
                            proj_min.y = DBL_MAX;


                            cinolib::vec2d p1 (bbx_min, section.vert(vid).y()); //bottom
                            cinolib::vec2d p2 (bbx_max, section.vert(vid).y()); //top

                            for(uint vidb = 0; vidb < vert_bound.size(); vidb ++)
                            {
                                uint curr = vert_bound.at(vidb);

                                uint next;
                                if(vidb < vert_bound.size()-1)
                                    next = vert_bound.at(vidb + 1);
                                else
                                    next = vert_bound.at(0);

                                cinolib::vec3d v0 = mesh.vert(curr);
                                cinolib::vec3d v1 = mesh.vert(next);

                                cinolib::vec2d v02d(v0.x(), v0.y());
                                cinolib::vec2d v12d(v1.x(), v1.y());


                                if(cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 2 || cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 1)
                                {
                                    //std::cout << "v02d = " <<  v02d << std::endl;
                                    //std::cout << "v12d = " <<  v12d << std::endl;

                                    cinolib::vec2d proj = segment_segment_intersection_2d(p1, p2, v02d, v12d);

                                    Point3D p_proj;
                                    p_proj.z = section.vert(vid).z();
                                    //p_proj.y = proj.y();

                                    if(proj.x() != DBL_MAX)
                                    {
                                        p_proj.x = proj.x();
                                        p_proj.y = proj.y();
                                    }
                                    else
                                    {
                                        cinolib::vec2d p (section.vert(vid).x(), section.vert(vid).y());
                                        if(p.dist(v02d) <= p.dist(v12d))
                                        {
                                            p_proj.x = v02d.x();
                                            p_proj.y = section.vert(vid).y();
                                        }
                                        else
                                        {
                                            p_proj.x = v12d.x();
                                            p_proj.y = section.vert(vid).y();
                                        }
                                    }


                                    if(p_proj.x < proj_min.x)
                                        proj_min = p_proj;

                                    if(p_proj.x > proj_max.x)
                                        proj_max = p_proj;
                                }
                            }
                            proj_top.push_back(proj_max);
                            proj_bot.push_back(proj_min);
                        }
                    }

                    dataProjection.mesh = geom_name;
                    dataProjection.proj_direction = setProjDir.getValue();
                    dataProjection.top_name = geom_name + "_top";
                    dataProjection.bottom_name = geom_name + "_bot";

                    manmeta.setDependencies(depsextr); //added dependencies
                    manmeta.setDataProjection(dataProjection);
                    manmeta.write(app_folder + "/geom_" + geom_name + ".json");


                    export3d_xyz(app_folder + "/geom_" + dataProjection.top_name + ".dat", proj_top);
                    export3d_xyz(app_folder + "/geom_" + dataProjection.bottom_name + ".dat", proj_bot);

                    std::cout << "n top projections : " << proj_top.size() << std::endl;
                    std::cout << "n bot projections : " << proj_bot.size() << std::endl;

                    std::cout << "\033[0;32mComputing points projection on mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;
                    std::cout << std::endl;
                }
            }
        }

        std::cout << "Points projection on section boundary ... COMPLETED." << std::endl;

    }


    if(setProjectionOnQSection.isSet())
    {
        std::string abs_datadir = out_folder + "/" + app_data;
        std::vector<std::string> list_dir = get_directories(abs_datadir);
        if(list_dir.empty())
            list_dir.push_back(abs_datadir);

        if((get_filename(list_dir.at(0)).compare("data") == 0 && get_filename(list_dir.at(1)).compare("metadata") == 0)
            || (get_filename(list_dir.at(1)).compare("data") == 0 && get_filename(list_dir.at(0)).compare("metadata") == 0))
        {
            list_dir.clear();
            list_dir.resize(1, abs_datadir);
        }

        int count_frame = 0;
        for(const std::string &l:list_dir)
        {
            count_frame++;

            filesystem::path dir = l;
            filesystem::path rel_datadir = filesystem::relative(dir, abs_datadir);
            std::cout << rel_datadir.string() << std::endl;

            app_folder.clear();
            app_folder = out_folder + "/" + app_name;
            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);

            if(rel_datadir.string().compare(".") != 0)
            {
                app_folder += "/" + rel_datadir.string();
                filesystem::create_directory(app_folder);

                std::cout << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << "### NUMBER OF TIME FRAMES: " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME N° " << count_frame << " ON " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME NAME: " << rel_datadir.string() << std::endl;
                std::cout << std::endl;
            }

            std::vector<std::string> list_json = get_files(l, ".json");
            if(list_json.size() > 1)
            {
                std::cerr << "ERROR. Only a file JSON is expected!" << std::endl;
                exit(1);
            }

            MUSE::ManipulateMeta manmeta;
            manmeta.setProject(Project);

            ManipulateMeta::DataProjection dataProjection;
            dataProjection.proj_is_set = true;
            dataProjection.data_type = setType.getValue();

            if(setType.getValue().compare("SAMPLES") == 0) //leggo in automatico le coordinate del set di dati (da json data)
            {
                std::vector<double> xCoord, yCoord, zCoord;
                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR. Set mesh files -m <filename>" << std::endl;
                    exit(1);
                }


                MUSE::DataMeta datameta;
                datameta.read(list_json.at(0));

                if(datameta.getInfoData().x_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().x_name + ".dat", xCoord);
                else
                {
                    std::cerr << "ERROR reading X coordinate: " << l + "/data/" + datameta.getInfoData().x_name + ".dat" << " NOT found." << std::endl;
                    exit(1);
                }

                if(datameta.getInfoData().y_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().y_name + ".dat", yCoord);
                else
                {
                    std::cerr << "ERROR reading Y coordinate: " << l + "/data/" + datameta.getInfoData().y_name + ".dat" << " NOT found." << std::endl;
                    exit(1);
                }

                if(datameta.getInfoData().z_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                else
                {
                    if(setZcoord.isSet())
                        readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                    else
                    {
                        std::cerr << "\033[0;32mWARNING: Z coordinate is Unknown. Set -z --name <variable> for setting the variable.\033[0;0m" << std::endl;
                        exit(1);
                    }
                }



                MUSE::Rotation dataRotation;

                std::vector<cinolib::vec3d> coord_samples;
                if(setRotAxis.isSet())
                {
                    dataRotation.rotation = true;
                    dataRotation.rotation_axis = setRotAxis.getValue();
                    dataRotation.rotation_center_x = setRotCenterX.getValue();
                    dataRotation.rotation_center_y = setRotCenterY.getValue();
                    dataRotation.rotation_center_z = setRotCenterZ.getValue();
                    dataRotation.rotation_angle = setRotAngle.getValue();

                    std::cout << std::endl;
                    std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                    std::cout << "Rotation axis: " << dataRotation.rotation_axis << std::endl;
                    std::cout << "Rotation center: [" << dataRotation.rotation_center_x << "; " << dataRotation.rotation_center_y << "; " << dataRotation.rotation_center_z << "]" <<  std::endl;
                    std::cout << "Rotation angle (degree): " << dataRotation.rotation_angle << std::endl;
                    std::cout << std::endl;


                    for(uint i=0; i< xCoord.size(); i++)
                    {
                        //rotazione coordinate all'inidice i
                        cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));
                        cinolib::vec3d axis = set_rotation_axis(dataRotation.rotation_axis);
                        cinolib::vec3d c (dataRotation.rotation_center_x, dataRotation.rotation_center_y, dataRotation.rotation_center_z);
                        sample = point_rotation(sample, axis, dataRotation.rotation_angle, c);

                        coord_samples.push_back(sample);
                    }

                    std::cout << FGRN("Rotation on data ... COMPLETED.") << std::endl;

                    manmeta.setRotation(dataRotation);
                }

                if(subDataset.isSet())
                {
                    MUSE::ExtractionMeta extrmeta;
                    extrmeta.read(app_folder + "/" + subDataset.getValue() + ".json");
                    std::cout << "Extraction sub-dataset is set. Reading ... " << app_folder + "/" + subDataset.getValue() + ".json" << std::endl;

                    //1) VERIFICARE ROTAZIONE DATI
                    MUSE::Rotation dataRotation = extrmeta.getRotation();
                    if(dataRotation.rotation == true)
                    {
                        std::cout << std::endl;
                        std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                        std::cout << "Rotation axis: " << dataRotation.rotation_axis << std::endl;
                        std::cout << "Rotation center: [" << dataRotation.rotation_center_x << "; " << dataRotation.rotation_center_y << "; " << dataRotation.rotation_center_z << "]" <<  std::endl;
                        std::cout << "Rotation angle (degree): " << dataRotation.rotation_angle << std::endl;
                        std::cout << std::endl;

                        //2) ESTRARRE SOTTODATASET DA INDICI
                        if(extrmeta.getDataExtraction().id_points.size() == 0)
                        {
                            std::cout << FRED("Vector of index is empty.") << std::endl;
                            exit(1);
                        }

                        for(uint i:extrmeta.getDataExtraction().id_points)
                        {
                            //rotazione coordinate all'inidice i
                            cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));
                            cinolib::vec3d axis = set_rotation_axis(dataRotation.rotation_axis);
                            cinolib::vec3d c (dataRotation.rotation_center_x, dataRotation.rotation_center_y, dataRotation.rotation_center_z);
                            sample = point_rotation(sample, axis, dataRotation.rotation_angle, c);

                            coord_samples.push_back(sample);
                        }
                        std::cout << FGRN("Rotation on data ... COMPLETED.") << std::endl;
                    }

                    manmeta.setRotation(dataRotation);
                }




                // 1) Lista dei file mesh in input su cui proiettare i punti

                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR set trimesh file to compute projections: --mgeom <file>" << std::endl;
                    exit(1);
                }
                std::vector<std::string> files = meshFiles.getValue();

                // Ciclo sui file mesh e procedo con le proiezioni
                for(uint i=0; i<files.size(); i++)
                {
                    // Carico la mesh in input
                    MUSE::SurfaceMesh<> mesh;
                    mesh.load(files.at(i).c_str());
                    std::cout << "\033[0;32mLoading mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;

                    std::string geom_name = files.at(i).substr(files.at(i).find_last_of("/")+1, files.at(i).length());
                    geom_name = get_basename(geom_name);

                    std::vector<Point3D> proj_top, proj_bot; //vettore delle proiezioni

                    std::vector<uint> vert_bound = mesh.get_ordered_boundary_vertices();


                    if(setProjDir.getValue().compare("Y") == 0)
                    {
                        double bby_max = mesh.bbox().max.y();
                        double bby_min = mesh.bbox().min.y();

                        for(uint j=0; j<coord_samples.size(); j++)
                        {
                            //std::cout << "ycoord = " <<  coord_samples.at(j).y() << std::endl;

                            Point3D proj_max;
                            proj_max.y = -DBL_MAX;

                            Point3D proj_min;
                            proj_min.y = DBL_MAX;


                            cinolib::vec2d p1 (coord_samples.at(j).x(), bby_min); //bottom
                            cinolib::vec2d p2 (coord_samples.at(j).x(), bby_max); //top

                            //std::cout << "p1 = " <<  p1 << std::endl;
                            //std::cout << "p2 = " <<  p2 << std::endl;

                            for(uint vidb = 0; vidb < vert_bound.size(); vidb ++)
                            {
                                uint curr = vert_bound.at(vidb);

                                uint next;
                                if(vidb < vert_bound.size()-1)
                                    next = vert_bound.at(vidb + 1);
                                else
                                    next = vert_bound.at(0);

                                cinolib::vec3d v0 = mesh.vert(curr);
                                cinolib::vec3d v1 = mesh.vert(next);

                                cinolib::vec2d v02d(v0.x(), v0.y());
                                cinolib::vec2d v12d(v1.x(), v1.y());


                                if(cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 2 || cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 1)
                                {
                                    //std::cout << "v02d = " <<  v02d << std::endl;
                                    //std::cout << "v12d = " <<  v12d << std::endl;

                                    cinolib::vec2d proj = segment_segment_intersection_2d(p1, p2, v02d, v12d);

                                    Point3D p_proj;
                                    p_proj.z = coord_samples.at(j).z();

                                    if(proj.y() != DBL_MAX)
                                    {
                                        p_proj.x = proj.x();
                                        p_proj.y = proj.y();
                                    }
                                    else
                                    {
                                        cinolib::vec2d p (coord_samples.at(j).x(), coord_samples.at(j).y());
                                        if(p.dist(v02d) <= p.dist(v12d))
                                        {
                                            p_proj.y = v02d.y();
                                            p_proj.x = coord_samples.at(j).x();
                                        }
                                        else
                                        {
                                            p_proj.y = v12d.y();
                                            p_proj.x = coord_samples.at(j).x();
                                        }
                                    }


                                    if(p_proj.y < proj_min.y)
                                        proj_min = p_proj;

                                    if(p_proj.y > proj_max.y)
                                        proj_max = p_proj;
                                }
                            }

                            proj_bot.push_back(proj_min);
                            proj_top.push_back(proj_max);

                        }
                    }
                    else if(setProjDir.getValue().compare("X") == 0)
                    {
                        double bbx_max = mesh.bbox().max.x();
                        double bbx_min = mesh.bbox().min.x();

                        for(uint j=0; j<coord_samples.size(); j++)
                        {
                            //std::cout << "ycoord = " <<  coord_samples.at(j).y() << std::endl;

                            Point3D proj_max;
                            proj_max.x = -DBL_MAX;

                            Point3D proj_min;
                            proj_min.x = DBL_MAX;


                            cinolib::vec2d p1 (bbx_min, coord_samples.at(j).y()); //bottom
                            cinolib::vec2d p2 (bbx_max, coord_samples.at(j).y()); //top

                            for(uint vidb = 0; vidb < vert_bound.size(); vidb ++)
                            {
                                uint curr = vert_bound.at(vidb);

                                uint next;
                                if(vidb < vert_bound.size()-1)
                                    next = vert_bound.at(vidb + 1);
                                else
                                    next = vert_bound.at(0);

                                cinolib::vec3d v0 = mesh.vert(curr);
                                cinolib::vec3d v1 = mesh.vert(next);

                                cinolib::vec2d v02d(v0.x(), v0.y());
                                cinolib::vec2d v12d(v1.x(), v1.y());

                                if(cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 2 || cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 1)
                                {

                                    cinolib::vec2d proj = segment_segment_intersection_2d(p1, p2, v02d, v12d);

                                    Point3D p_proj;
                                    p_proj.z = coord_samples.at(j).z();

                                    if(proj.x() != DBL_MAX)
                                    {
                                        p_proj.x = proj.x();
                                        p_proj.y = proj.y();
                                    }
                                    else
                                    {
                                        cinolib::vec2d p (coord_samples.at(j).x(), coord_samples.at(j).y());
                                        if(p.dist(v02d) <= p.dist(v12d))
                                        {
                                            p_proj.x = v02d.x();
                                            p_proj.y = coord_samples.at(j).y();
                                        }
                                        else
                                        {
                                            p_proj.x = v12d.x();
                                            p_proj.y = coord_samples.at(j).y();
                                        }
                                    }


                                    if(p_proj.x < proj_min.x)
                                        proj_min = p_proj;

                                    if(p_proj.x > proj_max.x)
                                        proj_max = p_proj;
                                }
                            }

                            proj_bot.push_back(proj_min);
                            proj_top.push_back(proj_max);
                        }

                    }

                    dataProjection.mesh = geom_name;
                    dataProjection.proj_direction = setProjDir.getValue();
                    dataProjection.top_name = geom_name + "_top";
                    dataProjection.bottom_name = geom_name + "_bot";
                    manmeta.setDataProjection(dataProjection);
                    manmeta.write(app_folder + "/samples_" + geom_name + ".json");


                    export3d_xyz(app_folder + "/samples_" + dataProjection.top_name + ".dat", proj_top);
                    export3d_xyz(app_folder + "/samples_" + dataProjection.bottom_name + ".dat", proj_bot);

                    std::cout << "n top projections : " << proj_top.size() << std::endl;
                    std::cout << "n bot projections : " << proj_bot.size() << std::endl;

                    std::cout << "\033[0;32mComputing points projection on mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;
                    std::cout << std::endl;
                }
            }

            if(setType.getValue().compare("GEOMETRY") == 0)
            {
                if(!geomModel.isSet())
                {
                    std::cout << "ERROR set trimesh --geom <file>"<< std::endl;
                    exit(1);
                }

                MUSE::SurfaceMesh<> section;
                section.load(geomModel.getValue().c_str());
                std::cout << "\033[0;32mLoading mesh file: " << geomModel.getValue() << " ... COMPLETED.\033[0m" << std::endl;
                std::cout << std::endl;


                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR set trimesh file to compute projections: --mgeom <file>" << std::endl;
                    exit(1);
                }

                std::vector<std::string> files = meshFiles.getValue();
                for(uint i=0; i<files.size(); i++)
                {
                    MUSE::SurfaceMesh<> mesh;
                    mesh.load(files.at(i).c_str());
                    std::cout << "\033[0;32mLoading mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;

                    std::string geom_name = files.at(i).substr(files.at(i).find_last_of("/")+1, files.at(i).length());
                    geom_name = get_basename(geom_name);

                    std::vector<Point3D> proj_top, proj_bot; //vettore delle proiezioni

                    std::vector<uint> vert_bound = mesh.get_ordered_boundary_vertices();

                    if(setProjDir.getValue().compare("Y") == 0)
                    {
                        double bby_max = mesh.bbox().max.y();
                        double bby_min = mesh.bbox().min.y();

                        //In questo caso i punti da proiettare sono i vertici interni della mesh

                        for(uint vid=0; vid<section.num_verts(); vid++)
                        {
                            //cinolib::vec3d p (model.vert(vid).x(), model.vert(vid).y(), model.vert(vid).z());

                            //std::vector<cinolib::vec2d> proiezioni;

                            Point3D proj_max;
                            proj_max.y = -DBL_MAX;

                            Point3D proj_min;
                            proj_min.y = DBL_MAX;


                            cinolib::vec2d p1 (section.vert(vid).x(), bby_min); //bottom
                            cinolib::vec2d p2 (section.vert(vid).x(), bby_max); //top

        //                    std::cout << "p1 = " <<  p1 << std::endl;
        //                    std::cout << "p2 = " <<  p2 << std::endl;

                            for(uint vidb = 0; vidb < vert_bound.size(); vidb ++)
                            {
                                uint curr = vert_bound.at(vidb);

                                uint next;
                                if(vidb < vert_bound.size()-1)
                                    next = vert_bound.at(vidb + 1);
                                else
                                    next = vert_bound.at(0);

                                cinolib::vec3d v0 = mesh.vert(curr);
                                cinolib::vec3d v1 = mesh.vert(next);

                                cinolib::vec2d v02d(v0.x(), v0.y());
                                cinolib::vec2d v12d(v1.x(), v1.y());


                                if(cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 2 || cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 1)
                                {
                                    //std::cout << "v02d = " <<  v02d << std::endl;
                                    //std::cout << "v12d = " <<  v12d << std::endl;

                                    cinolib::vec2d proj = segment_segment_intersection_2d(p1, p2, v02d, v12d);

                                    Point3D p_proj;
                                    p_proj.z = section.vert(vid).z();
                                    //p_proj.y = proj.y();

                                    if(proj.y() != DBL_MAX)
                                    {
                                        p_proj.x = proj.x();
                                        p_proj.y = proj.y();
                                    }
                                    else
                                    {
                                        cinolib::vec2d p (section.vert(vid).x(), section.vert(vid).y());
                                        if(p.dist(v02d) <= p.dist(v12d))
                                        {
                                            p_proj.y = v02d.y();
                                            p_proj.x = section.vert(vid).x();
                                        }
                                        else
                                        {
                                            p_proj.y = v12d.y();
                                            p_proj.x = section.vert(vid).x();
                                        }
                                    }


                                    if(p_proj.y < proj_min.y)
                                        proj_min = p_proj;

                                    if(p_proj.y > proj_max.y)
                                        proj_max = p_proj;
                                }
                            }
                            proj_top.push_back(proj_max);
                            proj_bot.push_back(proj_min);

                        }
                    }
                    else if(setProjDir.getValue().compare("X") == 0)
                    {
                        double bbx_max = mesh.bbox().max.x();
                        double bbx_min = mesh.bbox().min.x();

                        //In questo caso i punti da proiettare sono i vertici interni della mesh

                        for(uint vid=0; vid<section.num_verts(); vid++)
                        {
                            //cinolib::vec3d p (model.vert(vid).x(), model.vert(vid).y(), model.vert(vid).z());

                            //std::vector<cinolib::vec2d> proiezioni;

                            Point3D proj_max;
                            proj_max.y = -DBL_MAX;

                            Point3D proj_min;
                            proj_min.y = DBL_MAX;


                            cinolib::vec2d p1 (bbx_min, section.vert(vid).y()); //bottom
                            cinolib::vec2d p2 (bbx_max, section.vert(vid).y()); //top

                            for(uint vidb = 0; vidb < vert_bound.size(); vidb ++)
                            {
                                uint curr = vert_bound.at(vidb);

                                uint next;
                                if(vidb < vert_bound.size()-1)
                                    next = vert_bound.at(vidb + 1);
                                else
                                    next = vert_bound.at(0);

                                cinolib::vec3d v0 = mesh.vert(curr);
                                cinolib::vec3d v1 = mesh.vert(next);

                                cinolib::vec2d v02d(v0.x(), v0.y());
                                cinolib::vec2d v12d(v1.x(), v1.y());


                                if(cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 2 || cinolib::segment_segment_intersect_2d(p1, p2, v02d, v12d) == 1)
                                {
                                    //std::cout << "v02d = " <<  v02d << std::endl;
                                    //std::cout << "v12d = " <<  v12d << std::endl;

                                    cinolib::vec2d proj = segment_segment_intersection_2d(p1, p2, v02d, v12d);

                                    Point3D p_proj;
                                    p_proj.z = section.vert(vid).z();
                                    //p_proj.y = proj.y();

                                    if(proj.x() != DBL_MAX)
                                    {
                                        p_proj.x = proj.x();
                                        p_proj.y = proj.y();
                                    }
                                    else
                                    {
                                        cinolib::vec2d p (section.vert(vid).x(), section.vert(vid).y());
                                        if(p.dist(v02d) <= p.dist(v12d))
                                        {
                                            p_proj.x = v02d.x();
                                            p_proj.y = section.vert(vid).y();
                                        }
                                        else
                                        {
                                            p_proj.x = v12d.x();
                                            p_proj.y = section.vert(vid).y();
                                        }
                                    }


                                    if(p_proj.x < proj_min.x)
                                        proj_min = p_proj;

                                    if(p_proj.x > proj_max.x)
                                        proj_max = p_proj;
                                }
                            }
                            proj_top.push_back(proj_max);
                            proj_bot.push_back(proj_min);
                        }
                    }

                    dataProjection.mesh = geom_name;
                    dataProjection.proj_direction = setProjDir.getValue();
                    dataProjection.top_name = geom_name + "_top";
                    dataProjection.bottom_name = geom_name + "_bot";
                    manmeta.setDataProjection(dataProjection);
                    manmeta.write(app_folder + "/geom_" + geom_name + ".json");


                    export3d_xyz(app_folder + "/geom_" + dataProjection.top_name + ".dat", proj_top);
                    export3d_xyz(app_folder + "/geom_" + dataProjection.bottom_name + ".dat", proj_bot);

                    std::cout << "n top projections : " << proj_top.size() << std::endl;
                    std::cout << "n bot projections : " << proj_bot.size() << std::endl;

                    std::cout << "\033[0;32mComputing points projection on mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;
                    std::cout << std::endl;
                }
            }
        }

        std::cout << "Points projection on section boundary ... COMPLETED." << std::endl;
    }

    /////////////VERSIONE STRATIGRAFICHE PIÙ AGGIORNATA!! (20-02-2025: testate sul Polcevera)
    if(setProjectionOnVolume.isSet())
    {
        std::string abs_datadir = out_folder + "/" + app_data;
        std::vector<std::string> list_dir = get_directories(abs_datadir);
        if(list_dir.empty())
            list_dir.push_back(abs_datadir);

        if((get_filename(list_dir.at(0)).compare("data") == 0 && get_filename(list_dir.at(1)).compare("metadata") == 0)
            || (get_filename(list_dir.at(1)).compare("data") == 0 && get_filename(list_dir.at(0)).compare("metadata") == 0))
        {
            list_dir.clear();
            list_dir.resize(1, abs_datadir);
        }

        int count_frame = 0;
        for(const std::string &l:list_dir)
        {
            count_frame++;

            filesystem::path dir = l;
            filesystem::path rel_datadir = filesystem::relative(dir, abs_datadir);
            std::cout << rel_datadir.string() << std::endl;

            app_folder.clear();
            app_folder = out_folder + "/" + app_name;
            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);

            if(rel_datadir.string().compare(".") != 0)
            {
                app_folder += "/" + rel_datadir.string();
                filesystem::create_directory(app_folder);

                std::cout << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << "### NUMBER OF TIME FRAMES: " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME N° " << count_frame << " ON " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME NAME: " << rel_datadir.string() << std::endl;
                std::cout << std::endl;
            }

            std::vector<std::string> list_json = get_files(l, ".json");
            if(list_json.size() > 1)
            {
                std::cerr << "ERROR. Only a file JSON is expected!" << std::endl;
                exit(1);
            }

            MUSE::ManipulateMeta manmeta;
            manmeta.setProject(Project);

            std::vector<std::string> excommands;
            excommands.push_back(command);
            manmeta.setCommands(excommands);

            ManipulateMeta::DataProjection dataProjection;
            dataProjection.proj_is_set = true;
            dataProjection.data_type = setType.getValue();

            std::string geom_name;

            if((setType.getValue().compare("SAMPLES") == 0)  && (!setRegionGrowing.isSet())) //leggo in automatico le coordinate del set di dati (da json data)
            {
                std::vector<double> xCoord, yCoord, zCoord;
                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR. Set mesh files -m <filename>" << std::endl;
                    exit(1);
                }

                MUSE::DataMeta datameta;
                datameta.read(list_json.at(0));

                if(datameta.getInfoData().x_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().x_name + ".dat", xCoord);
                else
                {
                    std::cerr << "ERROR reading X coordinate: " << l + "/data/" + datameta.getInfoData().x_name + ".dat" << " NOT found." << std::endl;
                    exit(1);
                }

                if(datameta.getInfoData().y_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().y_name + ".dat", yCoord);
                else
                {
                    std::cerr << "ERROR reading Y coordinate: " << l + "/data/" + datameta.getInfoData().y_name + ".dat" << " NOT found." << std::endl;
                    exit(1);
                }

                if(datameta.getInfoData().z_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                else
                {
                    if(setZcoord.isSet())
                        readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                    else
                    {
                        std::cerr << "\033[0;32mWARNING: Z coordinate is Unknown. Set -z --name <variable> for setting the variable.\033[0;0m" << std::endl;
                        exit(1);
                    }
                }



                MUSE::Rotation dataRotation;

                std::vector<cinolib::vec3d> coord_samples;
                if(setRotAxis.isSet())
                {
                    std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                    std::cout << "Rotation axis: " << setRotAxis.getValue() << std::endl;
                    std::cout << "Rotation center: [" << setRotCenterX.getValue() << "; "<< setRotCenterY.getValue() << "; " << setRotCenterZ.getValue() << "]" << std::endl;
                    std::cout << "Rotation angle (degree): " << setRotAngle.getValue() << std::endl;
                    std::cout << std::endl;

                    dataRotation.rotation = true;
                    dataRotation.rotation_axis = setRotAxis.getValue();
                    dataRotation.rotation_center_x = setRotCenterX.getValue();
                    dataRotation.rotation_center_y = setRotCenterY.getValue();
                    dataRotation.rotation_center_z = setRotCenterZ.getValue();
                    dataRotation.rotation_angle = setRotAngle.getValue();

                    manmeta.setRotation(dataRotation);
                }

                std::vector<uint> sub_index;
                if(subDataset.isSet())
                {
                    MUSE::ExtractionMeta extrmeta;
                    extrmeta.read(app_folder + "/" + subDataset.getValue() + ".json");
                    sub_index = extrmeta.getDataExtraction().id_points;

                    dataRotation = extrmeta.getRotation();

                    std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                    std::cout << "Rotation axis: " << dataRotation.rotation_axis << std::endl;
                    std::cout << "Rotation center: [" << dataRotation.rotation_center_x << "; " << dataRotation.rotation_center_y << "; " << dataRotation.rotation_center_z << "]" <<  std::endl;
                    std::cout << "Rotation angle (degree): " << dataRotation.rotation_angle << std::endl;
                    std::cout << std::endl;

                    manmeta.setRotation(dataRotation);

                    dataProjection.subdataset_is_set = true;
                }
                else
                {
                    for(uint i=0; i< xCoord.size(); i++)
                        sub_index.push_back(i);
                }



                for(uint i:sub_index)
                {
                    cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));

                    if(dataRotation.rotation == true)
                    {
                        cinolib::vec3d axis = set_rotation_axis(dataRotation.rotation_axis);
                        cinolib::vec3d c (dataRotation.rotation_center_x, dataRotation.rotation_center_y, dataRotation.rotation_center_z);
                        sample = point_rotation(sample, axis, dataRotation.rotation_angle, c);
                    }
                    coord_samples.push_back(sample);
                }





                // 1) Lista dei file mesh in input su cui proiettare i punti

                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR set trimesh file to compute projections: --mgeom <file>" << std::endl;
                    exit(1);
                }
                std::vector<std::string> files = meshFiles.getValue();
                if(files.size() > 2)
                {
                    std::cerr << "ERROR on number of meshes to compute projections" << std::endl;
                    exit(1);
                }

                // Ciclo sui file mesh e procedo con le proiezioni
                cinolib::Polygonmesh<> mesh;
                mesh.load(files.at(0).c_str());
                std::cout << "\033[0;32mLoading mesh file: " << files.at(0) << " ... COMPLETED.\033[0m" << std::endl;

                geom_name = get_basename(get_filename(files.at(0))); //mesh0
                std::cout << "nome geometria: " << geom_name << std::endl;

                for(uint i=1; i<files.size(); i++)
                {
                    cinolib::Polygonmesh<> mesh_tmp; //surface mesh
                    mesh_tmp.load(files.at(i).c_str());
                    std::cout << "\033[0;32mLoading mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;
                    mesh+=mesh_tmp;
                    mesh_tmp.clear();

                    geom_name += "-" + get_basename(get_filename(files.at(i))); //mesh0
                    std::cout << "nome geometria: " << geom_name << std::endl;
                }

                //creazione octree da mesh (top+bottom o guscio)
                cinolib::Octree octree;
                octree.build_from_mesh_polys(mesh);

                std::vector<Point3D> proj_top, proj_bot; //vettore delle proiezioni

                if(setProjDir.getValue().compare("Y") == 0)
                {
                    double bby_max = mesh.bbox().max.y();
                    double bby_min = mesh.bbox().min.y();

                    for(uint j=0; j<coord_samples.size(); j++)
                    {
                        cinolib::vec3d p = coord_samples.at(j);

                        Point3D proj_max;
                        proj_max.y = -DBL_MAX;

                        Point3D proj_min;
                        proj_min.y = DBL_MAX;

                        cinolib::vec3d q1 (p.x(), bby_min - setBBEpsilon.getValue(), p.z()); //bottom
                        cinolib::vec3d q2 (p.x(), bby_max + setBBEpsilon.getValue(), p.z()); //top

                        cinolib::vec3d dir = q2-q1;

                        std::set<std::pair<double,uint>> all_hits;
                        octree.intersects_ray(q1, dir, all_hits);

                        std::set<std::pair<double, uint>>::iterator itr;
                        for (itr = all_hits.begin(); itr != all_hits.end(); itr++)
                        {
                            cinolib::vec3d p = q1 + itr->first * dir;

                            Point3D p_proj;

                            if(p.y() != DBL_MAX)
                            {
                                p_proj.x = p.x();
                                p_proj.y = p.y();
                                p_proj.z = p.z();
                            }

                            if(p_proj.y < proj_min.y)
                                proj_min = p_proj;

                            if(p_proj.y > proj_max.y)
                                proj_max = p_proj;
                        }

                        proj_top.push_back(proj_max);
                        proj_bot.push_back(proj_min);

                    }
                }
                else if(setProjDir.getValue().compare("Z") == 0)
                {
                    double bby_max = mesh.bbox().max.z();
                    double bby_min = mesh.bbox().min.z();

                    for(uint j=0; j<coord_samples.size(); j++)
                    {
                        cinolib::vec3d point = coord_samples.at(j);

                        Point3D proj_max;
                        proj_max.x = point.x();
                        proj_max.y = point.y();
                        proj_max.z = -DBL_MAX;

                        Point3D proj_min;
                        proj_min.x = point.x();
                        proj_min.y = point.y();
                        proj_min.z = DBL_MAX;

                        cinolib::vec3d q1 (point.x(), point.y(), bby_min - setBBEpsilon.getValue()); //bottom
                        cinolib::vec3d q2 (point.x(), point.y(), bby_max + setBBEpsilon.getValue()); //top

                        cinolib::vec3d dir = q2-q1;

                        std::set<std::pair<double,uint>> all_hits;
                        octree.intersects_ray(q1, dir, all_hits);
                        if(all_hits.size() == 0)
                        {
                            std::cerr << "ERROR: No intersections found for point ID: " << j << std::endl;
                            //exit(1);
                        }

                        std::set<std::pair<double, uint>>::iterator itr;
                        for (itr = all_hits.begin(); itr != all_hits.end(); itr++)
                        {
                            cinolib::vec3d p = q1 + itr->first * dir;

                            Point3D p_proj;

                            if(p.z() != DBL_MAX)
                            {
                                p_proj.x = p.x();
                                p_proj.y = p.y();
                                p_proj.z = p.z();
                            }

                            if((p_proj.z == DBL_MAX) || (p_proj.z == -DBL_MAX))
                            {
                                std::cout << "ERROR in point projection." << std::endl;
                            }

                            if(p_proj.z < proj_min.z)
                                proj_min = p_proj;

                            if(p_proj.z > proj_max.z)
                                proj_max = p_proj;
                        }


                        if((proj_max.z == DBL_MAX) || (proj_max.z == -DBL_MAX))
                        {
                            std::cout << std::setprecision(12) << "ID: " << j << "; " << point.x() << "; " << point.y() << "; " << point.z() << std::endl;
                            std::cout << std::setprecision(12) << "ID: " << j << " - ERROR in point projection - max." << std::endl;
                        }
                        if((proj_min.z == DBL_MAX) || (proj_min.z == -DBL_MAX))
                        {
                            std::cout << std::setprecision(12) << "ID: " << j << "; " << point.x() << "; " << point.y() << "; " << point.z() << std::endl;
                            std::cout << std::setprecision(12) << "ID: " << j << " - ERROR in point projection - min." << std::endl;
                        }

                        proj_top.push_back(proj_max);
                        proj_bot.push_back(proj_min);
                    }
                }
                else if(setProjDir.getValue().compare("X") == 0)
                {
                    exit(1);

                }
                else
                {
                    std::cerr << "ERROR: no projection direction is set between X,Y or Z" << std::endl;
                    exit(1);
                }

                dataProjection.mesh = geom_name;
                dataProjection.proj_direction = setProjDir.getValue();
                dataProjection.top_name = geom_name + "_top";
                dataProjection.bottom_name = geom_name + "_bot";
                manmeta.setDataProjection(dataProjection);
                manmeta.write(app_folder + "/samples_" + geom_name + ".json");


                export3d_xyz(app_folder + "/samples_" + dataProjection.top_name + ".dat", proj_top);
                export3d_xyz(app_folder + "/samples_" + dataProjection.bottom_name + ".dat", proj_bot);

                std::cout << "n top projections : " << proj_top.size() << std::endl;
                std::cout << "n bot projections : " << proj_bot.size() << std::endl;

                std::cout << "\033[0;32mComputing points projection on mesh file... COMPLETED.\033[0m" << std::endl;
                std::cout << std::endl;

            }


            if((setType.getValue().compare("SAMPLES") == 0)  && (setRegionGrowing.isSet())) //leggo in automatico le coordinate del set di dati (da json data)
            {
                std::vector<double> xCoord, yCoord, zCoord;
                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR. Set mesh files -m <filename>" << std::endl;
                    exit(1);
                }

                MUSE::DataMeta datameta;
                datameta.read(list_json.at(0));

                if(datameta.getInfoData().x_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().x_name + ".dat", xCoord);
                else
                {
                    std::cerr << "ERROR reading X coordinate: " << l + "/data/" + datameta.getInfoData().x_name + ".dat" << " NOT found." << std::endl;
                    exit(1);
                }

                if(datameta.getInfoData().y_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().y_name + ".dat", yCoord);
                else
                {
                    std::cerr << "ERROR reading Y coordinate: " << l + "/data/" + datameta.getInfoData().y_name + ".dat" << " NOT found." << std::endl;
                    exit(1);
                }

                if(datameta.getInfoData().z_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                else
                {
                    if(setZcoord.isSet())
                        readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                    else
                    {
                        std::cerr << "\033[0;32mWARNING: Z coordinate is Unknown. Set -z --name <variable> for setting the variable.\033[0;0m" << std::endl;
                        exit(1);
                    }
                }



                MUSE::Rotation dataRotation;

                std::vector<cinolib::vec3d> coord_samples;
                if(setRotAxis.isSet())
                {
                    std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                    std::cout << "Rotation axis: " << setRotAxis.getValue() << std::endl;
                    std::cout << "Rotation center: [" << setRotCenterX.getValue() << "; "<< setRotCenterY.getValue() << "; " << setRotCenterZ.getValue() << "]" << std::endl;
                    std::cout << "Rotation angle (degree): " << setRotAngle.getValue() << std::endl;
                    std::cout << std::endl;

                    dataRotation.rotation = true;
                    dataRotation.rotation_axis = setRotAxis.getValue();
                    dataRotation.rotation_center_x = setRotCenterX.getValue();
                    dataRotation.rotation_center_y = setRotCenterY.getValue();
                    dataRotation.rotation_center_z = setRotCenterZ.getValue();
                    dataRotation.rotation_angle = setRotAngle.getValue();

                    manmeta.setRotation(dataRotation);
                }

                std::vector<uint> sub_index;
                if(subDataset.isSet())
                {
                    MUSE::ExtractionMeta extrmeta;
                    extrmeta.read(app_folder + "/" + subDataset.getValue() + ".json");
                    sub_index = extrmeta.getDataExtraction().id_points;

                    dataRotation = extrmeta.getRotation();

                    std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                    std::cout << "Rotation axis: " << dataRotation.rotation_axis << std::endl;
                    std::cout << "Rotation center: [" << dataRotation.rotation_center_x << "; " << dataRotation.rotation_center_y << "; " << dataRotation.rotation_center_z << "]" <<  std::endl;
                    std::cout << "Rotation angle (degree): " << dataRotation.rotation_angle << std::endl;
                    std::cout << std::endl;

                    manmeta.setRotation(dataRotation);

                    dataProjection.subdataset_is_set = true;
                }
                else
                {
                    for(uint i=0; i< xCoord.size(); i++)
                        sub_index.push_back(i);
                }



                for(uint i:sub_index)
                {
                    cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));

                    if(dataRotation.rotation == true)
                    {
                        cinolib::vec3d axis = set_rotation_axis(dataRotation.rotation_axis);
                        cinolib::vec3d c (dataRotation.rotation_center_x, dataRotation.rotation_center_y, dataRotation.rotation_center_z);
                        sample = point_rotation(sample, axis, dataRotation.rotation_angle, c);
                    }
                    coord_samples.push_back(sample);
                }





                // 1) Lista dei file mesh in input su cui proiettare i punti

                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR set trimesh file to compute projections: --mgeom <file>" << std::endl;
                    exit(1);
                }
                // std::vector<std::string> files = meshFiles.getValue();
                // if(files.size() > 2)
                // {
                //     std::cerr << "ERROR on number of meshes to compute projections" << std::endl;
                //     exit(1);
                // }

                MUSE::VolumeMesh<> model;
                model.load(meshFiles.getValue().at(0).c_str());
                std::cout << "\033[0;32mLoading polyhedral mesh file: " << meshFiles.getValue().at(0).c_str() << " ... COMPLETED.\033[0m" << std::endl;
                std::cout << std::endl;

                cinolib::vec3d bbox_center = model.bbox().center();
                model.translate(-bbox_center);
                model.update_f_normals();
                model.translate(bbox_center);

                uint id_max, id_min;
                double max_normal = -DBL_MAX, min_normal = DBL_MAX;

                for (uint fid=0; fid < model.num_faces(); fid++)
                {
                    if (!model.face_is_on_srf(fid))
                        continue;

                    if (model.face_data(fid).normal.z() > max_normal) //// top
                    {
                        max_normal = model.face_data(fid).normal.z();
                        id_max = fid;
                    }

                    if (model.face_data(fid).normal.z() < min_normal) //// bot
                    {
                        min_normal = model.face_data(fid).normal.z();
                        id_min = fid;
                    }
                }

                std::cout << "max Z normal : " << max_normal << std::endl;
                std::cout << "min Z normal : " << min_normal << std::endl;


                model.face_data(id_max).label = 100;
                model.face_data(id_min).label = 100;

                std::queue<uint> top, bottom;

                top.push(id_max);
                bottom.push(id_min);

                cinolib::Octree octree_top, octree_bot; //costruisco l'octree da mesh laterale
                uint pid_octree=0;

                while (!top.empty())
                {
                    uint curr_fid = top.front();
                    top.pop();

                    if (sign(model.face_data(curr_fid).normal.z()) != 1)
                        continue;

                    const std::vector<cinolib::vec3d> &vertice = model.face_verts(curr_fid);
                    octree_top.push_triangle(pid_octree, vertice.at(0), vertice.at(1), vertice.at(2));


                    //model.face_data(curr_fid).color = Color::RED();
            //         for (uint v : model.adj_f2v(curr_fid))
            //             m.vert_data(v).color = Color::RED();

                    // counter++;

                    // std::cout << counter << " ::: " << curr_fid << std::endl;

                    //std::cout << curr_fid << std::endl;

                    for (uint adj_fid : model.adj_f2f(curr_fid))
                    {
                        if (!model.face_is_on_srf(adj_fid))
                            continue;

                        if (model.face_data(adj_fid).label == 100)
                            continue;

                        top.push(adj_fid);
                        model.face_data(adj_fid).label = 100;
                    }
                }

                pid_octree = 0;
                while (!bottom.empty())
                {
                    uint curr_fid = bottom.front();
                    bottom.pop();

                    if (sign(model.face_data(curr_fid).normal.z()) != -1)
                        continue;

                    const std::vector<cinolib::vec3d> &vertice = model.face_verts(curr_fid);
                    octree_bot.push_triangle(pid_octree, vertice.at(0), vertice.at(1), vertice.at(2));

                    //m.face_data(curr_fid).color = Color::BLUE();

            //         for (uint v : m.adj_f2v(curr_fid))
            //             m.vert_data(v).color = Color::BLUE();

                    // counter++;

                    // std::cout << counter << " ::: " << curr_fid << std::endl;

                    //std::cout << curr_fid << std::endl;

                    for (uint adj_fid : model.adj_f2f(curr_fid))
                    {
                        if (!model.face_is_on_srf(adj_fid))
                            continue;

                        if (model.face_data(adj_fid).label == 100)
                            continue;

                        bottom.push(adj_fid);
                        model.face_data(adj_fid).label = 100;
                    }
                }

                octree_top.build();
                octree_bot.build();
                std::cout << "Build octree ... COMPLETED." << std::endl;


                std::vector<Point3D> proj_top, proj_bot; //vettore delle proiezioni
                if(setProjDir.getValue().compare("Z") == 0)
                {
                    double bbz_max = model.bbox().max.z();
                    double bbz_min = model.bbox().min.z();

                    for(uint j=0; j<coord_samples.size(); j++)
                    {
                        cinolib::vec3d q1 (coord_samples.at(j).x(), coord_samples.at(j).y(), bbz_min - setBBEpsilon.getValue()); //bottom
                        cinolib::vec3d q2 (coord_samples.at(j).x(), coord_samples.at(j).y(), bbz_max + setBBEpsilon.getValue()); //top

                        cinolib::vec3d dir = q2-q1;
                        dir /= dir.norm();

                        double dist;
                        uint id_p;
                        bool intersects_top = octree_top.intersects_ray(q1, dir, dist, id_p);
                        if (!intersects_top)
                        {
                            cinolib::vec3d cp = octree_top.closest_point(coord_samples.at(j));
                            dist = cp.z() - q1.z();
                        }

                        //cinolib::vec3d p_top = q1 + dist * dir; //proiezione del punto "non top/bottom" su top
                        Point3D p_proj_top;
                        p_proj_top.x = q1.x();
                        p_proj_top.y = q1.y();
                        p_proj_top.z = q1.z() + dist;

                        dist = 0.0;
                        id_p = 0;
                        bool intersects_bot = octree_bot.intersects_ray(q1, dir, dist, id_p);
                        if (!intersects_bot)
                        {
                            cinolib::vec3d cp = octree_bot.closest_point(coord_samples.at(j));
                            dist = cp.z() - q1.z();
                            //std::cin.ignore();
                        }

                        //cinolib::vec3d p_bot = q1;
                        //p_bot.z() += dist; // * dir;
                        //cinolib::vec3d p_bot = q1 + dist * dir; //proiezione del punto "non top/bottom" su bottom

                        Point3D p_proj_bot;
                        p_proj_bot.x = q1.x();
                        p_proj_bot.y = q1.y();
                        p_proj_bot.z = q1.z() + dist;

                        proj_top.push_back(p_proj_top);
                        proj_bot.push_back(p_proj_bot);
                    }
                }
                else if(setProjDir.getValue().compare("X") == 0)
                {
                    std::cerr << "ERROR: X projection direction is not available." << std::endl;
                    exit(1);
                }
                else if(setProjDir.getValue().compare("Y") == 0)
                {
                    exit(1);
                }
                else
                {
                    std::cerr << "ERROR: Y projection direction is not available." << std::endl;
                    exit(1);
                }

                geom_name = get_basename(get_filename(meshFiles.getValue().at(0)));

                dataProjection.mesh = geom_name;
                dataProjection.proj_direction = setProjDir.getValue();
                dataProjection.top_name = geom_name + "_top";
                dataProjection.bottom_name = geom_name + "_bot";
                manmeta.setDataProjection(dataProjection);
                manmeta.write(app_folder + "/samples_" + geom_name + ".json");


                export3d_xyz(app_folder + "/samples_" + dataProjection.top_name + ".dat", proj_top);
                export3d_xyz(app_folder + "/samples_" + dataProjection.bottom_name + ".dat", proj_bot);

                std::cout << "n top projections : " << proj_top.size() << std::endl;
                std::cout << "n bot projections : " << proj_bot.size() << std::endl;

                std::cout << "\033[0;32mComputing points projection on mesh file... COMPLETED.\033[0m" << std::endl;
                std::cout << std::endl;

            }



            //if(setType.getValue().compare("TET") == 0)
            if((setType.getValue().compare("TET") == 0) && (!setRegionGrowing.isSet()))
            {
                //1) import volume mesh to process: loop on its vertices
                if(!geomModel.isSet())
                {
                    std::cout << "ERROR set volume mesh --geom <file>"<< std::endl;
                    exit(1);
                }
                MUSE::VolumeMesh<> model;
                model.load(geomModel.getValue().c_str());
                std::cout << "\033[0;32mLoading polyhedral mesh file: " << geomModel.getValue() << " ... COMPLETED.\033[0m" << std::endl;
                std::cout << std::endl;

                //2) import surface meshes as reference for projection of volume vertices
                if(!meshFiles.isSet())
                {
                    std::cerr << "ERROR set surface meshes (top/top-bottom) to compute projections: --mgeom <file>" << std::endl;
                    exit(1);
                }
                std::vector<std::string> files = meshFiles.getValue();


                // Ciclo sui file mesh e procedo con le proiezioni
                cinolib::Polygonmesh<> surf_mesh;
                surf_mesh.load(files.at(0).c_str());
                std::cout << "\033[0;32mLoading mesh file: " << files.at(0) << " ... COMPLETED.\033[0m" << std::endl;

                geom_name = get_basename(get_filename(files.at(0))); //mesh0
                std::cout << "nome geometria: " << geom_name << std::endl;

                std::vector<Point3D> proj_top, proj_bot; //vettore delle proiezioni

                uint num_verts_top = surf_mesh.num_verts();
                uint nv_bound = surf_mesh.get_boundary_vertices().size();

                std::cout << "#V volume mesh: " << model.num_verts() << std::endl;
                std::cout << "#V surface mesh: " << num_verts_top << std::endl;
                std::cout << "#V on boundary surface mesh: " << nv_bound << std::endl;
                std::cout << std::endl;

                std::cout << "### Number of steps of geometry model: " << setStepGeometry.getValue() << std::endl;
                std::cout << std::endl;
                std::cout << "### Vertices on surface box: " << num_verts_top + nv_bound * setStepGeometry.getValue() + num_verts_top << std::endl;

                for(uint vid=0; vid<num_verts_top; vid++)
                {
                    //std::cout << "### Extract vertex by surface mesh ... ID: " << vid << std::endl;
                    //std::cout << "COORD VERT: " << surf_mesh.vert(vid).x() << "; " << surf_mesh.vert(vid).y() << "; " << surf_mesh.vert(vid).z() << std::endl;

                    Point3D proj_max; //vertici sul top: per costruzione superficie 1
                    proj_max.x = model.vert(vid).x();
                    proj_max.y = model.vert(vid).y();
                    proj_max.z = model.vert(vid).z();

                    Point3D proj_min; //vertici sul bottom: per costruzione superficie 2 (append)
                    proj_min.x = model.vert(vid+ num_verts_top + nv_bound * setStepGeometry.getValue()).x();
                    proj_min.y = model.vert(vid+ num_verts_top + nv_bound * setStepGeometry.getValue()).y();
                    proj_min.z = model.vert(vid+ num_verts_top + nv_bound * setStepGeometry.getValue()).z();

                    proj_top.push_back(proj_max);
                    proj_bot.push_back(proj_min);

                    // std::cout << "COORD PROJ max - TOP: " << proj_max.x << "; " << proj_max.y << "; " << proj_max.z << std::endl;
                    // std::cout << "COORD PROJ min - BOT: " << proj_min.x << "; " << proj_min.y << "; " << proj_min.z << std::endl;
                }

                std::cout << "### Size vector top: " << proj_top.size() << std::endl;
                std::cout << "### Size vector bottom: " << proj_bot.size() << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << std::endl;
                std::cout << "### Continue for stepped vertices ..." << std::endl;

                if(setStepGeometry.getValue() > 0)
                {
                    std::vector<uint> bound_support = surf_mesh.get_ordered_boundary_vertices();
                    std::cout << "### Vertices on boundary: " << bound_support.size() << std::endl;
                    int v_ = 0;

                    for(uint vid=num_verts_top; vid<num_verts_top + nv_bound * setStepGeometry.getValue(); vid++)
                    {
                        std::cout << "### Point VID ---> " << vid << std::endl;
                        std::cout << "### Vertex ---> " << model.vert(vid).x() << "; " << model.vert(vid).y() << "; " << model.vert(vid).z() << std::endl;

                        uint bvid = bound_support.at(v_);
                        std::cout << "### Point BVID ---> " << bvid << std::endl;
                        v_++;

                        if (v_ % bound_support.size() == 0)
                            v_ = 0;


                        Point3D proj_max; //vertici sul top: per costruzione superficie 1
                        proj_max.x = model.vert(bvid).x();
                        proj_max.y = model.vert(bvid).y();
                        proj_max.z = model.vert(bvid).z();

                        Point3D proj_min; //vertici sul bottom: per costruzione superficie 2 (append)
                        proj_min.x = model.vert(bvid + num_verts_top + nv_bound * setStepGeometry.getValue()).x();
                        proj_min.y = model.vert(bvid + num_verts_top + nv_bound * setStepGeometry.getValue()).y();
                        proj_min.z = model.vert(bvid + num_verts_top + nv_bound * setStepGeometry.getValue()).z();

                        proj_top.push_back(proj_max);
                        proj_bot.push_back(proj_min);

                        std::cout << "COORD PROJ max - TOP: " << proj_max.x << "; " << proj_max.y << "; " << proj_max.z << std::endl;
                        std::cout << "COORD PROJ min - BOT: " << proj_min.x << "; " << proj_min.y << "; " << proj_min.z << std::endl;
                        std::cout << std::endl;
                    }
                }

                std::cout << "### Size vector top: " << proj_top.size() << std::endl;
                std::cout << "### Size vector bottom: " << proj_bot.size() << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << std::endl;
                std::cout << "### Continue for bottom vertices ..." << std::endl;

                for(uint vid=num_verts_top + nv_bound * setStepGeometry.getValue(); vid<num_verts_top*2 + nv_bound*setStepGeometry.getValue(); vid++)
                {
                    //std::cout << vid << std::endl;
                    //std::cout << "### Extract vertex by surface mesh ... ID: " << vid << std::endl;
                    //std::cout << "COORD VERT: " << surf_mesh.vert(vid).x() << "; " << surf_mesh.vert(vid).y() << "; " << surf_mesh.vert(vid).z() << std::endl;

                    Point3D proj_max; //vertici sul top: per costruzione superficie 1
                    proj_max.x = model.vert(vid-num_verts_top - nv_bound * setStepGeometry.getValue()).x();
                    proj_max.y = model.vert(vid-num_verts_top - nv_bound * setStepGeometry.getValue()).y();
                    proj_max.z = model.vert(vid-num_verts_top - nv_bound * setStepGeometry.getValue()).z();

                    Point3D proj_min; //vertici sul bottom: per costruzione superficie 2 (append)
                    proj_min.x = model.vert(vid).x();
                    proj_min.y = model.vert(vid).y();
                    proj_min.z = model.vert(vid).z();

                    proj_top.push_back(proj_max);
                    proj_bot.push_back(proj_min);

                    // std::cout << "COORD PROJ max - TOP: " << proj_max.x << "; " << proj_max.y << "; " << proj_max.z << std::endl;
                    // std::cout << "COORD PROJ min - BOT: " << proj_min.x << "; " << proj_min.y << "; " << proj_min.z << std::endl;
                }

                std::cout << "### Size vector top: " << proj_top.size() << std::endl;
                std::cout << "### Size vector bottom: " << proj_bot.size() << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << std::endl;
                std::cout << "### Continue for additional vertices in volume mesh ..." << std::endl;
                std::cout << "### Projected vertices: " << num_verts_top*2 + nv_bound*setStepGeometry.getValue() << std::endl;
                std::cout << "### Additional vertices in volume mesh (to project): " << model.num_verts() - num_verts_top*2 + nv_bound*setStepGeometry.getValue() << std::endl;

                if(model.num_verts() - num_verts_top*2 + nv_bound*setStepGeometry.getValue() > 0)
                {
                    if(files.size() <= 1)
                    {
                        std::cerr << "ERROR: bottom surface is necessary for computing the projections of internal points." << std::endl;
                        std::cerr << "Please set surfaces as: --mgeom <path/name_top.off> --mgeom <path/name_bottom.off>" << std::endl;
                        exit(1);
                    }

                    for(uint i=1; i<files.size(); i++)
                    {
                        cinolib::Polygonmesh<> mesh_tmp; //surface mesh
                        mesh_tmp.load(files.at(i).c_str());
                        std::cout << "\033[0;32mLoading mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;
                        surf_mesh+=mesh_tmp;
                        mesh_tmp.clear();

                        geom_name += "-" + get_basename(get_filename(files.at(i))); //mesh0
                        std::cout << "nome geometria: " << geom_name << std::endl;
                    }

                    cinolib::Octree octree; //costruisco l'octree da mesh laterale
                    octree.build_from_mesh_polys(surf_mesh);
                    std::cout << "Build octree from surface ... COMPLETED." << std::endl;

                    if(setProjDir.getValue().compare("Y") == 0)
                    {
                        double bby_max = model.bbox().max.y();
                        double bby_min = model.bbox().min.y();

                        for(uint vid=num_verts_top*2 + nv_bound*setStepGeometry.getValue(); vid<model.num_verts(); vid++)
                        {
                            //std::cout << std::endl;
                            //std::cout << "### Check vertex is on/in surface ... ID: " << vid << std::endl;
                            //if(!model.vert_is_on_srf(vid))
                            //{
                            std::cout << "### Starting projection of internal vertex by octree ..." << std::endl;
                            Point3D proj_max;
                            proj_max.y = -DBL_MAX;

                            Point3D proj_min;
                            proj_min.y = DBL_MAX;

                            cinolib::vec3d q1 (model.vert(vid).x(), bby_min - setBBEpsilon.getValue(), model.vert(vid).z()); //bottom
                            cinolib::vec3d q2 (model.vert(vid).x(), bby_max + setBBEpsilon.getValue(), model.vert(vid).z()); //top

                            cinolib::vec3d dir = q2-q1;

                            std::set<std::pair<double,uint>> all_hits;
                            octree.intersects_ray(q1, dir, all_hits);

                            std::set<std::pair<double, uint>>::iterator itr;
                            for (itr = all_hits.begin(); itr != all_hits.end(); itr++)
                            {
                                cinolib::vec3d p = q1 + itr->first * dir;

                                Point3D p_proj;

                                if(p.y() != DBL_MAX)
                                {
                                    p_proj.x = p.x();
                                    p_proj.y = p.y();
                                    p_proj.z = p.z();
                                }

                                if(p_proj.y < proj_min.y)
                                    proj_min = p_proj;

                                if(p_proj.y > proj_max.y)
                                    proj_max = p_proj;
                            }

                            proj_top.push_back(proj_max);
                            proj_bot.push_back(proj_min);
                            //}
                        }
                    }
                    else if(setProjDir.getValue().compare("Z") == 0)
                    {
                        double bbz_max = model.bbox().max.z();
                        double bbz_min = model.bbox().min.z();

                        for(uint vid=num_verts_top*2 + nv_bound*setStepGeometry.getValue(); vid<model.num_verts(); vid++)
                        //for(uint vid=260655; vid<model.num_verts(); vid++)
                        {
                            if(model.vert_is_on_srf(vid))
                            {
                                std::cout << "### Check vertex is on/in surface ... ID: " << vid << std::endl;
                                if(model.adj_v2p(vid).size() > 0)
                                    std::cout << "### vertici ha triangoli adiacenti: " << model.adj_v2f(vid).size() << std::endl;
                            }
                            // std::cout << std::endl;
                            // std::cout << "### Check vertex is on/in surface ... ID: " << vid << std::endl;
                            // if(!model.vert_is_on_srf(vid))
                            // {
                            //std::cout << "### Starting projection of internal vertex by octree ..." << std::endl;
                            std::cout << "COORD VERT: " << model.vert(vid).x() << "; " << model.vert(vid).y() << "; " << model.vert(vid).z() << std::endl;

                            Point3D proj_max;
                            proj_max.x = model.vert(vid).x();
                            proj_max.y = model.vert(vid).y();
                            proj_max.z = -DBL_MAX;

                            Point3D proj_min;
                            proj_min.x = model.vert(vid).x();
                            proj_min.y = model.vert(vid).y();
                            proj_min.z = DBL_MAX;

                            cinolib::vec3d q1 (model.vert(vid).x(), model.vert(vid).y(), bbz_min - setBBEpsilon.getValue()); //bottom
                            cinolib::vec3d q2 (model.vert(vid).x(), model.vert(vid).y(), bbz_max + setBBEpsilon.getValue()); //top

                            // std::cout << "COORD VERT min: " << q1.x() << "; " << q1.y() << "; " << q1.z() << std::endl;
                            // std::cout << "COORD VERT max: " << q2.x() << "; " << q2.y() << "; " << q2.z() << std::endl;

                            cinolib::vec3d dir = q2-q1;

                            std::set<std::pair<double,uint>> all_hits;
                            octree.intersects_ray(q1, dir, all_hits);

                            std::set<std::pair<double, uint>>::iterator itr;
                            for (itr = all_hits.begin(); itr != all_hits.end(); itr++)
                            {
                                cinolib::vec3d p = q1 + itr->first * dir;

                                Point3D p_proj;

                                if(p.z() != DBL_MAX)
                                {
                                    p_proj.x = p.x();
                                    p_proj.y = p.y();
                                    p_proj.z = p.z();
                                }
                                else
                                {
                                    std::cout << "Projection is not found!!" << std::endl;
                                    exit(1);
                                }

                                if(p_proj.z < proj_min.z)
                                    proj_min = p_proj;

                                if(p_proj.z > proj_max.z)
                                    proj_max = p_proj;
                            }

                            if((proj_max.z == DBL_MAX) || (proj_max.z == -DBL_MAX) || (proj_min.z == DBL_MAX) || (proj_min.z == -DBL_MAX))
                            {
                                cinolib::vec3d closest_point = octree.closest_point(model.vert(vid));
                                std::cout << closest_point << std::endl;

                                cinolib::vec3d q1 (closest_point.x(), closest_point.y(), bbz_min - setBBEpsilon.getValue()); //bottom
                                cinolib::vec3d q2 (closest_point.x(), closest_point.y(), bbz_max + setBBEpsilon.getValue()); //top
                                cinolib::vec3d dir = q2-q1;
                                std::set<std::pair<double,uint>> all_hits;
                                octree.intersects_ray(q1, dir, all_hits);

                                std::set<std::pair<double, uint>>::iterator itr;
                                for (itr = all_hits.begin(); itr != all_hits.end(); itr++)
                                {
                                    cinolib::vec3d p_ = q1 + itr->first * dir;

                                    if(p_.z() < proj_min.z)
                                        proj_min.z = p_.z();

                                    if(p_.z() > proj_max.z)
                                        proj_max.z = p_.z();
                                }
                            }

                            proj_top.push_back(proj_max);
                            proj_bot.push_back(proj_min);

                            std::cout << "COORD PROJ min: " << proj_min.x << "; " << proj_min.y << "; " << proj_min.z << std::endl;
                            std::cout << "COORD PROJ max: " << proj_max.x << "; " << proj_max.y << "; " << proj_max.z << std::endl;
                            //}
                        }
                    }
                    else if(setProjDir.getValue().compare("X") == 0)
                    {
                        double bbx_max = model.bbox().max.x();
                        double bbx_min = model.bbox().min.x();

                        for(uint vid=num_verts_top*2 + nv_bound*setStepGeometry.getValue(); vid<model.num_verts(); vid++)
                        {
                            // std::cout << std::endl;
                            // std::cout << "### Check vertex is on/in surface ... ID: " << vid << std::endl;
                            // if(!model.vert_is_on_srf(vid))
                            // {
                            std::cout << "### Starting projection of internal vertex by octree ..." << std::endl;
                            std::cout << "COORD VERT: " << model.vert(vid).x() << "; " << model.vert(vid).y() << "; " << model.vert(vid).z() << std::endl;

                            Point3D proj_max;
                            proj_max.x = -DBL_MAX;

                            Point3D proj_min;
                            proj_min.x = DBL_MAX;

                            cinolib::vec3d q1 (bbx_min - setBBEpsilon.getValue(), model.vert(vid).y(), model.vert(vid).z()); //bottom
                            cinolib::vec3d q2 (bbx_max + setBBEpsilon.getValue(), model.vert(vid).y(), model.vert(vid).z()); //top

                            // std::cout << "COORD VERT min: " << q1.x() << "; " << q1.y() << "; " << q1.z() << std::endl;
                            // std::cout << "COORD VERT max: " << q2.x() << "; " << q2.y() << "; " << q2.z() << std::endl;

                            cinolib::vec3d dir = q2-q1;

                            std::set<std::pair<double,uint>> all_hits;
                            octree.intersects_ray(q1, dir, all_hits);

                            std::set<std::pair<double, uint>>::iterator itr;
                            for (itr = all_hits.begin(); itr != all_hits.end(); itr++)
                            {
                                cinolib::vec3d p = q1 + itr->first * dir;

                                Point3D p_proj;

                                if(p.z() != DBL_MAX)
                                {
                                    p_proj.x = p.x();
                                    p_proj.y = p.y();
                                    p_proj.z = p.z();
                                }

                                if(p_proj.x < proj_min.x)
                                    proj_min = p_proj;

                                if(p_proj.x > proj_max.x)
                                    proj_max = p_proj;
                            }

                            proj_top.push_back(proj_max);
                            proj_bot.push_back(proj_min);

                            std::cout << "COORD PROJ min: " << proj_min.x << "; " << proj_min.y << "; " << proj_min.z << std::endl;
                            std::cout << "COORD PROJ max: " << proj_max.x << "; " << proj_max.y << "; " << proj_max.z << std::endl;
                        }
                    }
                    else
                    {
                        std::cerr << "ERROR: no projection direction is set between X,Y or Z" << std::endl;
                        exit(1);
                    }


                }

                dataProjection.mesh = geom_name;
                dataProjection.proj_direction = setProjDir.getValue();
                dataProjection.top_name = geom_name + "_top";
                dataProjection.bottom_name = geom_name + "_bot";
                manmeta.setDataProjection(dataProjection);
                manmeta.write(app_folder + "/geom_" + geom_name + ".json");


                export3d_xyz(app_folder + "/geom_" + dataProjection.top_name + ".dat", proj_top);
                export3d_xyz(app_folder + "/geom_" + dataProjection.bottom_name + ".dat", proj_bot);

                std::cout << std::endl;
                std::cout << "### Number of vertices: " << model.num_verts() << std::endl;
                std::cout << "### Number of projections on top layer: " << proj_top.size() << std::endl;
                std::cout << "### Number of projections on bottom layer: " << proj_bot.size() << std::endl;
            }

            if((setType.getValue().compare("TET") == 0) && (setRegionGrowing.isSet()))
            {
                if(!geomModel.isSet())
                {
                    std::cout << "ERROR set hexahedral mesh --geom <file>"<< std::endl;
                    exit(1);
                }

                MUSE::VolumeMesh<> model;
                model.load(geomModel.getValue().c_str());
                std::cout << "\033[0;32mLoading polyhedral mesh file: " << geomModel.getValue() << " ... COMPLETED.\033[0m" << std::endl;
                std::cout << std::endl;

                cinolib::vec3d bbox_center = model.bbox().center();
                model.translate(-bbox_center);
                model.update_f_normals();
                model.translate(bbox_center);

                uint id_max, id_min;
                double max_normal = -DBL_MAX, min_normal = DBL_MAX;

                for (uint fid=0; fid < model.num_faces(); fid++)
                {
                    if (!model.face_is_on_srf(fid))
                        continue;

                    if (model.face_data(fid).normal.z() > max_normal) //// top
                    {
                        max_normal = model.face_data(fid).normal.z();
                        id_max = fid;
                    }

                    if (model.face_data(fid).normal.z() < min_normal) //// bot
                    {
                        min_normal = model.face_data(fid).normal.z();
                        id_min = fid;
                    }
                }

                std::cout << "max Z normal : " << max_normal << std::endl;
                std::cout << "min Z normal : " << min_normal << std::endl;

                model.face_data(id_max).label = 100;
                model.face_data(id_min).label = 100;

                std::queue<uint> top, bottom;

                top.push(id_max);
                bottom.push(id_min);

                cinolib::Octree octree_top, octree_bot; //costruisco l'octree da mesh laterale
                uint pid_octree=0;

                while (!top.empty())
                {
                    uint curr_fid = top.front();
                    top.pop();

                    if (sign(model.face_data(curr_fid).normal.z()) != 1)
                        continue;

                    const std::vector<cinolib::vec3d> &vertice = model.face_verts(curr_fid);
                    octree_top.push_triangle(pid_octree, vertice.at(0), vertice.at(1), vertice.at(2));


                    //model.face_data(curr_fid).color = Color::RED();
            //         for (uint v : model.adj_f2v(curr_fid))
            //             m.vert_data(v).color = Color::RED();

                    // counter++;

                    // std::cout << counter << " ::: " << curr_fid << std::endl;

                    //std::cout << curr_fid << std::endl;

                    for (uint adj_fid : model.adj_f2f(curr_fid))
                    {
                        if (!model.face_is_on_srf(adj_fid))
                            continue;

                        if (model.face_data(adj_fid).label == 100)
                            continue;

                        top.push(adj_fid);
                        model.face_data(adj_fid).label = 100;
                    }
                }

                pid_octree = 0;
                while (!bottom.empty())
                {
                    uint curr_fid = bottom.front();
                    bottom.pop();

                    if (sign(model.face_data(curr_fid).normal.z()) != -1)
                        continue;

                    const std::vector<cinolib::vec3d> &vertice = model.face_verts(curr_fid);
                    octree_bot.push_triangle(pid_octree, vertice.at(0), vertice.at(1), vertice.at(2));

                    //m.face_data(curr_fid).color = Color::BLUE();

            //         for (uint v : m.adj_f2v(curr_fid))
            //             m.vert_data(v).color = Color::BLUE();

                    // counter++;

                    // std::cout << counter << " ::: " << curr_fid << std::endl;

                    //std::cout << curr_fid << std::endl;

                    for (uint adj_fid : model.adj_f2f(curr_fid))
                    {
                        if (!model.face_is_on_srf(adj_fid))
                            continue;

                        if (model.face_data(adj_fid).label == 100)
                            continue;

                        bottom.push(adj_fid);
                        model.face_data(adj_fid).label = 100;
                    }
                }

                octree_top.build();
                octree_bot.build();
                std::cout << "Build octree ... COMPLETED." << std::endl;

                std::vector<Point3D> proj_top, proj_bot; //vettore delle proiezioni
                if(setProjDir.getValue().compare("Z") == 0)
                {
                    double bbz_max = model.bbox().max.z();
                    double bbz_min = model.bbox().min.z();

                    for(uint vid=0; vid<model.num_verts(); vid++)
                    {
                        cinolib::vec3d q1 (model.vert(vid).x(), model.vert(vid).y(), bbz_min - setBBEpsilon.getValue()); //bottom
                        cinolib::vec3d q2 (model.vert(vid).x(), model.vert(vid).y(), bbz_max + setBBEpsilon.getValue()); //top

                        cinolib::vec3d dir = q2-q1;
                        dir /= dir.norm();

                        if(model.vert_data(vid).label == 2) //bottom
                        {
                            double dist;
                            uint id_p;
                            bool intersect_top = octree_top.intersects_ray(q1, dir, dist, id_p);

                            if (!intersect_top)
                            {
                                cinolib::vec3d cp = octree_top.closest_point(model.vert(vid));
                                dist = cp.z() - q1.z();
                            }

                            //cinolib::vec3d p = q1;
                            //p.z() += dist; // * dir;
                            Point3D p_proj, p_proj_original;
                            p_proj.x = q1.x();
                            p_proj.y = q1.y();
                            p_proj.z = q1.z() + dist;

                            p_proj_original.x = model.vert(vid).x();
                            p_proj_original.y = model.vert(vid).y();
                            p_proj_original.z = model.vert(vid).z();

                            proj_top.push_back(p_proj);
                            proj_bot.push_back(p_proj_original);
                        }
                        else if (model.vert_data(vid).label == 1) //// top
                        {
                            double dist;
                            uint id_p;
                            bool intersect_bot = octree_bot.intersects_ray(q1, dir, dist, id_p);

                            if (!intersect_bot)
                            {
                                cinolib::vec3d cp = octree_bot.closest_point(model.vert(vid));
                                dist = cp.z() - q1.z();
                            }

                            //cinolib::vec3d p = q1 + dist * dir; //proiezione del punto "non bottom" su bottom
                            //cinolib::vec3d p = q1;
                            //p.z() += dist; // * dir;
                            Point3D p_proj, p_original;
                            p_proj.x = q1.x();
                            p_proj.y = q1.y();
                            p_proj.z = q1.z() + dist;

                            p_original.x = model.vert(vid).x();
                            p_original.y = model.vert(vid).y();
                            p_original.z = model.vert(vid).z();

                            proj_top.push_back(p_original);
                            proj_bot.push_back(p_proj);
                        }
                        else
                        {
                            double dist;
                            uint id_p;
                            bool intersects_top = octree_top.intersects_ray(q1, dir, dist, id_p);

                            if (!intersects_top)
                            {
                                cinolib::vec3d cp = octree_top.closest_point(model.vert(vid));
                                dist = cp.z() - q1.z();
                            }

                            //cinolib::vec3d p_top = q1 + dist * dir; //proiezione del punto "non top/bottom" su top
                            Point3D p_proj_top;
                            p_proj_top.x = q1.x();
                            p_proj_top.y = q1.y();
                            p_proj_top.z = q1.z() + dist;

                            dist = 0.0;
                            id_p = 0;
                            bool intersects_bot = octree_bot.intersects_ray(q1, dir, dist, id_p);

                            if (!intersects_bot)
                            {
                                cinolib::vec3d cp = octree_bot.closest_point(model.vert(vid));
                                dist = cp.z() - q1.z();
                                //std::cin.ignore();
                            }

                            //cinolib::vec3d p_bot = q1;
                            //p_bot.z() += dist; // * dir;
                            //cinolib::vec3d p_bot = q1 + dist * dir; //proiezione del punto "non top/bottom" su bottom

                            Point3D p_proj_bot;
                            p_proj_bot.x = q1.x();
                            p_proj_bot.y = q1.y();
                            p_proj_bot.z = q1.z() + dist;

                            proj_top.push_back(p_proj_top);
                            proj_bot.push_back(p_proj_bot);
                        }
                    }
                }
                else if(setProjDir.getValue().compare("X") == 0)
                {
                    std::cerr << "ERROR: X projection direction is not available." << std::endl;
                    exit(1);
                }
                else if(setProjDir.getValue().compare("Y") == 0)
                {
                    exit(1);
                }
                else
                {
                    std::cerr << "ERROR: Y projection direction is not available." << std::endl;
                    exit(1);
                }

                geom_name = get_basename(get_filename(geomModel.getValue()));

                dataProjection.mesh = geom_name;
                dataProjection.proj_direction = setProjDir.getValue();
                dataProjection.top_name = geom_name + "_top";
                dataProjection.bottom_name = geom_name + "_bot";
                manmeta.setDataProjection(dataProjection);
                manmeta.write(app_folder + "/geom_" + geom_name + ".json");


                export3d_xyz(app_folder + "/geom_" + dataProjection.top_name + ".dat", proj_top);
                export3d_xyz(app_folder + "/geom_" + dataProjection.bottom_name + ".dat", proj_bot);

                std::cout << std::endl;
                std::cout << "### Number of vertices: " << model.num_verts() << std::endl;
                std::cout << "### Number of projections on top layer: " << proj_top.size() << std::endl;
                std::cout << "### Number of projections on bottom layer: " << proj_bot.size() << std::endl;
            }


            if(setType.getValue().compare("HEX") == 0)
            {
                if(!geomModel.isSet())
                {
                    std::cout << "ERROR set hexahedral mesh --geom <file>"<< std::endl;
                    exit(1);
                }

                MUSE::VolumeMesh<> model;
                model.load(geomModel.getValue().c_str());
                std::cout << "\033[0;32mLoading polyhedral mesh file: " << geomModel.getValue() << " ... COMPLETED.\033[0m" << std::endl;
                std::cout << std::endl;

                cinolib::vec3d bbox_center = model.bbox().center();
                model.translate(-bbox_center);
                model.update_f_normals();
                model.translate(bbox_center);

                // std::vector<std::vector<Point3D>> verts_top, verts_bot;
                std::vector<Point3D> proj_top, proj_bot; //vettore delle proiezioni
                std::vector<uint> id_top, id_bot;

                //octreee
                //passare dall'octree per i punti aggiuntivi nella tet
                // cinolib::Polygonmesh<> surf_mesh_from_model;
                // export_surface(model, surf_mesh_from_model);
                // std::cout << "Extract external surface of volume mesh ... COMPLETED." << std::endl;

                // std::cout << "Compute normals ..." << std::endl;

                // cinolib::vec3d normal(0.0,0.0,0.0);
                // if(setProjDir.getValue().compare("Z") == 0)
                //     normal.z() = 1.0;

                //cinolib::vec3d normal_z(0,0,1);
                cinolib::Polygonmesh mesh_test, mesh_test_top, mesh_test_bot;
                for(uint fid=0; fid < model.num_faces(); fid++)
                {
                    if(model.face_is_on_srf(fid))
                    {
                        int z_sign = sign(model.face_data(fid).normal.z());

                        if (z_sign == 0)
                            continue;

                        if(z_sign == 1)
                        {
                            // std::cout << "################################################################" << std::endl;
                            // std::cout << model.face_data(fid).normal << std::endl;

                            id_top.push_back(fid);

                            model.vert_data(model.face_vert_id(fid,0)).label = 1;
                            model.vert_data(model.face_vert_id(fid,1)).label = 1;
                            model.vert_data(model.face_vert_id(fid,2)).label = 1;
                            // std::vector<uint> vec_verts__top;
                            // vec_verts__top.push_back(mesh_test_top.vert_add(model.face_vert(fid,0)));
                            // vec_verts__top.push_back(mesh_test_top.vert_add(model.face_vert(fid,1)));
                            // vec_verts__top.push_back(mesh_test_top.vert_add(model.face_vert(fid,2)));
                            // mesh_test_top.poly_add(vec_verts__top);
                        }
                        else //if ()
                        {
                            // std::cout << "################################################################" << std::endl;
                            // //std::cout << model.face_data(fid).normal << std::endl;

                            id_bot.push_back(fid);

                            // if((model.vert_data(model.face_vert_id(fid,0)).label == 1) || (model.vert_data(model.face_vert_id(fid,1)).label == 1) || (model.vert_data(model.face_vert_id(fid,2)).label == 1))
                            // {
                            //     std::cerr << "ERROR: the label is equal to 1!" << std::endl;
                            //     exit(1);
                            // }
                            model.vert_data(model.face_vert_id(fid,0)).label = 2;
                            model.vert_data(model.face_vert_id(fid,1)).label = 2;
                            model.vert_data(model.face_vert_id(fid,2)).label = 2;

                            // std::vector<uint> vec_verts__bot;
                            // vec_verts__bot.push_back(mesh_test_bot.vert_add(model.face_vert(fid,0)));
                            // vec_verts__bot.push_back(mesh_test_bot.vert_add(model.face_vert(fid,1)));
                            // vec_verts__bot.push_back(mesh_test_bot.vert_add(model.face_vert(fid,2)));
                            // mesh_test_bot.poly_add(vec_verts__bot);
                        }
                        // else
                        // {
                        //     // std::cout << "################################################################" << std::endl;
                        //     // std::cout << model.face_data(fid).normal << std::endl;

                        //     std::vector<uint> vec_verts__;
                        //     vec_verts__.push_back(mesh_test.vert_add(model.face_vert(fid,0)));
                        //     vec_verts__.push_back(mesh_test.vert_add(model.face_vert(fid,1)));
                        //     vec_verts__.push_back(mesh_test.vert_add(model.face_vert(fid,2)));
                        //     mesh_test.poly_add(vec_verts__);
                        // }
                    }


                    // cinolib::vec3d normal_pid = surf_mesh_from_model.poly_data(pid).normal;
                    // std::cout << "ID: " << pid << " - " << normal_pid << std::endl;

                    // double dotn = normal_pid.dot(normal);
                    // std::cout << "dot: " << dotn << std::endl;
                    // if(dotn > 0)
                    // {
                    //     id_top.push_back(pid);
                    //     // for(cinolib::vec3d p:surf_mesh_from_model.poly_verts(pid))
                    //     // {
                    //     //     Point3D pmuse;
                    //     //     pmuse.x = p.x();
                    //     //     pmuse.y = p.y();
                    //     //     pmuse.z = p.z();
                    //     //     point_top.push_back(pmuse);
                    //     // }
                    //     // verts_top.push_back(point_top);
                    // }
                    // else if(dotn < 0)
                    // {
                    //     id_bot.push_back(pid);
                    //     //std::cout << "perpendicular ..." << std::endl;
                    //     // std::vector<Point3D> point_bot;
                    //     // for(cinolib::vec3d p:surf_mesh_from_model.poly_verts(pid))
                    //     // {
                    //     //     Point3D pmuse;
                    //     //     pmuse.x = p.x();
                    //     //     pmuse.y = p.y();
                    //     //     pmuse.z = p.z();
                    //     //     point_bot.push_back(pmuse);
                    //     // }
                    //     // verts_bot.push_back(point_bot);
                    // }
                    // else
                    // {
                    //     std::cout << "perpendicular ..." << std::endl;
                    //     // std::vector<Point3D> point_bot;
                    //     // for(cinolib::vec3d p:surf_mesh_from_model.poly_verts(pid))
                    //     // {
                    //     //     Point3D pmuse;
                    //     //     pmuse.x = p.x();
                    //     //     pmuse.y = p.y();
                    //     //     pmuse.z = p.z();
                    //     //     point_bot.push_back(pmuse);
                    //     // }
                    //     // verts_bot.push_back(point_bot);
                    // }
                }
                std::cout << "### Size vector top: " << id_top.size() << std::endl;
                std::cout << "### Size vector bot: " << id_bot.size() << std::endl;

                // std::string name_top = "/home/mariannamiola/Devel/muse/examples/MUSE_test/19_Polcevera_tet/out/geometry/volume/mesh_test_top.obj";
                // std::string name_bot = "/home/mariannamiola/Devel/muse/examples/MUSE_test/19_Polcevera_tet/out/geometry/volume/mesh_test_bot.obj";
                // std::string name = "/home/mariannamiola/Devel/muse/examples/MUSE_test/19_Polcevera_tet/out/geometry/volume/mesh_test.obj";
                // mesh_test_top.save(name_top.c_str());
                // mesh_test_bot.save(name_bot.c_str());
                // mesh_test.save(name.c_str());

                // Polygonmesh<> surf_mesh_tb(verts_top, surf_mesh_tb);
                // Polygonmesh<> surf_mesh_b(verts_bot, surf_mesh_b);
                // surf_mesh_tb+=surf_mesh_b;

                // surf_mesh_b.clear();


                bool mesh_is_tet = false;
                if(model.poly_is_tetrahedron(0))
                    mesh_is_tet = true;

                std::cout << FGRN("Check on volumetric mesh type ... COMPLETED.") << std::endl;
                std::cout << std::endl;


                cinolib::Octree octree_top, octree_bot; //costruisco l'octree da mesh laterale
                uint pid_octree=0;
                for(uint spid:id_top)
                {
                    if(mesh_is_tet)
                    {
                        const std::vector<cinolib::vec3d> &vertice = model.face_verts(spid);
                        octree_top.push_triangle(pid_octree++, vertice.at(0), vertice.at(1), vertice.at(2));
                    }
                    else
                    {
                        for(uint i=0; i<model.face_tessellation(spid).size()/3; ++i)
                        {
                            cinolib::vec3d v0 = model.vert(model.face_tessellation(spid).at(3*i+0));
                            cinolib::vec3d v1 = model.vert(model.face_tessellation(spid).at(3*i+1));
                            cinolib::vec3d v2 = model.vert(model.face_tessellation(spid).at(3*i+2));
                            octree_top.push_triangle(pid_octree++,v0,v1,v2);
                        }
                    }
                }

                pid_octree = 0;
                for(uint spid:id_bot)
                {
                    if(mesh_is_tet)
                    {
                        const std::vector<cinolib::vec3d> &vertice = model.face_verts(spid);
                        octree_bot.push_triangle(pid_octree++, vertice.at(0), vertice.at(1), vertice.at(2));
                    }
                    else
                    {
                        for(uint i=0; i<model.face_tessellation(spid).size()/3; ++i)
                        {
                            cinolib::vec3d v0 = model.vert(model.face_tessellation(spid).at(3*i+0));
                            cinolib::vec3d v1 = model.vert(model.face_tessellation(spid).at(3*i+1));
                            cinolib::vec3d v2 = model.vert(model.face_tessellation(spid).at(3*i+2));
                            octree_bot.push_triangle(pid_octree++,v0,v1,v2);
                        }
                    }
                }

                octree_top.build();
                octree_bot.build();


                //octree.build_from_mesh_polys(surf_mesh_tb);
                std::cout << "Build octree ... COMPLETED." << std::endl;

                if(setProjDir.getValue().compare("Y") == 0)
                {
                    // double bby_max = model.bbox().max.y();
                    // double bby_min = model.bbox().min.y();

                    // for(uint vid=0; vid<model.num_verts(); vid++)
                    // {
                    //     std::cout << "### Starting projection of internal vertex by octree ..." << std::endl;
                    //     Point3D proj_max;
                    //     proj_max.y = -DBL_MAX;

                    //     Point3D proj_min;
                    //     proj_min.y = DBL_MAX;

                    //     cinolib::vec3d q1 (model.vert(vid).x(), bby_min - setBBEpsilon.getValue(), model.vert(vid).z()); //bottom
                    //     cinolib::vec3d q2 (model.vert(vid).x(), bby_max + setBBEpsilon.getValue(), model.vert(vid).z()); //top

                    //     cinolib::vec3d dir = q2-q1;

                    //     std::set<std::pair<double,uint>> all_hits;
                    //     octree.intersects_ray(q1, dir, all_hits);

                    //     std::set<std::pair<double, uint>>::iterator itr;
                    //     for (itr = all_hits.begin(); itr != all_hits.end(); itr++)
                    //     {
                    //         cinolib::vec3d p = q1 + itr->first * dir;

                    //         Point3D p_proj;

                    //         if(p.y() != DBL_MAX)
                    //         {
                    //             p_proj.x = p.x();
                    //             p_proj.y = p.y();
                    //             p_proj.z = p.z();
                    //         }

                    //         if(p_proj.y < proj_min.y)
                    //             proj_min = p_proj;

                    //         if(p_proj.y > proj_max.y)
                    //             proj_max = p_proj;
                    //     }

                    //     proj_top.push_back(proj_max);
                    //     proj_bot.push_back(proj_min);
                    // }
                }
                else if(setProjDir.getValue().compare("Z") == 0)
                {
                    double bbz_max = model.bbox().max.z();
                    double bbz_min = model.bbox().min.z();

                    /**std::ofstream top_file__, bot_file__;
                    top_file__.open("/home/danielacabiddu/Devel/src/muse/examples/19_Polcevera_tet_cutmesh/top.xyz", std::ofstream::out );
                    bot_file__.open("/home/danielacabiddu/Devel/src/muse/examples/19_Polcevera_tet_cutmesh/bot.xyz", std::ofstream::out );

                    if (!bot_file__.is_open() || !top_file__.is_open())
                    {
                        if (!top_file__.is_open())
                        {
                            std::cerr << "Error opening file top..." << std::endl;
                        }

                        if (!bot_file__.is_open())
                        {
                            std::cerr << "Error opening file bot..." << std::endl;
                        }
                    }

                    for(uint vid=0; vid<model.num_verts(); vid++)
                    {
                        if (model.vert_data(vid).label == 1)
                            top_file__ << std::setprecision(10) << model.vert(vid).x() << " " << model.vert(vid).y() << " " << model.vert(vid).z() << "\r\n";
                        else if (model.vert_data(vid).label == 2)
                            bot_file__ << std::setprecision(10) << model.vert(vid).x() << " " << model.vert(vid).y() << " " << model.vert(vid).z() << "\r\n";
                    }

                    bot_file__.close();
                    top_file__.close();


                    std::cout << "saved top and bottom" << std::endl;**/

                    uint count_manifold = 0;
                    for(uint vid=0; vid<model.num_verts(); vid++)
                    //for(uint vid=12246; vid<model.num_verts(); vid++)
                    {
                        bool vertmanifold = model.vert_is_manifold(vid);
                        if(vertmanifold)
                        {
                            std::cout << "vert is manifold: VID " << vid << std::endl;
                            count_manifold++;
                        }
                        // Point3D proj_max;
                        // proj_max.x = model.vert(vid).x();
                        // proj_max.y = model.vert(vid).y();
                        // proj_max.z = -DBL_MAX;

                        // Point3D proj_min;
                        // proj_min.x = model.vert(vid).x();
                        // proj_min.y = model.vert(vid).y();
                        // proj_min.z = DBL_MAX;

                        cinolib::vec3d q1 (model.vert(vid).x(), model.vert(vid).y(), bbz_min - setBBEpsilon.getValue()); //bottom
                        cinolib::vec3d q2 (model.vert(vid).x(), model.vert(vid).y(), bbz_max + setBBEpsilon.getValue()); //top

                        cinolib::vec3d dir = q2-q1;
                        dir /= dir.norm();

                        if(model.vert_data(vid).label == 2) //bottom
                        {
                            double dist;
                            uint id_p;
                            bool intersect_top = octree_top.intersects_ray(q1, dir, dist, id_p);

                            if (!intersect_top)
                            {
                                cinolib::vec3d cp = octree_top.closest_point(model.vert(vid));
                                dist = cp.z() - q1.z();
                            }

                            //cinolib::vec3d p = q1;
                            //p.z() += dist; // * dir;
                            Point3D p_proj, p_proj_original;
                            p_proj.x = q1.x();
                            p_proj.y = q1.y();
                            p_proj.z = q1.z() + dist;

                            p_proj_original.x = model.vert(vid).x();
                            p_proj_original.y = model.vert(vid).y();
                            p_proj_original.z = model.vert(vid).z();

                            if(p_proj.z == p_proj_original.z)
                            {
                                std::cerr << "ERROR: Equal projection!! VID: " << vid << std::endl;
                                //exit(1);
                            }

                            proj_top.push_back(p_proj);
                            proj_bot.push_back(p_proj_original);
                        }
                        else if (model.vert_data(vid).label == 1) //// top
                        {
                            double dist;
                            uint id_p;
                            bool intersect_bot = octree_bot.intersects_ray(q1, dir, dist, id_p);

                            if (!intersect_bot)
                            {
                                cinolib::vec3d cp = octree_bot.closest_point(model.vert(vid));
                                dist = cp.z() - q1.z();
                            }

                            //cinolib::vec3d p = q1 + dist * dir; //proiezione del punto "non bottom" su bottom
                            //cinolib::vec3d p = q1;
                            //p.z() += dist; // * dir;
                            Point3D p_proj, p_original;
                            p_proj.x = q1.x();
                            p_proj.y = q1.y();
                            p_proj.z = q1.z() + dist;

                            p_original.x = model.vert(vid).x();
                            p_original.y = model.vert(vid).y();
                            p_original.z = model.vert(vid).z();

                            if(p_proj.z == p_original.z)
                            {
                                std::cerr << "ERROR: Equal projection!! VID: " << vid << std::endl;
                                //exit(1);
                            }

                            proj_top.push_back(p_original);
                            proj_bot.push_back(p_proj);
                        }
                        else
                        {
                            double dist;
                            uint id_p;
                            bool intersects_top = octree_top.intersects_ray(q1, dir, dist, id_p);

                            if (!intersects_top)
                            {                                
                                cinolib::vec3d cp = octree_top.closest_point(model.vert(vid));
                                dist = cp.z() - q1.z();
                            }

                            //cinolib::vec3d p_top = q1 + dist * dir; //proiezione del punto "non top/bottom" su top
                            Point3D p_proj_top;
                            p_proj_top.x = q1.x();
                            p_proj_top.y = q1.y();
                            p_proj_top.z = q1.z() + dist;

                            dist = 0.0;
                            id_p = 0;
                            bool intersects_bot = octree_bot.intersects_ray(q1, dir, dist, id_p);

                            if (!intersects_bot)
                            {
                                cinolib::vec3d cp = octree_bot.closest_point(model.vert(vid));
                                dist = cp.z() - q1.z();
                                //std::cin.ignore();
                            }

                            //cinolib::vec3d p_bot = q1;
                            //p_bot.z() += dist; // * dir;
                            //cinolib::vec3d p_bot = q1 + dist * dir; //proiezione del punto "non top/bottom" su bottom

                            Point3D p_proj_bot;
                            p_proj_bot.x = q1.x();
                            p_proj_bot.y = q1.y();
                            p_proj_bot.z = q1.z() + dist;

                            if(p_proj_top.z == p_proj_bot.z)
                            {
                                std::cerr << "ERROR: Equal projection!! VID: " << vid << std::endl;
                                //exit(1);
                            }

                            proj_top.push_back(p_proj_top);
                            proj_bot.push_back(p_proj_bot);
                        }
                    }

                    std::cout << "Number of manifold vertices " << count_manifold << std::endl;
                }
                else if(setProjDir.getValue().compare("X") == 0)
                {
                    exit(1);
                }
                else
                {
                    std::cerr << "ERROR: no projection direction is set between X,Y or Z" << std::endl;
                    exit(1);
                }


                geom_name = get_basename(get_filename(geomModel.getValue()));

                dataProjection.mesh = geom_name;
                dataProjection.proj_direction = setProjDir.getValue();
                dataProjection.top_name = geom_name + "_top";
                dataProjection.bottom_name = geom_name + "_bot";
                manmeta.setDataProjection(dataProjection);
                manmeta.write(app_folder + "/geom_" + geom_name + ".json");


                export3d_xyz(app_folder + "/geom_" + dataProjection.top_name + ".dat", proj_top);
                export3d_xyz(app_folder + "/geom_" + dataProjection.bottom_name + ".dat", proj_bot);

                std::cout << std::endl;
                std::cout << "### Number of vertices: " << model.num_verts() << std::endl;
                std::cout << "### Number of projections on top layer: " << proj_top.size() << std::endl;
                std::cout << "### Number of projections on bottom layer: " << proj_bot.size() << std::endl;
            }


            //export_surface(mesh, surf_mesh);
            //std::cout << "Extract external surface of volume mesh ... COMPLETED." << std::endl;


            //std::cout << "\033[0;32mComputing points projection on mesh file: " << geom_name << " ... COMPLETED.\033[0m" << std::endl;
            //std::cout << std::endl;
        }

        std::cout << "Points projection on section boundary ... COMPLETED." << std::endl;

    }


    if(setStratigraphicTransf.isSet())
    {
        // Opzione: trasformazione coordinate originali in coordinate stratigrafiche
        if(stratCondition.getValue().compare("NO") == 0) //Condizione di default
        {
            std::cout << "\033[0;31mERROR: No stratigraphic trasformation is set. The coordinate system remains unchanged.\033[0m" << std::endl;
            std::cout << "\033[0;31mSet --sttype <type> --top <name_top_surface> --bot <name_bot_surface> to compute stratigraphic coordinate transformation.\033[0m" << std::endl;
            exit(1);
        }
        else
        {
            std::string abs_datadir = out_folder + "/" + app_data;
            std::vector<std::string> list_dir = get_directories(abs_datadir);
            if(list_dir.empty())
                list_dir.push_back(abs_datadir);

            if((get_filename(list_dir.at(0)).compare("data") == 0 && get_filename(list_dir.at(1)).compare("metadata") == 0)
                || (get_filename(list_dir.at(1)).compare("data") == 0 && get_filename(list_dir.at(0)).compare("metadata") == 0))
            {
                list_dir.clear();
                list_dir.resize(1, abs_datadir);
            }

            int count_frame = 0;
            for(const std::string &l:list_dir)
            {
                count_frame++;

                filesystem::path dir = l;
                filesystem::path rel_datadir = filesystem::relative(dir, abs_datadir);
                std::cout << rel_datadir.string() << std::endl;

                app_folder.clear();
                app_folder = out_folder + "/" + app_name;
                if(!filesystem::exists(app_folder))
                    filesystem::create_directory(app_folder);

                if(rel_datadir.string().compare(".") != 0)
                {
                    app_folder += "/" + rel_datadir.string();
                    filesystem::create_directory(app_folder);

                    std::cout << std::endl;
                    std::cout << "###########################" << std::endl;
                    std::cout << "### NUMBER OF TIME FRAMES: " << list_dir.size() << std::endl;
                    std::cout << "### TIME FRAME N° " << count_frame << " ON " << list_dir.size() << std::endl;
                    std::cout << "### TIME FRAME NAME: " << rel_datadir.string() << std::endl;
                    std::cout << std::endl;
                }

                std::vector<std::string> list_json = get_files(l, ".json");
                if(list_json.size() > 1)
                {
                    std::cerr << "ERROR. Only a file JSON is expected!" << std::endl;
                    exit(1);
                }

                ManipulateMeta manmeta;
                ManipulateMeta::StratigraphicTransf strat;
                strat.strat_transf_is_set = true;
                strat.data_type = setType.getValue();

                strat.top_name = topSurface.getValue();
                strat.bottom_name = botSurface.getValue();

                strat.transformation_type = stratCondition.getValue();


                std::cout << "\033[0;33mWARNING: Stratigraphic transformation is set on " << stratCondition.getValue() << ".\033[0m" << std::endl;
                MUSE::stratigraphicCondition stratCond;
                convert_from_str(stratCondition.getValue(), stratCond);

                std::cout << "Direction of projection is set on " << setProjDir.getValue() << std::endl;

                if(setType.getValue().compare("SAMPLES") == 0) //leggo in automatico le coordinate del set di dati (da json data)
                {
                    manmeta.read(app_folder + "/samples_" + geomName.getValue() + ".json");

                    MUSE::DataMeta datameta;
                    datameta.read(list_json.at(0));

                    std::vector<double> xCoord, yCoord, zCoord;
                    if(datameta.getInfoData().x_name.compare("Unknown") != 0)
                        readCoordinate(l + "/data/" + datameta.getInfoData().x_name + ".dat", xCoord);
                    else
                    {
                        std::cerr << "ERROR reading X coordinate: " << l + "/data/" + datameta.getInfoData().x_name + ".dat" << " NOT found." << std::endl;
                        exit(1);
                    }

                    if(datameta.getInfoData().y_name.compare("Unknown") != 0)
                        readCoordinate(l + "/data/" + datameta.getInfoData().y_name + ".dat", yCoord);
                    else
                    {
                        std::cerr << "ERROR reading Y coordinate: " << l + "/data/" + datameta.getInfoData().y_name + ".dat" << " NOT found." << std::endl;
                        exit(1);
                    }

                    if(datameta.getInfoData().z_name.compare("Unknown") != 0)
                        readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                    else
                    {
                        if(setZcoord.isSet())
                            readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                        else
                        {
                            std::cerr << "\033[0;32mWARNING: Z coordinate is Unknown. Set -z --name <variable> for setting the variable.\033[0;0m" << std::endl;
                            exit(1);
                        }
                    }




                    //EXPLOITING CINOLIB FUNCTIONALITIES
                    MUSE::Rotation dataRotation;

                    std::vector<cinolib::vec3d> coord_samples;
                    if(setRotAxis.isSet())
                    {
                        std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                        std::cout << "Rotation axis: " << setRotAxis.getValue() << std::endl;
                        std::cout << "Rotation center: [" << setRotCenterX.getValue() << "; "<< setRotCenterY.getValue() << "; " << setRotCenterZ.getValue() << "]" << std::endl;
                        std::cout << "Rotation angle (degree): " << setRotAngle.getValue() << std::endl;
                        std::cout << std::endl;

                        dataRotation.rotation = true;
                        dataRotation.rotation_axis = setRotAxis.getValue();
                        dataRotation.rotation_center_x = setRotCenterX.getValue();
                        dataRotation.rotation_center_y = setRotCenterY.getValue();
                        dataRotation.rotation_center_z = setRotCenterZ.getValue();
                        dataRotation.rotation_angle = setRotAngle.getValue();

                        manmeta.setRotation(dataRotation);
                    }

                    std::vector<uint> sub_index;
                    if(subDataset.isSet())
                    {
                        MUSE::ExtractionMeta extrmeta;
                        extrmeta.read(app_folder + "/" + subDataset.getValue() + ".json");
                        sub_index = extrmeta.getDataExtraction().id_points;

                        dataRotation = extrmeta.getRotation();

                        std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                        std::cout << "Rotation axis: " << dataRotation.rotation_axis << std::endl;
                        std::cout << "Rotation center: [" << dataRotation.rotation_center_x << "; " << dataRotation.rotation_center_y << "; " << dataRotation.rotation_center_z << "]" <<  std::endl;
                        std::cout << "Rotation angle (degree): " << dataRotation.rotation_angle << std::endl;
                        std::cout << std::endl;

                        manmeta.setRotation(dataRotation);
                    }
                    else
                    {
                        for(uint i=0; i< xCoord.size(); i++)
                            sub_index.push_back(i);
                    }


                    for(uint i:sub_index)
                    {
                        cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));

                        if(dataRotation.rotation == true)
                        {
                            cinolib::vec3d axis = set_rotation_axis(dataRotation.rotation_axis);
                            cinolib::vec3d c (dataRotation.rotation_center_x, dataRotation.rotation_center_y, dataRotation.rotation_center_z);
                            sample = point_rotation(sample, axis, dataRotation.rotation_angle, c);
                        }
                        coord_samples.push_back(sample);
                    }



                    //leggi in input le proiezioni
                    std::vector<Point3D> pztop, pzbot;
                    load_xyzfile(app_folder + "/samples_" + topSurface.getValue() + ".dat", pztop);
                    load_xyzfile(app_folder + "/samples_" + botSurface.getValue() + ".dat", pzbot);

                    if((pztop.size() != coord_samples.size()) || (pzbot.size() != coord_samples.size()))
                    {
                        std::cerr << "ERROR size of top/bottom projection vectors is different from samples." << std::endl;
                        exit(1);
                    }

                    std::vector<Point3D> coord_samples_transformed;

                    if (setProjDir.getValue().compare("Y") == 0)
                    {
                        strat.avg_thick = 0.0;
                        double sum = 0.0;
                        for(size_t i=0; i<pztop.size(); i++)
                        {
                            double thick = abs(pztop.at(i).y - pzbot.at(i).y);
                            sum += thick;
                        }
                        strat.avg_thick = sum/pztop.size();

                        for(size_t i=0; i<coord_samples.size(); i++)
                        {
                            double transformed_y = 0.0;
                            forward_transformation(stratCond, coord_samples.at(i).y(), pztop.at(i).y, pzbot.at(i).y, transformed_y, strat.avg_thick);

                            Point3D sample;
                            sample.x = coord_samples.at(i).x();
                            sample.y = transformed_y;
                            sample.z = coord_samples.at(i).z();

                            coord_samples_transformed.push_back(sample);
                        }
                    }
                    else if (setProjDir.getValue().compare("X") == 0)
                    {
                        strat.avg_thick = 0.0;
                        double sum = 0.0;
                        for(size_t i=0; i<pztop.size(); i++)
                        {
                            double thick = abs(pztop.at(i).x - pzbot.at(i).x);
                            sum += thick;
                        }
                        strat.avg_thick = sum/pztop.size();

                        for(size_t i=0; i<coord_samples.size(); i++)
                        {
                            double transformed_x = 0.0;
                            forward_transformation(stratCond, coord_samples.at(i).x(), pztop.at(i).x, pzbot.at(i).x, transformed_x, strat.avg_thick);

                            Point3D sample;
                            sample.x = transformed_x;
                            sample.y = coord_samples.at(i).y();
                            sample.z = coord_samples.at(i).z();

                            coord_samples_transformed.push_back(sample);
                        }
                    }
                    else if (setProjDir.getValue().compare("Z") == 0)
                    {
                        strat.avg_thick = 0.0;
                        double sum = 0.0;
                        for(size_t i=0; i<pztop.size(); i++)
                        {
                            double thick = abs(pztop.at(i).z - pzbot.at(i).z);
                            sum += thick;
                        }
                        strat.avg_thick = sum/pztop.size();

                        for(size_t i=0; i<coord_samples.size(); i++)
                        {
                            double transformed_z = 0.0;
                            forward_transformation(stratCond, coord_samples.at(i).z(), pztop.at(i).z, pzbot.at(i).z, transformed_z, strat.avg_thick);

                            Point3D sample;
                            sample.x = coord_samples.at(i).x();
                            sample.y = coord_samples.at(i).y();
                            sample.z = transformed_z;

                            std::cout << "id: " << i << " ;" << sample.x << " ;" << sample.y << " ;" << sample.z << std::endl;
                            coord_samples_transformed.push_back(sample);
                        }
                    }
                    else
                    {
                        std::cout << FRED("Direction of projection is unknown. NOTHING TO DO ...") << std::endl;
                        exit(1);
                    }

                     manmeta.setStratigraphicTransf(strat);
                     manmeta.write(app_folder + "/samples_" + geomName.getValue() + ".json");
                     export3d_xyz(app_folder + "/samples_" + geomName.getValue() + ".xyz", coord_samples_transformed);
                }


                if(setType.getValue().compare("GEOMETRY") == 0)
                {
                    if(!geomModel.isSet())
                    {
                        std::cout << "\033[0;31mERROR: Set --geom <model.off>\033[0;0m" << std::endl;
                        exit(1);
                    }

                    manmeta.read(app_folder + "/geom_" + geomName.getValue() + ".json");

                    cinolib::Polygonmesh<> model, strat_model;
                    model.load(geomModel.getValue().c_str());
                    std::cout << "\033[0;32mLoading mesh file: " << geomModel.getValue() << " ... COMPLETED.\033[0m" << std::endl;
                    std::cout << std::endl;

                    // //AGGIUNGERE LA CONDIZIONE PER LA TRASLAZIONE
                    // cinolib::vec3d center = model.bbox().center();
                    // std::cout << "### Translate mesh at BBOX center: " << center << std::endl;
                    // model.translate(-center);

                    std::string geom_name = geomModel.getValue().substr(geomModel.getValue().find_last_of("/")+1, geomModel.getValue().length());
                    geom_name = get_basename(geom_name);


                    // Modificare la z con le trasformazioni in coordinate stratigrafiche, in base al tipo dichiarato da linea di comando --strat
                    std::vector<Point3D> pztop, pzbot;
                    load_xyzfile(app_folder + "/geom_" + topSurface.getValue() + ".dat", pztop);
                    load_xyzfile(app_folder + "/geom_" + botSurface.getValue() + ".dat", pzbot);


                    if (setProjDir.getValue().compare("Y") == 0)
                    {
                        strat.avg_thick = 0.0;
                        double sum = 0.0;
                        for(size_t i=0; i<pztop.size(); i++)
                        {
                            double thick = abs(pztop.at(i).y-pzbot.at(i).y);
                            sum += thick;
                        }
                        strat.avg_thick = sum/pztop.size();


                        strat_model = model;
                        for(size_t vid=0; vid< model.num_verts(); vid++)
                        {
                            if(pztop.at(vid).y == pzbot.at(vid).y)
                                std::cout << "ERRORE: i valori sono uguali: denominatore va a 0. ID = " << vid << std::endl;

                            forward_transformation(stratCond, model.vert(vid).y(), pztop.at(vid).y, pzbot.at(vid).y, strat_model.vert(vid).y(), strat.avg_thick);
                        }
                    }
                    else if (setProjDir.getValue().compare("X") == 0)
                    {
                        strat.avg_thick = 0.0;
                        double sum = 0.0;
                        for(size_t i=0; i<pztop.size(); i++)
                        {
                            double thick = abs(pztop.at(i).x-pzbot.at(i).x);
                            sum += thick;
                        }
                        strat.avg_thick = sum/pztop.size();


                        strat_model = model;
                        for(size_t vid=0; vid< model.num_verts(); vid++)
                        {
                            if(pztop.at(vid).x == pzbot.at(vid).x)
                                std::cout << "ERRORE: i valori sono uguali: denominatore va a 0. ID = " << vid << std::endl;

                            forward_transformation(stratCond, model.vert(vid).x(), pztop.at(vid).x, pzbot.at(vid).x, strat_model.vert(vid).x(), strat.avg_thick);
                        }
                    }
                    else
                    {
                        std::cout << FRED("Direction of projection is Z or unknown. NOTHING TO DO ...") << std::endl;
                        exit(1);
                    }


                    std::string ext_mesh = ".off";
                    if(objConversion.isSet() == true)
                        ext_mesh = ".obj";

                    std::string strat_model_name = app_folder + "/geom_" + geom_name + ext_mesh;
                    strat_model.save(strat_model_name.c_str());

                    manmeta.setStratigraphicTransf(strat);
                    manmeta.write(app_folder + "/geom_" + geom_name + ".json");
                }


                if(setType.getValue().compare("VOLUME") == 0)
                {
                    if(!geomModel.isSet())
                    {
                        std::cout << "\033[0;31mERROR: Set --geom <model.off>\033[0;0m" << std::endl;
                        exit(1);
                    }

                    manmeta.read(app_folder + "/geom_" + geomName.getValue() + ".json");

                    //cinolib::Polyhedralmesh<> model; //, strat_model;
                    MUSE::VolumeMesh<> model;
                    model.load(geomModel.getValue().c_str());
                    std::cout << "\033[0;32mLoading mesh file: " << geomModel.getValue() << " ... COMPLETED.\033[0m" << std::endl;
                    std::cout << std::endl;

                    std::string geom_name = geomModel.getValue().substr(geomModel.getValue().find_last_of("/")+1, geomModel.getValue().length());
                    geom_name = get_basename(geom_name);

                    // bool mesh_is_tet = false;
                    // for(uint pid=0; pid<model.num_polys(); pid++)
                    // {
                    //     if(model.poly_is_tetrahedron(pid))
                    //         mesh_is_tet = true;
                    // }
                    // std::cout << FGRN("Check on volumetric mesh type ... COMPLETED.") << std::endl;
                    // std::cout << std::endl;


                    // Modificare la z con le trasformazioni in coordinate stratigrafiche, in base al tipo dichiarato da linea di comando --strat
                    std::vector<Point3D> pztop, pzbot;
                    load_xyzfile(app_folder + "/geom_" + topSurface.getValue() + ".dat", pztop);
                    load_xyzfile(app_folder + "/geom_" + botSurface.getValue() + ".dat", pzbot);

                    /////////////////////////////////////////////////////////////////////////////////
                    /////////////////////////////////////////////////////////////////////////////////
                    /////////////////////////////////////////////////////////////////////////////////
                    //UPDATE CODE - 05/03/2025
                    if (setProjDir.getValue().compare("Y") == 0)
                    {
                        strat.avg_thick = 0.0;
                        double sum = 0.0;
                        for(size_t i=0; i<pztop.size(); i++)
                        {
                            double thick = abs(pztop.at(i).y-pzbot.at(i).y);
                            sum += thick;
                        }
                        strat.avg_thick = sum/pztop.size();

                        // strat_model = model;
                        for(size_t vid=0; vid< model.num_verts(); vid++)
                        {
                            if(pztop.at(vid).y == pzbot.at(vid).y)
                            {
                                std::cerr << "### ERROR - Equal values for ID = " << vid << std::endl;
                                std::cerr << "### y-value on top surface: " << pztop.at(vid).y << std::endl;
                                std::cerr << "### y-value on bottom surface: " << pzbot.at(vid).y << std::endl;
                                exit(1);
                            }

                            double y_original = model.vert(vid).y();
                            forward_transformation(stratCond, y_original, pztop.at(vid).y, pzbot.at(vid).y, model.vert(vid).y(), strat.avg_thick);
                        }
                    }
                    else if (setProjDir.getValue().compare("X") == 0)
                    {
                        strat.avg_thick = 0.0;
                        double sum = 0.0;
                        for(size_t i=0; i<pztop.size(); i++)
                        {
                            double thick = abs(pztop.at(i).x-pzbot.at(i).x);
                            sum += thick;
                        }
                        strat.avg_thick = sum/pztop.size();

                        // strat_model = model;
                        for(size_t vid=0; vid< model.num_verts(); vid++)
                        {
                            if(pztop.at(vid).x == pzbot.at(vid).x)
                            {
                                std::cerr << "### ERROR - Equal values for ID = " << vid << std::endl;
                                std::cerr << "### x-value on top surface: " << pztop.at(vid).x << std::endl;
                                std::cerr << "### x-value on bottom surface: " << pzbot.at(vid).x << std::endl;
                                exit(1);
                            }

                            double x_original = model.vert(vid).x();
                            forward_transformation(stratCond, x_original, pztop.at(vid).x, pzbot.at(vid).x, model.vert(vid).x(), strat.avg_thick);
                        }
                    }
                    else if (setProjDir.getValue().compare("Z") == 0)
                    {
                        strat.avg_thick = 0.0;
                        double sum = 0.0;
                        for(size_t i=0; i<pztop.size(); i++)
                        {
                            double thick = abs(pztop.at(i).z-pzbot.at(i).z);
                            sum += thick;
                        }
                        strat.avg_thick = sum/pztop.size();

                        //strat_model = model;
                        for(size_t vid=0; vid< model.num_verts(); vid++)
                        {
                            if(pztop.at(vid).z == pzbot.at(vid).z)
                            {
                                std::cerr << "### ERROR - Equal values for ID = " << vid << std::endl;
                                std::cerr << "### z-value on top surface: " << pztop.at(vid).z << std::endl;
                                std::cerr << "### z-value on bottom surface: " << pzbot.at(vid).z << std::endl;
                                exit(1);
                            }

                            double z_original = model.vert(vid).z();
                            forward_transformation(stratCond, z_original, pztop.at(vid).z, pzbot.at(vid).z, model.vert(vid).z(), strat.avg_thick);
                        }
                    }
                    else
                    {
                        std::cout << FRED("Direction of projection is unknown. NOTHING TO DO ...") << std::endl;
                        exit(1);
                    }

                    std::string ext_mesh = ".mesh";
                    if(vtkConversion.isSet() == true)
                        ext_mesh = ".vtk";

                    std::string strat_model_name = app_folder + "/geom_" + geom_name + ext_mesh;
                    model.write_poly_VTK(strat_model_name.c_str());
                    /////////////////////////////////////////////////////////////////////////////////
                    /////////////////////////////////////////////////////////////////////////////////
                    /////////////////////////////////////////////////////////////////////////////////


                    // if(mesh_is_tet)
                    // {
                    //     model.clear();

                    //     cinolib::Tetmesh<> model, strat_model;
                    //     model.load(geomModel.getValue().c_str());
                    //     std::cout << "\033[0;32mLoading mesh file: " << geomModel.getValue() << " ... COMPLETED.\033[0m" << std::endl;
                    //     std::cout << std::endl;

                    //     if (setProjDir.getValue().compare("Y") == 0)
                    //     {
                    //         strat.avg_thick = 0.0;
                    //         double sum = 0.0;
                    //         for(size_t i=0; i<pztop.size(); i++)
                    //         {
                    //             double thick = abs(pztop.at(i).y-pzbot.at(i).y);
                    //             sum += thick;
                    //         }
                    //         strat.avg_thick = sum/pztop.size();


                    //         strat_model = model;
                    //         for(size_t vid=0; vid< model.num_verts(); vid++)
                    //         {
                    //             if(pztop.at(vid).y == pzbot.at(vid).y)
                    //                 std::cout << "ERRORE: i valori sono uguali: denominatore va a 0. ID = " << vid << std::endl;

                    //             forward_transformation(stratCond, model.vert(vid).y(), pztop.at(vid).y, pzbot.at(vid).y, strat_model.vert(vid).y(), strat.avg_thick);
                    //         }
                    //     }
                    //     else if (setProjDir.getValue().compare("X") == 0)
                    //     {
                    //         strat.avg_thick = 0.0;
                    //         double sum = 0.0;
                    //         for(size_t i=0; i<pztop.size(); i++)
                    //         {
                    //             double thick = abs(pztop.at(i).x-pzbot.at(i).x);
                    //             sum += thick;
                    //         }
                    //         strat.avg_thick = sum/pztop.size();


                    //         strat_model = model;
                    //         for(size_t vid=0; vid< model.num_verts(); vid++)
                    //         {
                    //             if(pztop.at(vid).x == pzbot.at(vid).x)
                    //                 std::cout << "ERRORE: i valori sono uguali: denominatore va a 0. ID = " << vid << std::endl;

                    //             forward_transformation(stratCond, model.vert(vid).x(), pztop.at(vid).x, pzbot.at(vid).x, strat_model.vert(vid).x(), strat.avg_thick);
                    //         }
                    //     }
                    //     else if (setProjDir.getValue().compare("Z") == 0)
                    //     {
                    //         strat.avg_thick = 0.0;
                    //         double sum = 0.0;
                    //         for(size_t i=0; i<pztop.size(); i++)
                    //         {
                    //             double thick = abs(pztop.at(i).z-pzbot.at(i).z);
                    //             sum += thick;
                    //         }
                    //         strat.avg_thick = sum/pztop.size();


                    //         strat_model = model;
                    //         for(size_t vid=0; vid< model.num_verts(); vid++)
                    //         {
                    //             if(pztop.at(vid).z == pzbot.at(vid).z)
                    //             {
                    //                 std::cout << "ERRORE: i valori sono uguali: denominatore va a 0. ID = " << vid << std::endl;
                    //                 std::cout << "TOP-Z: " << pztop.at(vid).z << std::endl;
                    //                 std::cout << "BOTTOM-Z: " << pzbot.at(vid).z << std::endl;
                    //                 exit(1);
                    //             }

                    //             forward_transformation(stratCond, model.vert(vid).z(), pztop.at(vid).z, pzbot.at(vid).z, strat_model.vert(vid).z(), strat.avg_thick);
                    //         }
                    //     }
                    //     else
                    //     {
                    //         std::cout << FRED("Direction of projection is unknown. NOTHING TO DO ...") << std::endl;
                    //         exit(1);
                    //     }

                    //     std::string ext_mesh = ".mesh";
                    //     if(vtkConversion.isSet() == true)
                    //         ext_mesh = ".vtk";

                    //     std::string strat_model_name = app_folder + "/geom_" + geom_name + ext_mesh;
                    //     strat_model.save(strat_model_name.c_str());

                    // }
                    // else
                    // {
                    //     model.clear();

                    //     cinolib::Hexmesh<> model, strat_model;
                    //     model.load(geomModel.getValue().c_str());
                    //     std::cout << "\033[0;32mLoading mesh file: " << geomModel.getValue() << " ... COMPLETED.\033[0m" << std::endl;
                    //     std::cout << std::endl;

                    //     if (setProjDir.getValue().compare("Y") == 0)
                    //     {
                    //         strat.avg_thick = 0.0;
                    //         double sum = 0.0;
                    //         for(size_t i=0; i<pztop.size(); i++)
                    //         {
                    //             double thick = abs(pztop.at(i).y-pzbot.at(i).y);
                    //             sum += thick;
                    //         }
                    //         strat.avg_thick = sum/pztop.size();


                    //         strat_model = model;
                    //         for(size_t vid=0; vid< model.num_verts(); vid++)
                    //         {
                    //             if(pztop.at(vid).y == pzbot.at(vid).y)
                    //                 std::cout << "ERRORE: i valori sono uguali: denominatore va a 0. ID = " << vid << std::endl;

                    //             forward_transformation(stratCond, model.vert(vid).y(), pztop.at(vid).y, pzbot.at(vid).y, strat_model.vert(vid).y(), strat.avg_thick);
                    //         }
                    //     }
                    //     else if (setProjDir.getValue().compare("X") == 0)
                    //     {
                    //         strat.avg_thick = 0.0;
                    //         double sum = 0.0;
                    //         for(size_t i=0; i<pztop.size(); i++)
                    //         {
                    //             double thick = abs(pztop.at(i).x-pzbot.at(i).x);
                    //             sum += thick;
                    //         }
                    //         strat.avg_thick = sum/pztop.size();


                    //         strat_model = model;
                    //         for(size_t vid=0; vid< model.num_verts(); vid++)
                    //         {
                    //             if(pztop.at(vid).x == pzbot.at(vid).x)
                    //                 std::cout << "ERRORE: i valori sono uguali: denominatore va a 0. ID = " << vid << std::endl;

                    //             forward_transformation(stratCond, model.vert(vid).x(), pztop.at(vid).x, pzbot.at(vid).x, strat_model.vert(vid).x(), strat.avg_thick);
                    //         }
                    //     }
                    //     else if (setProjDir.getValue().compare("Z") == 0)
                    //     {
                    //         strat.avg_thick = 0.0;
                    //         double sum = 0.0;
                    //         for(size_t i=0; i<pztop.size(); i++)
                    //         {
                    //             double thick = abs(pztop.at(i).z-pzbot.at(i).z);
                    //             sum += thick;
                    //         }
                    //         strat.avg_thick = sum/pztop.size();


                    //         strat_model = model;
                    //         for(size_t vid=0; vid< model.num_verts(); vid++)
                    //         {
                    //             if(pztop.at(vid).z == pzbot.at(vid).z)
                    //             {
                    //                 std::cout << "ERRORE: i valori sono uguali: denominatore va a 0. ID = " << vid << std::endl;
                    //                 std::cout << "TOP-Z: " << pztop.at(vid).z << std::endl;
                    //                 std::cout << "BOTTOM-Z: " << pzbot.at(vid).z << std::endl;
                    //                 exit(1);
                    //             }

                    //             forward_transformation(stratCond, model.vert(vid).z(), pztop.at(vid).z, pzbot.at(vid).z, strat_model.vert(vid).z(), strat.avg_thick);
                    //         }
                    //     }

                    //     std::string ext_mesh = ".mesh";
                    //     if(vtkConversion.isSet() == true)
                    //         ext_mesh = ".vtk";

                    //     std::string strat_model_name = app_folder + "/geom_" + geom_name + ext_mesh;
                    //     strat_model.save(strat_model_name.c_str());
                    // }

                    manmeta.setStratigraphicTransf(strat);
                    manmeta.write(app_folder + "/geom_" + geom_name + ".json");
                }
            }

            std::cout << std::endl;
            std::cout << "Stratigraphic transformation... COMPLETED." << std::endl;
        }
    }


    //TO DO: ALLINEARE JSON
/*    if(setIntervalExtraction.isSet())
    {
        // 1) Reading coordinates
        std::vector<double> var;
        if(nameVar.isSet())
            readCoordinate(data_folder + "/" + nameVar.getValue() + ".dat", var);
        else
        {
            std::cout << "ERROR: Set name of variable to check: -v <name>" << std::endl;
            exit(1);
        }


        std::vector<uint> id_points_in;
        if(subDataset.isSet())
        {
            std::vector<int> index;

            json man_json;
            read_json(app_folder + "/" + subDataset.getValue() + ".json", man_json);

            for (auto& x : man_json["Data Extraction"]["Points Index"].items())
            {
                index.push_back(x.value());
                //std::cout << "key: " << x.key() << ", value: " << x.value() << '\n';
            }

            // Check on point is in interval
            for(size_t i=0; i<var.size(); i++)
                if(var.at(i) >= infInterval.getValue() && var.at(i) <= supInterval.getValue())
                    if(check_index (index, i) == true)
                        id_points_in.push_back(i);
        }
        else
        {
            // Check on point is in interval
            for(size_t i=0; i<var.size(); i++)
                if(var.at(i) >= infInterval.getValue() && var.at(i) <= supInterval.getValue())
                    id_points_in.push_back(i);
        }


        std::cout << id_points_in.size() << " points in interval: [" << infInterval.getValue() << "; " << supInterval.getValue() << "]" << std::endl;

        json meta;
        build_defaultJson(Project, meta);

        meta.push_back({"Data Extraction", {
                            {"Variable Name", nameVar.getValue()},
                            {"Inf Interval", infInterval.getValue()},
                            {"Sup Interval", supInterval.getValue()},
                            {"Extreme", "INCLUDED"},
                            {"N. points", id_points_in.size()},
                            {"Points Index", id_points_in}}
                      });

        if(subDataset.isSet())
            save_json(Project.folder + "/out/"+ app_name + "/" + nameVar.getValue()+ std::to_string(infInterval.getValue()) + "_" + std::to_string(supInterval.getValue()) + "_" + subDataset.getValue(), meta);
        else
            save_json(Project.folder + "/out/"+ app_name + "/" + nameVar.getValue()+ std::to_string(infInterval.getValue()) + "_" + std::to_string(supInterval.getValue()), meta);

        std::cout << "Points extrapolation... COMPLETED." << std::endl;
    }*/






    /*if(setProjectionOnVolume.isSet())
    {
        MUSE::ManipulateMeta manmeta;
        manmeta.setProject(Project);

        ManipulateMeta::DataProjection dataProjection;
        dataProjection.proj_is_set = true;
        dataProjection.data_type = setType.getValue();

        if(setType.getValue().compare("SAMPLES") == 0) //leggo in automatico le coordinate del set di dati (da json data)
        {
            std::vector<double> xCoord, yCoord, zCoord;
            if(!meshFiles.isSet())
            {
                std::cerr << "ERROR. Set mesh files -m <filename>" << std::endl;
                exit(1);
            }


            MUSE::DataMeta datameta;
            datameta.read(out_folder + "/data/" + Project.name +".json");

            if(datameta.getInfoData().x_name.compare("Unknown") != 0)
                readCoordinate(data_folder + "/" + datameta.getInfoData().x_name + ".dat", xCoord);
            else
            {
                std::cerr << "ERROR reading X coordinate: " << data_folder + "/" + datameta.getInfoData().x_name + ".dat" << " NOT found." << std::endl;
                exit(1);
            }

            if(datameta.getInfoData().y_name.compare("Unknown") != 0)
                readCoordinate(data_folder + "/" + datameta.getInfoData().y_name + ".dat", yCoord);
            else
            {
                std::cerr << "ERROR reading Y coordinate: " << data_folder + "/" + datameta.getInfoData().y_name + ".dat" << " NOT found." << std::endl;
                exit(1);
            }

            if(datameta.getInfoData().z_name.compare("Unknown") != 0)
                readCoordinate(data_folder + "/" + datameta.getInfoData().z_name + ".dat", zCoord);
            else
            {
                if(setZcoord.isSet())
                    readCoordinate(data_folder + "/" + datameta.getInfoData().z_name + ".dat", zCoord);
                else
                {
                    std::cerr << "\033[0;32mWARNING: Z coordinate is Unknown. Set -z --name <variable> for setting the variable.\033[0;0m" << std::endl;
                    exit(1);
                }
            }



            MUSE::Rotation dataRotation;

            std::vector<cinolib::vec3d> coord_samples;
            if(setRotAxis.isSet())
            {
                std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                std::cout << "Rotation axis: " << setRotAxis.getValue() << std::endl;
                std::cout << "Rotation center: [" << setRotCenterX.getValue() << "; "<< setRotCenterY.getValue() << "; " << setRotCenterZ.getValue() << "]" << std::endl;
                std::cout << "Rotation angle (degree): " << setRotAngle.getValue() << std::endl;
                std::cout << std::endl;

                dataRotation.rotation = true;
                dataRotation.rotation_axis = setRotAxis.getValue();
                dataRotation.rotation_center_x = setRotCenterX.getValue();
                dataRotation.rotation_center_y = setRotCenterY.getValue();
                dataRotation.rotation_center_z = setRotCenterZ.getValue();
                dataRotation.rotation_angle = setRotAngle.getValue();

                manmeta.setRotation(dataRotation);
            }

            std::vector<uint> sub_index;
            if(subDataset.isSet())
            {
                MUSE::ExtractionMeta extrmeta;
                extrmeta.read(app_folder + "/" + subDataset.getValue() + ".json");
                sub_index = extrmeta.getDataExtraction().id_points;

                dataRotation = extrmeta.getRotation();

                std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                std::cout << "Rotation axis: " << dataRotation.rotation_axis << std::endl;
                std::cout << "Rotation center: [" << dataRotation.rotation_center_x << "; " << dataRotation.rotation_center_y << "; " << dataRotation.rotation_center_z << "]" <<  std::endl;
                std::cout << "Rotation angle (degree): " << dataRotation.rotation_angle << std::endl;
                std::cout << std::endl;

                manmeta.setRotation(dataRotation);

                dataProjection.subdataset_is_set = true;
            }
            else
            {
                for(uint i=0; i< xCoord.size(); i++)
                    sub_index.push_back(i);
            }



            for(uint i:sub_index)
            {
                cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));

                if(dataRotation.rotation == true)
                {
                    cinolib::vec3d axis = set_rotation_axis(dataRotation.rotation_axis);
                    cinolib::vec3d c (dataRotation.rotation_center_x, dataRotation.rotation_center_y, dataRotation.rotation_center_z);
                    sample = point_rotation(sample, axis, dataRotation.rotation_angle, c);
                }
                coord_samples.push_back(sample);
            }





            // 1) Lista dei file mesh in input su cui proiettare i punti

            if(!meshFiles.isSet())
            {
                std::cerr << "ERROR set trimesh file to compute projections: --mgeom <file>" << std::endl;
                exit(1);
            }
            std::vector<std::string> files = meshFiles.getValue();

            // Ciclo sui file mesh e procedo con le proiezioni
            for(uint i=0; i<files.size(); i++)
            {
                // Carico la mesh in input
                cinolib::Tetmesh<> mesh;
                const std::string mesh_ext = get_extension(files.at(i));
                if(mesh_ext == ".vtk" || mesh_ext == ".VTK")
                {
                    std::vector<cinolib::vec3d> verts;
                    std::vector<std::vector<uint>> polys;
                    if(load_vtk(files.at(i), verts, polys) != 0)
                    {
                        std::cerr << "ERROR while loading vtk file: " << files.at(i) << std::endl;
                        exit(1);
                    }
                    mesh.init(verts, polys);
                }
                else
                {
                    mesh.load(files.at(i).c_str());
                }
                std::cout << "\033[0;32mLoading mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;

                std::string geom_name = files.at(i).substr(files.at(i).find_last_of("/")+1, files.at(i).length());
                geom_name = get_basename(geom_name);

                std::vector<Point3D> proj_top, proj_bot; //vettore delle proiezioni

                std::vector<uint> face_bound = mesh.get_surface_faces();


                if(setProjDir.getValue().compare("Y") == 0)
                {
                    for(uint j=0; j<coord_samples.size(); j++)
                    {
                        //std::cout << "coord = " <<  coord_samples.at(j) << std::endl;

                        Point3D proj_max;
                        proj_max.z = coord_samples.at(j).z();
                        proj_max.x = coord_samples.at(j).x();
                        proj_max.y = -DBL_MAX;

                        Point3D proj_min;
                        proj_min.z = coord_samples.at(j).z();
                        proj_min.x = coord_samples.at(j).x();
                        proj_min.y = DBL_MAX;

                        cinolib::vec3d axis = set_rotation_axis(dataRotation.rotation_axis);
                        cinolib::vec3d c (dataRotation.rotation_center_x, dataRotation.rotation_center_y, dataRotation.rotation_center_z);

                        cinolib::vec3d p_rot;
                        point_rotation(coord_samples.at(j).x(), coord_samples.at(j).y(), coord_samples.at(j).z(), axis, -dataRotation.rotation_angle, c, p_rot.x(), p_rot.y(), p_rot.z());
                        //std::cout << "p_rot = " <<  p_rot << std::endl;


                        for(uint fidb : face_bound)
                        {
                            cinolib::vec3d v0 = mesh.face_vert(fidb, 0);
                            cinolib::vec3d v1 = mesh.face_vert(fidb, 1);
                            cinolib::vec3d v2 = mesh.face_vert(fidb, 2);

                            v0 = point_rotation(v0, axis, -dataRotation.rotation_angle, c);
                            v1 = point_rotation(v1, axis, -dataRotation.rotation_angle, c);
                            v2 = point_rotation(v2, axis, -dataRotation.rotation_angle, c);

//                            std::cout << "v0 = " <<  v0 << std::endl;
//                            std::cout << "v1 = " <<  v1 << std::endl;
//                            std::cout << "v2 = " <<  v2 << std::endl;


                            cinolib::vec2d p2d(p_rot.x(), p_rot.y());

                            cinolib::vec2d v02d(v0.x(), v0.y());
                            cinolib::vec2d v12d(v1.x(), v1.y());
                            cinolib::vec2d v22d(v2.x(), v2.y());


                            if (cinolib::point_in_triangle_2d(p2d, v02d, v12d, v22d)!=0) //se trovo la proiezione
                            {
                                cinolib::vec3d tn = cinolib::triangle_normal(v0, v1, v2);

                                double d = -(v1.dot(tn));
                                double proj = -(d+tn.x()*p2d.x()+tn.y()*p2d.y())/tn.z();
                                //std::cout << "proj = " <<  proj << std::endl;


                                if(proj == DBL_MAX)
                                {
                                    double dd_min = DBL_MAX;
                                    std::vector<double> dd;
                                    dd.push_back(p2d.dist(v02d));
                                    dd.push_back(p2d.dist(v12d));
                                    dd.push_back(p2d.dist(v22d));

                                    int id_ddmin = 0;
                                    for(uint iii=0; iii<dd.size(); iii++)
                                    {
                                        if(dd.at(iii) < dd_min)
                                        {
                                            dd_min = dd.at(iii);
                                            id_ddmin = iii;
                                        }
                                    }

                                    if(id_ddmin == 0)
                                        proj = v0.z();
                                    else if (id_ddmin == 1)
                                        proj = v1.z();
                                    else
                                        proj = v2.z();
                                }



                                Point3D p_proj;
                                point_rotation(p2d.x(), p2d.y(), proj, axis, dataRotation.rotation_angle, c, p_proj.x, p_proj.y, p_proj.z);
//                                std::cout << "proj_x = " <<  p_proj.x << std::endl;
//                                std::cout << "proj_y = " <<  p_proj.y << std::endl;
//                                std::cout << "proj_z = " <<  p_proj.z << std::endl;


                                if(p_proj.y < proj_min.y)
                                    proj_min.y = p_proj.y;

                                if(p_proj.y > proj_max.y)
                                    proj_max.y = p_proj.y;
                            }
                        }


                        proj_bot.push_back(proj_min);
                        proj_top.push_back(proj_max);

                    }
                }
                else if(setProjDir.getValue().compare("X") == 0)
                {
                    exit(1);

                }

                dataProjection.mesh = geom_name;
                dataProjection.proj_direction = setProjDir.getValue();
                dataProjection.top_name = geom_name + "_top";
                dataProjection.bottom_name = geom_name + "_bot";
                manmeta.setDataProjection(dataProjection);
                manmeta.write(app_folder + "/samples_" + geom_name + ".json");


                export3d_xyz(Project.folder + "/out/"+ app_name + "/samples_" + dataProjection.top_name + ".dat", proj_top);
                export3d_xyz(Project.folder + "/out/"+ app_name + "/samples_" + dataProjection.bottom_name + ".dat", proj_bot);

                std::cout << "n top projections : " << proj_top.size() << std::endl;
                std::cout << "n bot projections : " << proj_bot.size() << std::endl;

                std::cout << "\033[0;32mComputing points projection on mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;
                std::cout << std::endl;
            }

        }

        if(setType.getValue().compare("GEOMETRY") == 0)
        {
            std::vector<cinolib::vec3d> coord_samples;

            if(!geomModel.isSet())
            {
                std::cout << "ERROR set trimesh --geom <file>"<< std::endl;
                exit(1);
            }

            cinolib::Tetmesh<> model;
            const std::string model_ext = get_extension(geomModel.getValue());
            if(model_ext == ".vtk" || model_ext == ".VTK")
            {
                std::vector<cinolib::vec3d> verts;
                std::vector<std::vector<uint>> polys;
                if(load_vtk(geomModel.getValue(), verts, polys) != 0)
                {
                    std::cerr << "ERROR while loading vtk file: " << geomModel.getValue() << std::endl;
                    exit(1);
                }
                model.init(verts, polys);
            }
            else
            {
                model.load(geomModel.getValue().c_str());
            }
            std::cout << "\033[0;32mLoading mesh file: " << geomModel.getValue() << " ... COMPLETED.\033[0m" << std::endl;
            std::cout << std::endl;


            if(!meshFiles.isSet())
            {
                std::cerr << "ERROR set trimesh file to compute projections: --mgeom <file>" << std::endl;
                exit(1);
            }

            std::vector<std::string> files = meshFiles.getValue();
            for(uint i=0; i<files.size(); i++)
            {
                cinolib::Tetmesh<> mesh;
                const std::string mesh_ext = get_extension(files.at(i));
                if(mesh_ext == ".vtk" || mesh_ext == ".VTK")
                {
                    std::vector<cinolib::vec3d> verts;
                    std::vector<std::vector<uint>> polys;
                    if(load_vtk(files.at(i), verts, polys) != 0)
                    {
                        std::cerr << "ERROR while loading vtk file: " << files.at(i) << std::endl;
                        exit(1);
                    }
                    mesh.init(verts, polys);
                }
                else
                {
                    mesh.load(files.at(i).c_str());
                }
                std::cout << "\033[0;32mLoading mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;

                std::string geom_name = files.at(i).substr(files.at(i).find_last_of("/")+1, files.at(i).length());
                geom_name = get_basename(geom_name);

                std::vector<Point3D> proj_top, proj_bot; //vettore delle proiezioni

                if(setProjDir.getValue().compare("Y") == 0)
                {
                    cinolib::vec3d c (0.0, 0.0, 0.0);
                    cinolib::vec3d axis = set_rotation_axis("X");
                    double angle = 270;
                    double rad = (angle * M_PI)/180;

                    cinolib::mat3d R = cinolib::mat3d::ROT_3D(axis, -rad);

                    for(uint vid=0; vid<model.num_verts(); ++vid)
                    {
                        model.vert(vid) -= c;
                        model.vert(vid)  = R*model.vert(vid);
                        model.vert(vid) += c;
                    }

                    for(uint vid=0; vid<mesh.num_verts(); ++vid)
                    {
                        mesh.vert(vid) -= c;
                        mesh.vert(vid)  = R*mesh.vert(vid);
                        mesh.vert(vid) += c;
                    }


                    //In questo caso i punti da proiettare sono i vertici interni della mesh

                    for(uint vid=0; vid<model.num_verts(); vid++)
                    {
                        cinolib::vec3d p = model.vert(vid);

                        Point3D pp;
                        pp.x = p.x();
                        pp.y = p.y();
                        //pp.z = p.z();

                        Point3D proj_max = pp;
                        proj_max.z = -DBL_MAX;

                        Point3D proj_min = pp;
                        proj_min.z = DBL_MAX;

                        for(uint fidb=0; fidb < mesh.num_faces(); fidb++)
                        {
                            cinolib::vec3d v0 = mesh.face_vert(fidb, 0);
                            cinolib::vec3d v1 = mesh.face_vert(fidb, 1);
                            cinolib::vec3d v2 = mesh.face_vert(fidb, 2);

                            cinolib::vec2d p2d(p.x(), p.y());

                            cinolib::vec2d v02d(v0.x(), v0.y());
                            cinolib::vec2d v12d(v1.x(), v1.y());
                            cinolib::vec2d v22d(v2.x(), v2.y());

                            if (cinolib::point_in_triangle_2d(p2d, v02d, v12d, v22d)!=0) //se trovo la proiezione
                            {
                                cinolib::vec3d tn = cinolib::triangle_normal(v0, v1, v2);

                                double d = -(v0.dot(tn));
                                double proj = -(d+tn.x()*p2d.x()+tn.y()*p2d.y())/tn.z();
                                std::cout << "proj (corrispondente alla coordinata z del punto proiettato) = " <<  proj << std::endl;

                                if(proj == DBL_MAX || proj == -DBL_MAX)
                                {
                                    double dd_min = DBL_MAX;
                                    std::vector<double> dd;
                                    dd.push_back(p2d.dist(v02d));
                                    dd.push_back(p2d.dist(v12d));
                                    dd.push_back(p2d.dist(v22d));

                                    int id_ddmin = 0;
                                    for(uint iii=0; iii<dd.size(); iii++)
                                    {
                                        if(dd.at(iii) < dd_min)
                                        {
                                            dd_min = dd.at(iii);
                                            id_ddmin = iii;
                                        }
                                    }

                                    if(id_ddmin == 0)
                                        proj = v0.z();
                                    else if (id_ddmin == 1)
                                        proj = v1.z();
                                    else
                                        proj = v2.z();
                                }

                                pp.z = proj;

                                if(pp.z < proj_min.z)
                                    proj_min.z = pp.z;

                                if(pp.z > proj_max.z)
                                    proj_max.z = pp.z;
                            }
                        }

                        proj_top.push_back(proj_max);
                        proj_bot.push_back(proj_min);
                    }
                }
                else if(setProjDir.getValue().compare("X") == 0)
                {
                    exit(1);

                }


                dataProjection.mesh = geom_name;
                dataProjection.proj_direction = setProjDir.getValue();
                dataProjection.top_name = geom_name + "_top";
                dataProjection.bottom_name = geom_name + "_bot";
                manmeta.setDataProjection(dataProjection);
                manmeta.write(app_folder + "/geom_" + geom_name + ".json");


                export3d_xyz(Project.folder + "/out/"+ app_name + "/geom_" + dataProjection.top_name + ".dat", proj_top);
                export3d_xyz(Project.folder + "/out/"+ app_name + "/geom_" + dataProjection.bottom_name + ".dat", proj_bot);

                std::cout << "n top projections : " << proj_top.size() << std::endl;
                std::cout << "n bot projections : " << proj_bot.size() << std::endl;

                std::cout << "\033[0;32mComputing points projection on mesh file: " << files.at(i) << " ... COMPLETED.\033[0m" << std::endl;
                std::cout << std::endl;
            }
        }

        std::cout << "Points projection on section boundary ... COMPLETED." << std::endl;

    }*/





    } catch (ArgException &e)  // catch exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

}

