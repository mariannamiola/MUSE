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
#include <algorithm>
#include <filesystem>
#include <float.h>
#include <set>

#include <tclap/CmdLine.h>

#include <cinolib/triangle_wrap.h>
#include <cinolib/meshes/drawable_trimesh.h>

#include <cinolib/tetgen_wrap.h>
#include <cinolib/meshes/drawable_tetmesh.h>
#include <cinolib/grid_mesh.h>

#include <cinolib/geometry/polygon_utils.h>
#include <cinolib/merge_meshes_at_coincident_vertices.h>

#include <cinolib/voxelize.h>
#include <cinolib/voxel_grid_to_hexmesh.h>
#include <cinolib/remesh_BotschKobbelt2004.h>
#include <cinolib/connected_components.h>
#include <cinolib/export_surface.h>

#include <concaveman.h>
// https://adared.ch/concaveman-cpp-a-very-fast-2d-concave-hull-maybe-even-faster-with-c-and-python/
// MODIFICHE:
//1) Comment out make_unique function definition
//2) Add using std::make_unique instead

#include "muselib/utils.h"
#include "muselib/colors.h"
#include "muselib/geometry/mesh.h"
#include "muselib/geometry/tools.h"
#include "muselib/geometry/grid_mesh.h"
#include "muselib/geometry/hexmesh.h"
#include "muselib/geometry/merge_meshes.h"
#include "muselib/geometry/polygon_mesh.h"

#include "muselib/data_structures/point.h"
#include "muselib/data_structures/project.h"
#include "muselib/data_structures/geometry.h"
#include "muselib/data_structures/surface.h"
#include "muselib/data_structures/volume.h"

#include "muselib/metadata/geometry_meta.h"
#include "muselib/metadata/surface_meta.h"
#include "muselib/metadata/volume_meta.h"

#include "muselib/geometry/surface_mesh.h"
#include "muselib/geometry/volume_mesh.h"

#include "muselib/interpolation/plane.h"

#include "muselib/input/load_vector.h"
#include "muselib/input/load_raster.h"
#include "muselib/input/load_xyz.h"
#include "muselib/output/save_vtk.h"

#include "muselib/reference_system/coordinate_systems.h"

#include "muselib/geometry/well_creation.h"



//for filesystem
#ifdef __APPLE__
    using namespace std::__fs;
#else
    using namespace std;
#endif


//color: 32 green
//color: 31

using namespace MUSE;
using namespace TCLAP;

int main(int argc, char** argv)
{
    std::cout << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "================== STARTING MUSE-GEOMETRY ===================" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;

    std::string app_name = "geometry"; //app name

    try {
    CmdLine cmd("MUSE - Modelling Uncertainty as a Support of Environment. MUSE-geometry application", ' ', "version 0.0");


    // ---------------------------------------------------------------------------------------------------------
    // MAIN FUNCTIONALITIES:

    // Option 0. New project creation
    /**
     * @brief Creation of new geometry environment in Project directory
     * @param geometry Flag to creation of new geometry environment in Project directory
     * @note This command initializes a new geometry environment within the specified project directory. 
     * It sets up the necessary folder structure (--pdir path/to/project/dir) to store geometrical items and models created by various geospatial input data sources and processing methods (computational geometry techniques). 
     * It is used with:
     * - --pdir: Project directory
     * - --setEPSG: Set coordinate reference system of geometry environment (optional)
     * @example muse_geometry -N -p /path/to/project 
     */
    SwitchArg geometryCreation ("N", "geometry", "Creation of new geometry environment in the project folder", cmd, false); //booleano

    /**
     * @brief Project directory
     * @param pdir Path where the project is created
     * @note Used with -N/--geometry for geometry environment creation (and in other cases in which the project locations must be specified). 
     * Required to specify the project directory where the geometry environment will be created. 
     * The command initializes the necessary folder structure within the project directory to store geometrical items and models created by various geospatial input data sources and processing methods (computational geometry techniques).
     * @example -p /path/to/project/dir
     */
    ValueArg<std::string> projectFolder ("p", "pdir", "Set project directory", false, "/path/to/project/dir", "string", cmd);

    // Option 0a. Project creation - optional: setting project EPSG
    /**
     * @brief Set project EPSG
     * @param setEPSG project epsg
     */
    ValueArg<std::string> setEPSG        ("", "setEPSG", "Set project EPSG", false, "Unknown", "authority", cmd);

    /**
     * @brief Copy input file(s) in the data project directory (path/project/in/geometry) to replace manual data copy
     * @param input File path of the input(s) to copy in the data folder project
     * @note To be used with -N swith flag, when create a new geometry environment in the project
     * @example --input user/path1/box.gpkg
     */
    MultiArg<std::string> setInput ("i", "input", "Copy input files in the project directory (in/geometry/)", false, "string", cmd );


    // Option 1. Reading vector file (+ flag for triangulation)
    // Include: shape (.shp), geopackage (.gpkg)
    /**
     * @brief Load Vector file
     * @param vector Enable load vector file
     * @note Mutually exclusive with -R/--raster and -P/--pcl
     * Used with -N/--geometry for geometry creation
     * Supports: .shp, .gpkg formats
     * OPTIONAL modifiers:
     * - --save: Save data content
     * - --attribute: Save attribute table
     */
    SwitchArg loadVector                ("V", "vector", "Load Vector file", cmd, false); //booleano
    
    /**
     * @brief Saving data content of geospatial files
     * @param save Enable saving data content of geospatial files
     */
    SwitchArg setSave                   ("", "save", "Saving data content of geospatial files", cmd, false); //booleano
    
    /**
     * @brief Save attribute table from geospatial file
     * @param attribute Enable save attribute table from geospatial file
     */
    SwitchArg setSaveAttributesTable    ("", "attribute", "Save attribute table from geospatial file", cmd, false); //booleano


    // Option 2. Reading raster file (+ flag for triangulation)
    // Include: ASCIIGRID (.ASCII)
    /**
     * @brief Load Raster file
     * @param raster Enable load raster file
     * @note Mutually exclusive with -V/--vector and -P/--pcl
     * Used with -N/--geometry for geometry creation
     * Supports: .ASCII format
     */

    SwitchArg loadRaster                ("R", "raster", "Load Raster file", cmd, false); //booleano

    // Option 3. Reading xyz_file (point cloud)
    // Include: yxz, dat, txt
    /**
     * @brief Load point cloud
     * @param pcl Enable load point cloud
     * @note Mutually exclusive with -V/--vector and -R/--raster
     * Used with -N/--geometry for geometry creation
     * Supports: .xyz, .dat, .txt formats
     * Can be used with:
     * - --points: Specify points geometry file
     * - --polygon: Specify polygon geometry file
     */

    SwitchArg loadPointCloud            ("P", "pcl", "Load point cloud", cmd, false); //booleano
    /**

     * @brief Load filename as POINTS geometry type

     * @param points Path to load filename as points geometry type

     */

    ValueArg<std::string> setPoints     ("", "points", "Load filename as POINTS geometry type", false, "", "filename", cmd);
    /**

     * @brief Load filename as POLYGON geometry type

     * @param polygon Path to load filename as polygon geometry type

     */

    ValueArg<std::string> setPolygon    ("", "polygon", "Load filename as POLYGON geometry type", false, "", "filename", cmd);

    /**
     * @brief Grid data - test
     * @param gridata Enable grid data - test
     */
    SwitchArg gridData                  ("G", "gridata", "Grid data - test", cmd, false); //booleano
    MultiArg<std::string> setBBPoints   ("", "bbp", "Set bounding box points", false, "string", cmd );


    /**
     * @brief Set rotation axis
     * @param rotaxis rotation axis
     * @note When using rotation, these flags work together:
     * - --rotaxis: Rotation axis (X, Y, Z)
     * - --rotangle: Rotation angle (required if rotaxis != NO)
     * - --rotcx, --rotcy, --rotcz: Rotation center coordinates
     * @example For Z-axis rotation: --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0
     */



    ValueArg<std::string> setRotAxis    ("", "rotaxis", "Set rotation axis", false, "NO", "rot_axis", cmd);
    /**
     * @brief Set clockwise rotation angle (in degree)
     * @param rotangle clockwise rotation angle (in degree)
     * @note Used together with --rotaxis flag. Required when rotaxis != NO
     */

    ValueArg<double> setRotAngle        ("", "rotangle", "Set clockwise rotation angle (in degree)", false, 0.0, "double", cmd);
    /**

     * @brief Set coordinte X of rotation center

     * @param rotcx coordinte x of rotation center

     */

    ValueArg<double> setRotCenterX      ("", "rotcx", "Set coordinte X of rotation center", false, 0.0, "double", cmd);
    /**

     * @brief Set coordinte Y of rotation center

     * @param rotcy coordinte y of rotation center

     */

    ValueArg<double> setRotCenterY      ("", "rotcy", "Set coordinte Y of rotation center", false, 0.0, "double", cmd);
    /**

     * @brief Set coordinte Z of rotation center

     * @param rotcz coordinte z of rotation center

     */

    ValueArg<double> setRotCenterZ      ("", "rotcz", "Set coordinte Z of rotation center", false, 0.0, "double", cmd);


    /**
     * @brief Set triangulation for 2D meshing
     * @param tri Enable set triangulation for 2d meshing
     * @note Mutually exclusive with --grid meshing method
     * Used with -N/--geometry for mesh creation
     * Triangulation configuration (choose one):
     * - --convex: Convex hull triangulation (mutually exclusive with --concave)
     * - --concave: Concave hull triangulation (mutually exclusive with --convex)
     * OPTIONAL:
     * - --boundary: External boundary file
     * - --opt: Optimization flags
     * @example --tri --convex OR --tri --concave --boundary /path/to/boundary.shp
     */



    SwitchArg triFlag                   ("", "tri", "Set triangulation for 2D meshing", cmd, false); //booleano
    /**
     * @brief Set convex hull for points triangulation
     * @param convex Enable set convex hull for points triangulation
     * @note Used with --tri flag. Mutually exclusive with --concave
     */

    SwitchArg convexFlag                ("", "convex", "Set convex hull for points triangulation", cmd, false); //booleano
    /**
     * @brief Set concave hull for points triangulation
     * @param concave Enable set concave hull for points triangulation
     * @note Used with --tri flag. Mutually exclusive with --convex
     */

    SwitchArg concaveFlag               ("", "concave", "Set concave hull for points triangulation", cmd, false); //booleano
    /**

     * @brief Set external boundary for points triangulation

     * @param boundary external boundary for points triangulation

     */

    ValueArg<std::string> setBoundary   ("", "boundary", "Set external boundary for points triangulation", false, "", "filename", cmd);
    
    /**
     * @brief Set optimization flags to optimize triangulation (e.g., "pq20" for preserve+quality 20) or tetrahedralization (e.g., "pq20" for preserve+quality 20 in TetGen)
     * @param opt optimization flags to optimize triangulation or tetrahedralization
     * @note Used with --tri flag for triangulation optimization and with --tet flag for tetrahedralization optimization. 
     * Refer to Triangle and TetGen documentation for available flags and options.
     * @example For Triangle: --tri --opt "pq20" OR For TetGen: --tet --opt "pq20"
     */
    ValueArg<std::string> optFlag       ("", "opt", "Set optimization flags enabled for triangulation or tetrahedralization", false, "", "string", cmd);

    // Option 4. Set grid
    /**
     * @brief Set grid for 2D meshing
     * @param grid Enable set grid for 2d meshing
     * @note Mutually exclusive with --tri triangulation method
     * Used with -N/--geometry for mesh creation
     * Grid configuration requires:
     * - --resx: X resolution (mandatory)
     * - --resy: Y resolution (mandatory)
     * OPTIONAL:
     * - --resz: Z resolution (for 3D grids)
     * @example --grid --resx 10.0 --resy 10.0
     */

    SwitchArg gridFlag                  ("", "grid", "Set grid for 2D meshing", cmd, false); //booleano
    /**
     * @brief Set x resolution
     * @param resx x resolution
     * @note Used with --grid flag. Required for grid meshing
     * Must be used together with --resy
     */

    ValueArg<double> setResx            ("", "resx", "Set x resolution", false, 1.0, "double", cmd);
    /**
     * @brief Set y resolution
     * @param resy y resolution
     * @note Used with --grid flag. Required for grid meshing
     * Must be used together with --resx
     */

    ValueArg<double> setResy            ("", "resy", "Set y resolution", false, 1.0, "double", cmd);
    /**

     * @brief Set z resolution

     * @param resz z resolution

     */

    ValueArg<double> setResz            ("", "resz", "Set z resolution", false, 1.0, "double", cmd);

    /**


     * @brief Set generic polygon mesh for 2D meshing


     * @param poly Enable set generic polygon mesh for 2d meshing


     */


    SwitchArg polygonFlag               ("", "poly", "Set generic polygon mesh for 2D meshing", cmd, false); //booleano
    //ValueArg<std::string> setFeatures   ("", "features", "Set features", false, "DEFAULT", "string" , cmd);

    /**


     * @brief Set (random) subset of points


     * @param subset (random) subset of points


     */


    ValueArg<int> subSet                ("", "subset", "Set (random) subset of points", false, 10,"int", cmd); //booleano

    // Option 5. Set z for new points (if mesh is a section in 3D space)
    // Option: compute variogram with variable/constant lag spacing
    std::vector<std::string> allowedMethod = {"MEAN","CONSTANT","NEAR","KRIGING"};
    ValuesConstraint<std::string> allowedValsMethod(allowedMethod);
    /**

     * @brief Set method for z values

     * @param meth method for z values

     */

    ValueArg<std::string> setMethodZ    ("", "meth", "Set method for z values", false, "CONSTANT", &allowedValsMethod, cmd);
    /**

     * @brief Set const z values for new points

     * @param setz const z values for new points

     */

    ValueArg<double> setNewZ            ("", "setz", "Set const z values for new points", false, 0.0, "double", cmd);


    // Option 6. Apply offset on mesh (only extrusion in z direction is enabled)
    /**

     * @brief Load polygon mesh and apply offset

     * @param offset Enable load polygon mesh and apply offset

     */

    SwitchArg setOffset                 ("O", "offset", "Load polygon mesh and apply offset", cmd, false); //booleano
    /**
     * @brief Set DELTA offset
     * @param delta Enable set delta offset
     */

    SwitchArg deltazExtrusion           ("", "delta", "Set DELTA offset", cmd, false); //booleano
    /**
     * @brief Set ABSOLUTE ELEVATION offset
     * @param abs Enable set absolute elevation offset
     */

    SwitchArg abszExtrusion             ("", "abs", "Set ABSOLUTE ELEVATION offset", cmd, false); //booleano
    /**
     * @brief Set offset in Z direction
     * @param zoffset offset in z direction
     */

    ValueArg<double> zOffset            ("z", "zoffset", "Set offset in Z direction", false, 0.0, "double" , cmd);


    /**
     * @brief Append meshes
     * @param append Enable append meshes
     */
    SwitchArg appendMeshes              ("A", "append", "Append meshes", cmd, false); //booleano


    // Option 4. Creating triobject: lateral closure of meshes
    /**

     * @brief Load trimeshes and create an object closed by surface meshes

     * @param triobj Flag to load trimeshes and create an object closed by surface meshes

     */

    SwitchArg createTriObject           ("T", "triobj", "Load trimeshes and create an object closed by surface meshes", cmd, false); //booleano
    MultiArg<std::string> meshFiles     ("m", "mesh", "Set (multi) mesh files", false, "string", cmd );

    /**


     * @brief Load quadmeshes and create an object closed by surface meshes


     * @param quadobj Flag to load quadmeshes and create an object closed by surface meshes


     */


    SwitchArg createQuadObject          ("Q", "quadobj", "Load quadmeshes and create an object closed by surface meshes", cmd, false); //booleano
    /**

     * @brief Clean quadrilateral mesh from isolated polys

     * @param clean Enable clean quadrilateral mesh from isolated polys

     */

    SwitchArg cleanPoly                 ("", "clean", "Clean quadrilateral mesh from isolated polys", cmd, false); //booleano


    // Option 7. Creating volumetric object
    /**
     * @brief Load polygonal mesh and create polyedral mesh
     * @param volmesh Flag to load polygonal mesh and create polyedral mesh
     */
    SwitchArg createVolObject           ("M", "volmesh", "Load polygonal mesh and create polyedral mesh", cmd, false); //booleano

    /**
     * @brief Set tetrahedralization
     * @param tet Enable set tetrahedralization
     */
    SwitchArg tetFlag                   ("", "tet", "Set tetrahedralization (using TetGen C++ library)", cmd, false); //booleano

    /**
     * @brief Set voxel as polyedralmesh
     * @param vox Enable set voxel as polyedralmesh
     */
    SwitchArg voxFlag                   ("", "vox", "Set voxel as polyedralmesh", cmd, false); //booleano
    
    /**
     * @brief Set hexmesh as polyedral
     * @param hex Enable set hexmesh as polyedral
     */
    SwitchArg hexFlag                   ("", "hex", "Set hexmesh as polyedral", cmd, false); //booleano

    /**
     * @brief Set n max voxel per side
     * @param nmaxvox Number of set n max voxel per side
     */
    ValueArg<int> setMaxVoxelperSide    ("", "nmaxvox", "Set n max voxel per side", false, 1, "int", cmd);

    /**
     * @brief Load (closed) polygonal mesh and create a tetrahedral mesh constrained to specific well(s)
     * @param vmwells Flag to load (closed) polygonal mesh and create a tetrahedral mesh constrained to specific well(s)
     * @note When using this option, these flags work together:
     * - -W/--vmwells: Load (closed) polygonal mesh and create a tetrahedral mesh constrained to specific well(s)
     * - --well: Specify well(s) file(s) (can be used multiple times for multiple wells)
     * @example muse_geometry -M --tet --vmwells --well /path/to/well1.shp --well /path/to/well2.shp
     */
    SwitchArg createVolObjectwithWells ("W", "vmwells", "Load (closed) polygonal mesh and create a tetrahedral mesh constrained to well(s)", cmd, false); //booleano

    // Well creation configuration
    ValueArg<std::string> generate_box_arg("", "generate-box", "Generate box with dimensions: width,height,depth (e.g., \"10,5,8\")", false, "", "string", cmd);

    ValueArg<std::string> output_mesh_arg("o", "output", "Output triangle mesh file (.off format)", false, "", "string", cmd);

    MultiArg<std::string> wells_arg("w", "well", "Well specification: CYL:XYZH:x,y,z,height,radius[,rel_z_sub,...] | CYL:XYZB:x,y,z,radius | CYL:XYH:x,y,height,radius[,rel_z_sub,...] | CYL:XYB:x,y,radius | CYL:XY:x,y,radius | BOX:XYZH:x,y,z,height,diag_x,diag_y[,rel_z_sub,...] | BOX:XYZB:x,y,z,diag_x,diag_y | BOX:XYH:x,y,height,diag_x,diag_y[,rel_z_sub,...] | BOX:XYB:x,y,diag_x,diag_y | BOX:XY:x,y,diag_x,diag_y | POLY:XYZH:z,height,num_vertices,x1,y1,...,xn,yn[,rel_z_sub,...] | POLY:XYZB:z,num_vertices,x1,y1,...,xn,yn | POLY:XYH:height,num_vertices,x1,y1,...,xn,yn[,rel_z_sub,...] | POLY:XYB:num_vertices,x1,y1,...,xn,yn | POLY:XY:num_vertices,x1,y1,...,xn,yn (H = numeric signed height, B = extrude to bottom surface, formats without H/B only embed the top cap ring on the surface; rel_z_sub values are relative to the top z)", false, "string", cmd);

    ValueArg<double> edge_length_arg("e", "edge-length", "Target edge length for remeshing (default: auto from input mesh)", false, -1.0, "double", cmd);

    SwitchArg refine_cylinders_arg("", "refine-cylinders", "Refine cylinder mesh by halving the target edge length", cmd, false);

    ValueArg<double> cylinder_edge_scale_arg("", "cylinder-edge-scale", "Scale factor for cylinder edge length (e.g., 0.5 for finer, 2.0 for coarser)", false, 1.0, "double", cmd);

    SwitchArg verbose_arg("v", "verbose", "Enable verbose output", cmd, false);

    ValueArg<double> max_tet_volume_arg("", "max-tet-volume", "Maximum tetrahedron volume for TetGen (default: auto)", false, -1.0, "double", cmd);

    SwitchArg save_no_wells_arg("", "save-no-wells", "Save tetrahedral mesh without wells (removes tets inside wells, suffix: _no_wells.mesh)", cmd, false);

    SwitchArg save_only_wells_arg("", "save-only-wells", "Save tetrahedral mesh with only wells (removes tets outside wells, label -1, suffix: _only_wells.mesh)", cmd, false);



    // Option 8. Loading surface mesh
    /**
     * @brief Load trimesh file
     * @param trimesh Enable load trimesh file
     */
    SwitchArg loadSurface               ("L", "trimesh", "Load trimesh file", cmd, false); //booleano

    /**
     * @brief Set polys split method
     * @param splmet polys split method
     */
    ValueArg<std::string> splitMethod   ("", "splmet", "Set polys split method", false, "CENTROID", "string", cmd);
    
    /**
     * @brief Set remeshing
     * @param remesh Enable set remeshing
     */
    SwitchArg setRemeshing              ("", "remesh", "Set remeshing", cmd, false); //booleano
    
    /**
     * @brief Set marked boundary edges for remeshing
     * @param mark Enable set marked boundary edges for remeshing
     */
    SwitchArg setMarkedEdge             ("", "mark", "Set marked boundary edges for remeshing", cmd, false); //booleano
    
    /**

     * @brief Set collapse on edge to simplify mesh boundary

     * @param collapse Enable set collapse on edge to simplify mesh boundary

     */

    SwitchArg setEdgeCollpase           ("", "collapse", "Set collapse on edge to simplify mesh boundary", cmd, false);
    /**

     * @brief Set extract boundary points

     * @param extractbp Enable set extract boundary points

     */

    SwitchArg boundaryExtract           ("", "extractbp", "Set extract boundary points", cmd, false); //booleano
    /**

     * @brief Set number of iterations

     * @param it Number of set number of iterations

     */

    ValueArg<int> setIterations         ("", "it", "Set number of iterations", false, 1.0, "int" , cmd);

    /**
     * @brief Set scale mesh
     * @param scale Enable set scale mesh
     */
    SwitchArg setScaleMesh              ("", "scale", "Set scale mesh", cmd, false);

    /**
     * @brief Set translate mesh of the quantity specified by the string (e.g., "10,20,5"). This can be used to translate the loaded surface mesh by the specified amounts in the x, y, and z directions. For example, if the string is "10,20,5", the mesh will be translated by 10 units in the x direction, 20 units in the y direction, and 5 units in the z direction.
     * @param translate Enable set translate mesh of the quantity specified by the string
     * @note The string must be in the format "x,y,z" where x, y, and z are the translation values in each direction. The command is able for loading surface switch.
     * @example -L -p /path/to/project -m /path/to/mesh.off --translate 10,20,5
     */
    ValueArg<std::string> setTranslate ("", "translate", "Set translate mesh of the quantity specified by the string", false, "0,0,0", "string", cmd);

    /**
     * @brief Set scale factor in X direction
     * @param sx scale factor in x direction
     */
    ValueArg<double> setScaleFactorX    ("", "sx", "Set scale factor in X direction", false, 1.0, "double" , cmd);

    /**
     * @brief Set scale factor in Y direction
     * @param sy scale factor in y direction
     */
    ValueArg<double> setScaleFactorY    ("", "sy", "Set scale factor in Y direction", false, 1.0, "double" , cmd);

    /**

     * @brief Set scale factor in Z direction

     * @param sz scale factor in z direction

     */

    ValueArg<double> setScaleFactorZ    ("", "sz", "Set scale factor in Z direction", false, 1.0, "double" , cmd);

    // Option 9. Loading volumetric mesh
    /**

     * @brief Load tetmesh file

     * @param tetmesh Enable load tetmesh file

     */

    SwitchArg loadVolume                ("Z", "tetmesh", "Load tetmesh file", cmd, false); //booleano
    /**

     * @brief Extract surface from volume

     * @param surf Enable extract surface from volume

     */

    SwitchArg extractSurface            ("", "surf", "Extract surface from volume", cmd, false); //booleano


    // Format conversion for saving meshes
    /**
     * @brief Saving surface mesh in obj format
     * @param obj Enable saving mesh in obj format
     * @example muse_geometry -L -p .. --obj
     */
    SwitchArg objConversion             ("", "obj", "Saving surface mesh in obj format (default: .off)", cmd, false); //booleano
    
    /**
     * @brief Saving volume mesh in vtk format
     * @param vtk Enable saving mesh in vtk format
     * @example muse_geometry -M -p .. --vtk
     */
    SwitchArg vtkConversion             ("", "vtk", "Saving volume mesh in vtk format (default: .mesh)", cmd, false); //booleano


    // Format conversion for saving text file (default: .dat)
    /**
     * @brief
     * 
     */
    SwitchArg xyzFormat                 ("", "xyz", "Saving text file in xyz format", cmd, false); //booleano

    /**
     * @brief
     * 
     */
    SwitchArg csvFormat                 ("", "csv", "Saving text file in csv format", cmd, false); //booleano



    // ---------------------------------------------------------------------------------------------------------
    // ADDITIONAL FUNCTIONALITIES:

    // Option 7. Merge two meshes
    /**

     * @brief Merge two trimesh

     * @param merge Enable merge two trimesh

     */

    SwitchArg mergeMeshes               ("U", "merge", "Merge two trimesh", cmd, false); //booleano
    /**

     * @brief Set proximaty threshold

     * @param proxthresh proximaty threshold

     */

    ValueArg<int> proxThreshold         ("", "proxthresh", "Set proximaty threshold", false, 0, "int" , cmd);

    /**


     * @brief Split two trimesh


     * @param split Enable split two trimesh


     */


    SwitchArg extractMeshes             ("S", "split", "Split two trimesh", cmd, false); //booleano


    /**
     * @brief Create scalar field from centroids configuration and real samples
     * @param cscalar Flag to create scalar field from centroids configuration and real samples
     */
    SwitchArg createScalarField             ("F", "cscalar", "Create scalar field from centroids configuration and real samples", cmd, false); //booleano
    
    /**
     * @brief Set samples mesh associated to (real) values
     * @param smesh samples mesh associated to (real) values
     */
    ValueArg<std::string> setSamplesMesh    ("","smesh","Set samples mesh associated to (real) values", false, "", "string", cmd);
    
    /**
     * @brief Set samples values associated to each vertex of samples mesh
     * @param sval samples values associated to each vertex of samples mesh
     */
    ValueArg<std::string> setSamplesValues  ("","sval","Set samples values associated to each vertex of samples mesh", false, "", "string", cmd);

    /**
     * @brief Restore scalar field from centroids configuration and real samples
     * @param rscalar Enable restore scalar field from centroids configuration and real samples
     */
    SwitchArg restoreScalarField            ("", "rscalar", "Restore scalar field from centroids configuration and real samples", cmd, false); //booleano


    // Option 9. Multi-resolution approach (associate a scalar field to meshes with different resolutions)
    /**

     * @brief Set multiresolution

     * @param res Enable set multiresolution

     */

    SwitchArg setMultiResolution            ("D", "res", "Set multiresolution", cmd, false); //booleano
    /**

     * @brief Set scalar field file

     * @param file Path to set scalar field file

     */

    ValueArg<std::string> setScalarField    ("f", "file", "Set scalar field file", false, "Directory", "path", cmd);
    /**

     * @brief Geometry model

     * @param refmod geometry model

     */

    ValueArg<std::string> setRefModel       ("", "refmod", "Geometry model", false, "name_geometry", "string", cmd);

    /**
     * @brief Set folder to save outputs
     * @param outf Path to set folder to save outputs
     */
    ValueArg<std::string> setOutFolder      ("", "outf", "Set folder to save outputs", false, "Directory", "string", cmd);

    ValueArg<int> setPrecision              ("", "prec", "Set precision", false, 6, "int" , cmd);
    ValueArg<double> setTolerance           ("", "tol", "Set tolerance", false, 1e-02, "double" , cmd);


    // ---------------------------------------------------------------------------------------------------------
    // PARSING:

    // Parse the argv array.
    cmd.parse(argc, argv);


    // ---------------------------------------------------------------------------------------------------------
    // SETTINGS:

    // 0) Project settings
    MUSE::Project Project;
    if(!projectFolder.isSet())
    {
        std::cerr << "\033[0;31mInput ERROR: Insert project folder with -p/--pdir flag\033[0m" << std::endl;
        exit(1);
    }
    Project.setFolder(projectFolder.getValue()); //cartella di progetto
    Project.setName(Project.folder.substr(Project.folder.find_last_of("/")+1, Project.folder.length()));

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


    // 0) Set folder (in/out)
    std::string in_geometry = Project.folder + "/in/"+ app_name;
    std::string out_geometry = Project.folder + "/out/" + app_name;
    std::string out_surf = out_geometry +"/surf";
    std::string out_volume = out_geometry +"/volume";


    // 0) Define file extension - surface
    std::string ext_surf = ".off";
    if(objConversion.isSet() == true)
        ext_surf = ".obj";

    // 0) Define file extension - volume
    std::string ext_vol = ".mesh";
    if(vtkConversion.isSet() == true)
        ext_vol = ".vtk";

    // 0) Define file extension - text file
    std::string ext_txt = ".dat";
    if(xyzFormat.isSet() == true)
        ext_txt = ".xyz";
    else if(csvFormat.isSet())
        ext_txt = ".csv";


    // ---------------------------------------------------------------------------------------------------------
    // STARTS:

    // Option 0. Project creation and settings
    if(geometryCreation.isSet())
    {
        if(!filesystem::exists(in_geometry))
            filesystem::create_directory(in_geometry);

        if(!filesystem::exists(out_geometry))
            filesystem::create_directory(out_geometry);

        Project.write(out_geometry + "/"+ Project.name + ".json");

        std::cout << "\033[0;32mCreation new geometry_in: " << in_geometry << " ... COMPLETED.\033[0m" << std::endl;
        std::cout << "\033[0;32mCreation new geometry_out: " << out_geometry << " ... COMPLETED.\033[0m" << std::endl;
    }

    //Load vector file
    if(loadVector.isSet())
    {
        if(!filesystem::exists(out_surf))
            filesystem::create_directory(out_surf);

        //MUSE::GeometryMeta geometa;
        MUSE::SurfaceMeta geometa;
        geometa.setProject(Project);

        std::vector<std::string> depsgeom;

        std::vector<std::string> excommands;
        excommands.push_back(command);
        geometa.setCommands(excommands);

        MUSE::GeospatialData Geometry;

        // Check on input files
        if(filesystem::is_empty(in_geometry))
        {
            std::cerr << "\033[0;31mInput ERROR: Insert file into: " << in_geometry << "\033[0m" << std::endl;
            exit(1);
        }

        // 2. Extraction directories from in_geometry
        std::vector<std::string> dirs = get_directories(in_geometry);

        std::vector<std::string> file_list;

        if(!dirs.empty()) //ci sono delle cartelle
        {
            //for(size_t i=0; i<dirs.size(); i++)
            for(const auto& dir : dirs)
            {
                auto file_list = get_vectorfiles(dir);

                if(file_list.empty())
                    continue; //vado alla dir_shape.at(i+1)
                
                for(const auto& file : file_list)
                {
                    std::vector<std::vector<Point3D>> boundaries, datasets;
                    std::string GDALtype;

                    //read shape
                    std::cout << "Loading shapefile: " << file << std::endl;

                    printSpatialReferenceInfo(file, Project.authority);
                    int loadvecfile = load_vectorfile(file, boundaries, datasets, GDALtype);
                    if (loadvecfile != IOSUCCESS)
                    {
                        std::cerr << "\033[0;31mERROR loading vector file: " << file << "\033[0m" << std::endl;
                        exit(1);
                    }


                    if(setSaveAttributesTable.isSet())
                    {
                        auto csv_path = out_surf + "/" + get_basename(get_filename(file)) + ext_txt;
                        if(export_attributes_to_csv(file, csv_path) == IOSUCCESS)
                            Geometry.setAttributeTable(get_basename(get_filename(file)) + ext_txt);
                    }

                    if(setSave.isSet())
                    {
                        const std::string basename = get_basename(get_filename(file));
                        const std::string ext = get_extensionND(get_filename(file));

                        if(!boundaries.empty())
                        {
                            for(uint id=0; id<boundaries.size(); id++)
                                export3d_xyz(out_surf + "/" + basename + "_" + std::to_string(id) + "@" + ext + ext_txt, boundaries[id]);
                            std::cout << "=== Export boundary points from geospatial file: " << file << std::endl;
                        }
                        if(!datasets.empty())
                        {
                            for(uint id=0; id<datasets.size(); id++)
                                export3d_xyz(out_surf + "/" + basename + "_" + std::to_string(id) + "@" + ext + ext_txt, datasets[id]);
                            std::cout << "=== Export data points from geospatial file: " << file  << std::endl;
                        }
                    }

                    if(!triFlag.isSet() && !gridFlag.isSet() && !polygonFlag.isSet())
                    {
                        std::cout << "=== Meshing algorithms are not selected!" << std::endl;
                        exit(0);
                    }

                    
                    // Applicazione trasformazioni
                    // applyCoordTransform(boundaries);
                    // applyCoordTransform(datasets);

                    Geometry.name = dir.substr(dir.find_last_of("/")+1);
                    Geometry.setFormat(get_extension(file));
                    Geometry.setDomains(boundaries.size());

                    if(!boundaries.empty() && !datasets.empty()) Geometry.geom_type = MULTI;
                    else setGeometryType(Geometry, GDALtype);

                    if(setEPSG.isSet()) Geometry.setAuthority(setEPSG.getValue());

                    std::vector<std::string> deps = {filesystem::relative(file, Project.folder)};
                    geometa.setDependencies(deps);
                    geometa.setGeospatialData(Geometry);


                    //per salvataggio mesh
                    std::string out_mesh = out_surf + "/"+ Geometry.name;
                    out_mesh = out_mesh + ext_surf;

                    // Creation of surface_metadata
                    MUSE::Surface Surface;
                    MUSE::Surface::Parameters paramSurface;

                    std::cout << std::endl;

                    // Caso a 1 dominio (boundaries.size = 1)
                    switch (Geometry.geom_type)
                    {
                    case POLYGON:
                    case MULTI:
                    {
                        if(boundaries.size() == 0) //il poligono di bordo ci deve essere sempre! sia in caso polygon che in caso multi; data può non esserci (caso di polygon)
                        {
                            std::cerr << FRED("ERROR on loading polygon!") << std::endl;
                            exit(1);
                        }

                        if(polygonFlag.isSet())
                        {
                            std::cout << "=== Polygonal mesh is set ..." << std::endl;
                            paramSurface.type = "POLYGONMESH";

                            MUSE::Polygonmesh<> polygonmesh (boundaries, polygonmesh);
                            polygonmesh.save(out_mesh.c_str());

                            Surface.setSummary(polygonmesh);
                            Surface.setParameters(paramSurface);

                            geometa.setMeshSummary(Surface);
                            geometa.setGeospatialData(Geometry);
                            geometa.write(out_surf +"/"+ Geometry.name + ".json");
                        }
                        else
                        {
                            for(size_t k=0; k < boundaries.size(); k++)
                            {
                                if(boundaries[k].empty())
                                {
                                    std::cout << "=== Domain " << k+1 << " empty - skipping." << std::endl;
                                    continue;
                                }

                                // SET DATA SUMMARY
                                MUSE::SurfaceMeta::DataSummary dataSummary; 

                                std::vector<Point3D> data_tmp = boundaries[k];
                                if(datasets.size() > 0)
                                    data_tmp.insert(data_tmp.end(), datasets.at(k).begin(), datasets.at(k).end());

                                dataSummary.setDataSummary(data_tmp); //saving original data (boundaries + datasets) summary in metadata
                                geometa.setDataSummary(dataSummary);


                                //Rimozione duplicati (per evitare problemi di triangolazione)
                                std::vector<Point3D> boundaries_unique, dataset_unique;
                                remove_duplicates_test_opt(boundaries[k], boundaries_unique, setTolerance.getValue());

                                const bool has_datasets = (k < datasets.size() && !datasets[k].empty());
                                if(has_datasets)
                                    remove_duplicates_test_opt(datasets[k], dataset_unique, setTolerance.getValue());

                                if(!datasets.empty() && datasets.size() != boundaries.size())
                                {
                                    std::cerr << FRED("ERROR: different size vectors (data/boundary).") << std::endl;
                                    exit(1);
                                }

                                //Define output mesh name
                                std::string out_mesh = out_surf + "/" + Geometry.name;
                                if(boundaries.size() > 1)
                                {
                                    Geometry.id_subdomain = std::to_string(k+1);
                                    out_mesh += "_" + Geometry.id_subdomain;
                                }
                                out_mesh += ext_surf;


                                /////////////////////ROTAZIONEEEEEEE ---- DA QUI IN POI MODIFICARE
                                MUSE::Rotation otfRotation;

                                std::vector<Point3D> data_xy;
                                std::vector<Point3D> boundary_xy;
                                
                                // 1. Costruisco un unico supporto geometrico.
                                std::vector<Point3D> support_xy = boundaries_unique;
                                const size_t n_boundary = support_xy.size();
                                
                                if(has_datasets)
                                {
                                    support_xy.insert(
                                        support_xy.end(),
                                        dataset_unique.begin(),
                                        dataset_unique.end()
                                    );
                                }
                                
                                // 2. Ruoto UNA SOLA VOLTA il supporto completo.
                                bool auto_aligned = align_points_to_xyplane(support_xy, otfRotation);
                                
                                // 3. Risplitto il vettore già ruotato.
                                boundary_xy.assign(
                                    support_xy.begin(),
                                    support_xy.begin() + n_boundary
                                );
                                
                                if(has_datasets)
                                {
                                    data_xy.assign(
                                        support_xy.begin() + n_boundary,
                                        support_xy.end()
                                    );
                                }
                                //////////////////////////////

                                if(triFlag.isSet())
                                {
                                    if(concaveFlag.isSet())
                                    {
                                        std::cerr << "=== Concave flag is not able for GDALTYPE=POLYGON triangulation." << std::endl;
                                        exit(0);
                                    }
                                    if(convexFlag.isSet())
                                    {
                                        std::cerr << "=== Convex flag is not able for GDALTYPE=POLYGON triangulation." << std::endl;
                                        exit(0);
                                    }

                                    std::cout << "=== WARNING: Triangulation is performed on XY plane." << std::endl;
                                    std::cout << "=== WARNING: Check data alignment on XY plane (and auto-align them)." << std::endl;

                                    cinolib::Trimesh<> trimesh;
                                    trimesh.clear();

                                    paramSurface.type = "TRIMESH";
                                    paramSurface.opt = "";
                                    paramSurface.boundary = "FIXED-BOUNDARY";
                                    
                                    if(optFlag.isSet())
                                        paramSurface.opt += optFlag.getValue();

                                    std::cout << "\033[0;32mStarted Triangulation on X-Y aligned data (with boundary) ...\033[0m" << std::endl;
                                    
                                    if(has_datasets)
                                        trimesh = constrained_triangulation2(boundary_xy, data_xy, paramSurface.opt);
                                    else
                                        trimesh = boundary_triangulation(boundary_xy, paramSurface.opt);                                        

                                    remove_isolate_vertices(trimesh);
                                    if(auto_aligned)
                                        rotation_on_trimesh(trimesh, otfRotation, true);
                                    
                                    Surface.setSummary(trimesh);

                                    trimesh.save(out_mesh.c_str());
                                    std::cout << "\033[0;32mSaving mesh file: " << out_mesh << "\033[0m" << std::endl;
                                    std::cout << std::endl;
                                }
                                else if(gridFlag.isSet())
                                {
                                    //FOR JSON ...
                                    paramSurface.type = "QUADMESH";
                                    paramSurface.resx = setResx.getValue();
                                    paramSurface.resy = setResy.getValue();
                                    paramSurface.resz = 0.0;

                                    MUSE::Quadmesh<> quadmesh (setResx.getValue(), setResy.getValue(), setNewZ.getValue(), boundary_xy);
                                    quadmesh.save(out_mesh.c_str());
                                    std::cout << "\033[0;32mSaving mesh file: " << out_mesh << "\033[0m" << std::endl;
                                    std::cout << std::endl;

                                    Surface.setSummary(quadmesh);
                                }
                                
                                Surface.setParameters(paramSurface);
                                geometa.setMeshSummary(Surface);
                                geometa.setGeospatialData(Geometry);
                                
                                std::string json_path = boundaries.size() > 1
                                            ? out_surf + "/" + Geometry.name + "_" + Geometry.id_subdomain + ".json"
                                            : out_surf + "/" + Geometry.name + ".json";
                                
                                geometa.write(json_path);
                            }
                        }
                        break;
                    }

                    case POINT:
                    {
                        if(datasets.empty())
                        {
                            std::cerr << FRED("ERROR: No point data.") << std::endl;
                            exit(1);
                        }

                        for(size_t ii=0; ii< datasets.size(); ii++)
                        {
                            MUSE::SurfaceMeta::DataSummary dataSummary;
                            dataSummary.setDataSummary(datasets.at(ii)); //saving original data (datasets) summary in metadata
                            geometa.setDataSummary(dataSummary);

                            std::vector<Point3D> data_unique;
                            remove_duplicates_test_opt(datasets.at(ii), data_unique, setTolerance.getValue());

                            if(data_unique.size() < 3)
                            {
                                std::cerr << FRED("ERROR: at least 3 unique points are required for triangulation.") << std::endl;
                                continue;
                            }

                            ///////////////////////
                            std::string out_mesh = out_surf + "/" + Geometry.name;
                            if(datasets.size() > 1)
                            {
                                Geometry.id_subdomain = std::to_string(ii+1);
                                out_mesh += "_" + std::to_string(ii+1);
                            }
                            out_mesh += ext_surf;

                            ///////////////////////
                            MUSE::Rotation otfRotation;

                            if(triFlag.isSet())
                            {
                                std::vector<Point3D> data_xy = data_unique;

                                //Rotazione
                                bool auto_aligned = align_points_to_xyplane(data_xy, otfRotation);
                                
                                cinolib::Trimesh<> trimesh;
                                trimesh.clear();

                                //FOR JSON ...
                                paramSurface.type = "TRIMESH";

                                std::cout << "WARNING: Triangulation is performed on XY plane." << std::endl;

                                // Convex hull
                                if (convexFlag.isSet())
                                {
                                    paramSurface.opt = "c";
                                    paramSurface.boundary = "CONVEX HULL";

                                    if(optFlag.isSet())
                                        paramSurface.opt += optFlag.getValue();

                                    trimesh = points_triangulation(data_xy, paramSurface.opt);
                                    remove_isolate_vertices(trimesh);

                                    if(auto_aligned)
                                        rotation_on_trimesh(trimesh, otfRotation, true);                                    

                                    std::cout << "\033[0;32mTriangulation with convex hull ... COMPLETED.\033[0m" << std::endl;
                                }

                                // Concave hull
                                else if (concaveFlag.isSet())
                                {
                                    paramSurface.boundary = "CONCAVE HULL";

                                    // 1. Calcolo il convex hull (passando per la triangolazione dei punti) e lo trasformo in int da uint
                                    trimesh = points_triangulation(data_xy, "c");

                                    std::vector<int> convexhull;
                                    std::vector<unsigned int> convex_uint = trimesh.get_ordered_boundary_vertices();
                                    for(int i: convex_uint)
                                        convexhull.push_back((short) i);

                                    std::vector<int> b_id;
                                    std::vector<Point3D> concavehull = computing_concavehull(data_xy, convexhull, b_id);

                                    // 2. Removing points of concavehull (boundary) from datasets
                                    std::sort(b_id.begin(), b_id.end());

                                    std::vector<Point3D> unique_data;
                                    for(size_t idp=0; idp<data_xy.size(); idp++)
                                    {
                                        if(!check_index(b_id, static_cast<size_t>(idp)))
                                            unique_data.push_back(data_xy.at(idp));
                                    }
                                    
                                    // for(size_t i=0; i< data.size(); i++)
                                    // {
                                    //     if (!check_index(b_id, i))
                                    //     {
                                    //         Point3D unique_p;
                                    //         unique_p.x = data.at(i).x;
                                    //         unique_p.y = data.at(i).y;
                                    //         unique_p.z = data.at(i).z;

                                    //         unique_data.push_back(unique_p);
                                    //     }
                                    // }

                                    if(optFlag.isSet())
                                        paramSurface.opt += optFlag.getValue();

                                    trimesh.clear();
                                    trimesh = concavehull_triangulation(concavehull, unique_data, paramSurface.opt);
                                    remove_isolate_vertices(trimesh);

                                    if(auto_aligned)
                                        rotation_on_trimesh(trimesh, otfRotation, true);

                                    std::cout << "\033[0;32mTriangulation with concave hull ... COMPLETED.\033[0m" << std::endl;
                                }

                                // External boundary from cmd
                                else if (setBoundary.isSet()) //se gli passo da linea di comando un bordo esterno: 1) leggi 2) triangola i punti vincolati al bordo
                                {
                                    std::string filename_boundary = setBoundary.getValue();

                                    std::vector<std::vector<Point3D>> boundaries_b, datasets_b;
                                    std::string GDALtype_b;

                                    std::cout << "=== Loading boundary file: " << filename_boundary << std::endl;
                                    int loadvecfile = load_vectorfile(filename_boundary, boundaries_b, datasets_b, GDALtype_b);
                                    if (loadvecfile != IOSUCCESS)
                                    {
                                        std::cerr << "\033[0;31mERROR vector loading file: " << filename_boundary << "\033[0m" << std::endl;
                                        exit(1);
                                    }
                                    std::cout << "\033[0;32mLoading file: " << filename_boundary << "... COMPLETED.\033[0m" << std::endl;

                                    if(boundaries_b.empty())
                                    {
                                        std::cerr << FRED("ERROR: Boundary file does not contain valid polygon boundary data.") << std::endl;
                                        exit(1);
                                    }

                                    paramSurface.boundary = "EXTERNAL BOUNDARY";
                                    paramSurface.opt = "";

                                    if(optFlag.isSet())
                                        paramSurface.opt += optFlag.getValue();

                                    //Se ci sono più di un poligono di bordo, triangolo tutti i poligoni con lo stesso dataset (dati) e salvo una mesh per boundary
                                    for(size_t jj=0; jj<boundaries_b.size(); jj++)
                                    {
                                        std::vector<Point3D> boundary_unique;
                                        remove_duplicates_test_opt(boundaries_b.at(jj), boundary_unique, setTolerance.getValue());

                                        if(boundary_unique.size() < 3)
                                        {
                                            std::cerr << FRED("ERROR: at least 3 unique points are required for triangulation.") << std::endl;
                                            continue;
                                        }

                                        std::vector<Point3D> boundary_xy = boundary_unique;
                                        if(auto_aligned)
                                            align_points_to_xyplane(boundary_xy, otfRotation);
                                        
                                        trimesh.clear();
                                        trimesh = constrained_triangulation2(boundary_xy, data_xy, paramSurface.opt);
                                        remove_isolate_vertices(trimesh);

                                        if(auto_aligned)
                                            rotation_on_trimesh(trimesh, otfRotation, true);
                                        
                                        //Fix saving? To do: save a mesh for each boundary (if more than one)
                                    }
                                }
                                else
                                {
                                    std::cerr << "ERROR: Required argument missing: --convex, --concave or --boundary -m <filename>" << std::endl;
                                    continue;
                                }

                                Surface.setParameters(paramSurface);
                                Surface.setSummary(trimesh);

                                trimesh.save(out_mesh.c_str());
                                std::cout << "\033[0;32mSaving mesh file: " << out_mesh << "\033[0m" << std::endl;
                                std::cout << std::endl;
                            }
                            else if (gridFlag.isSet())
                            {
                                std::cerr << "\033[0;31mGRID FLAG NOT ACTIVE for POINT/LINESTRING!\033[0m" << std::endl;
                                exit(1);
                            }

                            geometa.setMeshSummary(Surface);

                            if(datasets.size() > 1)
                                Geometry.id_subdomain = std::to_string(ii+1);

                            geometa.setGeospatialData(Geometry);

                            std::string json_path = datasets.size() > 1
                                                        ? out_surf + "/" + Geometry.name + "_" + std::to_string(ii+1) + ".json"
                                                        : out_surf + "/" + Geometry.name + ".json";
                            geometa.write(json_path);
                        }
                        break;
                    }
                    case LINESTRING:
                    {
                        break;
                    }
                    }
                }
            }
        }

        // Append list of files
        auto file_list_gpkg = get_vectorfiles(in_geometry);
        for(const auto& file : file_list_gpkg)
        {
            std::vector<std::vector<Point3D>> boundaries, datasets;
            std::string GDALtype;

            std::cout << "=== Loading: " << file << std::endl;

            printSpatialReferenceInfo(file, Project.authority);
            if(load_vectorfile(file, boundaries, datasets, GDALtype) != IOSUCCESS) {
                std::cerr << "\033[0;31mERROR loading: " << file << "\033[0m" << std::endl;
                continue;
            }

            // Export
            if(setSaveAttributesTable.isSet())
            {
                auto csv_path = out_surf + "/" + get_basename(get_filename(file)) + ext_txt;
                if(export_attributes_to_csv(file, csv_path) == IOSUCCESS) {
                    Geometry.setAttributeTable(get_basename(get_filename(file)) + ext_txt);
                }
            }

            if(setSave.isSet()) {
                const std::string basename = get_basename(get_filename(file));
                const std::string ext = get_extensionND(get_filename(file));

                if(!boundaries.empty())
                {
                    for(uint id=0; id<boundaries.size(); id++)
                        export3d_xyz(out_surf + "/" + basename + "_" + std::to_string(id) + "@" + ext + ext_txt, boundaries[id]);
                    std::cout << "=== Export boundary points from geospatial file: " << file << std::endl;
                }
                if(!datasets.empty())
                {
                    for(uint id=0; id<datasets.size(); id++)
                        export3d_xyz(out_surf + "/" + basename + "_" + std::to_string(id) + "@" + ext + ext_txt, datasets[id]);
                    std::cout << "=== Export data points from geospatial file: " << file  << std::endl;
                }
            }

            if(!triFlag.isSet() && !gridFlag.isSet() && !polygonFlag.isSet()) {
                std::cout << "=== No meshing algorithms selected!" << std::endl;
                continue;
            }
            
            // Trasformazioni
            //applyCoordTransform(boundaries);
            //applyCoordTransform(datasets);

            // Setup Geometry
            Geometry.setName(get_basename(get_filename(file)));
            Geometry.setFormat(get_extension(file));
            Geometry.setDomains(std::max(boundaries.size(), datasets.size()));

            if(!boundaries.empty() && !datasets.empty()) Geometry.geom_type = MULTI;
            else setGeometryType(Geometry, GDALtype);

            if(setEPSG.isSet()) Geometry.setAuthority(setEPSG.getValue());

            std::vector<std::string> deps = {filesystem::relative(file, Project.folder)};
            geometa.setDependencies(deps);
            geometa.setGeospatialData(Geometry);


        
            switch (Geometry.geom_type)
            {
            case POLYGON:
            case MULTI:
            {
                if(boundaries.empty()) 
                {
                    std::cerr << FRED("ERROR: No polygon boundary data.") << std::endl;
                    continue;
                }

                if(polygonFlag.isSet())
                {
                    //FOR JSON ...
                    MUSE::Surface::Parameters paramSurface;
                    paramSurface.type = "POLYGONMESH";

                    MUSE::Polygonmesh<> polygonmesh (boundaries, polygonmesh);
                    std::string out_mesh = out_surf + "/" + Geometry.name + ext_surf;
                    polygonmesh.save(out_mesh.c_str());

                    MUSE::Surface Surface;
                    Surface.setSummary(polygonmesh);
                    Surface.setParameters(paramSurface);

                    geometa.setMeshSummary(Surface);
                    geometa.setGeospatialData(Geometry);
                    geometa.write(out_surf +"/"+ Geometry.name + ".json");

                    break;
                }
                else
                {
                    if(boundaries.size() != datasets.size() && !datasets.empty())
                    {
                        std::cerr << FRED("ERROR: Different size vectors (data/boundary).") << std::endl;
                        continue;
                    }

                    for(size_t i=0; i<boundaries.size(); i++)
                    {
                        if(boundaries[i].size() <= 0)
                        {
                            std::cout << "### ID " << i+1 << " - boundary dimension is minor/equal to zero!" << std::endl;
                            std::cout << "### ID " << i+1 << " - NEGLECTED POLYGON!" << std::endl;
                            continue;
                        }

                        //////////////////
                        std::vector<Point3D> data_tmp = boundaries[i];
                        if(i < datasets.size() && !datasets[i].empty())
                            data_tmp.insert(data_tmp.end(), datasets[i].begin(), datasets[i].end());
                        MUSE::SurfaceMeta::DataSummary dataSummary;
                        dataSummary.setDataSummary(data_tmp);
                        geometa.setDataSummary(dataSummary);

                        // Rimozione duplicati (per evitare problemi di triangolazione) 
                        std::vector<Point3D> dataset_unique, boundaries_unique;
                        remove_duplicates_test_opt(boundaries[i], boundaries_unique, setTolerance.getValue());

                        const bool has_datasets = (i < datasets.size() && !datasets[i].empty());
                        if(has_datasets)
                            remove_duplicates_test_opt(datasets[i], dataset_unique, setTolerance.getValue());

                        if(boundaries_unique.size() < 3)
                        {
                            std::cerr << FRED("ERROR: at least 3 unique points are required for triangulation.") << std::endl;
                            continue;
                        }

                        std::string out_mesh = out_surf + "/"+ Geometry.name;
                        if(boundaries.size() > 1)
                        {
                            Geometry.id_subdomain = std::to_string(i+1);
                            out_mesh += "_" + Geometry.id_subdomain;
                        }
                        out_mesh += ext_surf;


                        ///////////////////////
                        MUSE::Rotation otfRotation;
                        std::vector<Point3D> support_xy = boundaries_unique;
                        const size_t n_boundary = support_xy.size();
                        if(has_datasets)
                            support_xy.insert(support_xy.end(), dataset_unique.begin(), dataset_unique.end());

                        bool auto_aligned = align_points_to_xyplane(support_xy, otfRotation);
                        std::vector<Point3D> boundary_xy(support_xy.begin(), support_xy.begin() + n_boundary);
                        std::vector<Point3D> data_xy;
                        if(has_datasets)
                            data_xy.assign(support_xy.begin() + n_boundary, support_xy.end());

                        MUSE::Surface Surface;
                        MUSE::Surface::Parameters paramSurface;    

                        ////
                        if(triFlag.isSet())
                        {
                            if(convexFlag.isSet())
                            {
                                std::cerr << "=== Convex flag is not able for GDALTYPE=POLYGON triangulation." << std::endl;
                                exit(0);
                            }
                            if(concaveFlag.isSet())
                            {
                                std::cerr << "=== Concave flag is not able for GDALTYPE=POLYGON triangulation." << std::endl;
                                exit(0);
                            }

                            // Procedo con la triangolazione in base al tipo di geometria
                            cinolib::Trimesh<> trimesh;
                            trimesh.clear();

                            paramSurface.type = "TRIMESH";
                            paramSurface.opt = "";
                            paramSurface.boundary = "FIXED BOUNDARY";

                            if(optFlag.isSet())
                                paramSurface.opt += optFlag.getValue();

                            std::cout << "\033[0;32mStarted Constrained Triangulation on boundary ...\033[0m" << std::endl;

                            if(has_datasets)
                                trimesh = constrained_triangulation2(boundary_xy, data_xy, paramSurface.opt);
                            else
                                trimesh = boundary_triangulation(boundary_xy, paramSurface.opt);

                            remove_isolate_vertices(trimesh);

                            if(auto_aligned)
                                rotation_on_trimesh(trimesh, otfRotation, true);

                            Surface.setSummary(trimesh);

                            trimesh.save(out_mesh.c_str());
                            std::cout << "\033[0;32mSaving mesh file: " << out_mesh << "\033[0m" << std::endl;
                            std::cout << std::endl;
                        }
                        else if(gridFlag.isSet())
                        {
                            //std::cout << "2D gridding is set on plane: " << setPlane.getValue() << std::endl;

                            //FOR JSON ...
                            paramSurface.type = "QUADMESH";
                            paramSurface.resx = setResx.getValue();
                            paramSurface.resy = setResy.getValue();
                            paramSurface.resz = 0.0;

                            MUSE::Quadmesh<> quadmesh (setResx.getValue(), setResy.getValue(), setNewZ.getValue(), boundary_xy);
                            quadmesh.save(out_mesh.c_str());
                            std::cout << "\033[0;32mSaving mesh file: " << out_mesh << "\033[0m" << std::endl;
                            std::cout << std::endl;

                            Surface.setSummary(quadmesh);
                        }

                        Surface.setParameters(paramSurface);

                        geometa.setMeshSummary(Surface);
                        geometa.setGeospatialData(Geometry);

                        std::string json_path = boundaries.size() > 1
                                                    ? out_surf + "/" + Geometry.name + "_" + Geometry.id_subdomain + ".json"
                                                    : out_surf + "/" + Geometry.name + ".json";
                        geometa.write(json_path);
                    }
                }
                break;
            }
            case LINESTRING:
            case POINT:
            {
                MUSE::Surface Surface;
                MUSE::Surface::Parameters paramSurface;

                if(datasets.empty())
                {
                    std::cerr << "ERROR: No point data available" << std::endl;
                    continue;
                }

                for(size_t i=0; i< datasets.size(); i++)
                {
                    if(datasets[i].size() <= 0)
                    {
                        std::cout << "### ID " << i+1 << " - dataset dimension is minor/equal to zero!" << std::endl;
                        std::cout << "### ID " << i+1 << " - NEGLECTED DATASET!" << std::endl;
                        continue;
                    }

                    MUSE::SurfaceMeta::DataSummary dataSummary;
                    dataSummary.setDataSummary(datasets.at(i)); //saving original data (datasets) summary in metadata
                    geometa.setDataSummary(dataSummary);


                    std::vector<Point3D> data_unique;
                    remove_duplicates_test_opt(datasets.at(i), data_unique, setTolerance.getValue());

                    if(data_unique.size() < 3)
                    {
                        std::cerr << FRED("ERROR: at least 3 unique points are required for triangulation.") << std::endl;
                        continue;
                    }

                    std::string out_mesh = out_surf + "/" + Geometry.name;
                    if(datasets.size() > 1) {
                        Geometry.id_subdomain = std::to_string(i+1);
                        out_mesh += "_" + std::to_string(i+1);
                    }
                    out_mesh += ext_surf;

                     
                    MUSE::Rotation otfRotation;

                    if(triFlag.isSet())
                    {
                        std::vector<Point3D> data_xy = data_unique;
                        bool auto_aligned = align_points_to_xyplane(data_xy, otfRotation);

                        cinolib::Trimesh<> trimesh;
                        trimesh.clear();

                        

                        paramSurface.type = "TRIMESH";
                        paramSurface.opt = "";

                        std::cout << "=== WARNING: Triangulation on XY plane." << std::endl;
                        std::cout << "=== WARNING: Data are automatically aligned to XY and restored to original space." << std::endl;

                        //Convex hull
                        if (convexFlag.isSet())
                        {
                            paramSurface.opt = "c";
                            paramSurface.boundary = "CONVEX HULL";

                            if(optFlag.isSet())
                                paramSurface.opt += optFlag.getValue();

                            trimesh = points_triangulation(data_xy, paramSurface.opt);
                            remove_isolate_vertices(trimesh);

                            if(auto_aligned)
                                rotation_on_trimesh(trimesh, otfRotation, true);                            

                            std::cout << "\033[0;32mTriangulation with convex hull ... COMPLETED.\033[0m" << std::endl;
                        }

                        else if (concaveFlag.isSet())
                        {
                            paramSurface.boundary = "CONCAVE HULL";

                            // 1. Calcolo il convex hull (passando per la triangolazione dei punti) e lo trasformo in int da uint
                            trimesh = points_triangulation(data_xy, "c");

                            std::vector<int> convexhull;
                            std::vector<unsigned int> convex_uint = trimesh.get_ordered_boundary_vertices();
                            for(int i: convex_uint)
                                convexhull.push_back((short) i);

                            std::vector<int> b_id;
                            std::vector<Point3D> concavehull = computing_concavehull(data_xy, convexhull, b_id);

                            // 2. Removing points of concavehull (boundary) from datasets
                            std::sort(b_id.begin(), b_id.end());

                            std::vector<Point3D> unique_data;
                            
                            for(size_t idp=0; idp< data_xy.size(); idp++)
                            {
                                if(!check_index(b_id, static_cast<size_t>(idp)))
                                    unique_data.push_back(data_xy.at(idp));

                                // if (!check_index(b_id, i))
                                // {
                                //     Point3D unique_p;
                                //     unique_p.x = data.at(i).x;
                                //     unique_p.y = data.at(i).y;
                                //     unique_p.z = data.at(i).z;

                                //     unique_data.push_back(unique_p);
                                // }
                            }

                            if(optFlag.isSet())
                                paramSurface.opt += optFlag.getValue();

                            trimesh.clear();
                            trimesh = concavehull_triangulation(concavehull, unique_data, paramSurface.opt);
                            remove_isolate_vertices(trimesh);

                            if(auto_aligned)
                                rotation_on_trimesh(trimesh, otfRotation, true);
                            
                            std::cout << "\033[0;32mTriangulation with concave hull ... COMPLETED.\033[0m" << std::endl;
                        }


                        // External boundary from cmd
                        else if (setBoundary.isSet()) //se gli passo da linea di comando un bordo esterno: 1) leggi 2) triangola i punti vincolati al bordo
                        {
                            paramSurface.opt = "";
                            paramSurface.boundary = "EXTERNAL-BOUNDARY";

                            std::string filename_boundary = setBoundary.getValue();

                            std::vector<std::vector<Point3D>> boundaries_b, datasets_b;
                            std::string GDALtype_b;

                            std::cout << "=== Loading file: " << filename_boundary << std::endl;

                            int loadvecfile = load_vectorfile(filename_boundary, boundaries_b, datasets_b, GDALtype_b);
                            if (loadvecfile != IOSUCCESS)
                            {
                                std::cerr << "\033[0;31mERROR vector loading file: " << filename_boundary << "\033[0m" << std::endl;
                                exit(1);
                            }
                            std::cout << "\033[0;32mLoading file: " << filename_boundary << "... COMPLETED.\033[0m" << std::endl;
                            std::cout << "\033[0;32mTYPE: " << GDALtype_b << std::endl;

                            if(boundaries_b.empty())
                            {
                                std::cerr << FRED("ERROR: Boundary file does not contain valid polygon boundary data.") << std::endl;
                                exit(1);
                            }

                            if(optFlag.isSet())
                                paramSurface.opt += optFlag.getValue();

                            for(size_t jj=0; jj<boundaries_b.size(); jj++)
                            {
                                std::vector<Point3D> boundary_unique;
                                remove_duplicates_test_opt(boundaries_b.at(jj), boundary_unique, setTolerance.getValue());

                                if(boundary_unique.size() < 3)
                                {
                                    std::cerr << FRED("ERROR: at least 3 unique points are required for triangulation.") << std::endl;
                                    continue;
                                }

                                std::vector<Point3D> boundary_xy = boundary_unique;
                                if(auto_aligned)
                                    align_points_to_xyplane(boundary_xy, otfRotation);

                                trimesh.clear();
                                trimesh = constrained_triangulation2(boundary_xy, data_xy, paramSurface.opt);
                                remove_isolate_vertices(trimesh);

                                if(auto_aligned)
                                    rotation_on_trimesh(trimesh, otfRotation, true);
                                
                                //Fix saving? To do: save a mesh for each boundary (if more than one)
                            }
                        }

                        else
                        {
                            std::cerr << "ERROR: Required argument missing: --convex, --concave or --boundary -m <filename>." << std::endl;
                            continue;
                        }

                        Surface.setParameters(paramSurface);
                        Surface.setSummary(trimesh);
                        geometa.setMeshSummary(Surface);

                        trimesh.save(out_mesh.c_str());
                        std::cout << "\033[0;32mSaving mesh file: " << out_mesh << "\033[0m" << std::endl;
                        std::cout << std::endl;
                    }
                    else if(gridFlag.isSet())
                    {
                        std::cerr << "\033[0;31mGRID FLAG NOT ACTIVE for POINT/LINESTRING!\033[0m" << std::endl;
                        exit(1);
                    }

                    if(datasets.size() > 1)
                        Geometry.id_subdomain = std::to_string(i+1);

                    geometa.setGeospatialData(Geometry);

                    std::string json_path = datasets.size() > 1
                                                ? out_surf + "/" + Geometry.name + "_" + std::to_string(i+1) + ".json"
                                                : out_surf + "/" + Geometry.name + ".json";
                    geometa.write(json_path);
                }
                break;            
            }
            }
        }
        std::cout << "\033[0;32m=== Vector processing ... COMPLETED.\033[0m" << std::endl;
    }



    ///
    /// Surface modeling from loading raster files
    /// To test: (1) removing convex/concave/boundary flags: the raster is always a grid!
    /// To test: (2) updating JSON information
    ///
    if(loadRaster.isSet())
    {
        MUSE::SurfaceMeta geometa;
        geometa.setProject(Project);

        std::vector<std::string> excommands;
        excommands.push_back(command);
        geometa.setCommands(excommands);

        std::string out_rast = out_geometry +"/surf";
        if(!filesystem::exists(out_rast))
            filesystem::create_directory(out_rast);

        if(filesystem::is_empty(in_geometry))
        {
            std::cerr << "\033[0;31m=== Input ERROR: Insert raster file into: " << in_geometry << "\033[0m" << std::endl;
            exit(1);
        }

        std::vector<std::string> dir_grid = get_directories(in_geometry);
        if(dir_grid.empty())
        {
            //std::cout << "=== Input ERROR: no directories found!" << std::endl;
            dir_grid.push_back(in_geometry);
            //std::cout << dir_grid.at(0) << std::endl;
        }

        MUSE::GeospatialData Geometry;
        MUSE::Surface Surface;
        MUSE::Surface::Parameters paramSurface;

        for(size_t i=0; i<dir_grid.size(); i++)
        {
            std::vector<std::string> list_grid = get_rasterfiles(dir_grid.at(i));

            if(list_grid.size() == 0)
                continue; //vado alla dir_shape.at(i+1)

            for(size_t j=0; j< list_grid.size(); j++)
            {
                std::vector<std::vector<float>> grid;
                float XOrigin, YOrigin;
                int nXSize, nYSize;
                float XSizePixel = 1.0;
                float YSizePixel = 1.0;

                // Read raster file
                printSpatialReferenceInfo(list_grid.at(j), Project.authority);
                int loadRc = load_rasterfile(list_grid.at(j), grid, XOrigin, YOrigin, nXSize, nYSize, XSizePixel, YSizePixel);
                if(loadRc != 0)
                {
                    std::cerr << "\033[0;31mERROR: Unable to read raster file: " << list_grid.at(j) << "\033[0m" << std::endl;
                    exit(1);
                }

                std::cout << "=== Columns number (nXSize): " << nXSize << ", Rows number (nYSize): " << nYSize << std::endl;
                std::cout << std::fixed << std::setprecision(setPrecision.getValue()) << "=== XOrigin: " << XOrigin << ", YOrigin: " << YOrigin << std::endl;
                std::cout << "=== Grid size: " << grid.size() << " x " << (grid.empty() ? 0 : grid[0].size()) << std::endl;
                std::cout << "=== X Pixel size: " << XSizePixel << ", Y Pixel size: " << YSizePixel << std::endl;
                std::cout << "\033[0;32m=== Import raster file: " << list_grid.at(j) << "... COMPLETED.\033[0m" << std::endl;

                // Set Geometry class
                Geometry.setName(list_grid.at(j).substr(list_grid.at(j).find_last_of("/")+1, list_grid.at(j).length()));
                Geometry.setFormat(get_extension(list_grid.at(j)));

                if(setEPSG.isSet())
                    Geometry.setAuthority(setEPSG.getValue());

                //geometa.write(out_rast + "/" + Geometry.getName() + ".json");

                std::vector<Point3D> data, uniq_data;
                for(int row = 0; row < nYSize; row++)
                {
                    for(int col = 0; col < nXSize; col++)
                    {
                        Point3D p;
                        p.x = XOrigin + col * (/* pixel_size_x se disponibile, altrimenti assumere 1.0 */ XSizePixel);
                        p.y = YOrigin + row * (/* pixel_size_y se disponibile, altrimenti assumere 1.0 */ YSizePixel);
                        p.z = grid.at(row).at(col);
                        p.index = row * nXSize + col;

                        data.push_back(p);
                    }
                }
                MUSE::SurfaceMeta::DataSummary dataSummary;
                dataSummary.setDataSummary(data);
                geometa.setDataSummary(dataSummary);
                std::cout << "=== Extract coordinates of pixel centroids ... COMPLETED." << std::endl;
                std::cout << std::endl;

                ///
                /// Starting meshing
                ///
                if(triFlag.isSet())
                {
                    cinolib::Trimesh<> trimesh;
                    trimesh.clear();

                    // FOR JSON ...
                    paramSurface.type = "TRIMESH";

                    std::cout << "WARNING: Triangulation is performed on XY plane." << std::endl;

                    // Convex hull
                    if(convexFlag.isSet())
                    {
                        paramSurface.opt = "c";

                        if(optFlag.isSet())
                            paramSurface.opt = paramSurface.opt + optFlag.getValue();

                        remove_duplicates_test_opt(data, uniq_data);
                        trimesh.clear();
                        trimesh = points_triangulation(uniq_data, paramSurface.opt);
                        remove_isolate_vertices(trimesh);

                        paramSurface.boundary = "CONVEX HULL";

                        std::cout << "\033[0;32mTriangulation with convex hull ... COMPLETED.\033[0m" << std::endl;
                    }
                    else if(concaveFlag.isSet())
                    {
                        // 1. Calcolo il convex hull e lo trasformo in int da uint
                        trimesh = points_triangulation(data, "c");
                        std::vector<int> convexhull;
                        std::vector<unsigned int> convex_uint = trimesh.get_ordered_boundary_vertices();
                        //for(unsigned int idx : convex_uint)
                        //    convexhull.push_back((int)idx);
                        for (unsigned int i : convex_uint)
                            convexhull.push_back(static_cast<int>(i));

                        std::vector<int> b_id;
                        std::vector<Point3D> concavehull = computing_concavehull(data, convexhull, b_id);

                        // 2. Removing points of concavehull (boundary) from datasets
                        std::sort(b_id.begin(), b_id.end());
                        std::vector<Point3D> unique_data;
                        for(size_t k = 0; k < data.size(); k++)
                        {
                            if(!check_index(b_id, k))
                            {
                                Point3D unique_p;
                                unique_p.x = data.at(k).x;
                                unique_p.y = data.at(k).y;
                                unique_p.z = data.at(k).z;

                                unique_data.push_back(unique_p);
                            }
                        }

                        if(optFlag.isSet())
                            paramSurface.opt = optFlag.getValue();
                        else
                            paramSurface.opt = "";

                        trimesh.clear();
                        trimesh = concavehull_triangulation(concavehull, unique_data, paramSurface.opt);
                        remove_isolate_vertices(trimesh);

                        paramSurface.boundary = "CONCAVE HULL";

                        std::cout << "\033[0;32mTriangulation with concave hull ... COMPLETED.\033[0m" << std::endl;
                    }
                    // External boundary from cmd
                    else if(setBoundary.isSet())
                    {
                        std::cout << "=== External boundary is interpreted as a 2D xy clipping/constrained domain for raster data triangulation." << std::endl;
                        std::cout << "=== No rotation is applied to raster data or boundary points." << std::endl;

                        std::vector<Point3D> boundary;
                        load_xyzfile(setBoundary.getValue(), boundary);

                        paramSurface.opt = "";

                        if(optFlag.isSet())
                            paramSurface.opt = optFlag.getValue();

                        trimesh.clear();
                        trimesh = constrained_triangulation2(boundary, data, paramSurface.opt);

                        paramSurface.boundary = "CONSTRAINED";
                    }
                    else
                    {
                        std::cerr << "ERROR: Required argument missing: --convex, --concave or --boundary -m <filename>." << std::endl;
                        exit(1);
                    }

                    Surface.setParameters(paramSurface);
                    Surface.setSummary(trimesh);

                    std::string out_mesh = out_rast + "/" + get_basename(Geometry.getName()) + ext_surf;
                    //trimesh.translate(center);
                    trimesh.save(out_mesh.c_str());
                }
                else if(gridFlag.isSet())
                {
                    paramSurface.type = "QUADMESH";

                    paramSurface.resx = setResx.isSet() ? setResx.getValue() : XSizePixel;
                    paramSurface.resy = setResy.isSet() ? setResy.getValue() : YSizePixel;
                    paramSurface.resz = 0.0;

                    std::string out_mesh = out_rast + "/" + get_basename(Geometry.getName()) + ext_surf;

                    //ADD DOWNSAMPLING (FROM MUSE-GEOM): TO DO!
                    MUSE::Quadmesh<> quadmesh(nYSize-1, nXSize-1, XSizePixel, YSizePixel, XOrigin, YOrigin, grid);

                    //std::string out_mesh = out_rast + "/grid_" + get_basename(Geometry.getName()) + ext_surf;
                    quadmesh.save(out_mesh.c_str());
                    Surface.setSummary(quadmesh);

                    Surface.setParameters(paramSurface);
                    std::cout << "\033[0;32m=== Saved quadmesh: " << out_mesh << "\033[0m" << std::endl;
                }

                geometa.setMeshSummary(Surface);
                geometa.setGeospatialData(Geometry);

                geometa.write(out_rast + "/" + get_basename(Geometry.getName()) + ".json");
                std::cout << "\033[0;32m=== Saved json: " << out_rast + "/" + get_basename(Geometry.getName()) + ".json" << "\033[0m" << std::endl;
            }
        }
    }



    if(loadPointCloud.isSet())
    {
        // Check on input files (.txt, .dat)
        if(filesystem::is_empty(in_geometry))
        {
            std::cerr << "\033[0;31mInput ERROR: Insert file into: " << in_geometry << "\033[0m" << std::endl;
            exit(1);
        }

        std::vector<std::string> file_list = get_xyzfiles(in_geometry);
        if(file_list.size() == 0)
        {
            std::cerr << "\033[0;31mInput ERROR: NO datafile (.txt, .dat, .xyz) in the folder"<< in_geometry << "\033[0m" << std::endl;
            exit(1);
        }


        if(triFlag.isSet() || gridFlag.isSet())
        {
            if(!filesystem::exists(out_surf))
                filesystem::create_directory(out_surf);
        }
        else
        {
            std::cout << FRED("ERROR: Set --tri/--grid for creating mesh.") << std::endl;
            exit(1);
        }


        MUSE::SurfaceMeta geometa;
        geometa.setProject(Project);

        std::vector<std::string> deps;

        std::vector<std::string> excommands;
        excommands.push_back(command);
        geometa.setCommands(excommands);

        if(!setPoints.isSet() && !setPolygon.isSet())
        {
            std::cout << FRED("ERROR: Set --points <filename> or --polygon <filename>.") << std::endl;
            exit(1);
        }

        //apro i file con la specifica della geometria (NO LOADING AUTOMATICO!!)
        //perchè per le point cloud non ho la possibilità di definire da qualche parte il tipo (come in gdal)
        MUSE::GeospatialData Geometry;
        if(setPoints.isSet())
        {
            Geometry.setName(setPoints.getValue().substr(setPoints.getValue().find_last_of("/")+1, setPoints.getValue().length()));
            Geometry.format = get_extension(Geometry.name);
            Geometry.name = get_basename(Geometry.name);

            Geometry.geom_type = geomType::POINT;

            if(setPoints.getValue().find(abspath) != std::string::npos)
            {
                filesystem::path realpath = filesystem::relative(setPoints.getValue(), abspath);
                deps.push_back(realpath);
            }

        }

        if(setPolygon.isSet())
        {
            Geometry.setName(setPolygon.getValue().substr(setPolygon.getValue().find_last_of("/")+1, setPolygon.getValue().length()));
            Geometry.format = get_extension(Geometry.name);
            Geometry.name = get_basename(Geometry.name);

            Geometry.geom_type = geomType::POLYGON;

            if(setPolygon.getValue().find(abspath) != std::string::npos)
            {
                filesystem::path realpath = filesystem::relative(setPolygon.getValue(), abspath);
                deps.push_back(realpath);
            }
        }

        geometa.setGeospatialData(Geometry);
        geometa.setDependencies(deps);


        //per salvataggio mesh
        std::string out_mesh = out_surf + "/"+ Geometry.name;
        out_mesh = out_mesh + ext_surf;


        // Procedo con la triangolazione in base al tipo di geometria
        MUSE::Surface Surface;
        MUSE::Surface::Parameters paramSurface;

        //il file è di tipo DATA -> POINT
        switch (Geometry.geom_type)
        {
        case POLYGON:
        {
            std::vector<Point3D> boundary;
            load_xyzfile(setPolygon.getValue(), boundary);

            if(boundary.empty())
            {
                std::cerr << "ERROR on loading data representing boundary" << std::endl;
                exit(1);
            }

            MUSE::SurfaceMeta::DataSummary dataSummary;
            dataSummary.setDataSummary(boundary);
            geometa.setDataSummary(dataSummary);

            std::vector<Point3D> boundary_unique;
            remove_duplicates_test_opt(boundary, boundary_unique, setTolerance.getValue());

            if(boundary_unique.size() < 3)
            {
                std::cerr << FRED("ERROR: at least 3 unique points are required for triangulation.") << std::endl;
                exit(1);
            }

            MUSE::Rotation otfRotation;

            if(triFlag.isSet())
            {
                std::vector<Point3D> boundary_xy = boundary_unique;
                bool auto_aligned = align_points_to_xyplane(boundary_xy, otfRotation);

                cinolib::Trimesh<> trimesh;
                trimesh.clear();

                paramSurface.type = "TRIMESH";
                paramSurface.opt = "";
                paramSurface.boundary = "FIXED BOUNDARY";

                std::cout << "WARNING: Triangulation is performed on XY plane." << std::endl;
                std::cout << "WARNING: Set --rotaxis <axis>, --rotcx <double>, --rotcy <double>, --rotcz <double>, --rotangle <degree> to perform data rotation." << std::endl;
                std::cout << std::endl;

                if(optFlag.isSet())
                    paramSurface.opt += optFlag.getValue();

                trimesh = boundary_triangulation(boundary_xy, paramSurface.opt);

                if(trimesh.num_verts() > boundary_xy.size())
                {
                    std::cout << std::endl;
                    std::cout << "Restore z for additional points ..." << std::endl;
                    if(setMethodZ.getValue().compare("CONSTANT") == 0)
                    {
                        std::cout << "Constant value " << setNewZ.getValue() << " is set for Z additional points." << std::endl;
                        for(uint vid=boundary_xy.size(); vid < trimesh.num_verts(); vid++)
                            trimesh.vert(vid).z() = setNewZ.getValue();
                    }
                    else if (setMethodZ.getValue().compare("MEAN") == 0)
                    {
                        std::cout << "Interpolation method is adopted to set z for additional points" << std::endl;
                        fittedPlane plane = fitPlane(boundary_xy);
                        for(uint vid=boundary_xy.size(); vid < trimesh.num_verts(); vid++)
                            trimesh.vert(vid).z() = (trimesh.vert(vid).x()-plane.meanX)*plane.meanA0+(trimesh.vert(vid).y()-plane.meanY)*plane.meanA1 + plane.meanZ;
                    }
                    else if (setMethodZ.getValue().compare("NEAR") == 0)
                    {
                        std::cout << "=== Nearest neighbor is not enabled." << std::endl;
                        exit(1);
                    }
                    std::cout << "=== Restore z for additional points ... COMPLETED." << std::endl;
                    std::cout << std::endl;
                }

                remove_isolate_vertices(trimesh);

                if(auto_aligned)
                    rotation_on_trimesh(trimesh, otfRotation, true);

                Surface.setSummary(trimesh);
                Surface.setParameters(paramSurface);
                trimesh.save(out_mesh.c_str());
            }

            else if(gridFlag.isSet())
            {
                std::cout << std::endl;
                std::cout << "Resolution in X direction: " << setResx.getValue() << std::endl;
                std::cout << "Resolution in Y direction: " << setResy.getValue() << std::endl;
                std::cout << "Set --resx <value>, --resy <value> to modify default resolutions." << std::endl;
                std::cout << std::endl;
                std::cout << "=== TO DO: auto-alignment is not enabled for grid meshing." << std::endl;

                //FOR JSON ...
                paramSurface.type = "QUADMESH";
                paramSurface.resx = setResx.getValue();
                paramSurface.resy = setResy.getValue();
                paramSurface.resz = 0.0;

                MUSE::Quadmesh<> quadmesh (setResx.getValue(), setResy.getValue(), setNewZ.getValue(), boundary);
                quadmesh.save(out_mesh.c_str());

                //paramSurface.type = "GRIDMESH";

                Surface.setSummary(quadmesh);
            }

            std::cout << std::endl;
            std::cout << "\033[0;32mSaving mesh file: " << out_mesh << "\033[0m" << std::endl;
            std::cout << std::endl;

            Surface.setParameters(paramSurface);

            geometa.setMeshSummary(Surface);
            geometa.setGeospatialData(Geometry);
            geometa.write(out_surf +"/"+ Geometry.name + ".json");

            break;
        }
        case MULTI:
        {
            std::cout << FYEL("MULTI CASE: NOTING TO DO.") << std::endl;
            break;
        }
        case POINT:
        {
            std::vector<Point3D> data, uniq_data;
            load_xyzfile(setPoints.getValue(), data);

            if(data.empty())
            {
                std::cerr << "ERROR on loading points" << std::endl;
                exit(1);
            }

            MUSE::SurfaceMeta::DataSummary dataSummary;
            dataSummary.setDataSummary(data);
            geometa.setDataSummary(dataSummary);


            if(subSet.isSet())
            {
                std::cout << "=== Random sampling of data vector is set." << std::endl;
                srand(time(NULL));
                std::vector<size_t> random_id(subSet.getValue());
                for (size_t i = 0; i < subSet.getValue(); i++)
                {
                    random_id[i] = rand() % data.size();
                    //std::cout << "rand ID: " << random_id[i] << std::endl;
                }

                std::sort(random_id.begin(), random_id.end());
                random_id.erase(std::unique( random_id.begin(), random_id.end() ), random_id.end() );

                std::vector<Point3D> data_rand=data;
                data.clear();
                for(int rid:random_id)
                    data.push_back(data_rand.at(rid));
                std::cout << "=== Size of data vector (before random sampling): " << data_rand.size() << std::endl;
                std::cout << "=== New size of data vector (after random sampling): " << data.size() << std::endl;
                std::cout << std::endl;

                std::string filename_rand = "_subset.xyz";
                export3d_xyz(out_surf + "/" + filename_rand, data);
            }

            std::vector<Point3D> data_unique;
            remove_duplicates_test_opt(data, data_unique, setTolerance.getValue());

            if(data_unique.size() < 3)
            {
                std::cerr << FRED("ERROR: at least 3 unique points are required for triangulation.") << std::endl;
                exit(1);
            }

            MUSE::Rotation otfRotation;

            if(triFlag.isSet())
            {
                std::vector<Point3D> data_xy = data_unique;
                bool auto_aligned = align_points_to_xyplane(data_xy, otfRotation);

                cinolib::Trimesh<> trimesh;
                trimesh.clear();

                //FOR JSON ...
                paramSurface.type = "TRIMESH";

                std::cout << "WARNING: Triangulation is performed on XY plane." << std::endl;

                //Convex hull
                if (convexFlag.isSet())
                {
                    paramSurface.opt = "c";

                    if(optFlag.isSet())
                        paramSurface.opt += optFlag.getValue();

                    trimesh = points_triangulation(data_xy, paramSurface.opt);

                    if(trimesh.num_verts() > data_xy.size())
                    {
                        std::cout << std::endl;
                        std::cout << "Restore z for additional points ..." << std::endl;
                        if(setMethodZ.getValue().compare("CONSTANT") == 0)
                        {
                            std::cout << "Constant value " << setNewZ.getValue() << " is set for Z additional points." << std::endl;
                            for(uint vid=data_xy.size(); vid < trimesh.num_verts(); vid++)
                                trimesh.vert(vid).z() = setNewZ.getValue();
                        }
                        else if (setMethodZ.getValue().compare("MEAN") == 0)
                        {
                            std::cout << "Interpolation method is adopted to set z for additional points" << std::endl;
                            fittedPlane plane = fitPlane(data_xy);
                            for(uint vid=data_xy.size(); vid < trimesh.num_verts(); vid++)
                                trimesh.vert(vid).z() = (trimesh.vert(vid).x()-plane.meanX)*plane.meanA0+(trimesh.vert(vid).y()-plane.meanY)*plane.meanA1 + plane.meanZ;
                        }
                        else if (setMethodZ.getValue().compare("NEAR") == 0)
                        {
                            std::cout << "Interpolation method is adopted to set z for additional points" << std::endl;
                            for(uint vid=data_xy.size(); vid < trimesh.num_verts(); vid++)
                            {
                                std::vector<uint> adj_vert = trimesh.adj_v2v(vid);
                                double mean, sum=0.0;
                                int count=0;
                                for(uint bv:adj_vert)
                                {
                                    if(trimesh.vert(bv).z() != 0.0)
                                    {
                                        sum+=trimesh.vert(bv).z();
                                        count++;
                                    }
                                    //std::cout << sum << std::endl;
                                }
                                mean=sum/count;
                                std::cout << mean << std::endl;
                                trimesh.vert(vid).z() = mean;
                            }
                        }
                        else if (setMethodZ.getValue().compare("KRIGING") == 0)
                        {
                            std::vector<double> res_uniq_data, uniq_coord_x, uniq_coord_y, uniq_coord_z;
                            fittedPlane plane = fitPlane(data_xy);
                            for(uint i=0; i<data_xy.size(); i++)
                            {
                                double h = plane.meanA0 * data_xy.at(i).x + plane.meanA1 * data_xy.at(i).y + plane.b; //formulazione generale: h = A0*X + A1*Y + b
                                res_uniq_data.push_back(data_xy.at(i).z - h); //creo il vettore dei residuali (x,y,res_z)
                                uniq_coord_x.push_back(data_xy.at(i).x);
                                uniq_coord_y.push_back(data_xy.at(i).y);
                                uniq_coord_z.push_back(data_xy.at(i).z);
                            }
                            std::cout << "Calculating distance between points and best fitting plane ... COMPLETED." << std::endl;
                        }
                        std::cout << "Restore z for additional points ... COMPLETED." << std::endl;
                        std::cout << std::endl;
                    }

                    remove_isolate_vertices(trimesh);
                    if(auto_aligned)
                        rotation_on_trimesh(trimesh, otfRotation, true);

                    paramSurface.boundary = "CONVEX HULL";

                    std::cout << "\033[0;32mTriangulation with convex hull ... COMPLETED.\033[0m" << std::endl;
                }

                else if (concaveFlag.isSet())
                {
                    // 1. Calcolo il convex hull (passando per la triangolazione dei punti) e lo trasformo in int da uint
                    trimesh = points_triangulation(data_xy, "c");

                    std::vector<int> convexhull;
                    std::vector<unsigned int> convex_uint = trimesh.get_ordered_boundary_vertices();
                    for(int i: convex_uint)
                        convexhull.push_back((short) i);

                    std::vector<int> b_id;
                    std::vector<Point3D> concavehull = computing_concavehull(data_xy, convexhull, b_id);

                    // 2. Removing points of concavehull (boundary) from datasets
                    std::sort(b_id.begin(), b_id.end());
                    std::vector<Point3D> unique_data;
                    for(size_t idp=0; idp< data_xy.size(); idp++)
                    {
                        if (!check_index(b_id, static_cast<int>(idp)))
                            unique_data.push_back(data_xy.at(idp));
                    }

                    if(optFlag.isSet())
                        paramSurface.opt += optFlag.getValue();

                    trimesh.clear();
                    trimesh = concavehull_triangulation(concavehull, unique_data, paramSurface.opt);
                    remove_isolate_vertices(trimesh);

                    paramSurface.boundary = "CONCAVE HULL";

                    if(auto_aligned)
                        rotation_on_trimesh(trimesh, otfRotation, true);

                    std::cout << "\033[0;32mTriangulation with concave hull ... COMPLETED.\033[0m" << std::endl;
                }

                // External boundary from cmd
                else if (setBoundary.isSet()) //se gli passo da linea di comando un bordo esterno: 1) leggi 2) triangola i punti vincolati al bordo
                {
                    std::vector<Point3D> boundary, boundary_unique;
                    load_xyzfile(setBoundary.getValue(), boundary);
                    
                    if(boundary.empty())
                    {
                        std::cerr << "ERROR on loading data representing boundary" << std::endl;
                        exit(1);
                    }

                    remove_duplicates_test_opt(boundary, boundary_unique, setTolerance.getValue());
                    if(boundary_unique.size() < 3)
                    {
                        std::cerr << FRED("ERROR: at least 3 unique points are required for triangulation.") << std::endl;
                        exit(1);
                    }

                    std::vector<Point3D> boundary_xy = boundary_unique;
                    if(auto_aligned)
                        align_points_to_xyplane(boundary_xy, otfRotation);

                    paramSurface.opt = "";

                    if(optFlag.isSet())
                        paramSurface.opt = paramSurface.opt + optFlag.getValue();

                    trimesh.clear();
                    trimesh = constrained_triangulation2(boundary_xy, data_xy, paramSurface.opt);
                    remove_isolate_vertices(trimesh);

                    if(auto_aligned)
                        rotation_on_trimesh(trimesh, otfRotation, true);
                }
                else
                {
                    std::cerr << "ERROR: Required argument missing: --convex, --concave or --boundary -m <filename>." << std::endl;
                    break;
                }

                Surface.setParameters(paramSurface);
                Surface.setSummary(trimesh);

                trimesh.save(out_mesh.c_str());
            }
            else if (gridFlag.isSet())
            {
                std::cerr << FRED("GRID FLAG IS NOT ACTIVE!!") << std::endl;
                exit(1);
            }

            geometa.setMeshSummary(Surface);
            geometa.setGeospatialData(Geometry);

            geometa.write(out_surf + "/"+ Geometry.name + ".json");

            break;
        }
        case LINESTRING:
            break;
        }
    }



    if(gridData.isSet())
    {
        std::vector<std::string> excommands;
        excommands.push_back(command);

        std::vector<Point3D> boundary;
        if(setBBPoints.isSet())
        {
            std::vector<std::string> bbpoints = setBBPoints.getValue();

            for(uint i=0; i< bbpoints.size(); i++)
                std::cout << "BBPOINTS: " << bbpoints.at(i) << std::endl;
            std::cout << std::endl;

            for(uint i=0; i< bbpoints.size(); i++)
            {
                std::vector<std::string> direc = split_string(bbpoints.at(i), ',');

                Point3D p0;
                p0.x = std::stod(direc.at(0));
                p0.y = std::stod(direc.at(1));
                p0.z = std::stod(direc.at(2));
                boundary.push_back(p0);
            }
        }
        else
        {
            std::cout << FRED("ERROR. Set --bbp boundary points to extract mesh from grid.") << std::endl;
            exit(1);
        }

        //TO DOOOOOOOOOOOOOOOOOO: CONTROLLARE FUNZIONE DI ROTAZIONE PUNTI!!!
        if(setRotAxis.isSet())
        {
            std::vector<Point3D> boundary_tmp = boundary;
            boundary.clear();
            for(size_t i=0; i<boundary_tmp.size(); i++)
            {
                Point3D p_rot = rotPoint(boundary_tmp.at(i), setRotAxis.getValue(), setRotAngle.getValue());
                boundary.push_back(p_rot);
            }
        }

        if(gridFlag.isSet())
        {
            if(!filesystem::exists(out_surf))
                filesystem::create_directory(out_surf);

            MUSE::SurfaceMeta geometa;
            geometa.setProject(Project);
            geometa.setCommands(excommands);

            MUSE::Quadmesh<> quadmesh (setResx.getValue(), setResy.getValue(), setNewZ.getValue(), boundary);

            std::string out_mesh = out_surf + "/grid" + ext_surf;

            quadmesh.save(out_mesh.c_str());

            MUSE::Surface summary;
            MUSE::Surface::Parameters par;
            par.resx = setResx.getValue();
            par.resy = setResy.getValue();
            summary.setParameters(par);
            summary.setSummary(quadmesh);
            geometa.setMeshSummary(summary);

            geometa.write(out_surf + "/grid" + ".json");
        }

        if(hexFlag.isSet())
        {
            if(!filesystem::exists(out_volume))
                filesystem::create_directory(out_volume);

            MUSE::VolumeMeta geometa;
            geometa.setProject(Project);
            geometa.setCommands(excommands);

            MUSE::Hexmesh<> mesh (setResx.getValue(), setResy.getValue(), setResz.getValue(), boundary);

            std::string out_mesh = out_volume + "/grid" + ext_vol;

            if(ext_vol == ".vtk" || ext_vol == ".VTK")
            {
                if(save_vtk(out_mesh, mesh.vector_verts(), volume_cells_from_poly_verts(mesh)) != 0)
                {
                    std::cerr << "ERROR while writing vtk file: " << out_mesh << std::endl;
                    exit(1);
                }
            }
            else
            {
                mesh.save(out_mesh.c_str());
            }

            MUSE::Volume summary;
            MUSE::Volume::Parameters par;
            par.resx = setResx.getValue();
            par.resy = setResy.getValue();
            par.resz = setResz.getValue();
            summary.setParameters(par);
            summary.setSummary(mesh);
            geometa.setMeshSummary(summary);

            geometa.write(out_volume + "/grid" + ".json");
        }       

        std::cout << "Grid creation ... COMPLETED." << std::endl;

    }

/*//    if(gridData.isSet())
//    {
//        if(!filesystem::exists(out_surf))
//            filesystem::create_directory(out_surf);

//        std::vector<Point3D> data;
//        load_xyzfile(meshFiles.getValue().at(0), data);

//        if(data.size() == 0)
//        {
//            std::cerr << "Error on loading points" << std::endl;
//            exit(1);
//        }


//        if(gridDimension.getValue().compare("2D") == 0)
//        {
//            //TO DO: DA ESTENDERE PER TUTTI I PIANI E PER IL 3D.............................................
//            //TO DO: DEFINIRE IL CENTRO DI ROTAZIONE........................................................


//            std::cout << "2D gridding is set on plane: " << setPlane.getValue() << std::endl;


//            if(setRotAxis.isSet())
//            {
//                std::cout << "Rotation is activate on data..." << std::endl;
//                std::cout << "Rotation axis: " << setRotAxis.getValue() << std::endl;
//                std::cout << "Rotation center: " << std::endl; //TO COMPLETE
//                std::cout << "Rotation angle (degree): " << setRotAngle.getValue() << std::endl;

//                std::vector<Point3D> data_tmp = data;
//                data.clear();
//                for(size_t i=0; i<data_tmp.size(); i++)
//                {
//                    Point3D p_rot = rotPoint(data_tmp.at(i), setRotAxis.getValue(), setRotAngle.getValue());
//                    data.push_back(p_rot);
//                }
//            }

//            double res_x =  setResx.getValue();
//            double res_y =  setResy.getValue();

//            if (setBoundary.isSet()) //se gli passo da linea di comando un bordo esterno: 1) leggi 2) triangola i punti vincolati al bordo
//            {
//                std::vector<Point3D> boundary;
//                load_xyzfile(setBoundary.getValue(), boundary);

//                if(setRotAxis.isSet())
//                {
//                    std::vector<Point3D> boundary_tmp = boundary;
//                    boundary.clear();
//                    for(size_t i=0; i<boundary_tmp.size(); i++)
//                    {
//                        Point3D p_rot = rotPoint(boundary_tmp.at(i), setRotAxis.getValue(), setRotAngle.getValue());
//                        boundary.push_back(p_rot);
//                    }
//                }


//                MUSE::Quadmesh<> quadmesh (setResx.getValue(), setResy.getValue(), setNewZ.getValue(), boundary);

//                std::string ext_mesh = ".off";
//                if(objConversion.isSet() == true)
//                    ext_mesh = ".obj";

//                std::string out_mesh = out_surf + "/grid" + ext_mesh;

//                quadmesh.save(out_mesh.c_str());
//                exit(1);
//            }








//            double min_x =  DBL_MAX;
//            double min_y =  DBL_MAX;

//            double max_x = -DBL_MAX;
//            double max_y = -DBL_MAX;


//            if (setBoundary.isSet()) //se gli passo da linea di comando un bordo esterno: 1) leggi 2) triangola i punti vincolati al bordo
//            {
//                std::vector<Point3D> boundary;
//                load_xyzfile(setBoundary.getValue(), boundary);

//                if(setRotAxis.isSet())
//                {
//                    std::vector<Point3D> boundary_tmp = boundary;
//                    boundary.clear();
//                    for(size_t i=0; i<boundary_tmp.size(); i++)
//                    {
//                        Point3D p_rot = rotPoint(boundary_tmp.at(i), setRotAxis.getValue(), setRotAngle.getValue());
//                        boundary.push_back(p_rot);
//                    }
//                }

//                // Definizione boundary 2d
//                std::vector<Point2D> boundary2d;
//                for (uint i=0; i < boundary.size(); i++)
//                {
//                    Point2D p;
//                    p.x = boundary.at(i).x;
//                    p.y = boundary.at(i).y;

//                    boundary2d.push_back(p);
//                }


//                std::cout << "Computing bounding box ... COMPLETED. " << std::endl;

//                // Verifica se i punti cadono all'interno del bordo: se si, aggiorna il minimo/massimo
//                for (const Point3D &p : data)
//                {
//                    Point2D pp;
//                    pp.x = p.x;
//                    pp.y = p.y;
//                    if (! point_in_polygon(pp, boundary2d))
//                        continue;

//                    if (p.x < min_x) min_x = p.x;
//                    if (p.y < min_y) min_y = p.y;
//                    //if (p.z < min_z) min_z = p.z;

//                    if (p.x > max_x) max_x = p.x;
//                    if (p.y > max_y) max_y = p.y;
//                    //if (p.z > max_z) max_z = p.z;
//                }

//                // Calcolo delta
//                double delta_x = max_x - min_x;
//                double delta_y = max_y - min_y;
//                //double delta_z = max_z - min_z;
//                std::cout << "deltax = " << delta_x << "; deltay = " << delta_y << std::endl;

//                delta_x = delta_x + res_x;
//                delta_y = delta_y + res_y;

//                uint ncelle_x = static_cast<uint>(delta_x / res_x );
//                uint ncelle_y = static_cast<uint>(delta_y / res_y );
//                //uint nz = static_cast<uint>(delta_z / res_z );
//                std::cout << "nx = " << ncelle_x << "; ny = " << ncelle_y << std::endl;

//                std::vector<Point3D> points;

//                double xstart = min_x - res_x/2;
//                double ystart = min_y - res_y/2;

//                for (uint dx=0; dx < ncelle_x+1; dx++)
//                {
//                    double x = xstart + res_x * dx ;

//                    for (uint dy=0; dy < ncelle_y+1; dy++)
//                    {
//                        double y = ystart + res_y * dy ;
//                        double z = -1 ;

//                        //std::cout << x << "; " << y << "; " << z << std::endl;

//                        Point3D p;
//                        p.x = x;
//                        p.y = y;
//                        p.z = z;
//                        points.push_back(p);

//                    }
//                }
//                std::cout << "N. points of grid: " << points.size() << std::endl;
//                export3d_xyz(out_geometry + "/grid.txt", points);






//            }
//            else
//            {
//                std::cout << "Set external boundary to control only internal points in a fixed boundary" << std::endl;
//                exit(1);
//            }
//        }

//    }*/



    //passare una superficie in lettura
    if(appendMeshes.isSet() && meshFiles.getValue().size()>=2)
    {
        MUSE::SurfaceMeta geometa;
        geometa.setProject(Project);

        std::vector<std::string> excommands;
        excommands.push_back(command);
        geometa.setCommands(excommands);

        std::vector<std::string> files = meshFiles.getValue();

        std::string filename_mesh0 = files.at(0);
        std::string surface_folder = filename_mesh0.substr(0, filename_mesh0.find_last_of("/")); //cartella di progetto/out/geometry/surface
        std::string project_folder = surface_folder.substr(0, surface_folder.find_last_of("/")); //cartella di progetto/out/geometry
        project_folder = project_folder.substr(0, project_folder.find_last_of("/")); //cartella di progetto/out
        Project.setFolder(project_folder.substr(0, project_folder.find_last_of("/"))); //cartella di progetto

        Project.setName(Project.folder.substr(Project.folder.find_last_of("/")+1, Project.folder.length()));

        std::cout << "\033[0;32mLoading mesh file: " << filename_mesh0 << " ... COMPLETED.\033[0m" << std::endl;
        std::string filename = filename_mesh0.substr(filename_mesh0.find_last_of("/")+1, filename_mesh0.length());
        std::string basename = get_basename (filename);


        cinolib::Polygonmesh<> mesh;
        mesh.load(filename_mesh0.c_str());

        for(size_t i=1; i<files.size(); i++)
        {
            std::string filename_mesh = files.at(i);

            cinolib::Polygonmesh<> mesh_i;
            mesh_i.load(filename_mesh.c_str());

            mesh += mesh_i;
        }

//        std::string ext_mesh = ".off";
//        if(objConversion.isSet() == true)
//            ext_mesh = ".obj";

        std::string out_mesh = surface_folder +"/" + basename +"_append" + ext_surf;
        mesh.save(out_mesh.c_str());
        std::cout << "\033[0;32mExport mesh file: " << out_mesh << " ... COMPLETED.\033[0m" << std::endl;
    }



    //passare una superficie in lettura
    if(setOffset.isSet())
    {
        if(!meshFiles.isSet())
        {
            std::cout << FRED("ERROR. Set mesh to extrude by -m or --mesh command.") << std::endl;
            exit(1);
        }

        if(meshFiles.getValue().size() > 1)
        {
            std::cout << FRED("ERROR. Only one mesh is supported for the extrusion.") << std::endl;
            exit(1);
        }


        std::cout << FMAG("##########################################") << std::endl;
        std::cout << FMAG("NOTA BENE: l'estrusione è solo abilitata in direzione z") << std::endl;
        std::cout << FMAG("##########################################") << std::endl;
        std::cout << std::endl;

        std::vector<std::string> files = meshFiles.getValue();

        MUSE::SurfaceMeta georef;
        georef.read(get_basename(files.at(0)) + ".json");


        MUSE::SurfaceMeta geometa;
        geometa.setProject(Project);

        std::vector<std::string> excommands;
        excommands.push_back(command);
        geometa.setCommands(excommands);

        std::vector<std::string> deps;
        deps.push_back(filesystem::relative(get_basename(files.at(0)) + ".json", Project.folder));
//        if(files.at(0).find("geometry/") != std::string::npos)
//            deps.push_back(get_basename(files.at(0).substr(files.at(0).find("geometry/"))) + ".json");
        geometa.setDependencies(deps);

        geometa.setDataSummary(georef.getDataSummary());
        geometa.setDataRotation(georef.getDataRotation());



        //cinolib::Trimesh<> trimesh;
        cinolib::Polygonmesh <> mesh;
        mesh.load(files.at(0).c_str());

        std::cout << "\033[0;32mLoading mesh file: " << files.at(0) << " ... COMPLETED.\033[0m" << std::endl;
        std::string basename = files.at(0).substr(files.at(0).find_last_of("/")+1, files.at(0).length());
        basename = get_basename (basename);

//        std::string ext_mesh = ".off";
//        if(objConversion.isSet() == true)
//            ext_mesh = ".obj";
        std::string out_name = out_surf +"/" + basename;


        MUSE::SurfaceMeta::Extrusion objinfo;
//        objinfo.method = "one surface";
//        objinfo.surface = basename;

        //cinolib::Trimesh<> extr_trimesh;
        if(deltazExtrusion.isSet()) //estrusione con delta costante (z + delta)
        {
            objinfo.type = "delta";
            std::cout << "The extrusion is set on delta ... " << std::endl;

            if(zOffset.isSet())
            {
                std::cout << "The extrusion value is set on " << zOffset.getValue() << " in z direction ..." << std::endl;
                objinfo.value = zOffset.getValue();
                objinfo.direction = "z";

                for(size_t vid =0 ; vid < mesh.num_verts(); vid++)
                    mesh.vert(vid).z() = mesh.vert(vid).z() + zOffset.getValue();

                //out_name = out_name + "dz" + std::to_string(zOffset.getValue());
                out_name = out_name + "_d" + objinfo.direction;
            }
        }


        else if(abszExtrusion.isSet()) //estrusione fino ad una quota assoluta fissata
        {
            objinfo.type = "absolute elevation";
            std::cout << "The extrusion is set on absolute elevation ... " << std::endl;

            if(zOffset.isSet())
            {
                std::cout << "The extrusion value is set on " << zOffset.getValue() << " in z direction ..." << std::endl;
                objinfo.value = zOffset.getValue();
                objinfo.direction = "z";

                for(uint pid=0; pid < mesh.num_polys(); pid++)
                {
                    std::vector<cinolib::vec2d> vec2d;
                    for(size_t i=0; i < mesh.poly_verts(pid).size(); i++)
                    {
                        cinolib::vec2d v;
                        v.x() = mesh.poly_verts(pid).at(i).x();
                        v.y() = mesh.poly_verts(pid).at(i).y();
                        vec2d.push_back(v);
                    }
                    //std::cout << polygon_is_CCW(vec2d) << std::endl;
                    //std::cout << polygon_signed_area(vec2d) << std::endl;
                    // se l'area è positiva, quindi ccw = 1 (true) -> normale uscente
                    if(cinolib::polygon_is_CCW(vec2d) == false)
                        mesh.poly_flip_winding_order(pid);
                }
                //extr_trimesh = trimesh;
                for(size_t vid =0 ; vid < mesh.num_verts(); vid++)
                    mesh.vert(vid).z() = zOffset.getValue();

                //out_name = out_name + "absz" + std::to_string(zOffset.getValue());
                out_name = out_name + "_abs" + objinfo.direction;
            }
        }

        else
        {
            std::cerr << "\033[0;31mERROR: Required argument missing: --delta or --abs for surface extrusion in z direction.\033[0m" << std::endl;
            exit(1);
        }

        mesh.save((out_name + ext_surf).c_str());
        std::cout << "\033[0;32mExport mesh file: " << out_name + ext_surf << " ... COMPLETED.\033[0m" << std::endl;


        MUSE::Surface summary;
        summary.setSummary(mesh);

        geometa.setExtrusion(objinfo);
        geometa.setMeshSummary(summary);

        geometa.write(out_name + ".json");
    }



    if(createTriObject.isSet() && meshFiles.getValue().size() == 2)
    {
        std::cout << FMAG("##########################################") << std::endl;
        std::cout << FMAG("NOTA BENE: la chiusura delle mesh avviene per ora solo per mesh triangolari e in direzione z") << std::endl;
        std::cout << FMAG("##########################################") << std::endl;
        std::cout << std::endl;

        std::vector<std::string> files = meshFiles.getValue();

        cinolib::Trimesh<> trimesh0;
        trimesh0.load(files.at(0).c_str());
        std::cout << "\033[0;32mLoading mesh file: " << files.at(0) << " ... COMPLETED.\033[0m" << std::endl;
        std::string filename0 = files.at(0).substr(files.at(0).find_last_of("/")+1, files.at(0).length());
        std::string basename0 = get_basename(filename0);

        cinolib::Trimesh<> trimesh1;
        trimesh1.load(files.at(1).c_str());
        std::cout << "\033[0;32mLoading mesh file: " << files.at(1) << " ... COMPLETED.\033[0m" << std::endl;
        std::string filename1 = files.at(1).substr(files.at(1).find_last_of("/")+1, files.at(1).length());
        std::string basename1 = get_basename(filename1);

        std::cout << "bb1 completa: " << trimesh1.bbox()<< std::endl;
        std::cout << "bb0 completa: " << trimesh0.bbox()<< std::endl;


        //Creazione json del triobject
        MUSE::SurfaceMeta geometa;
        geometa.setProject(Project);

        std::vector<std::string> excommands;
        excommands.push_back(command);
        geometa.setCommands(excommands);

        std::vector<std::string> deps;
        deps.push_back(filesystem::relative(get_basename(files.at(0)) + ".json", Project.folder));
        deps.push_back(filesystem::relative(get_basename(files.at(1)) + ".json", Project.folder));

//        if(files.at(0).find("geometry/") != std::string::npos)
//            deps.push_back(get_basename(files.at(0).substr(files.at(0).find("geometry/"))) + ".json");
//        if(files.at(1).find("geometry/") != std::string::npos)
//            deps.push_back(get_basename(files.at(1).substr(files.at(1).find("geometry/"))) + ".json");
        geometa.setDependencies(deps);



        // 5) Creazione superficie laterale
        // 5.1) Vector indici dei punti sul convex hull nelle rispettive mesh
        std::vector<uint> or_idch0, or_idch1;
        or_idch0 = trimesh0.get_ordered_boundary_vertices();
        or_idch1 = trimesh1.get_ordered_boundary_vertices();

        size_t n = 0; //n vertici uguali
        for(unsigned int vid1 : or_idch1)
        {
            cinolib::vec2d vert2d_1 (trimesh1.vert(vid1).x(), trimesh1.vert(vid1).y());
            for(unsigned int vid0 : or_idch0)
            {
                cinolib::vec2d vert2d_0 (trimesh0.vert(vid0).x(), trimesh0.vert(vid0).y());
                if(vert2d_0.dist(vert2d_1) < 1e-2)
                {
                    n++;
                    break;
                }
            }
        }

        std::cout << std::endl;
        std::cout << or_idch0.size() << " boundary points of: " << basename0 << std::endl;
        std::cout << or_idch1.size() << " boundary points of: " << basename1 << std::endl;
        std::cout << n << " equal boundary points between " << basename0 << " and " << basename1 << std::endl;
        std::cout << FGRN("Check on boundary ... COMPLETED.") << std::endl;
        std::cout << std::endl;


        // INTEGRAZIONE CODICE DI GEO3D -> VALIDA E FUNZIONANTE PER PUNTI TRIANGOLATI CON CONVEX HULL! DA ESTENDERE CON BOUNDARY/CONCAVE
        // TO DO ...
        if(n != or_idch0.size()) //CONDIZIONE SU BORDI UGUALE
        {
            std::cout << "\033[0;31mERROR: Meshes boundaries are different!\033[0m" << std::endl;

            std::cout << FMAG("##########################################") << std::endl;
            std::cout << FMAG("RIFERIMENTO: implementazione di GEO3D") << std::endl;
            std::cout << FMAG("##########################################") << std::endl;
            std::cout << std::endl;


            // Criterio di scelta del bordo: area mesh
            std::vector<double> polygon_area (2);
            polygon_area.at(0) = trimesh0.mesh_area();
            polygon_area.at(1) = trimesh1.mesh_area();

            // Calcolo mesh con area minima
            double min_area = DBL_MAX;
            uint min_index = 0;
            for(uint i=0; i<polygon_area.size(); i++)
            {
                if(polygon_area.at(i) < min_area)
                {
                    min_area = polygon_area.at(i);
                    min_index = i;
                }
            }
            std::cout << "Area minima: " << polygon_area.at(min_index) << std::endl;

            cinolib::Trimesh<> tmp;
            std::string basename_tmp;
            if(min_index != 0)
            {
                tmp = trimesh0;
                trimesh0 = trimesh1;
                trimesh1 = tmp;

                basename_tmp = basename0;
                basename0 = basename1;
                basename1 = basename_tmp;
                //std::cout << "ORDINE MESH INVERTITO" << std::endl;
            }
            tmp.clear();

             //estrazione ch di riferimento
             std::vector<Point2D> ref_ch;
             for(uint i : trimesh0.get_ordered_boundary_vertices())
             {
                 Point2D p;
                 p.x = trimesh0.vert(i).x();
                 p.y = trimesh0.vert(i).y();
                 //p.z = trimesh0.vert(i).y();
                 ref_ch.push_back(p);
             }

             std::vector<cinolib::vec3d> verts2 = trimesh1.vector_verts();
             std::vector<Point3D> sub_verts2;
             for(unsigned int i=0; i< verts2.size(); i++)
             {
                 Point2D p;
                 p.x = verts2.at(i).x();
                 p.y = verts2.at(i).y();

                 bool internal = point_in_polygon(p, ref_ch);
                 if(internal)
                 {
                     Point3D p3d;
                     p3d.x = verts2.at(i).x();
                     p3d.y = verts2.at(i).y();
                     p3d.z = verts2.at(i).z();

                     sub_verts2.push_back(p3d);
                 }
             }
             std::cout << sub_verts2.size() << " vertices into " << basename0 << " mesh convex hull." << std::endl;

             trimesh1.clear();
             trimesh1 = points_triangulation(sub_verts2, "c");


             std::vector<Point3D> ch1_tmp, ch2_tmp, ch;
             for(uint i : trimesh0.get_ordered_boundary_vertices())
             {
                 Point3D p;
                 p.x = trimesh0.vert(i).x();
                 p.y = trimesh0.vert(i).y();
                 p.z = trimesh0.vert(i).z();
                 ch1_tmp.push_back(p);
             }

             for(uint i : trimesh1.get_ordered_boundary_vertices())
             {
                 Point3D p;
                 p.x = trimesh1.vert(i).x();
                 p.y = trimesh1.vert(i).y();
                 p.z = trimesh1.vert(i).z();
                 ch2_tmp.push_back(p);
             }

             ch = ch1_tmp;
             ch.insert(ch.end(), ch2_tmp.begin(), ch2_tmp.end());


             // ////////////////////////////////////////////////////////////////////////
             // CH_0
             // ////////////////////////////////////////////////////////////////////////

             std::cout << "Interpolation for added points (related to unique convex hull of first level)" << std::endl;
             std::cout << std::endl;

             std::vector<Point3D> chf = ch;
             std::vector<Point3D> new_points_chf;
             for (unsigned int i=ch1_tmp.size(); i < chf.size(); i++)
                 new_points_chf.push_back(chf.at(i));

             std::cout << new_points_chf.size() << " points to estimate z value for first level." << std::endl;
             std::cout << std::endl;


             std::vector<Point3D> verts_1, verts_2;
             for(uint vid=0; vid < trimesh0.num_verts(); vid++)
             {
                 Point3D p;
                 p.x = trimesh0.vert(vid).x();
                 p.y = trimesh0.vert(vid).y();
                 p.z = trimesh0.vert(vid).z();
                 verts_1.push_back(p);
             }

             fittedPlane planef = fitPlane(verts_1);

             for(uint i=0; i<new_points_chf.size(); i++)
                 new_points_chf.at(i).z = (new_points_chf.at(i).x-planef.meanX)*planef.meanA0+(new_points_chf.at(i).y-planef.meanY)*planef.meanA1 + planef.meanZ;

             uint ii=0;
             for (uint i = ii+ch1_tmp.size(); i < chf.size(); i++)
             {
                 chf.at(i).z = new_points_chf.at(ii).z;
                 ii++;
             }

             std::vector<Point3D> points_exf;
             for(uint vid=0; vid<trimesh0.num_verts(); vid++)
             {
                 if(!trimesh0.vert_is_boundary(vid))
                 {
                     Point3D p;
                     p.x = trimesh0.vert(vid).x();
                     p.y = trimesh0.vert(vid).y();
                     p.z = trimesh0.vert(vid).z();
                     points_exf.push_back(p);
                 }
             }
             points_exf.insert(points_exf.end(), chf.begin(), chf.end());


             // ////////////////////////////////////////////////////////////////////////
             // CH_1
             // ////////////////////////////////////////////////////////////////////////

             std::cout << "Interpolation for added points (related to unique convex hull of second level)" << std::endl;
             std::cout << std::endl;

             std::vector<Point3D> chs = ch;
             std::vector<Point3D> new_points_chs;
             for (uint i=0; i < ch1_tmp.size(); i++)
                 new_points_chs.push_back(chs.at(i));

             for (uint i=ch1_tmp.size()+ch2_tmp.size(); i < chs.size(); i++)
                 new_points_chs.push_back(chs.at(i));

             std::cout << new_points_chs.size() << " points to estimate z value for second level." << std::endl;
             std::cout << std::endl;

             for(uint vid=0; vid < trimesh1.num_verts(); vid++)
             {
                 Point3D p;
                 p.x = trimesh1.vert(vid).x();
                 p.y = trimesh1.vert(vid).y();
                 p.z = trimesh1.vert(vid).z();
                 verts_2.push_back(p);
             }

             fittedPlane planes = fitPlane(verts_2);
             for(uint i=0; i<new_points_chs.size(); i++)
                 new_points_chs.at(i).z = (new_points_chs.at(i).x-planes.meanX)*planes.meanA0+(new_points_chs.at(i).y-planes.meanY)*planes.meanA1 + planes.meanZ;

             for (uint i=0; i < ch1_tmp.size(); i++)
                 chs.at(i).z = new_points_chs.at(i).z;

             uint jj = ch1_tmp.size();
             for (uint i = jj +ch2_tmp.size(); i < chs.size(); i++)
             {
                 chs.at(i).z = new_points_chs.at(jj).z;
                 jj++;
             }


             std::vector<Point3D> points_exs;
             for(uint vid=0; vid<trimesh1.num_verts(); vid++)
             {
                 if(!trimesh1.vert_is_boundary(vid))
                 {
                     Point3D p;
                     p.x = trimesh1.vert(vid).x();
                     p.y = trimesh1.vert(vid).y();
                     p.z = trimesh1.vert(vid).z();
                     points_exs.push_back(p);
                 }
             }
             points_exs.insert(points_exs.end(), chs.begin(), chs.end());


             trimesh0.clear();
             trimesh1.clear();

             trimesh0 = points_triangulation(points_exf, "c");
             trimesh1 = points_triangulation(points_exs, "c");

             //CHIUSURA MESH
             or_idch0.clear();
             or_idch1.clear();

             //chiusura laterale mesh!
             or_idch0 = trimesh0.get_ordered_boundary_vertices();
             or_idch1 = trimesh1.get_ordered_boundary_vertices();

             std::cerr << or_idch0.size() << " " << or_idch1.size() << std::endl;

             trimesh0.update_bbox();
             trimesh1.update_bbox();
        }




/*
//             // 1. Estrazione bordo di riferimento: boundary/ch/concave -> lo leggo dal json
//             std::vector<Point3D> ref_bound;
//             std::vector<Point2D> ref_bound2d;
//             for(unsigned int i : trimesh0.get_ordered_boundary_vertices())
//             {
//                 Point3D p;
//                 p.x = trimesh0.vert(i).x();
//                 p.y = trimesh0.vert(i).y();
//                 p.z = trimesh0.vert(i).z();
//                 ref_bound.push_back(p);

//                 Point2D p2d;
//                 p2d.x = p.x;
//                 p2d.y = p.y;
//                 ref_bound2d.push_back(p2d);
//             }


//             // Vettore vertici <- mesh2
//             //std::vector<cinolib::vec3d> verts1 = trimesh1.vector_verts();
//             std::vector<Point3D> verts1, sub_verts1;
//             for(unsigned int vid=0; vid < trimesh1.num_verts(); vid++)
//             {
//                 Point3D p;
//                 p.x = trimesh1.vert(vid).x();
//                 p.y = trimesh1.vert(vid).y();
//                 p.z = trimesh1.vert(vid).z();
//                 verts1.push_back(p);

//                 Point2D p2d;
//                 p2d.x = p.x;
//                 p2d.y = p.y;

//                 bool internal = point_in_polygon(p2d, ref_bound2d);
//                 if(internal)
//                    sub_verts1.push_back(p);
//             }

//             std::cout << sub_verts1.size() << " on " << verts1.size() << " vertices into " << basename0 << " mesh boundary." << std::endl;

//             trimesh1.clear();
//             //SE NON è CONVEX HULL?? CAMBIARE LA TRIANGOLAZIONE! DA VINCOLARE AL BORDO


//             fittedPlane plane1 = fitPlane(verts1);
//             for(unsigned int i=0; i<ref_bound.size(); i++)
//                 ref_bound.at(i).z = (ref_bound.at(i).x-plane1.meanX)*plane1.meanA0 + (ref_bound.at(i).y-plane1.meanY)*plane1.meanA1 + plane1.meanZ;

//             std::string opt = "";
//             trimesh1 = constrained_triangulation2(ref_bound, sub_verts1, opt);
//             remove_isolate_vertices(trimesh1);
//             //trimesh1.save("/Users/mariannamiola/Desktop/MUSE/MUSE_test/PROJECT_220622/out/geometry/surface/MESH_CONSTR2.off");

//             //trimesh1 = points_triangulation(sub_verts1, "cV");
//             std::cout << "Sub-dataset triangulation ... COMPLETED." << std::endl;


//            //Extraction boundary
//             std::vector<Point3D> ch1_tmp, ch2_tmp, ch;
//             for(uint i : trimesh0.get_ordered_boundary_vertices())
//             {
//                 Point3D p;
//                 p.x = trimesh0.vert(i).x();
//                 p.y = trimesh0.vert(i).y();
//                 p.z = trimesh0.vert(i).z();
//                 ch1_tmp.push_back(p);
//             }

//             for(uint i : trimesh1.get_ordered_boundary_vertices())
//             {
//                 Point3D p;
//                 p.x = trimesh1.vert(i).x();
//                 p.y = trimesh1.vert(i).y();
//                 p.z = trimesh1.vert(i).z();
//                 ch2_tmp.push_back(p);
//             }

//             ch = ch1_tmp;
//             ch.insert(ch.end(), ch2_tmp.begin(), ch2_tmp.end());


//             // ////////////////////////////////////////////////////////////////////////
//             // CH_0
//             // ////////////////////////////////////////////////////////////////////////

//             std::cout << "Interpolation for added points (related to unique convex hull of first level)" << std::endl;
//             std::cout << std::endl;

//             std::vector<Point3D> chf = ch;
//             std::vector<Point3D> new_points_chf;
//             for (unsigned int i=ch1_tmp.size(); i < chf.size(); i++)
//                 new_points_chf.push_back(chf.at(i));

//             std::cout << new_points_chf.size() << " points to estimate z value for first level." << std::endl;
//             std::cout << std::endl;


//             std::vector<Point3D> verts_1, verts_2;
//             for(uint vid=0; vid < trimesh0.num_verts(); vid++)
//             {
//                 Point3D p;
//                 p.x = trimesh0.vert(vid).x();
//                 p.y = trimesh0.vert(vid).y();
//                 p.z = trimesh0.vert(vid).z();
//                 verts_1.push_back(p);
//             }

//             fittedPlane plane0 = fitPlane(verts_1);

//             for(uint i=0; i<new_points_chf.size(); i++)
//                 new_points_chf.at(i).z = (new_points_chf.at(i).x-plane0.meanX)*plane0.meanA0 + (new_points_chf.at(i).y-plane0.meanY)*plane0.meanA1 + plane0.meanZ;

//             uint ii=0;
//             for (uint i = ii+ch1_tmp.size(); i < chf.size(); i++)
//             {
//                 chf.at(i).z = new_points_chf.at(ii).z;
//                 ii++;
//             }

//             std::vector<Point3D> points_exf;
//             for(uint vid=0; vid<trimesh0.num_verts(); vid++)
//             {
//                 if(!trimesh0.vert_is_boundary(vid))
//                 {
//                     Point3D p;
//                     p.x = trimesh0.vert(vid).x();
//                     p.y = trimesh0.vert(vid).y();
//                     p.z = trimesh0.vert(vid).z();
//                     points_exf.push_back(p);
//                 }
//             }
//             //points_exf.insert(points_exf.end(), chf.begin(), chf.end());


//             // ////////////////////////////////////////////////////////////////////////
//             // CH_1
//             // ////////////////////////////////////////////////////////////////////////

//             std::cout << "Interpolation for added points (related to unique convex hull of second level)" << std::endl;
//             std::cout << std::endl;

//             std::vector<Point3D> chs = ch;
//             std::vector<Point3D> new_points_chs;
//             for (uint i=0; i < ch1_tmp.size(); i++)
//                 new_points_chs.push_back(chs.at(i));

//             for (uint i=ch1_tmp.size()+ch2_tmp.size(); i < chs.size(); i++)
//                 new_points_chs.push_back(chs.at(i));

//             std::cout << new_points_chs.size() << " points to estimate z value for second level." << std::endl;
//             std::cout << std::endl;

//             for(uint vid=0; vid < trimesh1.num_verts(); vid++)
//             {
//                 Point3D p;
//                 p.x = trimesh1.vert(vid).x();
//                 p.y = trimesh1.vert(vid).y();
//                 p.z = trimesh1.vert(vid).z();
//                 verts_2.push_back(p);
//             }

//             //fittedPlane plane1 = fitPlane(verts_2);
//             for(uint i=0; i<new_points_chs.size(); i++)
//                 new_points_chs.at(i).z = (new_points_chs.at(i).x-plane1.meanX)*plane1.meanA0 + (new_points_chs.at(i).y-plane1.meanY)*plane1.meanA1 + plane1.meanZ;

//             for (uint i=0; i < ch1_tmp.size(); i++)
//                 chs.at(i).z = new_points_chs.at(i).z;

//             uint jj = ch1_tmp.size();
//             for (uint i = jj +ch2_tmp.size(); i < chs.size(); i++)
//             {
//                 chs.at(i).z = new_points_chs.at(jj).z;
//                 jj++;
//             }


//             std::vector<Point3D> points_exs;
//             for(uint vid=0; vid<trimesh1.num_verts(); vid++)
//             {
//                 if(!trimesh1.vert_is_boundary(vid))
//                 {
//                     Point3D p;
//                     p.x = trimesh1.vert(vid).x();
//                     p.y = trimesh1.vert(vid).y();
//                     p.z = trimesh1.vert(vid).z();
//                     points_exs.push_back(p);
//                 }
//             }
//             //points_exs.insert(points_exs.end(), chs.begin(), chs.end());

//             std::cout << std::endl;



//             trimesh0.clear();
//             trimesh1.clear();

////             trimesh0 = points_triangulation(points_exf, "c");
////             trimesh1 = points_triangulation(points_exs, "c");

//             trimesh0 = constrained_triangulation2(chf, points_exf, opt);
//             trimesh1 = constrained_triangulation2(chs, points_exs, opt);

////             //CHIUSURA MESH
////             or_idch0.clear();
////             or_idch1.clear();

////             //chiusura laterale mesh!
////             or_idch0 = trimesh0.get_ordered_boundary_vertices();
////             or_idch1 = trimesh1.get_ordered_boundary_vertices();

////             std::cerr << or_idch0.size() << " " << or_idch1.size() << std::endl;
//        }*/


        or_idch0.clear();
        or_idch1.clear();

//        std::cout << "bb1 completa: " << trimesh1.bbox()<< std::endl;
//        std::cout << "bb0 completa: " << trimesh0.bbox()<< std::endl;


        // Check on normals
        double offset = trimesh1.bbox().center().z() - trimesh0.bbox().center().z();
//        double offset = trimesh1.bbox().max.z() - trimesh0.bbox().min.z();
//        std::cout << "bb1: " << trimesh1.bbox().max.z()<< std::endl;
//        std::cout << "bb0: " << trimesh0.bbox().min.z()<< std::endl;


        std::cout << "Offset in z direction: " << offset << std::endl;

        if(offset > 0) //check for normals and updated (if necessary)
        {
            for(unsigned int pid=0; pid<trimesh0.num_polys(); pid++)
                trimesh0.poly_flip_winding_order(pid);
        }
        else if(offset < 0)
        {
            for(unsigned int pid=0; pid<trimesh1.num_polys(); pid++)
                trimesh1.poly_flip_winding_order(pid);
        }
        else
        {
            std::cerr << "ERROR: z offset cannot be equal to 0." << std::endl;
            exit(1);
        }
        std::cout << FGRN("Check on normals ... COMPLETED.") << std::endl;
        std::cout << std::endl;


        cinolib::Trimesh<> closed_m;
        double step = trimesh1.edge_avg_length();
        if(step < trimesh0.edge_avg_length())
            step = trimesh0.edge_avg_length();
        std::cout << "Step to discretize lateral gap: " << step << std::endl;

        std::cout << FMAG("##########################################") << std::endl;
        std::cout << FMAG("Lo step di discretizzazione viene definito in base all'edge medio (minimo edge medio tra le due mesh)") << std::endl;
        std::cout << FMAG("##########################################") << std::endl;

        trimesh0.edge_mark_boundaries();
        trimesh1.edge_mark_boundaries();

        if(offset < 0)
            closed_m = closing_2trimeshes(trimesh0, trimesh1, step);
        else
            closed_m = closing_2trimeshes(trimesh1, trimesh0, step);

        if(!check_closing_mesh(closed_m))
        {
            std::cout << "\033[0;31mERROR on surfaces closing!\033[0m" << std::endl;
            exit(1);
        }

        std::string out_closed_mesh = out_surf +"/" + basename0 +"-" + basename1 + ext_surf;
        closed_m.save(out_closed_mesh.c_str());
        std::cout << "\033[0;32mExport mesh file: " << out_closed_mesh << " ... COMPLETED.\033[0m" << std::endl;


        MUSE::Surface summary;
        summary.setSummary(closed_m);
        geometa.setMeshSummary(summary);

        geometa.write(out_surf + "/" + basename0 + "-" + basename1 + ".json");
    }


    if(createTriObject.isSet()  && meshFiles.getValue().size() > 2)
        std::cerr << "ERROR: Unexpected number of input files!" << std::endl;


    if(createQuadObject.isSet() && meshFiles.getValue().size() == 2)
    {
        //Creazione json del triobject
        MUSE::GeometryMeta geometa;
        geometa.setProject(Project);

        std::vector<std::string> excommands;
        excommands.push_back(command);
        geometa.setCommands(excommands);

        std::cout << FMAG("##########################################") << std::endl;
        std::cout << FMAG("NOTA BENE: la chiusura delle mesh avviene per ora solo per mesh triangolari e in direzione z") << std::endl;
        std::cout << FMAG("##########################################") << std::endl;
        std::cout << std::endl;

        std::vector<std::string> files = meshFiles.getValue();

        cinolib::Quadmesh<> trimesh0;
        trimesh0.load(files.at(0).c_str());
        std::cout << "\033[0;32mLoading mesh file: " << files.at(0) << " ... COMPLETED.\033[0m" << std::endl;
        std::string filename0 = files.at(0).substr(files.at(0).find_last_of("/")+1, files.at(0).length());
        std::string basename0 = get_basename(filename0);

        cinolib::Quadmesh<> trimesh1;
        trimesh1.load(files.at(1).c_str());
        std::cout << "\033[0;32mLoading mesh file: " << files.at(1) << " ... COMPLETED.\033[0m" << std::endl;
        std::string filename1 = files.at(1).substr(files.at(1).find_last_of("/")+1, files.at(1).length());
        std::string basename1 = get_basename(filename1);


        MUSE::GeometryMeta::ObjectInfo objinfo;
        objinfo.method = "TWO SURFACES";
        objinfo.surface = basename0;
        //.... da estendere




        // 5) Creazione superficie laterale
        // 5.1) Vector indici dei punti sul convex hull nelle rispettive mesh
        std::vector<uint> or_idch0, or_idch1;
        or_idch0 = trimesh0.get_ordered_boundary_vertices();
        or_idch1 = trimesh1.get_ordered_boundary_vertices();

        size_t n = 0; //n vertici uguali
        for(unsigned int vid1 : or_idch1)
        {
            cinolib::vec2d vert2d_1 (trimesh1.vert(vid1).x(), trimesh1.vert(vid1).y());
            for(unsigned int vid0 : or_idch0)
            {
                cinolib::vec2d vert2d_0 (trimesh0.vert(vid0).x(), trimesh0.vert(vid0).y());
                if(vert2d_0.dist(vert2d_1) < 1e-2)
                {
                    n++;
                    break;
                }
            }
        }

        std::cout << std::endl;
        std::cout << or_idch0.size() << " boundary points of: " << basename0 << std::endl;
        std::cout << or_idch1.size() << " boundary points of: " << basename1 << std::endl;
        std::cout << n << " equal boundary points between " << basename0 << " and " << basename1 << std::endl;
        std::cout << FGRN("Check on boundary ... COMPLETED.") << std::endl;
        std::cout << std::endl;


        // INTEGRAZIONE CODICE DI GEO3D -> VALIDA E FUNZIONANTE PER PUNTI TRIANGOLATI CON CONVEX HULL! DA ESTENDERE CON BOUNDARY/CONCAVE
        // TO DO ...
        if(n != or_idch0.size()) //CONDIZIONE SU BORDI UGUALE
        {
            std::cout << "\033[0;31mERROR: Meshes boundaries are different!\033[0m" << std::endl;
            exit(1);
        }


        // Check on normals
        double offset = trimesh1.bbox().center().z() - trimesh0.bbox().center().z();
        std::cout << "Offset in z direction: " << offset << std::endl;

        if(offset > 0) //check for normals and updated (if necessary)
        {
            for(unsigned int pid=0; pid<trimesh0.num_polys(); pid++)
                trimesh0.poly_flip_winding_order(pid);
        }
        else if(offset < 0)
        {
            for(unsigned int pid=0; pid<trimesh1.num_polys(); pid++)
                trimesh1.poly_flip_winding_order(pid);
        }
        else
        {
            std::cerr << "ERROR: z offset cannot be equal to 0." << std::endl;
            exit(1);
        }
        std::cout << FGRN("Check on normals ... COMPLETED.") << std::endl;
        std::cout << std::endl;


        cinolib::Quadmesh<> closed_m;
        double step = trimesh1.edge_avg_length();
        if(step < trimesh0.edge_avg_length())
            step = trimesh0.edge_avg_length();
        std::cout << "Step to discretize lateral gap: " << step << std::endl;

        std::cout << FMAG("##########################################") << std::endl;
        std::cout << FMAG("Lo step di discretizzazione viene definito in base all'edge medio (minimo edge medio tra le due mesh)") << std::endl;
        std::cout << FMAG("##########################################") << std::endl;

        if(offset < 0)
            closed_m = closing_2quadmeshes(trimesh0, trimesh1, step);
        else
            closed_m = closing_2quadmeshes(trimesh1, trimesh0, step);

        if(!check_closing_mesh(closed_m))
        {
            std::cout << "\033[0;31mERROR on surfaces closing!\033[0m" << std::endl;
            exit(1);
        }

        std::string out_closed_mesh = out_surf +"/" + basename0 +"-" + basename1 + ext_surf;
        closed_m.save(out_closed_mesh.c_str());
        std::cout << "\033[0;32mExport mesh file: " << out_closed_mesh << " ... COMPLETED.\033[0m" << std::endl;


        MUSE::Surface summary;
        summary.setSummary(closed_m);
        geometa.setMeshSummary(summary);

        geometa.write(out_surf + "/" + basename0 + "-" + basename1 + ".json");
    }



    //ESTENSIONE APPLICATIVO PER GENERARE MESH TETRAEDRICHE CON ALTRI TIPI DI MESH!! IN BASE ALLA FLAG CHE GLI PASSO: --tet, --hex, --vox
    if(createVolObject.isSet() && meshFiles.getValue().size() == 1)
    {
        // 0) Creazione cartella per il salvataggio delle mesh volumetriche
        if(!filesystem::exists(out_volume))
            filesystem::create_directory(out_volume);


        // 1) Passaggio meshfile
        std::vector<std::string> files = meshFiles.getValue();
        //std::string filename_mesh = files.at(0);

        MUSE::VolumeMeta geometa;
        geometa.setProject(Project);

        std::vector<std::string> excommands;
        excommands.push_back(command);
        geometa.setCommands(excommands);

        std::vector<std::string> deps;
        deps.push_back(filesystem::relative(get_basename(files.at(0)) + ".json", Project.folder));
        geometa.setDependencies(deps);


        MUSE::Volume summary;

        std::cout << "\033[0;32mLoading mesh file: " << files.at(0) << " ... COMPLETED.\033[0m" << std::endl;
        std::string basename = files.at(0).substr(files.at(0).find_last_of("/")+1, files.at(0).length());
        basename = get_basename(basename);

        std::string out_mesh = out_volume +"/" + basename + ext_vol;

        // Se è settato il flag per i tetraedri ...
        if(tetFlag.isSet())
        {
            std::cout << "### tetFlag is set ... " << std::endl;

            //se voglio i tet ...
            cinolib::Trimesh<> trimesh;
            trimesh.load(files.at(0).c_str());

            double delta_max = trimesh.bbox().delta_x();
            if(trimesh.bbox().delta_y() >= delta_max)
                delta_max = trimesh.bbox().delta_y();

            std::cout << delta_max << std::endl;
            std::cout << trimesh.bbox().delta_z() << std::endl;
            double ratio = delta_max/trimesh.bbox().delta_z();
            std::cout << "### Ratio between max{delta_x,delta_y}/delta_Z: " << ratio << std::endl;

            //AGGIUNGERE LA CONDIZIONE PER LA TRASLAZIONE
            cinolib::vec3d center = trimesh.bbox().center();
            std::cout << "### Translate mesh at BBOX center: " << center << std::endl;
            trimesh.translate(-center);
            if(setSave.isSet())
                trimesh.save((get_basename(files.at(0)) + "_translate"+ext_surf).c_str());

            // Set parameters in opt
            std::string opt = "";
            if(optFlag.isSet())
                opt = opt + optFlag.getValue();

            // Run tetrahedralization by exploting Tetgen Library in Cinolib and create a tetrahedralization mesh (m_tet)
            cinolib::Tetmesh<> volmesh;
            cinolib::tetgen_wrap(trimesh.vector_verts(), trimesh.vector_polys(), trimesh.vector_edges(), opt, volmesh);

            double voltet_min=DBL_MAX, voltet_max=-DBL_MAX;
            for(uint pid=0; pid<volmesh.num_polys(); pid++)
            {
                double voltet = volmesh.poly_volume(pid);
                if(voltet < voltet_min)
                    voltet_min = voltet;
                if(voltet > voltet_max)
                    voltet_max = voltet;
            }
            std::cout << "### Compute min/max poly volumes ... COMPLETED." << std::endl;

            volmesh.translate(center);
            std::cout << "### Restore coordinates mesh from BBOX center: " << center << " COMPLETED." <<std::endl;

            std::cout << std::endl;
            std::cout << "=============================================" << std::endl;
            std::cout << "=== Statistical report about tetrahedral mesh ... " << std::endl;
            std::cout << "=== Poly average volume: " << volmesh.mesh_volume()/volmesh.num_polys() << std::endl;
            std::cout << "=== Poly max volume: " << voltet_max << std::endl;
            std::cout << "=== Poly min volume: " << voltet_min << std::endl;
            std::cout << "=== Edge average length: " << volmesh.edge_avg_length() << std::endl;
            std::cout << "=== Edge max length: " << volmesh.edge_max_length() << std::endl;
            std::cout << "=== Edge min length: " << volmesh.edge_min_length() << std::endl;
            
            if(volmesh.edge_max_length() > volmesh.edge_avg_length() * 1.5)
                std::cout << FYEL("### WARNING: (max) edge length major than 1.5 times edge average length ...") << std::endl;

            std::cout << "=============================================" << std::endl;
            std::cout << std::endl;

            if(ext_vol == ".vtk" || ext_vol == ".VTK")
            {
                if(save_vtk(out_mesh, volmesh.vector_verts(), volume_cells_from_poly_verts(volmesh)) != 0)
                {
                    std::cerr << "ERROR while writing vtk file: " << out_mesh << std::endl;
                    exit(1);
                }
            }
            else
            {
                volmesh.save(out_mesh.c_str());
            }
            std::cout << "\033[0;32mExport mesh file: " << out_mesh << " ... COMPLETED.\033[0m" << std::endl;

            summary.setSummary(volmesh);
        }


        if(voxFlag.isSet())
        {
            std::cout << "voxFlag is set ... " << std::endl;

            //MUSE::Quadmesh<> quadmesh;
            cinolib::Polygonmesh<> quadmesh;
            quadmesh.load(files.at(0).c_str());

            uint max_voxels_per_side = setMaxVoxelperSide.getValue();
            cinolib::VoxelGrid grid;
            cinolib::voxelize(quadmesh, max_voxels_per_side, grid);

            std::cout << "Grid dimensions: " << grid.dim[0] << " x " << grid.dim[1] << " x " << grid.dim[2] << std::endl;

            cinolib::Hexmesh<> volmesh;
            voxel_grid_to_hexmesh(grid, volmesh, cinolib::VOXEL_INSIDE);

            if(ext_vol == ".vtk" || ext_vol == ".VTK")
            {
                if(save_vtk(out_mesh, volmesh.vector_verts(), volume_cells_from_poly_verts(volmesh)) != 0)
                {
                    std::cerr << "ERROR while writing vtk file: " << out_mesh << std::endl;
                    exit(1);
                }
            }
            else
            {
                volmesh.save(out_mesh.c_str());
            }
            std::cout << "\033[0;32mExport mesh file: " << out_mesh << " ... COMPLETED.\033[0m" << std::endl;

            summary.setSummary(volmesh);
        }


        if(hexFlag.isSet())
        {
            std::cout << "hexFlag is set ... " << std::endl;

            //cinolib::Quadmesh<> quadmesh;
            cinolib::Trimesh<> mesh;
            mesh.load(files.at(0).c_str());

            MUSE::Hexmesh<> hexmesh(setResx.getValue(), setResy.getValue(), setResz.getValue(), mesh);

            if(ext_vol == ".vtk" || ext_vol == ".VTK")
            {
                if(save_vtk(out_mesh, hexmesh.vector_verts(), volume_cells_from_poly_verts(hexmesh)) != 0)
                {
                    std::cerr << "ERROR while writing vtk file: " << out_mesh << std::endl;
                    exit(1);
                }
            }
            else
            {
                hexmesh.save(out_mesh.c_str());
            }
            std::cout << "\033[0;32mExport mesh file: " << out_mesh << " ... COMPLETED.\033[0m" << std::endl;

            summary.setSummary(hexmesh);
        }

        geometa.setMeshSummary(summary);
        geometa.write(out_volume +"/" + basename + ".json");
    }


    if(createVolObject.isSet()  && meshFiles.getValue().size() > 1)
        std::cerr << "ERROR: Unexpected number of input files!" << std::endl;



    if(mergeMeshes.isSet() && meshFiles.getValue().size() == 2)
    {
        std::vector<std::string> files = meshFiles.getValue();

        std::string filename_mesh0 = files.at(0);
        std::string ext0 = get_extension(filename_mesh0);

        std::string filename_mesh1 = files.at(1);
        std::string ext1 = get_extension(filename_mesh1);


        if(ext0.compare(".off") == 0 || ext0.compare(".obj") == 0)
        {
            if(ext1.compare(".off") == 0 || ext1.compare(".obj") == 0)
            {
                std::cout << "Meshes are surfaces." << std::endl;
                //Le mesh sono superfici (controllo sull'estensione), quindi le carico come trimesh

                MUSE::SurfaceMesh<> trimesh0;
                trimesh0.load(filename_mesh0.c_str());
                std::cout << "\033[0;32mLoading mesh file: " << filename_mesh0 << " ... COMPLETED.\033[0m" << std::endl;
                std::string basename0 = get_basename(get_filename(filename_mesh0));

                MUSE::SurfaceMesh<> trimesh1;
                trimesh1.load(filename_mesh1.c_str());
                std::cout << "\033[0;32mLoading mesh file: " << filename_mesh1 << " ... COMPLETED.\033[0m" << std::endl;
                std::string basename1 = get_basename(get_filename(filename_mesh1));

                MUSE::SurfaceMesh<> trimesh;
                std::string out_mesh = out_surf +"/" + basename0 + "_" + basename1 + ext_surf;

                if(!trimesh0.check_lateral_closing() && !trimesh1.check_lateral_closing())
                {
                    merge_meshes(trimesh0, trimesh1, trimesh);
                    std::cout << "Meshes merge on boundary ... COMPLETED." << std::endl;
                }
                else if(trimesh0.check_lateral_closing() && trimesh1.check_lateral_closing())
                {
                    cinolib::merge_meshes_at_coincident_vertices(trimesh0, trimesh1, trimesh);
                    std::cout << "Meshes merge at coincident vertices ... COMPLETED." << std::endl;
                }
                else
                {
                    std::cout << "Error on meshes type!" << std::endl;
                    exit(1);
                }
                trimesh.save(out_mesh.c_str());

                std::cout << std::endl;
                std::cout << "P " << trimesh.num_polys() << std::endl;
                std::cout << "E "<< trimesh.num_edges() << std::endl;
                std::cout << "V "<< trimesh.num_verts() << std::endl;
                std::cout << "\033[0;32mSaving mesh file in : " << out_mesh << " ... COMPLETED.\033[0m" << std::endl;
            }
            else
            {
                std::cerr << "ERROR: Meshes format are different!" << std::endl;
                exit(1);
            }
        }
        else if(ext0.compare(".mesh") == 0 || ext0.compare(".vtk") == 0) //caso volumetrico
        {
            if(ext1.compare(".mesh") == 0 || ext0.compare(".vtk") == 0)
            {
                std::cout << "Meshes are volumes." << std::endl;

                MUSE::VolumeMesh<> tetmesh0;
                //cinolib::Hexmesh<> tetmesh0;
                tetmesh0.load(filename_mesh0.c_str());
                MeshType type0 = tetmesh0.set_meshtype();
                std::cout << "\033[0;32mLoading mesh file: " << filename_mesh0 << " ... COMPLETED.\033[0m" << std::endl;
                std::string basename0 = get_basename(get_filename(filename_mesh0));

                MUSE::VolumeMesh<> tetmesh1;
                tetmesh1.load(filename_mesh1.c_str());
                MeshType type1 = tetmesh1.set_meshtype();
                std::cout << "\033[0;32mLoading mesh file: " << filename_mesh1 << " ... COMPLETED.\033[0m" << std::endl;
                std::string basename1 = get_basename(get_filename(filename_mesh1));

                if(type0 != type1)
                {
                    std::cout << FRED("Mesh types are different. Merge not possible!") << std::endl;
                    exit(1);
                }

                std::string out_mesh = out_volume +"/" + basename0 + "_" + basename1 + ext_vol;

                if(type0 == MeshType::HEXMESH)
                {
                    cinolib::Hexmesh<> tetmesh;
                    //cinolib::Tetmesh<> tetmesh;
                    std::cout << "The proximity threshold is set on " << proxThreshold.getValue() << std::endl;
                    merge_meshes_at_coincident_vertices(tetmesh0, tetmesh1, tetmesh, proxThreshold.getValue());
                    std::cout << "Meshes merge at coincident vertices ... COMPLETED." << std::endl;

                    std::cout << std::endl;
                    std::cout << "P " << tetmesh.num_polys() << std::endl;
                    std::cout << "F " << tetmesh.num_faces() << std::endl;
                    std::cout << "E "<< tetmesh.num_edges() << std::endl;
                    std::cout << "V "<< tetmesh.num_verts() << std::endl;

                    if(ext_vol == ".vtk" || ext_vol == ".VTK")
                    {
                        if(save_vtk(out_mesh, tetmesh.vector_verts(), volume_cells_from_poly_verts(tetmesh)) != 0)
                        {
                            std::cerr << "ERROR while writing vtk file: " << out_mesh << std::endl;
                            exit(1);
                        }
                    }
                    else
                    {
                        tetmesh.save(out_mesh.c_str());
                    }
                }
                else
                {
                    //cinolib::Hexmesh<> tetmesh;
                    cinolib::Tetmesh<> tetmesh;
                    std::cout << "The proximity threshold is set on " << proxThreshold.getValue() << std::endl;
                    merge_meshes_at_coincident_vertices(tetmesh0, tetmesh1, tetmesh, proxThreshold.getValue());
                    std::cout << "Meshes merge at coincident vertices ... COMPLETED." << std::endl;

                    std::cout << std::endl;
                    std::cout << "P " << tetmesh.num_polys() << std::endl;
                    std::cout << "F " << tetmesh.num_faces() << std::endl;
                    std::cout << "E "<< tetmesh.num_edges() << std::endl;
                    std::cout << "V "<< tetmesh.num_verts() << std::endl;

                    if(ext_vol == ".vtk" || ext_vol == ".VTK")
                    {
                        if(save_vtk(out_mesh, tetmesh.vector_verts(), volume_cells_from_poly_verts(tetmesh)) != 0)
                        {
                            std::cerr << "ERROR while writing vtk file: " << out_mesh << std::endl;
                            exit(1);
                        }
                    }
                    else
                    {
                        tetmesh.save(out_mesh.c_str());
                    }
                }
                std::cout << "\033[0;32mSaving mesh file in : " << out_mesh << " ... COMPLETED.\033[0m" << std::endl;
            }
            else
            {
                std::cerr << "ERROR: Meshes format are different!" << std::endl;
                exit(1);
            }
        }
        else
        {
            std::cerr << "ERROR: Mesh format is not supported." << std::endl;
            exit(1);
        }
    }




    if(extractMeshes.isSet() && meshFiles.getValue().size() == 1)
    {
        std::vector<std::string> excommands;
        excommands.push_back(command);

        std::vector<std::string> deps;

        std::vector<std::string> files = meshFiles.getValue();

        if(gridFlag.isSet())
        {
            MUSE::SurfaceMeta geometa;
            geometa.setProject(Project);
            geometa.setCommands(excommands);

            std::cout << "### Load grid: " << files.at(0) << std::endl;
            MUSE::Quadmesh<> quadmesh;
            quadmesh.load(files.at(0).c_str());
            deps.push_back(filesystem::relative(get_basename(files.at(0)) + ".json", Project.folder));

            std::string name = get_basename(files.at(0));

            std::cout << "### Load mesh for extracting boundary: " << setBoundary.getValue() << std::endl;
            cinolib::Trimesh<> mesh_bound;
            mesh_bound.load(setBoundary.getValue().c_str());
            deps.push_back(filesystem::relative(get_basename(setBoundary.getValue()) + ".json", Project.folder));

            std::string bound_name = setBoundary.getValue().substr(setBoundary.getValue().find_last_of("/")+1, setBoundary.getValue().length());
            bound_name = get_basename(bound_name);

            //split mesh
            std::vector<Point2D> bound_2d;
            for(uint i: mesh_bound.get_ordered_boundary_vertices())
            {
                Point2D p;
                p.x = mesh_bound.vert(i).x();
                p.y = mesh_bound.vert(i).y();
                bound_2d.push_back(p);
            }

            MUSE::Quadmesh<> sub_quadmesh;
            std::map<cinolib::vec3d, uint> verts;

            for(uint pid=0; pid <quadmesh.num_polys(); pid++)
            {
                cinolib::vec3d centr = quadmesh.poly_centroid(pid);

                Point2D c;
                c.x = centr.x();
                c.y = centr.y();

                if(point_in_polygon(c, bound_2d))
                {
                    cinolib::vec3d v0_pos = quadmesh.poly_vert(pid, 0);
                    cinolib::vec3d v1_pos = quadmesh.poly_vert(pid, 1);
                    cinolib::vec3d v2_pos = quadmesh.poly_vert(pid, 2);
                    cinolib::vec3d v3_pos = quadmesh.poly_vert(pid, 3);

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
                        v0_id = sub_quadmesh.vert_add(v0_pos);
                        verts.insert(std::pair<cinolib::vec3d,uint> (v0_pos, v0_id));
                    }
                    else
                        v0_id = v0_it->second;

                    if (v1_it == verts.end())
                    {
                        v1_id = sub_quadmesh.vert_add(v1_pos);
                        verts.insert(std::pair<cinolib::vec3d,uint> (v1_pos, v1_id));
                    }
                    else
                        v1_id = v1_it->second;

                    if (v2_it == verts.end())
                    {
                        v2_id = sub_quadmesh.vert_add(v2_pos);
                        verts.insert(std::pair<cinolib::vec3d,uint> (v2_pos, v2_id));
                    }
                    else
                        v2_id = v2_it->second;

                    if (v3_it == verts.end())
                    {
                        v3_id = sub_quadmesh.vert_add(v3_pos);
                        verts.insert(std::pair<cinolib::vec3d,uint> (v3_pos, v3_id));
                    }
                    else
                        v3_id = v3_it->second;

                    std::vector<uint> vlist;
                    vlist.push_back(v0_id);
                    vlist.push_back(v1_id);
                    vlist.push_back(v2_id);
                    vlist.push_back(v3_id);

                    sub_quadmesh.poly_add(vlist);
                }
            }
            //std::cout << cinolib::connected_components(sub_quadmesh) << std::endl;
            if(cleanPoly.isSet())
                sub_quadmesh.remove_isolate_poly();

            std::string out_mesh = name + "_" + bound_name + ext_surf;
            sub_quadmesh.save(out_mesh.c_str());

            std::cout << std::endl;
            std::cout << "Saving quadmesh: " << out_mesh << std::endl;

            MUSE::Surface summary;
            MUSE::Surface::Parameters par;
            par.resx = setResx.getValue();
            par.resy = setResy.getValue();
            summary.setParameters(par);
            summary.setSummary(quadmesh);
            geometa.setMeshSummary(summary);
            geometa.setDependencies(deps);

            geometa.write(name + "_" + bound_name + ".json");
        }

        if(hexFlag.isSet())
        {
            MUSE::VolumeMeta geometa;
            geometa.setProject(Project);
            geometa.setCommands(excommands);

            std::cout << "### Load grid: " << files.at(0) << std::endl;
            MUSE::Hexmesh<> hexmesh;
            hexmesh.load(files.at(0).c_str());
            deps.push_back(filesystem::relative(get_basename(files.at(0)) + ".json", Project.folder));

            std::string name = files.at(0).substr(files.at(0).find_last_of("/")+1, files.at(0).length());
            name = get_basename(name);

            //std::string ext0 = get_extension(files.at(0));

            std::cout << "### Load mesh for extracting boundary: " << setBoundary.getValue() << std::endl;
            cinolib::Trimesh<> mesh_bound;
            mesh_bound.load(setBoundary.getValue().c_str());
            deps.push_back(filesystem::relative(get_basename(setBoundary.getValue()) + ".json", Project.folder));

            std::string bound_name = setBoundary.getValue().substr(setBoundary.getValue().find_last_of("/")+1, setBoundary.getValue().length());
            bound_name = get_basename(bound_name);

            MUSE::Hexmesh<> sub_hexmesh;
            sub_hexmesh.subHexmesh_from_trimesh(hexmesh, mesh_bound);

            MUSE::Volume summary;
            MUSE::Volume::Parameters par;
            par.resx = setResx.getValue();
            par.resy = setResy.getValue();
            par.resz = setResz.getValue();
            summary.setParameters(par);
            summary.setSummary(sub_hexmesh);
            geometa.setMeshSummary(summary);
            geometa.setDependencies(deps);

            geometa.write(out_volume + "/" + name + "_" + bound_name + ".json");

            std::string out_mesh = out_volume +"/" + name + "_" + bound_name + ext_vol;
            if(ext_vol == ".vtk" || ext_vol == ".VTK")
            {
                if(save_vtk(out_mesh, sub_hexmesh.vector_verts(), volume_cells_from_poly_verts(sub_hexmesh)) != 0)
                {
                    std::cerr << "ERROR while writing vtk file: " << out_mesh << std::endl;
                    exit(1);
                }
            }
            else
            {
                sub_hexmesh.save(out_mesh.c_str());
            }

            std::cout << std::endl;
            std::cout << "Saving hexmesh: " << out_mesh << std::endl;
        }
        std::cout << FGRN("Extracting sub-mesh constrained to boundary ... COMPLETED.") << std::endl;
    }




    if(loadSurface.isSet() && setRemeshing.isSet())
    {
        if(!meshFiles.isSet())
        {
            std::cout << FRED("ERROR. Set a mesh (surface/volume) by -m command") << std::endl;
            exit(1);
        }

        if(meshFiles.getValue().size() >= 2)
        {
            std::cout << FRED("ERROR. Only a mesh (surface/volume) is supported.") << std::endl;
            exit(1);
        }

        std::cout << "############################" << std::endl;
        std::cout << "### REMESHING ALGORITHM" << std::endl;
        std::cout << "### Reference: M.Botsch, L.Kobbelt, A Remeshing Approach to Multiresolution Modeling." << std::endl;
        std::cout << "### Remeshing ONLY accepts triangular meshes as input." << std::endl;
        std::cout << "############################" << std::endl;
        std::cout << std::endl;

        std::string filename_mesh = meshFiles.getValue().at(0);

        MUSE::SurfaceMeta geometa;
        geometa.setProject(Project);

        std::vector<std::string> excommands;
        excommands.push_back(command);
        geometa.setCommands(excommands);

        std::vector<std::string> deps;
        deps.push_back(filesystem::relative(get_basename(filename_mesh) + ".json", Project.folder));
        geometa.setDependencies(deps);

        cinolib::Trimesh<> mesh;
        mesh.load(filename_mesh.c_str());
        std::cout << "\033[0;32mLoading mesh file: " << filename_mesh << " ... COMPLETED.\033[0m" << std::endl;
        std::cout << std::endl;


        MUSE::Surface surf;
        MUSE::Surface::Parameters surf_par;

        if(setRotAxis.isSet())
        {
            double rad = (setRotAngle.getValue() * M_PI)/180;
            cinolib::vec3d axis = set_rotation_axis(setRotAxis.getValue());

            cinolib::mat3d R = cinolib::mat3d::ROT_3D(axis, rad);
            cinolib::vec3d rotcenter {setRotCenterX.getValue(), setRotCenterY.getValue(), setRotCenterZ.getValue()};

            for(uint vid=0; vid<mesh.num_verts(); vid++)
            {
                mesh.vert(vid) -= rotcenter;
                mesh.vert(vid) = R*mesh.vert(vid);
                mesh.vert(vid) += rotcenter;
            }
        }

        if(setMarkedEdge.isSet())
        {
            mesh.edge_mark_boundaries();
            std::cout << "### Remeshing with marked boundary edges." << std::endl;
        }

        std::cout << "### Remeshing fixed on: mean edge." << std::endl;
        remesh_Botsch_Kobbelt_2004(mesh, -1, setMarkedEdge.getValue());

        std::cout << "Remeshing ... " << mesh.num_verts() << "V / " << mesh.num_edges() << "E / " << mesh.num_polys() << "P" << std::endl;

        surf.setParameters(surf_par);
        surf.setSummary(mesh);
        geometa.setMeshSummary(surf);

        std::string out_mesh = out_surf + "/"+ get_basename(get_filename(filename_mesh)) + "_rem";
        mesh.save((out_mesh + ext_surf).c_str());
        std::cout << "\033[0;32mSaving mesh file: " << out_mesh + ext_surf << "\033[0m" << std::endl;

        if(setRotAxis.isSet())
        {
            out_mesh += "_rot";
            mesh.save((out_mesh + ext_surf).c_str());
            std::cout << "\033[0;32mSaving mesh file: " << out_mesh + ext_surf << "\033[0m" << std::endl;
        }

        geometa.write(out_mesh + ".json");
    }



    //Lettura vettore di superfici
    //Questo comando permette la lettura di superfici di origine esterna e la creazione del file json corrispondente, secondo la MUSE encoding
    //Questo comando ha anche la possibilità di effettuare infittimento mediante split su centroide o punto medio edge
    //Può prendere in input più superfici (-m <filename> -m filename ...)
    if(loadSurface.isSet() && !setRemeshing.isSet())
    {
        if(!meshFiles.isSet())
        {
            std::cout << FRED("ERROR. Set a mesh (surface/volume) by -m command") << std::endl;
            exit(1);
        }

        if(meshFiles.getValue().size() >= 2)
        {
            std::cout << FRED("ERROR. Only a mesh (surface/volume) is supported.") << std::endl;
            exit(1);
        }

        std::string filename_mesh = meshFiles.getValue().at(0);

        MUSE::SurfaceMeta geometa;
        geometa.setProject(Project);

        std::vector<std::string> excommands;
        excommands.push_back(command);
        geometa.setCommands(excommands);

        if(splitMethod.isSet())
        {
            std::vector<std::string> deps;
            deps.push_back(filesystem::relative(get_basename(filename_mesh) + ".json", Project.folder));

            //            if(filename_mesh.find("geometry/") != std::string::npos)
            //                deps.push_back(get_basename(filename_mesh.substr(filename_mesh.find("geometry/"))) + ".json");
            geometa.setDependencies(deps);
        }

        MUSE::SurfaceMesh<>mesh;
        mesh.load(filename_mesh.c_str());
        std::cout << "\033[0;32mLoading mesh file: " << filename_mesh << " ... COMPLETED.\033[0m" << std::endl;

        MeshType type = mesh.set_meshtype();
        std::cout << "Check mesh type ... " << std::endl;
        std::cout << "Number of verts per poly: " << mesh.verts_per_poly(0) <<  std::endl;

        MUSE::Surface surf;
        MUSE::Surface::Parameters surf_par; 

        if(setRotAxis.isSet())
        {
            double rad = (setRotAngle.getValue() * M_PI)/180;
            cinolib::vec3d axis = set_rotation_axis(setRotAxis.getValue());

            cinolib::mat3d R = cinolib::mat3d::ROT_3D(axis, rad);
            cinolib::vec3d rotcenter {setRotCenterX.getValue(), setRotCenterY.getValue(), setRotCenterZ.getValue()};

            for(uint vid=0; vid<mesh.num_verts(); vid++)
            {
                mesh.vert(vid) -= rotcenter;
                mesh.vert(vid) = R*mesh.vert(vid);
                mesh.vert(vid) += rotcenter;
            }
        }


        if(type == MeshType::TRIMESH)
        {
            std::cout << "Mesh type: TRIMESH" <<  std::endl;
            std::cout << std::endl;

            surf_par.type = "TRIMESH";

            if(splitMethod.isSet())
            {
                if(splitMethod.getValue().compare("CENTROID") == 0)
                {
                    std::cout << "Management of new degree of resolution by poly split at centroid ..." << std::endl;
                    mesh.triangles_split_on_centroid();
                }
                else if(splitMethod.getValue().compare("EDGE") == 0)
                {
                    std::cout << "Management of new degree of resolution by poly split at edges middle point ..." << std::endl;
                    mesh.triangles_split_on_edge();
                }
                else if(splitMethod.getValue().compare("BEDGE") == 0)
                {
                    std::cout << "Management of new degree of resolution by poly split at edges middle point ..." << std::endl;
                    double inedge_avg = 0.0;
                    int n_inedge = 0;
                    for(uint eid=0; eid < mesh.num_edges(); eid++)
                    {
                        if(!mesh.edge_is_boundary(eid))
                        {
                            n_inedge++;
                            inedge_avg += mesh.edge_length(eid);
                        }
                    }
                    inedge_avg = inedge_avg/n_inedge;
                    std::cout << "### (Internal) edge average lenght: " << inedge_avg << std::endl;
                    std::cout << "### Edge average lenght: " << mesh.edge_avg_length() << std::endl;

                    for(uint eid=0; eid < mesh.num_edges(); eid++)
                    {
                        if(mesh.edge_is_boundary(eid))
                        {
                            if(mesh.edge_length(eid) > inedge_avg)
                            {
                                std::cout << "### Edge ID: " << eid << " - Edge lenght: " << mesh.edge_length(eid) << std::endl;
                                for(int iter=0; iter < setIterations.getValue(); iter++)
                                {
                                    std::cout << "### Start split edge ... iteration: " << iter << std::endl;

                                    std::vector<uint> pid_eid = mesh.adj_e2p(eid);
                                    if(pid_eid.size() > 1)
                                        exit(1);

                                    std::cout << "### Pid adj edge: " << pid_eid.size() << std::endl;

                                    cinolib::vec3d v0 = mesh.edge_vert(eid, 0);
                                    cinolib::vec3d v1 = mesh.edge_vert(eid, 1);
                                    cinolib::vec3d delta = (v1-v0)/2;

                                    cinolib::vec3d v_med (v0.x()+delta.x(), v0.y()+delta.y(), v0.z()+delta.z());
                                    mesh.vert_add(v_med);

                                    // uint n_polys = this->num_polys();
                                    // for(uint pid=0; pid < n_polys; pid++)
                                    // {

                                    // uint vid0 = mesh.poly_vert_id(pid_eid, 0);
                                    // uint vid1 = mesh.poly_vert_id(pid_eid, 1);
                                    // uint vid2 = mesh.poly_vert_id(pid_eid, 2);

                                    // std::vector<uint> pid_adj_edge = mesh.adj_p2e(pid);

                                    // uint new0 = pid_adj_edge.at(0) + n_verts;
                                    // uint new1 = pid_adj_edge.at(1) + n_verts;
                                    // uint new2 = pid_adj_edge.at(2) + n_verts;

                                    //     std::vector<uint> list0 {vid0, new0, new2};
                                    //     std::vector<uint> list1 {new0, vid1, new1};
                                    //     std::vector<uint> list2 {new2, new1, vid2};
                                    //     std::vector<uint> list3 {new0, new1, new2};

                                    //     uint new_pid;
                                    //     new_pid = this->poly_add(list0);
                                    //     this->poly_data(new_pid) = this->poly_data(pid);

                                    //     new_pid = this->poly_add(list1);
                                    //     this->poly_data(new_pid) = this->poly_data(pid);

                                    //     new_pid = this->poly_add(list2);
                                    //     this->poly_data(new_pid) = this->poly_data(pid);

                                    //     new_pid = this->poly_add(list3);
                                    //     this->poly_data(new_pid) = this->poly_data(pid);

                                    //     this->poly_remove(pid);
                                    // }
                                }
                            }
                        }
                    }
                }
                else
                {
                    std::cout << FRED("ERROR. Split method: ") << splitMethod.getValue()  << FRED(" is not supported.") << std::endl;
                    exit(1);
                }
            }

            if(boundaryExtract.isSet())
            {
                std::vector<Point3D> vec_bv;
                for(uint i:mesh.get_ordered_boundary_vertices())
                {
                    Point3D bv;
                    bv.x = mesh.vert(i).x();
                    bv.y = mesh.vert(i).y();
                    bv.z = mesh.vert(i).z();
                    vec_bv.push_back(bv);
                }
                export3d_xyz(out_surf + "/"+ get_basename(get_filename(filename_mesh)) + "_BP.xyz", vec_bv);
            }

            // if(setEdgeCollpase.isSet())
            // {
            //     for(uint eid=0; eid < mesh.num_edges(); eid++)
            //     {
            //         if(mesh.edge_is_boundary(eid))
            //         {
            //             mesh.edge_remove()
            //         }
            //     }
            // }
        }
        else if(type == MeshType::QUADMESH)
        {
            std::cout << "Mesh type: QUADMESH" <<  std::endl;
            std::cout << std::endl;

            surf_par.type = "QUADMESH";

            if(splitMethod.isSet())
            {
                std::cout << "### For quads mesh: poly split on edge/centroid corresponds." << std::endl;
                if(splitMethod.getValue().compare("EDGE") == 0 || splitMethod.getValue().compare("CENTROID") == 0)
                {
                    std::cout << "Management of new degree of resolution by poly split ..." << std::endl;
                    mesh.quads_split_on_edge();
                }
                else
                {
                    std::cout << FRED("ERROR. Split method: ") << splitMethod.getValue()  << FRED(" is not supported.") << std::endl;
                    exit(1);
                }
            }
        }
        else
        {
            std::cout << FRED("ERROR. Only triangle/quadrilateral meshes are supported!") << std::endl;
            exit(1);
        }

        surf.setParameters(surf_par);
        surf.setSummary(mesh);
        geometa.setMeshSummary(surf);

        std::string out_mesh = out_surf + "/"+ get_basename(get_filename(filename_mesh));

        //AGGIUNGERE LA CONDIZIONE PER LA TRASLAZIONE
        if(setTranslate.isSet())
        {
            std::vector<std::string> point_to_translate_string = split_string(setTranslate.getValue(), ',');
            if(point_to_translate_string.size() > 3)
            {
                std::cout << FRED("ERROR. Set translation point with 3 coordinates (x,y,z) separated by comma.") << std::endl;
                exit(1);
            }
                
            out_mesh += "_tr";
            std::cout << "=== Translate mesh at point: ( " << setTranslate.getValue() << " )" << std::endl;
            
            cinolib::vec3d point_to_translate;
            point_to_translate.x() = std::stod(point_to_translate_string.at(0));
            point_to_translate.y() = std::stod(point_to_translate_string.at(1));
            point_to_translate.z() = std::stod(point_to_translate_string.at(2));

            mesh.translate(point_to_translate);
            mesh.save((out_mesh + ext_surf).c_str());
            std::cout << "\033[0;32mSaving mesh file: " << out_mesh + ext_surf << "\033[0m" << std::endl;
        }

        if(splitMethod.isSet())
        {
            out_mesh += "_res";
            mesh.save((out_mesh + ext_surf).c_str());
            std::cout << "\033[0;32mSaving mesh file: " << out_mesh + ext_surf << "\033[0m" << std::endl;
        }

        if(setRotAxis.isSet())
        {
            out_mesh += "_rot";
            mesh.save((out_mesh + ext_surf).c_str());
            std::cout << "\033[0;32mSaving mesh file: " << out_mesh + ext_surf << "\033[0m" << std::endl;
        }

        if(setScaleMesh.isSet())
        {
            mesh.scale(setScaleFactorX, setScaleFactorY, setScaleFactorZ);

            out_mesh += "_scale";
            mesh.save((out_mesh + ext_surf).c_str());
            std::cout << "\033[0;32mSaving mesh file: " << out_mesh + ext_surf << "\033[0m" << std::endl;
        }

        std::cout << "======================================" << std::endl;
        std::cout << "=== BBOX - min_x:" << std::setprecision(10) << mesh.bbox().min.x() << "; - min_y:" << mesh.bbox().min.y() <<  std::endl;
        std::cout << "=== BBOX - max_x:" << std::setprecision(10) << mesh.bbox().max.x() << "; - min_y " << mesh.bbox().min.y() <<  std::endl;
        std::cout << "=== BBOX - max_x:" << std::setprecision(10) << mesh.bbox().max.x() << "; - max_y " << mesh.bbox().max.y() <<  std::endl;
        std::cout << "=== BBOX - min_x:" << std::setprecision(10) << mesh.bbox().min.x() << "; - max_y " << mesh.bbox().max.y() <<  std::endl;

        std::cout << "=== BBOX - diag: " << std::setprecision(10) << mesh.bbox().diag() << std::endl;
        std::cout << "======================================" << std::endl;
        geometa.write(out_mesh + ".json");
    }




    //Comando per estrarre le celle della mesh dove ricadono dei punti (dati in input)
    if(loadSurface.isSet() && setScalarField.isSet())
    {
        //1) Caricamento mesh dalla quale estrarre la configurazione dei centroidi (per poi simulare il campionamento)
        if(!meshFiles.isSet())
        {
            std::cerr << FRED("ERROR. Set a mesh (surface/volume) by -m command") << std::endl;
            exit(1);
        }
        if(meshFiles.getValue().size() >= 2)
        {
            std::cerr << FRED("ERROR. Only a mesh (surface/volume) is supported.") << std::endl;
            exit(1);
        }


        //0) Definizione file di output
        std::ofstream file_out;
        std::string filename = out_surf + "/points_in_mesh.csv";
        file_out.open(filename, std::fstream::out);
        if(!file_out.is_open())
        {
            std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
            exit(1);
        }

        //1) Apertura mesh
        MUSE::SurfaceMesh<> mesh;
        mesh.load(meshFiles.getValue().at(0).c_str());
        std::cout << "\033[0;32mLoading mesh file: " << meshFiles.getValue().at(0) << " ... COMPLETED.\033[0m" << std::endl;
        MeshType type = mesh.set_meshtype();
        std::cout << "Check mesh type ... " << std::endl;
        std::cout << "Number of verts per poly: " << mesh.verts_per_poly(0) <<  std::endl;


        //2) Caricamento point cloud
        std::vector<Point3D> values;
        load_xyzfile(setScalarField.getValue(), values);
        // for(uint pid=0; pid <6; pid++)
        // {
        //     cinolib::vec3d centr = mesh.poly_centroid(pid);
        //     Point3D p;
        //     p.x = centr.x();
        //     p.y = centr.y();
        //     p.z = centr.z();
        //     values.push_back(p);
        //     std::cout << centr << std::endl;
        // }

        std::vector<uint> id_polys;
        if(type == MeshType::TRIMESH)
        {
            for(uint i=0; i<values.size(); i++)
            {
                cinolib::vec3d p (values.at(i).x, values.at(i).y, values.at(i).z);

                for(uint pid=0; pid<mesh.num_polys(); pid++)
                {
                    cinolib::vec3d v1 = mesh.poly_vert(pid, 0);
                    cinolib::vec3d v2 = mesh.poly_vert(pid, 1);
                    cinolib::vec3d v3 = mesh.poly_vert(pid, 2);

                    cinolib::vec2d p2d(p.x(), p.y());

                    cinolib::vec2d v12d(v1.x(), v1.y());
                    cinolib::vec2d v22d(v2.x(), v2.y());
                    cinolib::vec2d v32d(v3.x(), v3.y());

                    if (cinolib::point_in_triangle_2d(p2d, v12d, v22d, v32d)!=0) //se trovo la proiezione
                        id_polys.push_back(pid);
                }
            }
            std::sort(id_polys.begin(), id_polys.end());
            id_polys.erase( std::unique( id_polys.begin(), id_polys.end() ), id_polys.end() );
        }

        for(uint id:id_polys)
            file_out << id << std::endl;
        file_out.close();
        std::cout << "Saving id polys ... COMPLETED." << std::endl;
    }



    //Lettura vettore di superfici
    if(loadVolume.isSet())
    {
        if(!filesystem::exists(out_volume))
            filesystem::create_directory(out_volume);

        std::vector<std::string> files = meshFiles.getValue();

        for(size_t i=0; i< files.size(); i++)
        {
            std::string filename_mesh = files.at(i);

            MUSE::VolumeMeta geometa;
            geometa.setProject(Project);

            std::vector<std::string> excommands;
            excommands.push_back(command);
            geometa.setCommands(excommands);

            if(splitMethod.isSet())
            {
                std::vector<std::string> deps;
                deps.push_back(filesystem::relative(get_basename(filename_mesh) + ".json", Project.folder));

//                if(filename_mesh.find("geometry/") != std::string::npos)
//                    deps.push_back(get_basename(filename_mesh.substr(filename_mesh.find("geometry/"))) + ".json");
                geometa.setDependencies(deps);
            }


            MUSE::VolumeMesh<>mesh;
            mesh.load(filename_mesh.c_str());
            std::cout << "\033[0;32mLoading mesh file: " << filename_mesh << " ... COMPLETED.\033[0m" << std::endl;

            MeshType type = mesh.set_meshtype();
            std::cout << "### Check mesh type ... " << std::endl;
            std::cout << "### Number of verts per poly: " << mesh.verts_per_poly(0) <<  std::endl;

            MUSE::Volume vol;
            MUSE::Volume::Parameters vol_par;

            if(setRotAxis.isSet())
            {
                double rad = (setRotAngle.getValue() * M_PI)/180;
                cinolib::vec3d axis = set_rotation_axis(setRotAxis.getValue());

                cinolib::mat3d R = cinolib::mat3d::ROT_3D(axis, rad);
                cinolib::vec3d rotcenter {setRotCenterX.getValue(), setRotCenterY.getValue(), setRotCenterZ.getValue()};

                for(uint vid=0; vid<mesh.num_verts(); vid++)
                {
                    mesh.vert(vid) -= rotcenter;
                    mesh.vert(vid) = R*mesh.vert(vid);
                    mesh.vert(vid) += rotcenter;
                }
            }

            if(type == MeshType::TETMESH)
            {
                std::cout << "Mesh type: TETMESH" <<  std::endl;
                std::cout << std::endl;

                vol_par.type = "TETMESH";

                std::cout << "X = " << std::setprecision(10) << mesh.bbox().min.x() << "; " << mesh.bbox().max.x() << std::endl;
                std::cout << "Y = " << std::setprecision(10) << mesh.bbox().min.y() << "; " << mesh.bbox().max.y() << std::endl;
                std::cout << "Z = " << std::setprecision(10) << mesh.bbox().min.z() << "; " << mesh.bbox().max.z() << std::endl;

                if(splitMethod.isSet())
                {
                    std::cout << FRED("TO BE IMPLEMENTED ...") << std::endl;
                    exit(1);
                }
            }
            else if(type == MeshType::HEXMESH)
            {
                std::cout << "Mesh type: HEXMESH" <<  std::endl;
                std::cout << std::endl;

                vol_par.type = "HEXMESH";

                if(splitMethod.isSet())
                {
                    std::cout << FRED("TO BE IMPLEMENTED ...") << std::endl;
                    exit(1);
                }
            }
            else
            {
                std::cout << FRED("ERROR. Only tetrahedral/hexahedral meshes are supported!") << std::endl;
                exit(1);
            }

            std::cout << "\033[0;32mLoading mesh file: " << filename_mesh << " ... COMPLETED.\033[0m" << std::endl;

            vol.setParameters(vol_par);
            vol.setSummary(mesh);
            geometa.setMeshSummary(vol); 

            std::string out_mesh = out_volume + "/"+ get_basename(get_filename(filename_mesh));
            geometa.write(out_mesh + ".json");

            if(splitMethod.isSet())
            {
                out_mesh += "_res";
                mesh.save((out_mesh + ext_vol).c_str(), type);
                geometa.write(out_mesh + ".json");
                std::cout << "\033[0;32mSaving mesh file: " << out_mesh + ext_vol << "\033[0m" << std::endl;
            }

            if(setRotAxis.isSet())
            {
                out_mesh += "_rot";
                mesh.save((out_mesh + ext_vol).c_str(), type);
                geometa.write(out_mesh + ".json");
                std::cout << "\033[0;32mSaving mesh file: " << out_mesh + ext_vol << "\033[0m" << std::endl;
            }

            if(setScaleMesh.isSet())
            {
                mesh.scale(setScaleFactorX, setScaleFactorY, setScaleFactorZ);
                out_mesh += "_scale";
                mesh.save((out_mesh + ext_vol).c_str(), type);
                geometa.write(out_mesh + ".json");
                std::cout << "\033[0;32mSaving mesh file: " << out_mesh + ext_vol << "\033[0m" << std::endl;
            }

            if(extractSurface.isSet())
            {
                std::cout << "Extract surface mesh from volume ... " << std::endl;
                std::string out_surfmesh = out_surf + "/" + get_basename(get_filename(filename_mesh));
                MUSE::SurfaceMesh<> surf_mesh;
                export_surface(mesh, surf_mesh);
                surf_mesh.save((out_surfmesh + ext_surf).c_str());
                std::cout << "\033[0;32mSaving mesh file: " << out_surfmesh + ext_surf << "\033[0m" << std::endl;
            }
        }
    }


    if(createScalarField.isSet() && meshFiles.isSet())
    {
        if(meshFiles.getValue().size() > 1)
            std::cerr << "ERROR: For scalar field (-F), one mesh file is accepted by command -m!" << std::endl;


        //0) Definizione file di output: scalar_field
        std::ofstream file_out;
        std::string filename = projectFolder.getValue() + "/out/realtime" + "/scalar_field.csv";
        file_out.open(filename, std::fstream::out);
        if(!file_out.is_open())
        {
            std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
            exit(1);
        }

        //1) Caricamento mesh dalla quale estrarre la configurazione dei centroidi (per poi simulare il campionamento)
        std::cout << "Loading ... " << meshFiles.getValue().at(0) << std::endl;
        cinolib::Polygonmesh<> mesh (meshFiles.getValue().at(0).c_str());
        std::cout << "Loading mesh to extract centroids configuration ... COMPLETED." << std::endl;
        std::cout << std::endl;

        //2) Caricamento valori reali di campionamento (dal quale estrarre i valori da associare ai singoli centroidi) - VALORI: i valori sono associati ad ogni vertice della (3) mesh
        std::cout << "Loading ... " << setSamplesValues.getValue() << std::endl;
        std::vector<double> values;
        std::ifstream s_filev;
        s_filev.open(setSamplesValues.getValue());

        if (!s_filev.is_open())
        {
            std::cerr << "Error opening Scalar field " << setSamplesValues.getValue() << std::endl;
            exit(1);
        }

        double valv;
        while (s_filev >> valv)
            values.push_back(valv);
        std::cout << "Loading values ... COMPLETED." << std::endl;
        std::cout << std::endl;

        //3) Caricamento valori reali di campionamento (dal quale estrarre i valori da associare ai singoli centroidi) - MESH
        std::cout << "Loading ... " << setSamplesMesh.getValue() << std::endl;
        cinolib::Polygonmesh<> mesh_campioni (setSamplesMesh.getValue().c_str());
        std::cout << "Loading mesh ... COMPLETED." << std::endl;

        //cinolib::Octree octree_campioni;
        //octree_campioni.build_from_mesh_polys(mesh_campioni);


        std::vector<double> scalfi;
        for(uint pid=0; pid<mesh.num_polys(); pid++)
        {
            cinolib::vec3d p = mesh.poly_centroid(pid);

            /*cinolib::vec3d cp = octree_campioni.closest_point(p); //cerca il punto più vicino nell'octree (mesh)
            uint i_cp = mesh_campioni.pick_vert(cp);

            std::vector<bool> visited (mesh_campioni.num_polys(), false);
            std::queue<uint> queue;
            queue.push(mesh_campioni.adj_v2p(i_cp).at(0));

            while (!queue.empty())
            {
                uint curr = queue.front();
                //std::cout << curr << std::endl;
                queue.pop();

                visited.at(curr) = true;

                cinolib::vec3d v1 = mesh_campioni.poly_vert(curr, 0);
                cinolib::vec3d v2 = mesh_campioni.poly_vert(curr, 1);
                cinolib::vec3d v3 = mesh_campioni.poly_vert(curr, 2);

                std::vector<uint> vecid;
                vecid.push_back(mesh_campioni.poly_vert_id(curr, 0));
                vecid.push_back(mesh_campioni.poly_vert_id(curr, 1));
                vecid.push_back(mesh_campioni.poly_vert_id(curr, 2));

                cinolib::vec2d p2d(p.x(), p.y());

                cinolib::vec2d v12d(v1.x(), v1.y());
                cinolib::vec2d v22d(v2.x(), v2.y());
                cinolib::vec2d v32d(v3.x(), v3.y());

                if (cinolib::point_in_triangle_2d(p2d, v12d, v22d, v32d)!=0) //se trovo la proiezione
                {
                    double dmin = DBL_MAX;
                    int idmin = 0;

                    std::vector<double> vecd;
                    vecd.push_back(p2d.dist(v12d));
                    vecd.push_back(p2d.dist(v22d));
                    vecd.push_back(p2d.dist(v32d));

                    for(uint d=0; d<vecd.size(); d++)
                    {
                        if(vecd.at(d) < dmin)
                        {
                            dmin = vecd.at(d);
                            idmin = vecid.at(d);
                        }
                    }
                    scalfi.push_back(values.at(idmin));
                }

                for (uint pid : mesh_campioni.adj_p2p(curr))
                    if (!visited.at(pid))
                        queue.push(pid);

            }*/

            for(uint pid2=0; pid2<mesh_campioni.num_polys(); pid2++)
            {
                cinolib::vec3d v1 = mesh_campioni.poly_vert(pid2, 0);
                cinolib::vec3d v2 = mesh_campioni.poly_vert(pid2, 1);
                cinolib::vec3d v3 = mesh_campioni.poly_vert(pid2, 2);

                std::vector<uint> vecid;
                vecid.push_back(mesh_campioni.poly_vert_id(pid2, 0));
                vecid.push_back(mesh_campioni.poly_vert_id(pid2, 1));
                vecid.push_back(mesh_campioni.poly_vert_id(pid2, 2));

                cinolib::vec2d p2d(p.x(), p.y());

                cinolib::vec2d v12d(v1.x(), v1.y());
                cinolib::vec2d v22d(v2.x(), v2.y());
                cinolib::vec2d v32d(v3.x(), v3.y());

                if (cinolib::point_in_triangle_2d(p2d, v12d, v22d, v32d)!=0) //se trovo la proiezione
                {
                    double dmin = DBL_MAX;
                    int idmin = 0;

                    std::vector<double> vecd;
                    vecd.push_back(p2d.dist(v12d));
                    vecd.push_back(p2d.dist(v22d));
                    vecd.push_back(p2d.dist(v32d));

                    for(uint d=0; d<vecd.size(); d++)
                    {
                        if(vecd.at(d) < dmin)
                        {
                            dmin = vecd.at(d);
                            idmin = vecid.at(d);
                        }
                    }
                    scalfi.push_back(values.at(idmin));
                }
            }

            //std::cout << mesh.poly_centroid(pid) << "; " << scalfi.at(pid) << std::endl;
            file_out << scalfi.at(pid) << std::endl;
        }

        file_out.close();

        std::cout << std::endl;
        std::cout << "Creating scalar field associated to mesh ... " << meshFiles.getValue().at(0) << std::endl;
        std::cout << "Saving scalar field in " << filename << std::endl;
    }

    //Comando per ordinare un campo scalare secondo l'ordine delle celle della mesh (vedi esempio: PRL - Liguria GRID per fase di controllo risultati)
    if(restoreScalarField.isSet() && meshFiles.isSet())
    {
        if(meshFiles.getValue().size() > 1)
        {
            std::cerr << "ERROR: For scalar field, one mesh file is accepted by command -m!" << std::endl;
            exit(1);
        }

        //1) Caricamento mesh dalla quale estrarre la configurazione dei centroidi (per poi simulare il campionamento)
        std::cout << "Loading mesh ... " << meshFiles.getValue().at(0) << std::endl;
        cinolib::Polygonmesh<> mesh (meshFiles.getValue().at(0).c_str());
        std::cout << "Loading mesh to extract centroids configuration ... COMPLETED." << std::endl;
        std::cout << std::endl;

        //2) Caricamento valori reali di campionamento (dal quale estrarre i valori da associare ai singoli centroidi) - VALORI: i valori sono associati ad ogni vertice della (3) mesh
        std::cout << "Loading scalar field (to re-order) ... " << setSamplesValues.getValue() << std::endl;
        std::vector<Point3D> values;
        load_xyzfile(setSamplesValues.getValue(), values);
        std::cout << "Loading values ... COMPLETED." << std::endl;
        std::cout << std::endl;

        if(values.size() != mesh.num_polys())
        {
            std::cerr << "ERROR: number of values: " << values.size() << " is NOT equal to number of polys mesh: " << mesh.num_polys() << std::endl;
            exit(1);
        }

        std::queue<uint> queue_values;
        std::vector<bool> visited (values.size(), false);

        std::vector<double> scalfi;
        for(uint pid=0; pid<mesh.num_polys(); pid++)
        {
            Point2D centr_pid;
            centr_pid.x = mesh.poly_centroid(pid).x();
            centr_pid.y = mesh.poly_centroid(pid).y();

            for(uint id=0; id<values.size(); id++)
                queue_values.push(id);

            while (!queue_values.empty())
            {
                uint curr = queue_values.front();
                queue_values.pop();
                //std::cout << curr << std::endl;
                if(visited.at(curr) == false)
                {
                    Point2D point;
                    point.x = values.at(curr).x;
                    point.y = values.at(curr).y;

                    if(dist(point, centr_pid) <= 1e-6)
                    {
                        scalfi.push_back(values.at(curr).z);
                        visited.at(curr) = true;
                        continue;
                    }
                }
            }
        }
        export1d_xyz(get_basename(setSamplesValues.getValue()) + "_tomesh.csv", scalfi);

        std::cout << std::endl;
        std::cout << "Creating scalar field associated to mesh ... " << meshFiles.getValue().at(0) << std::endl;
        //std::cout << "Saving scalar field in " << filename << std::endl;
    }







    if(setMultiResolution.isSet() && setPolygon.isSet())
    {
        std::cout << "Multi-Resolution command is activate for extracting scalar field according to different resolution meshes ... " << std::endl;
        std::cout << std::endl;

        std::vector<std::vector<Point3D>> boundaries;
        std::vector<std::vector<Point3D>> points;
        std::string geometry_type;

        std::vector<double> values;
        load1d_xyzfile(setScalarField.getValue(), values);
        std::cout << "### Load scalar field: " << setScalarField.getValue() << " ... COMPLETED." << std::endl;

        std::cout << std::endl;

        std::string ext_mesh = get_extension(setRefModel.getValue());
        if(ext_mesh.compare(".off") == 0 || ext_mesh.compare(".obj") == 0)
        {
            //std::cout << "Check on mesh extension: surface." << std::endl;

            MUSE::SurfaceMesh<> mesh;
            mesh.load(setRefModel.getValue().c_str());
            std::cout << "### Load reference mesh (connected to scalar field): " << setRefModel.getValue() << " ... COMPLETED." << std::endl;

            //read shape
            std::cout << "### Loading shapefile: " << std::endl;

            int loadvecfile = load_vectorfile(setPolygon.getValue(), boundaries, points, geometry_type);
            if (loadvecfile != IOSUCCESS)
            {
                std::cerr << FRED("ERROR loading vector file: ") << setPolygon.getValue() << std::endl;
                exit(1);
            }

            if(boundaries.size() == 0)
            {
                std::cerr << FRED("ERROR vector is empty!") << std::endl;
                exit(1);
            }

            std::vector<double> scalar_field;
            scalar_field.resize(boundaries.size(), 0.0);

            for(size_t b=0; b<boundaries.size(); b++)
            {
                //std::cout << "### POLYGON ID: " << b << std::endl;

                std::vector<Point2D> points_b;
                for(size_t p=0; p<boundaries.at(b).size(); p++)
                {
                    Point2D pb;
                    pb.x = boundaries.at(b).at(p).x;
                    pb.y = boundaries.at(b).at(p).y;

                    points_b.push_back(pb);
                }

                int count=0;
                for(uint pid=0; pid<mesh.num_polys(); pid++)
                {
                    Point2D centroid;
                    centroid.x = mesh.poly_centroid(pid).x();
                    centroid.y = mesh.poly_centroid(pid).y();                    

                    if(point_in_polygon(centroid, points_b))
                    {
                        //std::cout << "### TRIANGLE ID: " << pid << " | VALUE: " << values.at(pid) <<  std::endl;

                        count++;
                        scalar_field.at(b)+=values.at(pid);

                        //std::cout << "### MEAN VALUE: " << scalar_field.at(b)/count << std::endl;
                        //std::cout << std::endl;
                    }
                    if(count > 0)
                        scalar_field.at(b) = scalar_field.at(b)/count;
                }

                //std::cout << "### POLYGON ID: " << b << " | VALUE: " << scalar_field.at(b) <<  std::endl;
                //std::cout << std::endl;
            }

            std::string var_name = get_filename(setScalarField.getValue());
            var_name = get_basename(var_name);

            if(setOutFolder.isSet())
            {
                if(!filesystem::exists(Project.getFolder() + "/out/" + setOutFolder.getValue()))
                    filesystem::create_directories(Project.getFolder() + "/out/" + setOutFolder.getValue());
                export1d_xyz (Project.getFolder() + "/out/" + setOutFolder.getValue() + "/" +  var_name + ".csv", scalar_field);
            }
            else
                export1d_xyz (get_path(setScalarField.getValue()) + "/" +  var_name + "_atshape" + ".csv", scalar_field);
        }
        else
        {
            std::cout << FRED("Error: mesh format not supported!") << std::endl;
            exit(1);
        }
    }

    if(setMultiResolution.isSet() && !setPolygon.isSet())
    {
        std::cout << "Multi-Resolution command is activate for extracting scalar field according to different resolution meshes ... " << std::endl;
        std::cout << std::endl;

        std::vector<double> values;
        std::cout << "Load scalar field ... " << std::endl;
        load1d_xyzfile(setScalarField.getValue(), values);

        std::cout << std::endl;
        std::cout << "Load reference mesh connected to scalar field ..." << std::endl;
        std::string ext_mesh = get_extension(setRefModel.getValue());
        if(ext_mesh.compare(".off") == 0 || ext_mesh.compare(".obj") == 0)
        {
            std::cout << "### Check on mesh extension: surface." << std::endl;

            MUSE::SurfaceMesh<> mesh;
            mesh.load(setRefModel.getValue().c_str());

            std::cout << std::endl;
            std::cout << "Load mesh with lower resolution than the reference mesh ... " << std::endl;
            std::vector<std::string> files = meshFiles.getValue();
            for(size_t i=0; i< files.size(); i++)
            {
                std::string ext_mesh = get_extension(files.at(i));

                //Distinguo le mesh surf/vol in base all'estensione
                if(ext_mesh.compare(".mesh") == 0 || ext_mesh.compare(".vtk") == 0)
                {
                    std::cout << "ERROR. Surface mesh required!" << std::endl;
                    exit(1);
                }

                MUSE::SurfaceMesh<> low_mesh;
                low_mesh.load(files.at(i).c_str());

                if(low_mesh.num_polys() >= mesh.num_polys())
                {
                    std::cout << FRED("ERROR. Lower mesh have more polys than reference mesh!") << std::endl;
                    exit(1);
                }

                std::string mesh_name = get_filename(files.at(i));
                mesh_name = get_basename(mesh_name);

                if(mesh_name.find("_") != std::string::npos)
                    mesh_name = mesh_name.substr(mesh_name.find_last_of("_")+1, mesh_name.length());

                //devo vedere i centroidi della mesh più fitta in quali triangoli della mesh piùm grossolana ricadono

                std::vector<double> mean_values;

                for(uint pid2=0; pid2<low_mesh.num_polys(); pid2++)
                {
                    cinolib::vec3d v1 = low_mesh.poly_vert(pid2, 0);
                    cinolib::vec3d v2 = low_mesh.poly_vert(pid2, 1);
                    cinolib::vec3d v3 = low_mesh.poly_vert(pid2, 2);

                    //                    cinolib::vec2d v12d(v1.x(), v1.y());
                    //                    cinolib::vec2d v22d(v2.x(), v2.y());
                    //                    cinolib::vec2d v32d(v3.x(), v3.y());

                    double sum = 0.0;
                    int count = 0;

                    for(uint pid=0; pid<mesh.num_polys(); pid++)
                    {
                        //std::cout << "triangle pid = " << pid << std::endl;

                        cinolib::vec3d c = mesh.poly_centroid(pid);
                        //cinolib::vec2d p2d(c.x(), c.y());

                        //if (cinolib::point_in_triangle_2d(p2d, v12d, v22d, v32d)!=0) //se trovo la proiezione
                        if (cinolib::point_in_triangle_3d(c, v1, v2, v3)!=0)
                        {
                            count++;
                            sum += values.at(pid);
                        }
                    }
                    mean_values.push_back(sum/count);
                }
                //std::cout << "vec_per_pid dimension = " << vec.size() << std::endl;
                //std::cout << "mean dimension = " << mean_values.size() << std::endl;

                std::string var_name = get_filename(setScalarField.getValue());
                var_name = get_basename(var_name);

                if(setOutFolder.isSet())
                {
                    if(!filesystem::exists(Project.getFolder() + "/out/" + setOutFolder.getValue()))
                        filesystem::create_directories(Project.getFolder() + "/out/" + setOutFolder.getValue());
                    export1d_xyz (Project.getFolder() + "/out/" + setOutFolder.getValue() + "/" +  var_name + ".csv", mean_values);
                }
                else
                    export1d_xyz (get_path(setScalarField.getValue()) + "/" +  var_name + "_" + mesh_name + ".csv", mean_values);
            }
        }
        else if(ext_mesh.compare(".mesh") == 0 || ext_mesh.compare(".vtk") == 0)
        {
            std::cout << "### Check on mesh extension: volume." << std::endl;

            MUSE::VolumeMesh<> mesh;
            mesh.load(setRefModel.getValue().c_str());

            std::cout << std::endl;
            std::cout << "Load mesh with lower resolution than the reference mesh ... " << std::endl;
            std::vector<std::string> files = meshFiles.getValue();
            for(size_t i=0; i< files.size(); i++)
            {
                std::string ext_mesh = get_extension(files.at(i));

                //Distinguo le mesh surf/vol in base all'estensione
                if(ext_mesh.compare(".off") == 0 || ext_mesh.compare(".obj") == 0)
                {
                    std::cout << "ERROR. Volume mesh required!" << std::endl;
                    exit(1);
                }

                MUSE::VolumeMesh<> low_mesh;
                low_mesh.load(files.at(i).c_str());

                if(low_mesh.num_polys() >= mesh.num_polys())
                {
                    std::cout << FRED("ERROR. Lower mesh have more polys than reference mesh!") << std::endl;
                    exit(1);
                }

                std::string mesh_name = get_filename(files.at(i));
                mesh_name = get_basename(mesh_name);

                if(mesh_name.find("_") != std::string::npos)
                    mesh_name = mesh_name.substr(mesh_name.find_last_of("_")+1, mesh_name.length());

                std::vector<double> mean_values;

                for(uint pid2=0; pid2<low_mesh.num_polys(); pid2++)
                {
                    cinolib::vec3d v1 = low_mesh.poly_vert(pid2, 0);
                    cinolib::vec3d v2 = low_mesh.poly_vert(pid2, 1);
                    cinolib::vec3d v3 = low_mesh.poly_vert(pid2, 2);
                    cinolib::vec3d v4 = low_mesh.poly_vert(pid2, 3);


                    double sum = 0.0;
                    int count = 0;

                    for(uint pid=0; pid<mesh.num_polys(); pid++)
                    {
                        //std::cout << "triangle pid = " << pid << std::endl;

                        cinolib::vec3d c = mesh.poly_centroid(pid);

                        if (cinolib::point_in_tet(c, v1, v2, v3, v4)!=0) //se trovo la proiezione
                        {
                            count++;
                            sum += values.at(pid);
                        }
                    }
                    mean_values.push_back(sum/count);
                }
                std::string var_name = get_filename(setScalarField.getValue());
                var_name = get_basename(var_name);

                if(setOutFolder.isSet())
                {
                    if(!filesystem::exists(Project.getFolder() + "/out/" + setOutFolder.getValue()))
                        filesystem::create_directories(Project.getFolder() + "/out/" + setOutFolder.getValue());
                    export1d_xyz (Project.getFolder() + "/out/" + setOutFolder.getValue() + "/" +  var_name + ".csv", mean_values);
                }
                else
                    export1d_xyz (get_path(setScalarField.getValue()) + "/" +  var_name + "_" + mesh_name + ".csv", mean_values);
            }
        }
        else
        {
            std::cout << FRED("Error: mesh format not supported!") << std::endl;
            exit(1);
        }
    }


    //This switch allows to create a volume mesh starting from a surface mesh and a set of wells (defined as strings) with the command -createVolObjectwithWells. 
    //The output is a tetmesh with the wells as cylindrical holes (if generate_tet is set to true) or as cylinders (if generate_tet is set to false). 
    //The command accepts several parameters for the generation of the volume mesh, such as the target edge length, the maximum tet volume, the option to refine the cylindrical holes, etc.
    //The command saves the volume mesh in the output folder (project/out/geometry/volume) defined by the user (-p path/to/project)
    //For more details on the parameters, please refer to the documentation of the command -createVolObjectwithWells.
    if(createVolObjectwithWells.isSet())
    {
        std::cout << "=== Creating volume mesh with wells ... " << std::endl;
        if(!filesystem::exists(out_volume))
        {
            std::cout << "=== Creating volume directory in Project folder ... " << Project.getFolder() << std::endl;
            filesystem::create_directory(out_volume);
        }

        if(!meshFiles.isSet() && !generate_box_arg.isSet())
        {
            std::cerr << "ERROR: For volume object, one mesh file is required by command -m or the option --generate_box!" << std::endl;
            exit(1);
        }

        CreateWellsConfig config;
        if(!meshFiles.isSet() && generate_box_arg.isSet())
        {
            std::cout << "=== Generate box mesh for volume creation ... " << std::endl;
            config.generate_box = generate_box_arg.getValue();
        }

        if(meshFiles.isSet())
        {
            if(meshFiles.getValue().size() > 1)
            {
                std::cerr << "ERROR: For volume object, one mesh file is accepted by command -m!" << std::endl;
                exit(1);
            }

            std::cout << "=== Load surface mesh for volume creation ... " << std::endl;
            std::cout << "=== ... neglecting box generation even if option --generate_box is set!" << std::endl;
            std::cout << "=== Mesh file: " << meshFiles.getValue().at(0) << std::endl;

            config.input_file = meshFiles.getValue().at(0);
        }

        config.output_file = out_volume + "/" + output_mesh_arg.getValue();
        config.volmesh_format = ext_vol;
        config.well_strings = wells_arg.getValue();
        config.target_edge_length = edge_length_arg.getValue();
        config.verbose = verbose_arg.getValue();
        config.generate_tet = tetFlag.getValue(); //generate_tet_arg.getValue();
        config.max_tet_volume = max_tet_volume_arg.getValue();
        config.save_no_wells = save_no_wells_arg.getValue();
        config.save_only_wells = save_only_wells_arg.getValue();
        config.tetgen_flags = optFlag.getValue(); //tetgen_flags_arg.getValue();
        config.refine_cylinders = refine_cylinders_arg.getValue();
        config.cylinder_edge_scale = cylinder_edge_scale_arg.getValue();

        create_tetmesh_with_wells(config);
        std::cout << "=== Volume mesh with wells created ... COMPLETED." << std::endl;

        MUSE::VolumeMeta geometa;
        geometa.setProject(Project);

        std::vector<std::string> excommands;
        excommands.push_back(command);
        geometa.setCommands(excommands);
        
        MUSE::Volume vol;
        MUSE::Volume::Parameters vol_par;
        vol.setParameters(vol_par);

        geometa.write(out_volume + "/" + get_basename(output_mesh_arg.getValue()) + ".json");
        std::cout << "=== Saving JSON ... TO DO!" << std::endl;
    }


    } catch (ArgException &e)  // catch exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

}
