#include <iostream>
#include <filesystem>
#include <string>

#include <tclap/CmdLine.h>

#include "geostatslib/statistics/decluster.h"
#include "muselib/metadata/extraction_meta.h"
#include "muselib/utils.h"
#include "muselib/utils_timing.h"
#include "muselib/colors.h"

#include "muselib/data_structures/project.h"
#include "muselib/data_structures/data.h"

#include "muselib/metadata/metadata.h"
#include "muselib/metadata/vario_meta.h"
#include "muselib/metadata/data_summary.h"
#include "muselib/metadata/compute_meta.h"

#include "muselib/geostatistics/utils.h"
#include "muselib/geometry/tools.h"

#include "muselib/geometry/surface_mesh.h"
#include "muselib/geometry/volume_mesh.h"

#include "muselib/input/load_xyz.h"


#include "muselib/geostatistics/stats.h"

#include "create_db.h"
#include "insert_db.h"

#include "geostatslib/../utils/geom_utils.h"
#include "geostatslib/statistics/data_structures.h"
#include "geostatslib/statistics/normal_score.h"
#include "geostatslib/statistics/sgs.h"
#include "geostatslib/statistics/stats.h"
#include "geostatslib/statistics/indicator_kriging.h"
#include "geostatslib/statistics/sis.h"

#include "muselib/geostatistics/indicator.h"

#include <cinolib/meshes/drawable_trimesh.h>
#include <cinolib/meshes/drawable_tetmesh.h>


//for filesystem
#ifdef __APPLE__
    using namespace std::__fs;
#else
    using namespace std;
#endif


using namespace MUSE;
using namespace TCLAP;

int main(int argc, char** argv)
{
    std::cout << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "================== STARTING MUSE-COMPUTE ===================" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;

    std::string app_name = "compute"; //app name
    std::string app_vario = "vario"; //app vario name
    std::string app_data = "data"; //app data name
    std::string app_manipulate = "manipulate";

    // Timing logger
    MUSE::TimingLogger timing_logger("muse-" + app_name);
                    


    try {
    CmdLine cmd("MUSE - Modelling Uncertainty as a Support of Environment. MUSE-compute application", ' ', "version 0.0");


    // ---------------------------------------------------------------------------------------------------------
    // MAIN FUNCTIONALITIES:

    // Option 0. New project creation
    /**
     * @brief Enable computation mode for MUSE-compute application. This flag activates the computation mode of the MUSE-compute application, allowing you to perform geostatistical computations and analyses based on the specified parameters and configurations. When this flag is set, the application will execute the computational workflow defined by the provided options and parameters, enabling you to generate results such as simulations, and statistical analyses based on your project data, variogram models and settings.
     * @note When using this flag, the following parameters become important for defining the computation workflow:
     * - --var: Specify the variable name to analyze (mandatory)
     * - --geom: Specify the geometry model name (mandatory)
     * @example muse_compute -C -p /path/to/project_directory --var temperature --geom mesh_model
     */
    SwitchArg interpolationCompute      ("C", "compute", "Enable computation mode", cmd, false); //booleano

    /**
     * @brief Set project directory for MUSE-compute application. This option allows you to specify the path to the project directory for the MUSE-compute application, which is where the application will look for input data, variogram models, geometry models, and where it will save output results and logs. The project directory should contain the necessary files and subdirectories for the computation process, and specifying this path correctly is important for ensuring that the application can access the required resources and save results in the appropriate location.
     * @required true (this parameter is mandatory for the computation process).
     * @format string (path to the project directory)
     * @default "path/to/project_directory" (placeholder value, should be replaced with an actual path to the project directory).
     * @example muse_compute -C -p /path/to/project_directory --var temperature --geom mesh_model
     */
    ValueArg<std::string> projectFolder ("p", "pdir", "Set project directory", false, "path/to/project_directory", "path", cmd);
    
    /**
     * @brief Set debug mode for MUSE-compute application. This flag enables debug mode in the MUSE-compute application, which allows for saving additional support files during the computation process for troubleshooting and analysis purposes. When this flag is set, the application will generate and save intermediate results, logs, and diagnostic information that can be useful for investigating the computation process in more detail or when encountering issues. This can help users to understand the internal workings of the computation, identify potential problems, and analyze the results more effectively.
     * @default false (debug mode is disabled by default).
     * @format boolean flag
     * @example muse_compute -C -p /path/to/project_directory --var temperature --geom mesh_model --debug
     */
    SwitchArg setDebug ("", "debug", "Set debug mode to save additional support files", cmd, false); //booleano

    /**
     * @brief Set computation mode for MUSE-compute application. This option allows you to specify the computation mode for the MUSE-compute application, which can influence how the computations are performed and how parameters are selected.
     * @default "AUTO" (automatic parameter selection is applied by default).
     * @format string
     * @values AUTO, MANUAL
     * @note When using this option, the following modes are available for selection:
     *       - AUTO: Automatic parameter selection
     *       - MANUAL: Manual parameter configuration required
     * @example muse_compute -C -p /path/to/project_directory --var temperature --geom mesh_model --mode AUTO
     */
    ValueArg<std::string> modeCompute   ("", "mode", "Set computation mode", false, "AUTO", "string", cmd);
    
    /**
     * @brief Set variable name for MUSE-compute application. This option allows you to specify the name of the variable to be processed in the MUSE-compute application. The variable name should correspond to a variable that exists in the project data, and it is mandatory when using the -C/--compute flag to ensure that the application knows which variable to analyze and perform computations on. Specifying the correct variable name is crucial for the computation process, as it determines which data will be used for variogram modeling, simulations, and other geostatistical analyses.
     * @default "name_var" (placeholder value, should be replaced with the actual variable name from the project data).
     * @format string (name of the variable)
     * @required true (this parameter is mandatory when using the -C/--compute flag).
     * @example muse_compute -C -p /path/to/project_directory --var temperature --geom mesh_model
     */
    ValueArg<std::string> Variable      ("v", "var", "Set variable name to perform computations", false, "variable-name", "string", cmd);
    
    /**
     * @brief Set geometry model for MUSE-compute application. This option allows you to specify the name of the geometry model to be used in the MUSE-compute application. The geometry model should be defined in the project and can be a surface mesh, volume mesh, or any other type of geometric representation that is compatible with the application. Specifying the geometry model is mandatory when using the -C/--compute flag, as it defines the spatial framework for the computations and analyses that will be performed on the specified variable. The geometry model must be available in the project for the application to access and utilize it during the computation process.
     * @default "name_geometry" (placeholder value, should be replaced with the actual name of the geometry model from the project).
     * @format string (name of the geometry model)  
     * @required true (this parameter is mandatory when using the -C/--compute flag).
     * @example muse_compute -C -p /path/to/project_directory --var temperature --geom mesh_model
     */
    ValueArg<std::string> geomModel     ("m", "geom", "Set geometry model name to perform computations", false, "geometry-name", "string", cmd);

    /**
     * @brief Set extracted sub-dataset referring to specified geometry domain from project data. This option allows you to specify a sub-dataset that corresponds to a particular geometry domain within the project data (derived from muse-manipulate).
     * @default false
     * @format string value (name of the sub-dataset)
     * @note When using this flag, ensure that the specified sub-dataset is properly extracted (by muse-manipulate) and corresponds to the geometry domain you want to analyze. This allows for more targeted variogram computation based on specific spatial domains within the project data.
     * @example --sub subdataset-name
     */
    ValueArg<std::string> subDataset        ("", "sub", "Set extracted sub-dataset referring to specified geometry domain", false, "subdataset-name", "string", cmd);

    /**
     * @brief Set rotation axis for data rotation. This option allows you to specify the axis around which the data will be rotated. The rotation can be applied to the spatial coordinates of the data, which may be useful for aligning the data with a particular orientation or for performing certain types of analyses that require a specific coordinate system.
     * @default NO (no rotation is applied)
     * @format string value (X, Y, Z)
     * @note When using this flag, you typically need to specify the rotation angle (with --rotangle) and the rotation center coordinates (with --rotcx, --rotcy, --rotcz) to fully define the rotation transformation. The rotation axis can be set to X, Y, or Z depending on the desired rotation direction.
     * @example --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0
     */
    ValueArg<std::string> setRotAxis        ("", "rotaxis", "Set rotation axis for data rotation (X, Y, Z)", false, "NO", "string", cmd);
    
    /**
     * @brief Set rotation angle (clockwise) for data rotation. This option allows you to specify the angle by which the data will be rotated in a clockwise direction. The rotation is applied around the axis specified with --rotaxis and centered at the coordinates specified with --rotcx, --rotcy, and --rotcz.
     * @default 0.0 (no rotation)
     * @format double value (rotation angle in degrees)
     * @note When using this flag, you typically need to specify the rotation axis (with --rotaxis) and the rotation center coordinates (with --rotcx, --rotcy, --rotcz) to fully define the rotation transformation. The rotation angle should be provided in degrees, and the rotation will be applied in a clockwise direction based on the specified axis and center.
     * @example --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0
     */
    ValueArg<double> setRotAngle            ("", "rotangle", "Set rotation angle (clockwise) for data rotation", false, 0.0, "double", cmd);
    
    /**
     * @brief Set rotation center x coordinate for data rotation. This option allows you to specify the x-coordinate of the center point around which the data will be rotated. The rotation is applied based on the axis specified with --rotaxis and the angle specified with --rotangle.
     * @default 0.0 (rotation around the origin)
     * @format double value (x coordinate of rotation center)
     * @note Default is 0.0 (rotation around the origin). When using this flag, you typically need to specify the rotation axis (with --rotaxis) and the rotation angle (with --rotangle) to fully define the rotation transformation. The rotation center coordinates (rotcx, rotcy, rotcz) define the point in space around which the rotation will occur. The x-coordinate (rotcx) is used in conjunction with the y and z coordinates (rotcy, rotcz) to specify the full rotation center.
     * @example --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0
     */
    ValueArg<double> setRotCenterX          ("", "rotcx", "Set rotation center x coordinate for data rotation", false, 0.0, "double", cmd);
    
    /**
     * @brief Set rotation center y coordinate for data rotation. This option allows you to specify the y-coordinate of the center point around which the data will be rotated. The rotation is applied based on the axis specified with --rotaxis and the angle specified with --rotangle.
     * @default 0.0 (rotation around the origin)
     * @format double value (y coordinate of rotation center)
     * @note Default is 0.0 (rotation around the origin). When using this flag, you typically need to specify the rotation axis (with --rotaxis) and the rotation angle (with --rotangle) to fully define the rotation transformation. The rotation center coordinates (rotcx, rotcy, rotcz) define the point in space around which the rotation will occur. The y-coordinate (rotcy) is used in conjunction with the x and z coordinates (rotcx, rotcz) to specify the full rotation center.
     * @example --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0
     */
    ValueArg<double> setRotCenterY          ("", "rotcy", "Set rotation center y coordinate for data rotation", false, 0.0, "double", cmd);
    
    /**
     * @brief Set rotation center z coordinate for data rotation. This option allows you to specify the z-coordinate of the center point around which the data will be rotated. The rotation is applied based on the axis specified with --rotaxis and the angle specified with --rotangle.
     * @default 0.0 (rotation around the origin)
     * @format double value (z coordinate of rotation center)
     * @note Default is 0.0 (rotation around the origin). When using this flag, you typically need to specify the rotation axis (with --rotaxis) and the rotation angle (with --rotangle) to fully define the rotation transformation. The rotation center coordinates (rotcx, rotcy, rotcz) define the point in space around which the rotation will occur. The z-coordinate (rotcz) is used in conjunction with the x and y coordinates (rotcx, rotcy) to specify the full rotation center.
     * @example --rotaxis Z --rotangle 45 --rotcx 100 --rotcy 200 --rotcz 0
     */
    ValueArg<double> setRotCenterZ          ("", "rotcz", "Set rotation center z coordinate for data rotation", false, 0.0, "double", cmd);
    
    /**
     * @brief Set loading of variogram configuration from file. This option allows you to specify a path to a variogram configuration file that contains predefined settings for variogram modeling and computation. The configuration file can include parameters such as variogram type, direction, dimension, range, and other relevant settings that can be loaded into the application to streamline the variogram setup process. Using a configuration file can help to ensure consistency in variogram settings across different projects or analyses and can save time by allowing you to reuse predefined configurations.
     * @default "none" (no configuration file is loaded)
     * @format string value (path to the variogram configuration file)
     * @example --vario path/to/variogram_config.txt
     */
    ValueArg<std::string> setVario      ("", "vario", "Set loading of variogram configuration from file", false, "none", "string", cmd);

    // Option: types of variogram directions
    std::vector<std::string> allowedVarioDir = {"OMNI","DIR"};
    ValuesConstraint<std::string> allowedValsVD(allowedVarioDir);
    /**
     * @brief Set variogram direction type related to variogram configuration. This option allows you to specify the type of variogram direction to be used in the variogram modeling and computation process. The direction type can influence how the variogram is computed and interpreted, especially in cases where anisotropy is present in the data.
     * @default "OMNI" (omnidirectional variogram)
     * @format string value (type of variogram direction)
     * @values OMNI, DIR
     * @note When using this flag, the following direction types are available for selection:   
     * - OMNI: An omnidirectional variogram is computed, which does not consider any specific directionality in the spatial data. This type of variogram is useful when the spatial continuity is assumed to be the same in all directions.
     * - DIR: A directional variogram is computed, which considers specific directions in the spatial data. This type of variogram is useful when anisotropy is present, meaning that the spatial continuity varies with direction. When using DIR, you typically need to specify the direction angles and tolerances to define the directional variogram computation.
     * @example --dir DIR
     */
    ValueArg<std::string> varioDirection ("", "dir", "Set variogram direction type", false, "OMNI", &allowedValsVD, cmd);

    // Option: types of variogram dimensions
    std::vector<std::string> allowedVarioDim = {"3D","3Dxy","3Dz","2D","1Dz","1D"};
    ValuesConstraint<std::string> allowedValsVDm(allowedVarioDim);
    /**
     * @brief Set variogram dimension type related to variogram configuration. This option allows you to specify the type of variogram dimension to be used in the variogram modeling and computation process. The dimension type can influence how the variogram is computed and interpreted, especially in cases where the spatial data has different characteristics in different dimensions.
     * @default "3D" (three-dimensional variogram)
     * @format string value (type of variogram dimension)
     * @values 3D, 3Dxy, 3Dz, 2D, 1Dz, 1D
     * @note Variogram configuration flags work together:
     *       - --dir: Direction type (OMNI, DIR)
     *       - --dim: Dimension type
     *       - --zrange: Z direction range (used with 3D dimensions)
     * @example --dir OMNI --dim 3D --zrange 50.0
     */
    ValueArg<std::string> varioDimension ("", "dim", "Set variogram dimension type", false, "3D", &allowedValsVDm, cmd);
    
    /**
     * @brief Set range in Z direction. This option allows you to specify the range value in the Z direction for variogram computation when using three-dimensional variogram dimensions (3Dxy). The Z range can influence how the variogram is computed and interpreted in cases where there is significant variability in the vertical direction. Setting an appropriate Z range can help to capture the spatial continuity and variability in the Z direction more effectively, especially when the data exhibits different characteristics in the vertical dimension compared to the horizontal dimensions.
     * @default 1.0 (default Z range value)
     * @format double value (range in Z direction)
     * @note This flag is used in conjunction with the variogram configuration flags --dir and --dim. When using three-dimensional variogram dimensions (3Dxy), this flag becomes important for defining the Z direction range, which can help to manage the influence of vertical variability in the variogram computation and ensure that the variogram captures the spatial continuity in the Z direction appropriately.
     * @example --dir OMNI --dim 3Dxy --zrange 50.0
     */
    ValueArg<double> setZRange           ("", "zrange", "Set range in Z direction", false, 1.0, "double", cmd);

    // Option: set interpolation criteria
    std::vector<std::string> allowedCRIT = {"SGS","IK","SISIM"};
    ValuesConstraint<std::string> allowedValsCRIT(allowedCRIT);
    /**
     * @brief Set interpolation algorithm for geostatistical computations. This option allows you to specify the interpolation algorithm to be used in the geostatistical computations and analyses performed by the MUSE-compute application. The choice of interpolation algorithm can influence the results of simulations, predictions, and other analyses based on the spatial data and variogram models.
     * @default "SGS" (sequential Gaussian simulation)
     * @format string value (type of interpolation algorithm)
     * @values SGS, IK, SISIM
     * @note When using this flag, the following interpolation algorithms are available for selection:
     *   - SGS: Sequential Gaussian Simulation, a geostatistical simulation method that generates realizations of spatial variables based on a Gaussian distribution and the variogram model.
     *   - IK: Indicator Kriging, a geostatistical interpolation method that estimates the probability of a variable exceeding a certain threshold based on indicator variables and the variogram model.
     *   - SISIM: Sequential Indicator Simulation, a geostatistical simulation method that generates realizations of spatial variables based on indicator variables and the variogram model, often used for categorical or non-Gaussian data.
     * @example --crit IK
     */
    ValueArg<std::string> setCRIT       ("", "crit", "Set interpolation algorithm for geostatistical computations", false, "SGS", &allowedValsCRIT, cmd);

    // Option 0b. Parameters for simulations
    /**
     * @brief Enable back normal score transformation integrated into SGS. This flag allows you to enable the back normal score transformation as part of the sequential Gaussian simulation (SGS) process. When this flag is set, the application will perform a back normal score transformation on the data after the SGS computations. The back normal score transformation can be particularly useful when dealing with skewed or non-normal data distributions, as it can help to normalize the data and make it more suitable for geostatistical modeling and simulation.
     * @default false (back normal score transformation is disabled by default).
     * @format boolean flag
     * @note When using this flag, it is typically applied in conjunction with the --crit SGS option, as it is specifically designed to work with the sequential Gaussian simulation method. Enabling this flag can help to improve the quality of simulations and predictions when the data exhibits non-normal characteristics, by transforming the data back to a normal distribution after the SGS computations.
     * @example --crit SGS --bnscore
     */
    SwitchArg setBackNormalScore        ("", "bnscore", "Enable back normal score transformation (integrated into SGS algorithm)", cmd, false); //booleano
    
    /**
     * @brief Set extrapolation type in back normal score transformation. This option allows you to specify the type of extrapolation to be applied in the back normal score transformation process. Extrapolation can be used to manage values that fall outside the range of the original data distribution, especially when performing transformations on skewed or non-normal data. The choice of extrapolation type can influence how the back normal score transformation handles extreme values and can help to ensure that the transformed data remains within a reasonable range, which can be important for maintaining the integrity of the geostatistical modeling and simulation process.
     * @default "none" (no extrapolation is applied)
     * @format string value (type of extrapolation)
     * @values none, Extr
     * @param extr Type of extrapolation (default: none)
     * @note When using this flag, the following extrapolation types are available for selection:
     *       - none: No extrapolation is applied in the back normal score transformation.
     *      - Extr: Extrapolation is applied in the back normal score transformation, and the specific method of extrapolation can be defined based on the requirements of the data and analysis. When using extrapolation, you typically need to specify the minimum and maximum extrapolation values (with --minextr and --maxextr) to define the range of extrapolation for values that fall outside the original data distribution.
     * @example --extr Extr --minextr 10.0 --maxextr 70.0
     */
    ValueArg<std::string> setExtrType   ("", "extr", "Set extrapolation type in back normal score transformation", false, "none", "string", cmd); //di default settata su "none"
    
    /**
     * @brief Set minimum value for extrapolation in back normal score transformation. This option allows you to specify the minimum value to be used for extrapolation in the back normal score transformation process. When extrapolation is enabled (with --extr Extr), this minimum value defines the lower bound for extrapolating values that fall outside the original data distribution. Setting an appropriate minimum extrapolation value can help to manage extreme values and ensure that the transformed data remains within a reasonable range, which can be important for maintaining the integrity of geostatistical modeling and simulation.
     * @default 0.0 (default minimum extrapolation value)
     * @format double value (minimum extrapolation value)
     * @required false (this parameter is optional and only relevant when extrapolation is enabled)
     * @note This flag is used in conjunction with the --extr flag when extrapolation is enabled. When using extrapolation, you typically need to specify both the minimum and maximum extrapolation values (with --minextr and --maxextr) to define the range of extrapolation for values that fall outside the original data distribution. Setting appropriate minimum and maximum extrapolation values can help to ensure that the back normal score transformation handles extreme values effectively and maintains the quality of the geostatistical modeling and simulation process.
     * @example --extr Extr --minextr 10.0
     */
    ValueArg<double> setMinExtr         ("", "minextr", "Set minimum value for extrapolation in back normal score transformation", false, 0.0, "double", cmd); //n. di simulazioni = 10 di default
    
    /**
     * @brief Set maximum value for extrapolation in back normal score transformation. This option allows you to specify the maximum value to be used for extrapolation in the back normal score transformation process. When extrapolation is enabled (with --extr Extr), this maximum value defines the upper bound for extrapolating values that fall outside the original data distribution. Setting an appropriate maximum extrapolation value can help to manage extreme values and ensure that the transformed data remains within a reasonable range, which can be important for maintaining the integrity of geostatistical modeling and simulation.
     * @default 100000.0 (default maximum extrapolation value)
     * @format double value (maximum extrapolation value)
     * @required false (this parameter is optional and only relevant when extrapolation is enabled)
     * @note This flag is used in conjunction with the --extr flag when extrapolation is enabled. When using extrapolation, you typically need to specify both the minimum and maximum extrapolation values (with --minextr and --maxextr) to define the range of extrapolation for values that fall outside the original data distribution. Setting appropriate minimum and maximum extrapolation values can help to ensure that the back normal score transformation handles extreme values effectively and maintains the quality of the geostatistical modeling and simulation process.
     * @example --extr Extr --maxextr 70.0
     */
    ValueArg<double> setMaxExtr         ("", "maxextr", "Set maximum value for extrapolation in back normal score transformation", false, 100000.0, "double", cmd); //n. di simulazioni = 10 di default

    /**
     * @brief Set number of simulation iterations. This option allows you to specify the number of iterations to be performed during the simulation process. The number of iterations can influence the quality and stability of the simulation results, as well as the computational time required to complete the simulations. Setting an appropriate number of iterations can help to ensure that the simulations converge to a stable solution and that the results are reliable for analysis and decision-making.
     * @default 10 (default number of simulation iterations)
     * @format unsigned integer value (number of simulation iterations)
     * @required false (this parameter is optional and can be adjusted based on the desired balance between simulation quality and computational time)
     * @note When setting the number of simulation iterations, consider the complexity of the spatial data, the variogram model, and the computational resources available. In general, a higher number of iterations can lead to more stable and reliable simulation results, but it may also increase the computational time required to complete the simulations. It is often recommended to start with a moderate number of iterations (e.g., 10) and adjust based on the observed results and computational performance.
     * @example --nsim 20
     */
    ValueArg<uint> setNsim              ("", "nsim", "Set number of iterations of simulation process", false, 10, "uint", cmd); //n. di simulazioni = 10 di default

    // Option: set 2D declustering on data
    /**
     * @brief Set cell size for 2D declustering. This option allows you to specify the cell size to be used for 2D declustering when computing the variogram. The cell size defines the spatial resolution of the grid used for declustering, where data points within the same cell are considered part of the same cluster. Choosing an appropriate cell size is important for effective declustering, as it can influence the weights assigned to data points and ultimately affect the variogram results.
     * @default 0.0 (no cell size specified).
     * @format double value (cell size)
     * @required Must be specified when using --decl flag for 2D declustering.
     * @note The specified cell size should be chosen based on the spatial characteristics of the data and the desired level of declustering. 
     * It is often recommended to experiment with different cell sizes to find the optimal value for your specific dataset and analysis goals.
     * When using declustering, the cell size should be provided in the same units as the spatial coordinates of the data (e.g., meters) to ensure proper declustering based on the spatial distribution of the data points.
     * @example --decl --csize 100 --nstep 5 
     */
    ValueArg<double> setCellSize            ("", "csize", "Set cell size for 2D declustering", false, 0.0, "double", cmd);

    /**
     * @brief Set number of grid translation steps for 2D declustering. This option allows you to specify the number of grid translation steps to be used for 2D declustering when computing the variogram. Grid translation is a technique used in declustering to reduce the influence of clustered data points by translating the grid multiple times and averaging the results. The number of steps determines how many times the grid will be translated, which can help to further mitigate the effects of clustering in the data.
     * @param nstep Flag to set number of grid translation steps for 2D declustering
     * @default 0 (no grid translation). 
     * @format positive integer
     * @required Must be specified when using --decl flag for 2D declustering.
     * @note The specified number of steps should be chosen based on the level of declustering desired and the computational resources available, as increasing the number of steps can lead to more effective declustering but also increases the computational time required for variogram computation.
     * When using declustering, the number of grid translation steps should be a positive integer, and it is often recommended to experiment with different values to find the optimal number of steps for your specific dataset and analysis goals.
     * @example --decl --csize 100 --nstep 5
     */
    ValueArg<int> setNStep                  ("", "nstep", "Set number of grid translation steps for 2D declustering", false, 0, "int", cmd);

    /**
     * @brief Enable statistical analysis on simulation results. This flag allows you to enable the computation of statistical analysis on the results generated from the simulations. When this flag is set, the application will perform various statistical analyses on the simulated data, which can include mean, variance, quantiles, and other relevant statistics that can help to summarize and interpret the simulation results. Enabling statistical analysis can provide valuable insights into the characteristics of the simulated data and can assist in making informed decisions based on the simulation outcomes.
     * @default false (statistical analysis is disabled by default).
     * @format boolean flag
     * @note When using this flag, the application will automatically compute and save the statistical analysis results based on the simulations performed on the _stats folder within the project directory. 
     * It requires the following parameters to be specified for the computation process:
     * - --var: Specify the variable name to analyze (mandatory)
     * - --geom: Specify the geometry model name (mandatory)
     * - --dir: Specify the directory for the simulation results (mandatory)
     * - --dim: Specify the dimensionality of the analysis (mandatory)
     * - --space: Specify the space type for analysis (optional, default: NORMAL)
     * @example -S -p /path/to/project_directory --var temperature --geom mesh_model --dir DIR --dim 2D --space VAR
     */
    SwitchArg statisticalAnalysis       ("S", "stats", "Compute statistical analysis on simulation results", cmd, false); //booleano
    
    // Option: set interpolation criteria
    std::vector<std::string> allowedSPACE = {"NORMAL","VAR"};
    ValuesConstraint<std::string> allowedValsSPACE(allowedSPACE);
    /**
     * @brief Set space type for statistical analysis on simulation results. This option allows you to specify the type of space to be used for the statistical analysis of simulation results. The space type can influence how the statistical analysis is performed and interpreted, especially in cases where the spatial characteristics of the data play a significant role in the analysis.
     * @default "NORMAL" (normal space is used by default).
     * @format string value (type of space)
     * @values NORMAL, VAR
     * @note When using this flag, the following space types are available for selection:
     *       - NORMAL: The statistical analysis is performed in the normal data space (after normal score transformation).
     *       - VAR: The statistical analysis is performed in the original variable space (after back normal score transformation).
     * @example -S -p /path/to/project_directory --var temperature --geom mesh_model --dir DIR --dim 2D --space VAR
     */
    ValueArg<std::string> setSpace      ("", "space", "Set space type for statistical analysis", false, "NORMAL", &allowedValsSPACE, cmd);

    // Option 2. Back normal score
    /**
     * @brief Enable back normal score transformation after SGS computations. This flag allows you to enable the back normal score transformation after saving the results of the sequential Gaussian simulation (SGS) process. When this flag is set, the application will perform a back normal score transformation on the data after the SGS computations. 
     * @default false (back normal score transformation is disabled by default).
     * @format boolean flag
     * @note When using this flag, it is typically applied in conjunction with the --crit SGS option, as it is specifically designed to work with the sequential Gaussian simulation method. Enabling this flag can help to improve the quality of simulations and predictions when the data exhibits non-normal characteristics, by transforming the data back to a normal distribution after the SGS computations. The back normal score transformation can be particularly useful for managing skewed or non-normal data distributions, ensuring that the transformed data is more suitable for geostatistical modeling and simulation.
     * @example -B -p /path/to/project_directory --var temperature --geom mesh_model --crit SGS --extr Extr --minextr 10.0 --maxextr 70.0
     */
    SwitchArg doBackNormalScore         ("B", "back-normalscore", "Enable back normal score transformation", cmd, false); //booleano
    
    /**
     * @brief Set input file path. This option allows you to specify the path to an input file that contains data to be used in the MUSE-compute application. The input file can include various types of data relevant to the computations and analyses performed by the application, such as back normal score transformation. The file should be formatted according to the requirements of the application: a column of estimates in normal space is accepted.
     * @default "/path/to/file" (placeholder value, should be replaced with the actual path to the input file).
     * @format string value (path to the input file)
     * @example --file /path/to/input_file.txt
     */
    ValueArg<std::string> setFile       ("f", "file", "Set input file path to apply back normal score transformation", false, "/path/to/file", "string", cmd);



    // Option 3. Database creation to store simulation results
    /**
     * @brief Enable database creation from simulations (preliminary version of database implementation). This flag allows you to enable the creation of a database to store the results generated from the simulations. When this flag is set, the application will create a database structure to organize and manage the simulation results, which can facilitate data retrieval, and analysis. The database can be designed to store various types of information related to the simulations, such as input parameters, variogram models, simulation outputs, and statistical analyses.
     */
    SwitchArg createDatabase            ("D", "db", "Create database from simulations", cmd, false); //booleano


    // ---------------------------------------------------------------------------------------------------------
    // ADDITIONAL FUNCTIONALITIES:

    /**
     * @brief Set number of input samples. This option allows you to specify the number of input samples to be used in the nearest neighbor search for the SGS algorithm. The number of input samples can influence the quality and accuracy of the simulations, as it determines how many neighboring data points are considered when performing the simulation process. Setting an appropriate number of input samples can help to ensure that the simulations capture the spatial variability of the data effectively while also managing computational resources.
     * @default 4 (default number of input samples)
     * @format unsigned integer value (number of input samples)
     * @required false (this parameter is optional and can be adjusted based on the desired balance between simulation quality and computational time)
     * @note When setting the number of input samples, consider the spatial density of the data and the variogram model, as well as the computational resources available. In general, a higher number of input samples can lead to more accurate simulations by capturing more spatial variability, but it may also increase the computational time required to complete the simulations. It is often recommended to start with a moderate number of input samples (e.g., 4) and adjust based on the observed results and computational performance.
     * @example --input 6
     */
    ValueArg<uint> setInputSamples      ("", "input", "Set number of input samples", false, 4, "int", cmd);
    
    /**
     * @brief Set number of simulated points. This option allows you to specify the number of points to be simulated during the simulation process. The number of simulated points can influence the quality and stability of the simulation results, as well as the computational time required to complete the simulations. Setting an appropriate number of simulated points can help to ensure that the simulations capture the spatial variability of the data effectively while also managing computational resources.
     * @default 3 (default number of simulated points)
     * @format unsigned integer value (number of simulated points)
     * @required false (this parameter is optional and can be adjusted based on the desired balance between simulation quality and computational time)
     * @note When setting the number of simulated points, consider the spatial density of the data and the variogram model, as well as the computational resources available. In general, a higher number of simulated points can lead to more accurate simulations by capturing more spatial variability, but it may also increase the computational time required to complete the simulations. It is often recommended to start with a moderate number of simulated points (e.g., 3) and adjust based on the observed results and computational performance.
     * @example --simulated 5
     */
    ValueArg<uint> setSimulatedPoints   ("", "simulated", "Set number of simulated points", false, 3, "int", cmd);
    
    /**
     * @brief Set scale factor of search radius. This option allows you to specify a scale factor for the search radius used in the nearest neighbor search during the simulation algorithm. The search radius determines how far the algorithm looks for neighboring data points when performing simulations, and the scale factor can be used to adjust the size of this search radius based on the spatial characteristics of the data and the variogram model. Setting an appropriate scale factor can help to ensure that the simulations capture the spatial variability of the data effectively while also managing computational resources.
     * @default 1.0 (default scale factor for search radius)
     * @format double value (scale factor for search radius)
     * @required false (this parameter is optional and can be adjusted based on the desired balance between simulation quality and computational time)
     * @note When setting the scale factor for the search radius, consider the spatial density of the data and the variogram model, as well as the computational resources available. In general, a higher scale factor can lead to more accurate simulations by capturing more spatial variability, but it may also increase the computational time required to complete the simulations. It is often recommended to start with a moderate scale factor (e.g., 1.0) and adjust based on the observed results and computational performance.
     * @example --scaleradius 1.5
     */
    ValueArg<double> setScaleRadius     ("", "scaleradius", "Set scale factor of search radius", false, 1.0, "double", cmd);
    
    /**
     * @brief Set octant search in simulation algorithm. This flag allows you to enable the octant search method in the simulation algorithm, which can be used to optimize the nearest neighbor search process. When this flag is set, the algorithm will divide the search space into octants (in 3D) or quadrants (in 2D) and perform the search within these subdivisions, which can help to reduce the computational time required for finding neighboring data points during simulations. Enabling octant search can be particularly beneficial when dealing with large datasets or complex variogram models, as it can improve the efficiency of the simulation process while still capturing the spatial variability of the data effectively.
     * @default false (octant search is disabled by default).
     * @format boolean flag
     * @example --octant
     */
    SwitchArg doOctantSearch            ("", "octant", "Set octant search in simulation algorithm", cmd, false); //booleano

    /**
     * @brief Set CSV format for output files. This flag allows you to specify that the output files generated by the application should be saved in CSV (Comma-Separated Values) format. When this flag is set, the application will format the output data as CSV, which can be easily opened and analyzed using spreadsheet software or other data analysis tools. Saving output files in CSV format can facilitate data sharing and further analysis, as CSV is a widely supported format for tabular data.
     * @default false (CSV format is disabled by default)
     * @format boolean flag
     * @example --csv
     */
    SwitchArg csvConversion             ("", "csv", "Set CSV format for output files", cmd, false); //booleano

    std::vector<std::string> allowedSGS = {"MEAN","VECSIM"};
    ValuesConstraint<std::string> allowedValsSGS(allowedSGS);
    /**
     * @brief Set type of SGS output. This option allows you to specify the type of output to be generated from the sequential Gaussian simulation (SGS) process (due to dual version of SGS algorithm in geostatslib).
     * @default "MEAN" (mean of simulations is used as default SGS output)
     * @format string value (type of SGS output)
     * @values MEAN, VECSIM
     * @example --crit SGS --out VECSIM
     */
    ValueArg<std::string> setSGSoutput  ("", "out", "Set type of SGS output (due to dual version of SGS algorithm in geostatslib)", false, "MEAN", &allowedValsSGS, cmd); //di default settata su "none"

    /**
     * @brief Set flag to json output for data format encoding. This flag allows you to specify whether the output data should be encoded in JSON format, which can be useful for structured data representation and interoperability with other applications. When this flag is set, the application will format the output data as JSON, which can facilitate data sharing and further analysis, as JSON is a widely supported format for structured data.
     * @default YES (JSON format encoding is enabled by default)
     * @format string value (YES or NO for JSON format encoding)
     * @example --format NO
     */
    ValueArg<std::string> setFormat     ("", "format", "Set for encoding output by data format", false, "YES", "string", cmd);

    // ---------------------------------------------------------------------------------------------------------

    allowedCRIT.clear();
    allowedSPACE.clear();


    // ---------------------------------------------------------------------------------------------------------
    // PARSING:

    // Parse the argv array.
    cmd.parse(argc, argv);


    // ---------------------------------------------------------------------------------------------------------
    // SETTINGS:

    // 0) Project settings
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


    // 0) Set folder (in/out)
    std::string out_folder = Project.folder + "/out";
    std::string app_folder = out_folder + "/" + app_name;
    std::string out_vario = out_folder + "/" + app_vario;
    std::string out_man = out_folder + "/" + app_manipulate;

    std::string debug_folder = "";
    std::string prefix_timing = "";
    if(setDebug.isSet())
    {
        debug_folder = app_folder + "/_debug";
        std::cout << FYEL("=== WARNING. Debug mode is ON!") << std::endl;
            
        timing_logger.set_output_folder(debug_folder);
    }
    timing_logger.start();


    // ---------------------------------------------------------------------------------------------------------
    // STARTS:

    //---------------------------------------------------
    //---------------------------------------------------
    //---------------------------------------------------
    //---> COMPUTE SWITCH ARGUMENT (MODE NOT SET - DEFAULT: AUTO)
    //---------------------------------------------------
    //---------------------------------------------------
    //---------------------------------------------------
    if(interpolationCompute.isSet() && !modeCompute.isSet())
    {
        if(!Variable.isSet())
        {
            std::cout << FRED("ERROR. Variable is NOT set!") << std::endl;
            exit(1);
        }

        if(!geomModel.isSet())
        {
            std::cout << FYEL("WARNING. Geometry support is NOT set!") << std::endl;
            //exit(1);
        }

        if(!filesystem::exists(app_folder))
            filesystem::create_directory(app_folder);
        if(setDebug.isSet())
        {
            if(!filesystem::exists(debug_folder))
                filesystem::create_directory(debug_folder);
        }

        /////////////////////////////
        //////////SUMMARY CSV FOR FRAMES
        ///
        ///
        std::cout << "Save summary of multi-frame variography analysis ... " << std::endl;
        std::ofstream file_summary;
        file_summary.open(app_folder + "/" + Variable.getValue() + "_SGSsummary.csv", std::fstream::out);
        if(!file_summary.is_open())
        {
            std::cerr << "\033[0;31mError in file opening: " << app_folder + "/" + Variable.getValue() + "_SGSsummary.csv" << "\033[0m" << std::endl;
            exit(1);
        }
        std::string delimiter = ";";

        std::vector<std::string> vec_csv;
        vec_csv.push_back("frame_name");
        vec_csv.push_back("domain");
        vec_csv.push_back(Variable.getValue()+"_mean");
        vec_csv.push_back(Variable.getValue()+"_var");
        vec_csv.push_back(Variable.getValue()+"_mean_zscore");
        vec_csv.push_back(Variable.getValue()+"_var_zscore");
        vec_csv.push_back("model_type");
        vec_csv.push_back("nugget");
        vec_csv.push_back("sill");
        vec_csv.push_back("partial_sill");
        vec_csv.push_back("range");
        vec_csv.push_back("range_max");
        vec_csv.push_back("range_min");
        vec_csv.push_back("range_z");
        for(uint col =0; col < vec_csv.size(); col++)
        {
            file_summary << vec_csv.at(col);
            if(col != vec_csv.size() - 1)
                file_summary << delimiter; // No comma at end of line
        }
        file_summary << "\n";
        ///////////////////////////////////////////////////////////////////////


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
            vec_csv.clear();

            count_frame++;

            filesystem::path dir = l;
            filesystem::path rel_datadir = filesystem::relative(dir, abs_datadir);
            vec_csv.push_back(rel_datadir.string());
            if(subDataset.isSet())
                vec_csv.push_back(subDataset.getValue());
            else
                vec_csv.push_back("");


            app_folder.clear();
            app_folder = out_folder + "/" + app_name;

            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);

            out_vario.clear();
            out_vario = out_folder + "/" + app_vario;
            out_man.clear();
            out_man = out_folder + "/" + app_manipulate;

            if(rel_datadir.string().compare(".") != 0)
            {
                app_folder += "/" + rel_datadir.string();
                out_vario += "/" + rel_datadir.string();
                out_man += "/" + rel_datadir.string();
                filesystem::create_directory(app_folder);

                std::cout << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << "### NUMBER OF TIME FRAMES: " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME N° " << count_frame << " ON " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME NAME: " << rel_datadir.string() << std::endl;
            }

            std::vector<std::string> list_json = get_files(l, ".json");
            if(list_json.size() > 1)
            {
                std::cerr << "ERROR. Only a file JSON is expected!" << std::endl;
                exit(1);
            }

            ComputeMeta metacompute;
            metacompute.setProject(Project);

            std::vector<std::string> excommands;
            excommands.push_back(command);
            metacompute.setCommands(excommands);

            out_vario += "/" + Variable.getValue();
            app_folder += "/" + Variable.getValue();
            if(subDataset.isSet())
            {
                out_vario += "_" + subDataset.getValue();
                app_folder += "_" + subDataset.getValue();
            }

            out_vario += "_" + varioDirection.getValue() + varioDimension.getValue();
            app_folder += "_" + varioDirection.getValue() + varioDimension.getValue();

            app_folder += "_" + get_basename(get_filename(geomModel.getValue()));
            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);

            // 0) Define meta for vario - dependencies
            std::vector<std::string> deps;

            filesystem::path rel_variopath = filesystem::relative(out_vario, Project.folder);
            if(subDataset.isSet())
                rel_variopath += "/" + Variable.getValue() + "_" + subDataset.getValue() + ".json";
            else
                rel_variopath += "/" + Variable.getValue() + ".json";
            deps.push_back(rel_variopath);

            filesystem::path rel_geompath = filesystem::relative(geomModel.getValue(), Project.folder);
            deps.push_back(get_basename(rel_geompath) + ".json");
            metacompute.setDependencies(deps);

            timing_logger.stop("inizialization");


            // ================================
            // 0. LOAD VALUES
            // ================================
            // Storing json information into class Data
            MUSE::Metadata meta_input;
            meta_input.read(l + "/metadata/" + Variable.getValue() + ".json");
            Data data = meta_input.getData(0);

            data.setType(data.flag);
            readTextValues(l + "/data/" + Variable.getValue() + ".dat", data.text_values);

            DataSummary sumdata;
            sumdata.setSummary(data);

            size_t n_sample = data.text_values.size();


            // 3. Check on json vario files
            std::vector<std::string> vario_files = get_files(out_vario, ".json");
            if(vario_files.size() == 0)
            {
                std::cerr << "=== ERROR: vario folder is empty!" << std::endl;
                exit(1);
            }

            if (data.type == varType::NUMBER)
            {
                if(vario_files.size() > 1)
                {
                    std::cerr << "=== ERROR: Number of JSON file is major than 1. Only a JSON is accepted for numerical type variable." << std::endl;
                    exit(1);
                }
            }

            
            VarioMeta metavario;
            metavario.read(vario_files.at(0));

            #ifdef DEBUG
            std::cout << std::endl;
            std::cout << FMAG("RICORDA - Le informazioni sul variogramma sono estratte dal primo file json!! Anche in condizioni di file json multipli (come può succedere in caso di variabili categoriche)") << std::endl;
            std::cout << std::endl;
            #endif

            std::vector<std::string> id;
            std::vector<double> xCoord, yCoord, zCoord;

            // LAMBDA FUNCTION TO APPLY ROTATION (IF SET):
            auto apply_rotation = [&](const MUSE::Rotation& rot)
            {
                cinolib::vec3d axis = set_rotation_axis(rot.rotation_axis);
                cinolib::vec3d c (rot.rotation_center_x, rot.rotation_center_y, rot.rotation_center_z);

                for(size_t i=0; i< xCoord.size(); i++)
                {
                    //rotazione coordinate all'inidice i
                    cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));
                    sample = point_rotation(sample, axis, rot.rotation_angle, c);

                    xCoord.at(i) = sample.x();
                    yCoord.at(i) = sample.y();
                    zCoord.at(i) = sample.z();
                }

                std::cout << "=== Rotation is activate on data ... " << rot.rotation << std::endl;
                std::cout << "=== Rotation axis: " << rot.rotation_axis << std::endl;
                std::cout << "=== Rotation center: [" << rot.rotation_center_x << "; " << rot.rotation_center_y << "; " << rot.rotation_center_z << "]" <<  std::endl;
                std::cout << "=== Rotation angle (degree): " << rot.rotation_angle << std::endl;
            };

            bool strat_transf = true;
            if(metavario.getManipulate().stratigraphic_transf.compare("NO") == 0)
                strat_transf = false;

            // ================================
            // 1. LOADING COORDINATES AND ID
            // ================================
            if(!strat_transf) //Condizione di default
            {
                std::cout << "\033[0;33m=== WARNING: No stratigraphic trasformation is set. The coordinate system remains unchanged.\033[0m" << std::endl;

                if(metavario.getInfoData().id_name.compare("Unknown") != 0)
                {
                    readTextValues(l + "/data/" + metavario.getInfoData().id_name + ".dat", id);
                    std::cout << "=== Loaded ... coordinates points ID - size: " << id.size() << std::endl;
                }
                else
                    std::cerr << "ERROR reading ID: " << l + "/data/" + metavario.getInfoData().id_name + ".dat" << " NOT found." << std::endl;

                if(metavario.getInfoData().x_name.compare("Unknown") != 0)
                {
                    readCoordinate(l + "/data/" + metavario.getInfoData().x_name + ".dat", xCoord);
                    std::cout << "=== Loaded ... X coordinates - size: " << xCoord.size() << std::endl;
                }
                else
                    std::cerr << "ERROR reading X coordinate: " << l + "/data/" + metavario.getInfoData().x_name + ".dat" << " NOT found." << std::endl;

                if(metavario.getInfoData().y_name.compare("Unknown") != 0)
                {
                    readCoordinate(l + "/data/" + metavario.getInfoData().y_name + ".dat", yCoord);
                    std::cout << "=== Loaded ... Y coordinates - size: " << yCoord.size() << std::endl;
                }   
                else
                    std::cerr << "ERROR reading Y coordinate: " << l + "/data/" + metavario.getInfoData().y_name + ".dat" << " NOT found." << std::endl;


                if(metavario.getInfoData().z_name.compare("Unknown") != 0)
                {
                    readCoordinate(l + "/data/" + metavario.getInfoData().z_name + ".dat", zCoord);
                    std::cout << "=== Loaded ... Z coordinates - size: " << zCoord.size() << std::endl;
                }
                else
                {
                    zCoord.resize(xCoord.size(), 0.0);
                    //std::fill(zCoord.begin(), zCoord.end(), 0.0);
                    std::cout << "=== Z coordinate is missing. Set it as 0.0 for all points." << std::endl;
                }
                std::cout << std::endl;
            }
            else
            {
                std::cout << "\033[0;33m=== WARNING: Stratigraphic transformation is set on " << metavario.getManipulate().stratigraphic_transf << ". Variogram is computed in stratigraphic coordinate system.\033[0m" << std::endl;
                std::cout << "=== Stratigraphic coordinates are located in " << out_man + "/" << std::endl;
                load_xyzfile(out_man + "/" + metavario.getManipulate().filename + ".xyz", xCoord, yCoord, zCoord);
                std::cout << "=== Loading coordinates points (x,y,z) in stratigraphic coordinate system ... COMPLETED." << std::endl;
            }

            // ================================
            // 2. CHECK DIMENSIONS
            // ================================
            if((xCoord.size() != yCoord.size()) || (xCoord.size() != zCoord.size()) || (yCoord.size() != zCoord.size()))
            {
                std::cerr << "=== ERROR in loading vector coordinates data. Please check the dimensions of the coordinate vectors." << std::endl;
                exit(1);
            }

            metacompute.setInfoData(metavario.getInfoData());

            // Check if the number of samples in the coordinate vectors matches the number of samples in the variable data
            if(xCoord.size() != n_sample)
            {
                std::cerr << "=== WARNING in loading data. The number of samples in the coordinate vectors does not match the number of samples in the variable data." << std::endl;
                std::cerr << "=== WARNING: mismatch coordinates (" << xCoord.size() << " vs " << n_sample << ")." << std::endl;
            }

            if(!strat_transf && setRotAxis.isSet()) //Se non sono in coordinate stratigrafiche, posso applicare una rotazione ai dati grezzi prima di qualsiasi altra operazione
            {
                std::cout << "=== Applying rotation on data before any other operation ... " << std::endl;

                MUSE::Rotation dataRotation_vario;

                dataRotation_vario.rotation = true;
                dataRotation_vario.rotation_axis = setRotAxis.getValue();
                dataRotation_vario.rotation_center_x = setRotCenterX.getValue();
                dataRotation_vario.rotation_center_y = setRotCenterY.getValue();
                dataRotation_vario.rotation_center_z = setRotCenterZ.getValue();
                dataRotation_vario.rotation_angle = setRotAngle.getValue();

                apply_rotation(dataRotation_vario);
                metacompute.setRotation(dataRotation_vario);
                std::cout << FGRN("=== Rotation on data ... COMPLETED.") << std::endl;
            }
                
            // String to double Conversion
            std::vector<std::string> corr_id;
            std::vector<double> conv_values, corr_x, corr_y, corr_z; //sampled data
            
            if(subDataset.isSet()) //sotto dataset da manipulate
            {
                std::cout << "=== Sub-dataset is set on " << subDataset.getValue() << ". Computation is performed on the sub-dataset selected by muse-manipulate." << std::endl;
                std::cout << "=== Reading ... " << out_man + "/" + metavario.getManipulate().domain + ".json" << std::endl;

                MUSE::ExtractionMeta extrmeta;
                extrmeta.read(out_man + "/" + metavario.getManipulate().domain + ".json");
                
                //2) ESTRARRE SOTTODATASET DA INDICI
                const auto& indices = extrmeta.getDataExtraction().id_points;
                if(indices.empty())
                {
                    std::cerr << "=== ERROR: Vector of index is empty." << std::endl;
                    std::cerr << "=== Please check the JSON file for the sub-dataset extraction: " << out_man + "/" + subDataset.getValue() + ".json" << std::endl;
                    std::cerr << "=== or use muse-manipulate (-E command)." << std::endl;
                    exit(1);
                }

                if(strat_transf)
                {
                    //Le coordinate sono già filtrate e ordinate secondo il vettore 'indices' 
                    //(prodotte da muse-manipulate): xCoord, yCoord, zCoord hanno già dimensione indices.size()
                    //Bisogna solo allinere i valori della variabile usando il vettore indices come riferimento
                    if(xCoord.size() != indices.size())
                    {
                        std::cerr << "=== ERROR: Mismatch between coordinate vectors and index vector for sub-dataset extraction." << std::endl;
                        std::cerr << "=== Coordinate vectors size: " << xCoord.size() << ", Index vector size: " << indices.size() << std::endl;
                        std::cerr << "=== Please check the JSON file for the sub-dataset extraction: " << out_man + "/" + subDataset.getValue() + ".json" << std::endl;
                        std::cerr << "=== or use muse-manipulate (-E command)." << std::endl;
                        exit(1);
                    }

                    for(size_t k=0; k<indices.size(); k++)
                    {
                        uint idx = indices.at(k);
                        if(idx >= data.text_values.size())
                        {
                            std::cerr << "=== ERROR: Index " << idx << " is out of bounds for variable data." << std::endl;
                            continue; //skip this index
                        }
                        std::string val_str = data.text_values.at(idx);
                        if(val_str.empty() || val_str == "nd" || val_str == "*")
                        {
                            std::cerr << "=== WARNING: Missing or invalid value at index " << idx << ". Skipping this sample." << std::endl;
                            continue; //skip this index
                        }

                        try
                        {
                            double val = std::stod(val_str);
                            conv_values.push_back(val);

                            if(!id.empty())
                                corr_id.push_back(id.at(k));
                            
                            corr_x.push_back(xCoord.at(k)); //k è l'indice del vettore indices, che è allineato con le coordinate filtrate
                            corr_y.push_back(yCoord.at(k));
                            corr_z.push_back(zCoord.at(k));
                        }
                        catch(const std::exception& e)
                        {
                            std::cerr << "=== ERROR: Exception while converting value at index " << idx << ": " << e.what() << std::endl;
                            std::cerr << "=== Invalid Value string: '" << val_str << "'" << std::endl;
                        }
                    }

                    std::cout << "=== Loaded sub-dataset with " << conv_values.size() << " valid values out of " << indices.size() << " sub-dataset indices." << std::endl;
                }
                else //no stratigraphic transformation, quindi le coordinate non sono state ancora filtrate e ordinate secondo il vettore 'indices'
                {
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

                        apply_rotation(dataRotation);
                        metacompute.setRotation(dataRotation);
                    }

                    for(uint i:indices)
                    {
                        if(i >= data.text_values.size())
                        {
                            std::cerr << "=== ERROR: Index " << i << " is out of bounds for variable data." << std::endl;
                            continue; //skip this index
                        }
                        std::string val_str = data.text_values.at(i);
                        if(val_str.empty() || val_str == "nd" || val_str == "*")
                        {
                            std::cerr << "=== WARNING: Missing or invalid value at index " << i << ". Skipping this sample." << std::endl;
                            continue; //skip this index
                        }

                        try
                        {
                            double val = std::stod(val_str);
                            conv_values.push_back(val);

                            if(!id.empty())
                                corr_id.push_back(id.at(i));
                            
                            corr_x.push_back(xCoord.at(i));
                            corr_y.push_back(yCoord.at(i));
                            corr_z.push_back(zCoord.at(i));
                        }
                        catch(const std::exception& e)
                        {
                            std::cerr << "=== ERROR: Exception while converting value at index " << i << ": " << e.what() << std::endl;
                            std::cerr << "=== Invalid Value string: '" << val_str << "'" << std::endl;
                        }
                    }
                }    
            }
            else
                string_to_double_conversion_vectors(data.text_values, id, xCoord, yCoord, zCoord, conv_values, corr_id, corr_x, corr_y, corr_z);
            
            std::cout << std::endl;

            int n_conv_samples = conv_values.size(); //numero campioni convertiti da stringa a double
            if(n_conv_samples == 0)
            {
                std::cerr << "=== ERROR: No valid sample is available for computation after conversion. Please check the variable data and the sub-dataset extraction." << std::endl;
                exit(1);
            }
            else
            {
                std::cout << "=== Data Statistical Summary ..." << std::endl;
                if(n_sample > n_conv_samples)
                    std::cout << "N (original)" << n_sample << std::endl;
                summary(conv_values);

                vec_csv.push_back(to_string(mean(conv_values)));
                vec_csv.push_back(to_string(variance(conv_values)));
            }

            std::cout << "\033[0;32mReading MUSE format and data analysis... COMPLETED.\033[0m" << std::endl;
            std::cout << std::endl;

            xCoord.clear();
            yCoord.clear();
            zCoord.clear();

            timing_logger.stop("data_loading");


            // ================================
            // STARTING COMPUTATION
            // ================================
            ComputeMeta::Processing infovar;
            if(metavario.getProcessing().v_name != Variable.getValue())
            {
                std::cerr << "=== ERROR: Variable name in JSON file (" << metavario.getProcessing().v_name << ") does not match the variable name set in command line (" << Variable.getValue() << ")." << std::endl;
                std::cerr << "=== Please check the JSON file for the variogram: " << vario_files.at(0) << std::endl;
                exit(1);
            }
            infovar.v_name = Variable.getValue();
            infovar.normal_score = metavario.getProcessing().normal_score;
            infovar.declustering = metavario.getProcessing().declustering;
            metacompute.setProcessing(infovar);

            switch (data.type)
            {
            case varType::CATEGORIC:
            {
                std::cout << std::endl;
                std::cout << FGRN("### VARTYPE CHECK: The variable is categoric.") << std::endl;
                //std::cout << "### Only Indicator Kriging is active for categoric variables" << std::endl;
                #ifdef DEBUG
                std::cout << FMAG("### RICORDA - In questo caso mi aspetto più file json, uno per ogni categoria") << std::endl;
                #endif
                std::cout << std::endl;


                // 1) Codifica ad indicatori prima di passare al variogramma!!
                std::vector<int> categ = categories_extraction(conv_values);

                //La trasformazione agli indicatori viene effettuata all'interno della funzione del kriging

                // 2) Preparazione dati di input -> vector<point3d>
                std::vector<point3d> input;
                for(uint i=0; i< conv_values.size(); i++)
                    input.push_back(point3d({corr_x.at(i), corr_y.at(i), corr_z.at(i)}, {conv_values.at(i)}));
                    //input.push_back(point3d({xCoord.at(i), yCoord.at(i), zCoord.at(i)}, {conv_values.at(i)}));


                // 3) Load geometry model
                std::string geom_name = geomModel.getValue().substr(geomModel.getValue().find_last_of("/")+1, geomModel.getValue().length());
                std::string ext_mesh = get_extension(geom_name);

                //Distinguo le mesh surf/vol in base all'estensione
                std::vector<point3d> nodes;
                MUSE::SurfaceMesh<> surf_mesh;
                MUSE::VolumeMesh<> vol_mesh;

                if(ext_mesh.compare(".off") == 0 || ext_mesh.compare(".obj") == 0) // Valutare successivamente se aggiungere il formato vtk per le mesh in input
                {
                    std::cout << "Mesh is surface." << std::endl;

                   // MUSE::SurfaceMesh<> surf_mesh;
                    //cinolib::Quadmesh<> surf_mesh;
                    surf_mesh.load(geomModel.getValue().c_str());

                    for(uint pid=0; pid<surf_mesh.num_polys(); pid++)
                    {
                        point3d p = point3d({surf_mesh.poly_centroid(pid).x(), surf_mesh.poly_centroid(pid).y(), surf_mesh.poly_centroid(pid).z()});
                        nodes.push_back(p);
                    }
                }
                else if(ext_mesh.compare(".mesh") == 0 || ext_mesh.compare(".vtk") == 0)
                {
                    std::cout << "Mesh is volumetric." << std::endl;

                   // MUSE::VolumeMesh<> vol_mesh;
                    vol_mesh.load(geomModel.getValue().c_str());

                    for(uint pid=0; pid<vol_mesh.num_polys(); pid++)
                    {
                        point3d p = point3d({vol_mesh.poly_centroid(pid).x(), vol_mesh.poly_centroid(pid).y(), vol_mesh.poly_centroid(pid).z()});
                        nodes.push_back(p);
                    }
                }
                else
                {
                    std::cerr << "Mesh format is not supported!" << std::endl;
                    exit(1);
                }

                timing_logger.stop("mesh_loading");

                std::vector<Variogram> variograms;

                VarioDirection dir;
                convert_from_str(metavario.getInfoVariogram().direction, dir);

                ComputeMeta::InfoVariogram info_vario;
                info_vario.dimension = metavario.getInfoVariogram().dimension;
                info_vario.direction = metavario.getInfoVariogram().direction;
                metacompute.setInfoVariogram(info_vario);


                #ifdef DEBUG
                std::cout << std::endl;
                std::cout << FMAG("L'ordine dei file corrisponde al vettore delle categorie!! Matching json-categoria rispettato per COSTRUZIONE!") << std::endl;
                std::cout << std::endl;
                #endif

                for(uint c=0; c<categ.size(); c++)
                {
                    std::cout << "=== Category ID: " << c << std::endl;
                    std::cout << "=== Category VALUE: " << categ.at(c) << std::endl;

                    std::string vario_name = out_vario + "/" + Variable.getValue() + std::to_string(categ.at(c));
                    if(subDataset.isSet())
                        vario_name += "_" + subDataset.getValue() + ".json";
                    else
                        vario_name += ".json";

                    std::cout << "=== Metadata file: " << vario_name << std::endl;

                    VarioMeta metavario_cat;
                    metavario_cat.read(vario_name);

                    Variogram fvm_cat;
                    MUSE::variogram_methods fitvariov;
                    switch (dir)
                    {
                    case VarioDirection::OMNI:
                    {
                        fvm_cat.set_range(metavario_cat.getFitExpVariog(0).range);
                        fvm_cat.nugget = metavario_cat.getFitExpVariog(0).nugget;

                        //MODIFICATO COME 1 - NUGGET!!!!!!!!!!!!!!!!!
                        fvm_cat.sill = metavario_cat.getFitExpVariog(0).sill - fvm_cat.nugget;

                        variogram_type type;
                        convert_from_str(metavario_cat.getFitExpVariog(0).type, type);
                        fvm_cat.type = type;

                        //for json
                        fitvariov.setNugget(fvm_cat.nugget);
                        fitvariov.setSill(fvm_cat.sill);
                        fitvariov.set_range(metavario_cat.getFitExpVariog(0).range);
                        fitvariov.setType(metavario_cat.getFitExpVariog(0).type);

                        break;
                    }
                    case VarioDirection::DIR:
                    {
                        //std::cout << "Range min is set on: " << metavario_cat.getSummary().min_semiaxis << std::endl;
                        //std::cout << "Range max is set on: " << metavario_cat.getSummary().max_semiaxis << std::endl;

                        if(setZRange.isSet())
                        {
                            //std::cout << "Range in Z direction is set on: " << setZRange.getValue() << std::endl;
                            fvm_cat.set_range(metavario_cat.getSummary().min_semiaxis, metavario_cat.getSummary().max_semiaxis, setZRange.getValue());
                        }
                        else
                            fvm_cat.set_range(metavario_cat.getSummary().min_semiaxis, metavario_cat.getSummary().max_semiaxis);


                        fvm_cat.set_azimuth(metavario_cat.getSummary().max_direction);
                        //std::cout << "Azimuth is set on max continuity direction: " << fvm_cat.get_azimuth() << " degree from North" << std::endl;

                        //Settati sulla massima direzione, ma non cambiano (per costruzione -> calcolo automatico del vario direzionale)
                        fvm_cat.nugget = metavario_cat.getFitExpVariog(0).nugget;
                        //std::cout << "Nugget is set on: " << fvm_cat.nugget << std::endl;

                        //MODIFICATO COME 1 - NUGGET!!!!!!!!!!!!!!!!!
                        fvm_cat.sill = metavario_cat.getFitExpVariog(0).sill - fvm_cat.nugget;
                        //std::cout << "Sill is set on: " << fvm_cat.sill << std::endl;

                        //anche il tipo è uguale tra tutti, quindi prendo quello a modello in dir 0
                        variogram_type type;
                        convert_from_str(metavario_cat.getFitExpVariog(0).type, type);
                        fvm_cat.type = type;
                        //std::cout << "Type is set on: " << metavario_cat.getFitExpVariog(0).type << std::endl;
                        //std::cout << std::endl;


                        //for json
                        fitvariov.setNugget(fvm_cat.nugget);
                        fitvariov.setSill(fvm_cat.sill);
                        fitvariov.range_max = fvm_cat.get_maxrange();
                        fitvariov.range_min = fvm_cat.get_minrange();
                        fitvariov.setType(metavario_cat.getFitExpVariog(0).type);

                        break;
                    }
                    }

                    std::cout << std::endl;
                    std::cout << "==========================================" << std::endl;
                    std::cout << FMAG("=== (Prior to simulation) Check variogram parameters ... ") << std::endl;
                    std::string string_type;
                    convert_to_str(string_type, fvm_cat.type);
                    std::cout << " | Azimuth is set on max continuity direction: " << fvm_cat.get_azimuth() << " degree from North" << std::endl;
                    std::cout << " | Type = " << string_type << std::endl;
                    std::cout << " | Dir max (azimuth) = " << fvm_cat.get_azimuth() << " degree from North." << std::endl;
                    std::cout << " | Range max = " << fvm_cat.get_maxrange() << std::endl;
                    std::cout << " | Range min = " << fvm_cat.get_minrange() << std::endl;
                    std::cout << " | Range z = " << fvm_cat.get_zrange() << std::endl;
                    std::cout << " | Nugget = " << fvm_cat.nugget << std::endl;
                    std::cout << " | Partial sill = " << fvm_cat.sill << std::endl;
                    std::cout << " | Sill = " << fvm_cat.sill + fvm_cat.nugget << std::endl;
                    
                    if(fvm_cat.type == GAUSSIAN)
                    {
                        std::cout << "=== Check on nugget for Gaussian model ..." << std::endl;
                        if(fvm_cat.nugget == 0.0)
                        {
                            std::cout << "=== Instability problems are encountered with a Gaussian model with no nugget effect." << std::endl;
                            fvm_cat.nugget = fvm_cat.nugget + 0.001;
                            fvm_cat.sill = fvm_cat.sill - 0.001;

                            std::cout << "=== Nugget value is perturbed as: " << fvm_cat.nugget << std::endl;
                            std::cout << "=== Updating sill value as: " << fvm_cat.sill << std::endl;
                        }
                    }

                    std::cout << "==========================================" << std::endl;
                    std::cout << std::endl;

                    variograms.push_back(fvm_cat);
                }
                timing_logger.stop("variogram_loading");


                //4. Starting simulations (CHOSEN THE INTERPOLATION METHOD: KRIGING, SGS??)
                std::cout << std::endl;
                std::cout << "=== Starting simulations ..." << std::endl;
                std::cout << "=== Chosen algorithm: " << setCRIT.getValue() << std::endl;
                std::cout << "=== Selected parameters:" << std::endl;
                
                std::cout << "=== | Number of input samples: " << setInputSamples.getValue() << std::endl;
                std::cout << "=== | Number of simulated points: " << setSimulatedPoints.getValue() << std::endl;
                std::cout << "=== | Set octant search: " << doOctantSearch.getValue() << std::endl;
                std::cout << "=== | Scale radius: " << setScaleRadius.getValue() << std::endl;
                std::cout << std::endl;
                
                ComputeMeta::Simulation sim;
                sim.geometry = geom_name;
                sim.sim_criterion = setCRIT.getValue();
                sim.n_iterations = setNsim.getValue();


                metacompute.setSimulation(sim);

                std::string indicator_json = app_folder + "/" + data.name;
                if(subDataset.isSet())
                    indicator_json += "_" + subDataset.getValue();

                // Start timing
                //timing_logger.start();
                //timing_logger.set_output_folder(app_folder);
                    
                if(setCRIT.getValue().compare("IK") == 0)
                {
                    std::cout << "=== Running Indicator Kriging ..." << std::endl;
                    indicator_kriging(nodes, input, categ, variograms, setInputSamples.getValue(), doOctantSearch.getValue(), setScaleRadius.getValue());
                    std::cout << FGRN("=== Indicator Kriging ... COMPLETED.") << std::endl;
                    metacompute.write(indicator_json + ".json");
                }
                else if(setCRIT.getValue().compare("SISIM") == 0)
                {
                    std::cout << "=== Running Indicator Simulation ..." << std::endl;
                    std::cout << "=== Number of simulations: " << setNsim.getValue() << std::endl;
                    if(ext_mesh == ".off" || ext_mesh == ".obj")
                    {
                        std::cout << "=== ... on surface mesh (off / obj formats)" << std::endl;
                        parallel_sis(nodes, surf_mesh ,input, categ, variograms, setNsim.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue());
                    }
                    else if(ext_mesh == ".mesh" || ext_mesh == ".vtk")
                    {
                        std::cout << "=== ... on volume mesh (mesh / vtk formats)" << std::endl;
                        parallel_sis(nodes, vol_mesh ,input, categ, variograms, setNsim.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue());

                    }

                    app_folder += "/_stats"; //sisim";
                    if(!filesystem::exists(app_folder))
                        filesystem::create_directory(app_folder);

                    //metacompute.write(indicator_json + ".json"); //AGGIORNARE INFORMAZIONI IN JSON!! (TO DO)
                    std::cout << FGRN("=== Indicator Simulation ... COMPLETED.") << std::endl;
                }
                else
                {
                    std::cerr << "=== ERROR. Default algorithm is not available for categorical variable." << std::endl;
                    std::cerr << "=== Please set --crit IK for Indicator Kriging or --crit SISIM for Indicator Simulation." << std::endl;
                    exit(1);
                }

                // Record SGS simulation time
                prefix_timing = setCRIT.getValue() + "_simulation";
                timing_logger.stop(prefix_timing);

                std::vector<double> results_x, results_y, results_z, results_v;
                for(uint n=0; n < nodes.size(); n++)
                {
                    results_x.push_back(nodes.at(n).get(0));
                    results_y.push_back(nodes.at(n).get(1));
                    results_z.push_back(nodes.at(n).get(2));
                    results_v.push_back(nodes.at(n).get_value(0));
                }
                if(setDebug.isSet())
                    export_idxyzv (app_folder + "/" + data.getName() + "_best_withlocations.csv", results_x, results_y, results_z, results_v);
                
                export1d_xyz (app_folder + "/" + data.getName() + "_best.csv", results_v);

                break;
            }
            case varType::NUMBER: //VARIABILE CONTINUA
            {
                //4. Starting simulations (CHOSEN THE INTERPOLATION METHOD: KRIGING, SGS??)
                if(setCRIT.getValue().compare("SGS") != 0)
                {
                    std::cerr << "=== ERROR. Algorithm "<< setCRIT.getValue() << " is not available for continous variable." << std::endl;
                    std::cerr << "=== Please set --crit SGS for Sequential Gaussian Simulation." << std::endl;
                    exit(1);
                }

                if(metavario.getProcessing().normal_score.compare("YES") != 0)
                {
                    std::cerr << "=== Normal Score Transformation is required for computing SGS algorithm." << std::endl;
                    exit(1);
                }


                normalscore normal_values;
                std::cout << "=== Loading normal score values ..." << out_vario + "/" + Variable.getValue() + "_nscore.dat" << std::endl;
                load_xyzfile(out_vario + "/" + Variable.getValue() + "_nscore.dat", normal_values.values, normal_values.x, normal_values.nsco);
                vec_csv.push_back(to_string(mean(normal_values.values)));
                vec_csv.push_back(to_string(variance(normal_values.values)));


                // 2. Read fitted variogram model from json (into vario folder)
                VarioDirection dir;
                convert_from_str(metavario.getInfoVariogram().direction, dir);

                ComputeMeta::InfoVariogram info_vario;
                info_vario.dimension = metavario.getInfoVariogram().dimension;
                info_vario.direction = metavario.getInfoVariogram().direction;
                metacompute.setInfoVariogram(info_vario);


                Variogram fvm;
                MUSE::variogram_methods fitvariov;
                switch (dir)
                {
                case VarioDirection::OMNI:
                {
                    fvm.set_range(metavario.getFitExpVariog(0).range);
                    fvm.nugget = metavario.getFitExpVariog(0).nugget;

                    //MODIFICATO COME 1 - NUGGET!!!!!!!!!!!!!!!!!
                    fvm.sill = metavario.getFitExpVariog(0).sill - fvm.nugget;

                    variogram_type type;
                    convert_from_str(metavario.getFitExpVariog(0).type, type);
                    fvm.type = type;

                    //for json
                    fitvariov.setNugget(fvm.nugget);
                    fitvariov.setSill(fvm.sill);
                    fitvariov.set_range(metavario.getFitExpVariog(0).range);
                    fitvariov.setType(metavario.getFitExpVariog(0).type);

                    break;
                }
                case VarioDirection::DIR:
                {
                    //std::cout << "Range min is set on: " << metavario.getSummary().min_semiaxis << std::endl;
                    //std::cout << "Range max is set on: " << metavario.getSummary().max_semiaxis << std::endl;

                    if(setZRange.isSet())
                    {
                        //std::cout << "Range in Z direction is set on: " << setZRange.getValue() << std::endl;
                        fvm.set_range(metavario.getSummary().min_semiaxis, metavario.getSummary().max_semiaxis, setZRange.getValue());
                    }
                    else
                        fvm.set_range(metavario.getSummary().min_semiaxis, metavario.getSummary().max_semiaxis);


                    fvm.set_azimuth(metavario.getSummary().max_direction);
                    //std::cout << "Azimuth is set on max continuity direction: " << fvm.get_azimuth() << " degree from North" << std::endl;

                    //Settati sulla massima direzione, ma non cambiano (per costruzione -> calcolo automatico del vario direzionale)
                    fvm.nugget = metavario.getFitExpVariog(0).nugget;
                    //std::cout << "Nugget is set on: " << fvm.nugget << std::endl;

                    //MODIFICATO COME 1 - NUGGET!!!!!!!!!!!!!!!!!
                    fvm.sill = metavario.getFitExpVariog(0).sill - fvm.nugget; //che deve essere ovviamente = 1
                    //std::cout << "Sill is set on: " << fvm.sill << std::endl;

                    //anche il tipo è uguale tra tutti, quindi prendo quello a modello in dir 0
                    variogram_type type;
                    convert_from_str(metavario.getFitExpVariog(0).type, type);
                    fvm.type = type;
                    //std::cout << "Type is set on: " << metavario.getFitExpVariog(0).type << std::endl;
                    //std::cout << std::endl;

                    //for json
                    fitvariov.setNugget(fvm.nugget);
                    fitvariov.setSill(fvm.sill);
                    fitvariov.range_max = fvm.get_maxrange();
                    fitvariov.range_min = fvm.get_minrange();
                    fitvariov.setRangeZ(fvm.get_zrange());
                    fitvariov.setType(metavario.getFitExpVariog(0).type);

                    break;
                }
                }
                metacompute.setFitExpVariog(fitvariov);

                vec_csv.push_back(fitvariov.type);
                vec_csv.push_back(to_string(fitvariov.nugget));
                vec_csv.push_back(to_string(1.0));
                vec_csv.push_back(to_string(fitvariov.sill));
                vec_csv.push_back(to_string(fitvariov.range));
                vec_csv.push_back(to_string(fitvariov.range_max));
                vec_csv.push_back(to_string(fitvariov.range_min));
                vec_csv.push_back(to_string(fitvariov.getRangeZ()));

                std::cout << std::endl;
                std::cout << "==========================================" << std::endl;
                std::cout << FMAG("=== (Prior to simulation) Check variogram parameters ... ") << std::endl;
                std::string string_type;
                convert_to_str(string_type, fvm.type);
                std::cout << " | Azimuth is set on max continuity direction: " << fvm.get_azimuth() << " degree from North" << std::endl;
                std::cout << " | Type = " << string_type << std::endl;
                std::cout << " | Dir max (azimuth) = " << fvm.get_azimuth() << " degree from North." << std::endl;
                std::cout << " | Range max = " << fvm.get_maxrange() << std::endl;
                std::cout << " | Range min = " << fvm.get_minrange() << std::endl;
                std::cout << " | Range z = " << fvm.get_zrange() << std::endl;
                std::cout << " | Nugget = " << fvm.nugget << std::endl;
                std::cout << " | Partial sill = " << fvm.sill << std::endl;
                std::cout << " | Sill = " << fvm.sill + fvm.nugget << std::endl;

                if(fvm.type == GAUSSIAN)
                {
                    std::cout << "=== Check on nugget for Gaussian model ..." << std::endl;
                    if(fvm.nugget == 0.0)
                    {
                        std::cout << "=== Instability problems are encountered with a Gaussian model with no nugget effect." << std::endl;
                        fvm.nugget = fvm.nugget + 0.001;
                        fvm.sill = fvm.sill - 0.001;

                        std::cout << "=== Nugget value is corrected as: " << fvm.nugget << std::endl;
                        std::cout << "=== Partial sill value is updated as: " << fvm.sill << std::endl;
                    }
                }
                std::cout << "==========================================" << std::endl;
                std::cout << std::endl;

                timing_logger.stop("variogram_loading");


                // 3. Load geometry model
                std::string geom_name = geomModel.getValue().substr(geomModel.getValue().find_last_of("/")+1, geomModel.getValue().length());
                std::string ext_mesh = get_extension(geom_name);


                SGSResults sgs_output;
                std::vector<std::vector<point3d>> sgs;

                ComputeMeta::Simulation sim;
                sim.geometry = geom_name;
                sim.sim_criterion = setCRIT.getValue();
                sim.n_iterations = setNsim.getValue();


                std::cout << "#########################"<< std::endl;
                std::cout << "### SGS Output type is set on " << setSGSoutput.getValue() << std::endl;

                bool back_normal_score_inSGS = false; //SGSresults non terranno in conto della back normal score (farla a posteriori)
                if(setBackNormalScore.isSet())
                {
                    back_normal_score_inSGS = true;
                    std::cout << "### Back Normal Score trasformation is performed into parallel_sgs algorithm." << std::endl;
                }
                else
                {
                    std::cout << "### Back Normal Score transformation is not performed into parallel_sgs algorithm." << std::endl;
                    std::cout << "### Simulation results have a normal distribution!" << std::endl;
                }
                std::cout << std::endl;

                std::cout << "### Number of input samples is set on " << setInputSamples.getValue() << std::endl;
                std::cout << "### Number of simulated points in SGS algorithm is set on " << setSimulatedPoints.getValue() << std::endl;
                std::cout << "### Scale factor for search radius is set on " << setScaleRadius.getValue() << std::endl;
                std::cout << "### Search by octant is set on " << doOctantSearch.getValue() << std::endl;
                std::cout << std::endl;
                std::cout << "######################### START SIMULATIONS ..."<< std::endl;


                //APPLICARE LA BACK NORMAL SCORE (SE NECESSARIO)
                if(!back_normal_score_inSGS)
                    sim.back_normal_score = false;
                else
                    sim.back_normal_score = true;

                sim.extrapolation_type = setExtrType.getValue();
                sim.min_extrapolation_value = setMinExtr.getValue();
                sim.max_extrapolation_value = setMaxExtr.getValue();

                //std::vector<MUSE::Data> vec_encode_output;
                MUSE::Metadata meta_output;
                std::vector<MUSE::Data> multi_output;

                ///////////// CENTROIDS
                std::vector<point3d> krigCenter;
                if(setFile.isSet())
                {
                    std::vector<double> krigCenter_x, krigCenter_y, krigCenter_z;
                    load_xyzfile(setFile.getValue(), krigCenter_x, krigCenter_y, krigCenter_z);
                    //std::cout << "SIZE = " << krigCenter_x.size() << std::endl;

                    for(uint pid=0; pid < krigCenter_x.size(); pid++)
                    {
                        point3d kC = point3d({krigCenter_x.at(pid), krigCenter_y.at(pid), krigCenter_z.at(pid)}, {0.0});
                        krigCenter.push_back(kC);
                    }
                }

                //Distinguo le mesh surf/vol in base all'estensione
                if(ext_mesh.compare(".off") == 0 || ext_mesh.compare(".obj") == 0)
                {
                    std::cout << "=== Mesh is surface." << std::endl;

                    MUSE::SurfaceMesh<> surf_mesh;
                    surf_mesh.load(geomModel.getValue().c_str());
                    sim.n_elements = surf_mesh.num_polys();

                    timing_logger.stop("mesh_loading");

                    
                    // std::cout << std::endl;
                    // std::cout << "==========================================" << std::endl;
                    // std::cout << FMAG("=== Check used variogram parameters ... ") << std::endl;
                    // std::string string_type;
                    // convert_to_str(string_type, fvm.type);
                    // std::cout << " | Type = " << string_type << std::endl;
                    // std::cout << " | Dir max (azimuth) = " << fvm.get_azimuth() << " degree from North." << std::endl;
                    // std::cout << " | Range max = " << fvm.get_maxrange() << std::endl;
                    // std::cout << " | Range min = " << fvm.get_minrange() << std::endl;
                    // std::cout << " | Range z = " << fvm.get_zrange() << std::endl;
                    // std::cout << " | Nugget = " << fvm.nugget << std::endl;
                    // std::cout << " | Sill = " << fvm.sill << std::endl;
                    // std::cout << "==========================================" << std::endl;
                    // std::cout << std::endl;

                    // if(fvm.type == GAUSSIAN)
                    // {
                    //     std::cout << "### Check on nugget for gaussian model ..." << std::endl;
                    //     if(fvm.nugget == 0.0)
                    //     {
                    //         std::cout << "Instability problems are encountered with a Gaussian model with no nugget effect." << std::endl;
                    //         fvm.nugget = fvm.nugget + 0.001;
                    //         fvm.sill = fvm.sill - 0.001;

                    //         std::cout << "Nugget value is perturbed as: " << fvm.nugget << std::endl;
                    //         std::cout << "Updating sill value as: " << fvm.sill << std::endl;
                    //     }
                    // }


                    // Start timing
                    //timing_logger.start();
                    //timing_logger.set_output_folder(app_folder);

                    if(setSGSoutput.getValue().compare("VECSIM") == 0)
                    {
                        #ifdef DEBUG
                        std::cout << std::endl;
                        std::cout << FMAG("### SGS OUTPUT - VECSIM: vector of simulation results in the normal space -> a CSV file for each simulation") << std::endl;
                        std::cout << FMAG("### Back normal score is managed by using the command -B") << std::endl;
                        std::cout << std::endl;
                        #endif

                        app_folder += "/_normspace";
                        if(!filesystem::exists(app_folder))
                            filesystem::create_directory(app_folder);

                        if(setFile.isSet())
                            sgs = parallel_sgs2 (krigCenter, normal_values.values, corr_x, corr_y, corr_z, fvm, setNsim.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue(), doOctantSearch.getValue());
                        else
                            sgs = parallel_sgs2 (surf_mesh, normal_values.values, corr_x, corr_y, corr_z, fvm, setNsim.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue(), doOctantSearch.getValue());

                        std::cout << std::endl;
                        for(uint it=0; it< setNsim.getValue(); it++)
                        {
                            std::vector<double> results_per_sim;

                            for(uint pid=0; pid < surf_mesh.num_polys(); pid++)
                            {
                                double v = sgs.at(it).at(pid).get_value(0);
                                results_per_sim.push_back(v);
                            }

                            stringstream filename_sim;
                            filename_sim << std::setw(4) << std::setfill('0') << it;
                            //filename_sim << data.name << "_sgs_" << std::setw(4) << std::setfill('0') << it;
                            //filename_sim << data.name << "_" << std::setw(4) << std::setfill('0') << it;
                            export1d_xyz (app_folder + "/" + data.getName() + "_" + filename_sim.str() + ".csv", results_per_sim);

                            if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
                            {
                                MUSE::Data encode_output = data;
                                encode_output.setFlag("R");
                                encode_output.setDescription(filename_sim.str());
                                encode_output.setComments(app_name + "OUTPUT-SIM");

                                multi_output.push_back(encode_output);
                            }
                            std::cout << FGRN("### Saving files for simulation ... ") << it << FGRN(" COMPLETED.") << std::endl;
                        }

                        std::string out_filename = app_folder + "/" + data.name;
                        if(subDataset.isSet())
                            out_filename += "_" + subDataset.getValue();

                        metacompute.setSimulation(sim);
                        metacompute.write(out_filename + ".json");

                        std::vector<std::string> deps_sgs;
                        filesystem::path realpath_sgs = filesystem::relative(out_filename + ".json", abspath);
                        deps_sgs.push_back(realpath_sgs);
                        meta_output.setDependencies(deps_sgs);
                    }
                    else if(setSGSoutput.getValue().compare("MEAN") == 0)
                    {
                        std::cout << std::endl;
                        std::cout << FMAG("### SGS OUTPUT - MEAN: mean of estimates in the variable space -> unique CSV file") << std::endl;
                        std::cout << FMAG("### Back normal score is managed in the SGS algorithm") << std::endl;
                        std::cout << std::endl;

                        sgs_output = parallel_sgs2 (surf_mesh, normal_values.values, corr_x, corr_y, corr_z, fvm, setNsim.getValue(), normal_values,
                                                back_normal_score_inSGS, setExtrType.getValue(), setMinExtr.getValue(), setMaxExtr.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue(), doOctantSearch.getValue());



                        std::string out_filename = app_folder + "/" + data.name;
                        if(subDataset.isSet())
                            out_filename += "_" + subDataset.getValue();

                        export1d_xyz (out_filename + ".csv", sgs_output.estimates);
                        export1d_xyz (out_filename + "_unc.csv", sgs_output.uncertainties);

                        metacompute.setSimulation(sim);
                        metacompute.write(out_filename + ".json");

                        double max_unc =  -DBL_MAX;
                        uint id_max = 0;
                        for(uint pid=0; pid<surf_mesh.num_polys(); pid++)
                        {
                            if(sgs_output.uncertainties.at(pid) >= max_unc)
                            {
                                max_unc = sgs_output.uncertainties.at(pid);
                                id_max = pid;
                            }
                        }
                        std::cout << "### Max uncertainty: " << max_unc << "at point " << surf_mesh.poly_centroid(id_max) << std::endl;



                        //MESH IN COORDINATE STRATIGRAFICHE
                        if(!filesystem::exists(app_folder + "/_fordebug"))
                            filesystem::create_directory(app_folder + "/_fordebug");

                        std::ofstream file_out1;
                        file_out1.open(app_folder + "/_fordebug/" + Variable.getValue() + "centr_sim" + to_string(setNsim.getValue()) + ".dat", std::fstream::out);
                        if(!file_out1.is_open())
                        {
                            std::cerr << "\033[0;31mError in file opening: \033[0m" << std::endl;
                            exit(1);
                        }

                        else
                        {
                            for(uint pid = 0; pid < surf_mesh.num_polys(); pid++)
                            {
                                cinolib::vec3d centroid = surf_mesh.poly_centroid(pid);
                                file_out1 << std::setprecision(4) << centroid.x() << " " << centroid.y() << " " << centroid.z() << " " << sgs_output.estimates.at(pid) << std::endl;
                            }
                            file_out1.close();
                        }
                        std::cout << FGRN("Saving files in _fordebug folder ... COMPLETED.") << std::endl;

                        if (metavario.getManipulate().stratigraphic_transf.compare("NO") != 0)
                        {
                            //MESH IN COORDINATE CARTESIANE
                            //cinolib::Trimesh<> surf_mesh_piega;
                            MUSE::SurfaceMesh<> surf_mesh_piega;
                            std::string surf_name = geom_name.substr(geom_name.find_first_of("_")+1, Project.folder.length()); //nome progetto
                            std::string surf_filename = out_folder + "/geometry/surf/" + surf_name;
                            surf_mesh_piega.load(surf_filename.c_str());

                            std::ofstream file_out2;
                            file_out2.open(app_folder + "/_fordebug/" + Variable.getValue() + "centr_sim" + to_string(setNsim.getValue()) + "_original.dat", std::fstream::out);
                            if(!file_out2.is_open())
                            {
                                std::cerr << "\033[0;31mError in file opening: \033[0m" << std::endl;
                                exit(1);
                            }

                            else
                            {
                                for(uint pid = 0; pid < surf_mesh_piega.num_polys(); pid++)
                                {
                                    cinolib::vec3d centroid = surf_mesh_piega.poly_centroid(pid);
                                    file_out2 << std::setprecision(4) << centroid.x() << " " << centroid.y() << " " << centroid.z() << " " << sgs_output.estimates.at(pid) << std::endl;
                                }
                                file_out2.close();
                            }
                            std::cout << FGRN("Saving files (stratigraphic condition - original model) in _fordebug folder ... COMPLETED.") << std::endl;
                        }
                    }
                }
                else if (ext_mesh.compare(".mesh") == 0 || ext_mesh.compare(".vtk") == 0)
                {
                    // std::cout << std::endl;
                    // std::cout << "==========================================" << std::endl;
                    // std::cout << FMAG("=== (Prior to simulations) Check variogram parameters ... ") << std::endl;
                    // std::cout << FMAG("PER CONTROLLO (PRIMA DELLE SIMULAZIONI):") << std::endl;
                    // std::cout << FMAG("La funzione delle SGS considera il VARIO DIREZIONALE con i seguenti parametri: ") << std::endl;
                    // std::string string_type;
                    // convert_to_str(string_type, fvm.type);
                    // std::cout << "Type = " << string_type << std::endl;
                    // std::cout << "Dir max (azimuth) = " << fvm.get_azimuth() << " degree from North." << std::endl;
                    // std::cout << "Range max = " << fvm.get_maxrange() << std::endl;
                    // std::cout << "Range min = " << fvm.get_minrange() << std::endl;
                    // std::cout << "Range z = " << fvm.get_zrange() << std::endl;
                    // std::cout << "Nugget = " << fvm.nugget << std::endl;
                    // std::cout << "Sill = " << fvm.sill << std::endl;
                    // std::cout << "==========================================" << std::endl;
                    // std::cout << std::endl;

                    std::cout << "=== Mesh is volumetric." << std::endl;

                    MUSE::VolumeMesh<> vol_mesh;
                    vol_mesh.load(geomModel.getValue().c_str());

                    sim.n_elements = vol_mesh.num_polys();
                    timing_logger.stop("mesh_loading");

                    // Start timing
                    //timing_logger.start();
                    //timing_logger.set_output_folder(app_folder);
                    if(setSGSoutput.getValue().compare("VECSIM") == 0)
                    {
                        #ifdef DEBUG
                        std::cout << std::endl;
                        std::cout << FMAG("### SGS OUTPUT - VECSIM: vector of simulation results in the normal space -> a CSV file for each simulation") << std::endl;
                        std::cout << FMAG("### Back normal score is managed by using the command -B") << std::endl;
                        std::cout << std::endl;
                        #endif

                        app_folder += "/_normspace";
                        if(!filesystem::exists(app_folder))
                            filesystem::create_directory(app_folder);

                        sgs = parallel_sgs2 (vol_mesh, normal_values.values, corr_x, corr_y, corr_z, fvm, setNsim.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue(), doOctantSearch.getValue());

                        for(uint it=0; it< setNsim.getValue(); it++)
                        {
                            std::vector<double> results_per_sim;

                            for(uint pid=0; pid < vol_mesh.num_polys(); pid++)
                            {
                                double v = sgs.at(it).at(pid).get_value(0);
                                results_per_sim.push_back(v);
                            }

                            stringstream filename_sim;
                            filename_sim << std::setw(4) << std::setfill('0') << it;
                            export1d_xyz (app_folder + "/" + data.getName() + "_" + filename_sim.str() + ".csv", results_per_sim);

                            if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
                            {
                                MUSE::Data encode_output = data;
                                encode_output.setFlag("R");
                                encode_output.setDescription(filename_sim.str());
                                encode_output.setComments(app_name + "OUTPUT-SIM");

                                multi_output.push_back(encode_output);
                            }
                            std::cout << FGRN("### Saving files for simulation ... ") << it << FGRN(" COMPLETED.") << std::endl;
                        }

                        std::string out_filename = app_folder + "/" + data.name;
                        if(subDataset.isSet())
                            out_filename += "_" + subDataset.getValue();

                        metacompute.setSimulation(sim);
                        metacompute.write(out_filename + ".json");

                        std::vector<std::string> deps_sgs;
                        filesystem::path realpath_sgs = filesystem::relative(out_filename + ".json", abspath);
                        deps_sgs.push_back(realpath_sgs);
                        meta_output.setDependencies(deps_sgs);

                    }
                    else if(setSGSoutput.getValue().compare("MEAN") == 0)
                    {
                        std::cout << std::endl;
                        std::cout << FMAG("### SGS OUTPUT - MEAN: mean of estimates in the variable space -> unique CSV file") << std::endl;
                        std::cout << FMAG("### Back normal score is managed in the SGS algorithm") << std::endl;
                        std::cout << std::endl;

                        sgs_output = parallel_sgs2 (vol_mesh, normal_values.values, corr_x, corr_y, corr_z, fvm, setNsim.getValue(), normal_values,
                                                        back_normal_score_inSGS, setExtrType.getValue(), setMinExtr.getValue(), setMaxExtr.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue(), doOctantSearch.getValue());

                        std::string out_filename = app_folder + "/" + data.name;
                        if(subDataset.isSet())
                            out_filename += "_" + subDataset.getValue();

                        export1d_xyz (out_filename + ".csv", sgs_output.estimates);
                        export1d_xyz (out_filename + "_unc.csv", sgs_output.uncertainties);

                        metacompute.setSimulation(sim);
                        metacompute.write(out_filename + ".json");


                        //MESH IN COORDINATE STRATIGRAFICHE
                        if(!filesystem::exists(app_folder + "/_fordebug"))
                            filesystem::create_directory(app_folder + "/_fordebug");

                        std::ofstream file_out1;
                        file_out1.open(app_folder + "/_fordebug/" + Variable.getValue() + "centr_sim" + to_string(setNsim.getValue()) + ".dat", std::fstream::out);
                        if(!file_out1.is_open())
                        {
                            std::cerr << "\033[0;31mError in file opening: \033[0m" << std::endl;
                            exit(1);
                        }
                        else
                        {
                            for(uint pid = 0; pid < vol_mesh.num_polys(); pid++)
                            {
                                cinolib::vec3d centroid = vol_mesh.poly_centroid(pid);
                                file_out1 << std::setprecision(4) << centroid.x() << " " << centroid.y() << " " << centroid.z() << " " << sgs_output.estimates.at(pid) << std::endl;
                            }
                            file_out1.close();
                        }
                        std::cout << FGRN("Saving files in _fordebug folder ... COMPLETED.") << std::endl;

                        if (metavario.getManipulate().stratigraphic_transf.compare("NO") != 0)
                        {
                            std::cout << FYEL("SAVE fordebug file for model in stratigraphic coordinate ... TO DO!!") << std::endl;

                            //MESH IN COORDINATE CARTESIANE
                            MUSE::VolumeMesh<> vol_mesh_piega;
                            std::string surf_name = geom_name.substr(geom_name.find_first_of("_")+1, Project.folder.length()); //nome progetto
                            std::string surf_filename = out_folder + "/geometry/volume/" + surf_name;
                            vol_mesh_piega.load(surf_filename.c_str());

                            std::ofstream file_out2;
                            file_out2.open(app_folder + "/_fordebug/" + Variable.getValue() + "centr_sim" + to_string(setNsim.getValue()) + "_original.dat", std::fstream::out);
                            if(!file_out2.is_open())
                            {
                                std::cerr << "\033[0;31mError in file opening: \033[0m" << std::endl;
                                exit(1);
                            }

                            else
                            {
                                for(uint pid = 0; pid < vol_mesh_piega.num_polys(); pid++)
                                {
                                    cinolib::vec3d centroid = vol_mesh_piega.poly_centroid(pid);
                                    file_out2 << std::setprecision(4) << centroid.x() << " " << centroid.y() << " " << centroid.z() << " " << sgs_output.estimates.at(pid) << std::endl;
                                }
                                file_out2.close();
                            }
                            std::cout << FGRN("Saving files (stratigraphic condition - original model) in _fordebug folder ... COMPLETED.") << std::endl;
                        }
                    }
                }
                else
                {
                    std::cerr << "ERROR: Mesh format is not supported." << std::endl;
                    exit(1);
                }

                // Record SGS simulation time
                prefix_timing = setCRIT.getValue() + "_simulation";
                timing_logger.stop(prefix_timing);


                meta_output.setProject(Project);
                meta_output.setDependencies(deps);
                meta_output.setCommands(excommands);
                meta_output.setMultiData(multi_output);
                meta_output.write(app_folder + "/_" + data.name + ".json");

                for(uint col =0; col < vec_csv.size(); col++)
                {
                    file_summary << vec_csv.at(col);
                    if(col != vec_csv.size() - 1)
                        file_summary << delimiter; // No comma at end of line
                }
                file_summary << "\n";

                break;
            }
            case COORDINATE:
            case ERROR:
            case ID:
            case TEXT:
                break;
            }
        }

        file_summary.close();// Close the file
        std::cout << FGRN("Save summary of multi-frame variography analysis ... COMPLETED.") << std::endl;
    }


    //---------------------------------------------------
    //---------------------------------------------------
    //---------------------------------------------------
    //---> STATISTICAL SWITCH ARGUMENT
    //---------------------------------------------------
    //---------------------------------------------------
    //---------------------------------------------------
    if(statisticalAnalysis.isSet())
    {
        if(!Variable.isSet())
        {
            std::cout << FRED("ERROR. Variable is NOT set!") << std::endl;
            exit(1);
        }

        if(!geomModel.isSet())
        {
            std::cout << FRED("ERROR. Geometry support is NOT set!") << std::endl;
            //exit(1);
        }

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
            //vec_csv.clear();

            count_frame++;

            filesystem::path dir = l;
            filesystem::path rel_datadir = filesystem::relative(dir, abs_datadir);
            //std::cout << rel_datadir.string() << std::endl;
            // vec_csv.push_back(rel_datadir.string());
            // if(subDataset.isSet())
            //     vec_csv.push_back(subDataset.getValue());
            // else
            //     vec_csv.push_back("");

            app_folder.clear();
            app_folder = out_folder + "/" + app_name;

            if(rel_datadir.string().compare(".") != 0)
            {
                app_folder += "/" + rel_datadir.string();

                std::cout << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << "### NUMBER OF TIME FRAMES: " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME N° " << count_frame << " ON " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME NAME: " << rel_datadir.string() << std::endl;
                std::cout << std::endl;
            }
            app_folder += "/" + Variable.getValue();

            // 1. Define filename vario
            if(subDataset.isSet())
                app_folder += "_" + subDataset.getValue();
            app_folder += "_" + varioDirection.getValue() + varioDimension.getValue();

            app_folder += "_" + get_basename(get_filename(geomModel.getValue()));
            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);

            app_folder += "/_normspace";
            if(setSpace.getValue().compare("NORMAL") != 0)
                app_folder += "/../_varspace";

            std::string out_filename = app_folder + "/" + Variable.getValue();
            if(subDataset.isSet())
                out_filename += "_" + subDataset.getValue();

            // Check su file list nella cartella di compute
            std::vector<std::string> file_list; // = get_xyzfiles(app_folder);
            for (const auto& entry : std::filesystem::directory_iterator(app_folder))
            {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    std::string search_string = Variable.getValue() + "_";
                    // Check if the filename matches the pattern "name_XXXX.csv"
                    if (filename.find(search_string) == 0 && filename.substr(filename.size() - 4) == ".csv") {
                        file_list.push_back(filename);
                        std::cout << "=== Found simulation file ... " << filename << std::endl;
                    }
                }
            }
            if (file_list.size() == 0)
            {
                std::cerr << "=== ERROR: " << app_folder << " is empty!" << std::endl;
                exit(1);
            }
            std::cout << "=== Number of simulation files found in " << app_folder << " is: " << file_list.size() << std::endl;
            std::cout << std::endl;

            // Sort the files based on the numeric part extracted from the filenames
            std::sort(file_list.begin(), file_list.end(), [](const std::string& a, const std::string& b) {
                return extractNumber(a) < extractNumber(b);
            });

            //std::string name_sim = get_basename(get_filename(file_list.at(0)));
            std::string name_sim = get_basename(file_list.at(0));
            name_sim = name_sim.substr(0, name_sim.find_last_of("_"));

            //per ogni elemento, estraggo i valori ottenuti dalle singole simulazioni
            std::vector<std::vector<double>> results_sgs;
            for(size_t i=0; i < file_list.size(); i++)
            {
                std::vector<double> results_per_it;
                load1d_xyzfile(app_folder + "/" + file_list.at(i), results_per_it);

                results_sgs.push_back(results_per_it);
            }
            //exit(1);

            // Lettura metadato di compute
            ComputeMeta metacompute;
            metacompute.read(out_filename + ".json");

            if(metacompute.getSimulation().n_elements == 0)
            {
                std::cerr << "ERROR: number of mesh elements is null!" << std::endl;
                exit(1);
            }
            if(metacompute.getSimulation().n_iterations == 0)
            {
                std::cerr << "ERROR: number of iterations is null!" << std::endl;
                exit(1);
            }

            std::vector<double> mean_pids, var_pids, mean_plus_stdev_pids, mean_minus_stdev_pids, med_pids, q1_pids, q3_pids, p95_pids, qcd_pids, stdevlower_pids, stdevupper_pids; //stdev_pids

            std::cout << "### Computing statistics for probability distribution in " << setSpace.getValue() << " SPACE ... " << std::endl;
            for(uint pid=0; pid < metacompute.getSimulation().n_elements; pid++)
            {
                std::vector<double> results_per_pid;
                for(int it=0; it< metacompute.getSimulation().n_iterations; it++)
                {
                    //std::cout << results_sgs.at(it).at(pid) << std::endl;
                    results_per_pid.push_back(results_sgs.at(it).at(pid));
                }

                //COMPUTING STATISTICS FOR NORMAL/VARIABLE SPACE
                if(setSpace.getValue().compare("NORMAL") == 0)
                {
                    mean_pids.push_back(mean(results_per_pid));         //MEAN
                    var_pids.push_back(variance(results_per_pid));      //VARIANCE

                    stdevlower_pids.push_back(stdev(results_per_pid));  //DEVSTD-_SIMMETRIC
                    stdevupper_pids.push_back(stdev(results_per_pid));  //DEVSTD+_SIMMETRIC

                    med_pids.push_back(median(results_per_pid));        //MEDIAN

                    double q1 = quartile(results_per_pid, 0.25);        //QUARTILE Q1-0.25
                    double q3 = quartile(results_per_pid, 0.75);        //QUARTILE Q3-0.75
                    //double q2 = quartile(results_per_pid, 0.5);        //QUARTILE Q2-0.5
                    double p95 = quartile(results_per_pid, 0.95);       //95esimo PERCENTILE 0.95

                    //std::cout << q2 << std::endl;

                    q1_pids.push_back(q1);
                    q3_pids.push_back(q3);
                    p95_pids.push_back(p95);
                }
                else
                {
                    //std::cout << "### Computing statistics for probability distribution in VARIABLE SPACE: " << std::endl;

                    mean_pids.push_back(mean(results_per_pid));         //MEAN
                    var_pids.push_back(variance(results_per_pid));      //VARIANCE

                    double q1 = quartile(results_per_pid, 0.25);        //QUARTILE Q1-0.25
                    double q2 = quartile(results_per_pid, 0.5);         //QUARTILE Q2-0.5 (MEDIAN)
                    double q3 = quartile(results_per_pid, 0.75);        //QUARTILE Q3-0.75
                    double p95 = quartile(results_per_pid, 0.95);       //95esimo PERCENTILE 0.95

                    //Deviazione standard calcolata intorno alla mediana
                    //1) Calcolo dei percentili: area sottesa dalla curva - valori inferiori a 0.1/0.84
                    double p16 = quartile(results_per_pid, 0.16); //PERCENTILE 0.16
                    double p84 = quartile(results_per_pid, 0.84); //PERCENTILE 0.84

                    double stdevlower = q2 - p16; //DEVSTD-_ASIMMETRIC 0.16 = MEDIANA - P16
                    double stdevupper = p84 - q2; //DEVSTD+_ASIMMETRIC 0.84 = P84 - MEDIANA

                    //double stdevlower = mean(results_per_pid) - p16; //DEVSTD-_ASIMMETRIC 0.16 = MEDIANA - P16
                    //double stdevupper = p84 - mean(results_per_pid); //DEVSTD+_ASIMMETRIC 0.84 = P84 - MEDIANA
                    stdevlower_pids.push_back(stdevlower);
                    stdevupper_pids.push_back(stdevupper);

                    q1_pids.push_back(q1);
                    //q2_pids.push_back(q2);
                    med_pids.push_back(q2);
                    q3_pids.push_back(q3);

                    p95_pids.push_back(p95);
                }
            }

            for(size_t i=0; i<mean_pids.size(); i++)
            {
                mean_plus_stdev_pids.push_back(mean_pids.at(i) + stdevupper_pids.at(i));    //MEAN + DEVSTD+_A/SIMMETRIC
                mean_minus_stdev_pids.push_back(mean_pids.at(i) - stdevlower_pids.at(i));   //MEAN - DEVSTD-_A/SIMMETRIC

                double qcd = (q3_pids.at(i) - q1_pids.at(i))/(q3_pids.at(i) + q1_pids.at(i));
                qcd_pids.push_back(qcd);
            }


            app_folder += "/_stats";
            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);

            std::string ext = ".dat";
            if(csvConversion.isSet() == true)
                ext = ".csv";

            MUSE::Metadata meta_input, meta_output;
            meta_input.read(l + "/metadata/" + Variable.getValue() + ".json");

            meta_output.setProject(Project);

            std::vector<std::string> excommands;
            excommands.push_back(command);
            meta_output.setCommands(excommands);

            std::vector<std::string> deps;
            filesystem::path realpath = filesystem::relative(out_filename + ".json", abspath);
            deps.push_back(realpath);
            meta_output.setDependencies(deps);


            std::vector<MUSE::Data> multi_output;
            //std::string appendix = setSpace.getValue().compare("NORMAL") == 0 ? "NORMSP" : "VARSP";

            export1d_xyz (app_folder + "/" + name_sim + "_mean" + ext, mean_pids, 2);
            if(setFormat.getValue().compare("YES")==0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
            {
                MUSE::Data encode_output = meta_input.getData(0);
                if(setSpace.getValue().compare("NORMAL") == 0)
                    encode_output.setFlag("R");
                encode_output.setComments(app_name + "OUTPUT-STATS");
                encode_output.setDescription("mean");
                multi_output.push_back(encode_output);

                // vec_csv.push_back(to_string(mean(mean_pids)));
                // vec_csv.push_back(to_string(variance(mean_pids)));
            }

            export1d_xyz (app_folder + "/" + name_sim + "_var" + ext, var_pids, 2);
            if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
            {
                MUSE::Data encode_output = meta_input.getData(0);
                if(setSpace.getValue().compare("NORMAL") == 0)
                    encode_output.setFlag("R");
                encode_output.setUnit(encode_output.getUnit()+"2");
                encode_output.setComments(app_name + "OUTPUT-STATS");
                encode_output.setDescription("var");
                multi_output.push_back(encode_output);
            }

            // export1d_xyz (app_folder + "/" + name_sim + "_stdev" + ext, stdev_pids);
            // if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
            // {
            //     MUSE::Data encode_output = meta_input.getData(0);
            //     if(setSpace.getValue().compare("NORMAL") == 0)
            //         encode_output.setFlag("R");
            //     encode_output.setComments(app_name + "OUTPUT-STATS");
            //     encode_output.setDescription("stdev");
            //     multi_output.push_back(encode_output);
            // }

            export1d_xyz (app_folder + "/" + name_sim + "_mean_p_stdev" + ext, mean_plus_stdev_pids, 2);
            if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
            {
                MUSE::Data encode_output = meta_input.getData(0);
                if(setSpace.getValue().compare("NORMAL") == 0)
                    encode_output.setFlag("R");
                encode_output.setComments(app_name + "OUTPUT-STATS");
                encode_output.setDescription("mean_p_stdev");
                multi_output.push_back(encode_output);
            }

            export1d_xyz (app_folder + "/" + name_sim + "_mean_m_stdev" + ext, mean_minus_stdev_pids, 2);
            if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
            {
                MUSE::Data encode_output = meta_input.getData(0);
                if(setSpace.getValue().compare("NORMAL") == 0)
                    encode_output.setFlag("R");
                encode_output.setComments(app_name + "OUTPUT-STATS");
                encode_output.setDescription("mean_m_stdev");
                multi_output.push_back(encode_output);
            }

            export1d_xyz (app_folder + "/" + name_sim + "_median" + ext, med_pids, 2);
            if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
            {
                MUSE::Data encode_output = meta_input.getData(0);
                if(setSpace.getValue().compare("NORMAL") == 0)
                    encode_output.setFlag("R");
                encode_output.setComments(app_name + "OUTPUT-STATS");
                encode_output.setDescription("median");
                multi_output.push_back(encode_output);
            }

            export1d_xyz (app_folder + "/" + name_sim + "_95percentile" + ext, p95_pids, 2);
            if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
            {
                MUSE::Data encode_output = meta_input.getData(0);
                if(setSpace.getValue().compare("NORMAL") == 0)
                    encode_output.setFlag("R");
                encode_output.setComments(app_name + "OUTPUT-STATS");
                encode_output.setDescription("95percentile");
                multi_output.push_back(encode_output);
            }

            export1d_xyz (app_folder + "/" + name_sim + "_Q1" + ext, q1_pids, 2);
            if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
            {
                MUSE::Data encode_output = meta_input.getData(0);
                if(setSpace.getValue().compare("NORMAL") == 0)
                    encode_output.setFlag("R");
                encode_output.setComments(app_name + "OUTPUT-STATS");
                encode_output.setDescription("Q1");
                multi_output.push_back(encode_output);
            }

            export1d_xyz (app_folder + "/" + name_sim + "_Q3" + ext, q3_pids, 2);
            if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
            {
                MUSE::Data encode_output = meta_input.getData(0);
                if(setSpace.getValue().compare("NORMAL") == 0)
                    encode_output.setFlag("R");
                encode_output.setComments(app_name + "OUTPUT-STATS");
                encode_output.setDescription("Q3");
                multi_output.push_back(encode_output);
            }

            export1d_xyz (app_folder + "/" + name_sim + "_QCD" + ext, qcd_pids, 2);
            if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
            {
                MUSE::Data encode_output = meta_input.getData(0);
                if(setSpace.getValue().compare("NORMAL") == 0)
                    encode_output.setFlag("R");
                encode_output.setUnit(" ");
                encode_output.setComments(app_name + "OUTPUT-STATS");
                encode_output.setDescription("QCD");
                multi_output.push_back(encode_output);
            }
            meta_output.setMultiData(multi_output);
            meta_output.write(app_folder + "/" + name_sim + ".json");

            std::cout << FGRN("Statistics on values ... COMPLETED.") << std::endl;

            // Record statistics computation time
            prefix_timing = "stats_"+setSpace.getValue();
            timing_logger.stop(prefix_timing);
        }
    }


    //---------------------------------------------------
    //---------------------------------------------------
    //---------------------------------------------------
    //---> BACK NORMAL SCORE TRANSFORMATION SWITCH ARGUMENT
    //---------------------------------------------------
    //---------------------------------------------------
    //---------------------------------------------------
    if(doBackNormalScore.isSet())
    {
        ComputeMeta metacompute;
        if(!Variable.isSet())
        {
            std::cout << FRED("ERROR. Variable is NOT set!") << std::endl;
            exit(1);
        }

        if(!geomModel.isSet())
        {
            std::cout << FRED("ERROR. Geometry support is NOT set!") << std::endl;
            //exit(1);
        }

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

            out_vario.clear();
            out_vario = out_folder + "/" + app_vario;

            if(rel_datadir.string().compare(".") != 0)
            {
                app_folder += "/" + rel_datadir.string();
                out_vario += "/" + rel_datadir.string();

                std::cout << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << "### NUMBER OF TIME FRAMES: " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME N° " << count_frame << " ON " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME NAME: " << rel_datadir.string() << std::endl;
                std::cout << std::endl;
            }
            app_folder += "/" + Variable.getValue();
            out_vario += "/" + Variable.getValue();

            // 1. Define filename vario
            if(subDataset.isSet())
            {
                app_folder += "_" + subDataset.getValue();
                out_vario += "_" + subDataset.getValue();
            }
            app_folder += "_" + varioDirection.getValue() + varioDimension.getValue();
            out_vario += "_" + varioDirection.getValue() + varioDimension.getValue();

            app_folder += "_" + get_basename(get_filename(geomModel.getValue()));

            std::string norm_path = app_folder + "/_normspace";
            std::string var_path = app_folder + "/_varspace";


            // 0. Load normal score
            normalscore normal_val;
            if(modeCompute.isSet())
                load_xyzfile(app_folder + "/" + Variable.getValue() + "_nscore.dat", normal_val.values, normal_val.x, normal_val.nsco);
            else
                load_xyzfile(out_vario + "/" + Variable.getValue() + "_nscore.dat", normal_val.values, normal_val.x, normal_val.nsco);

            std::string out_filename = out_vario + "/" + Variable.getValue();
            if(subDataset.isSet())
                out_filename += "_" + subDataset.getValue();

            // 0. Load info vario
            VarioMeta metavario;
            if(modeCompute.isSet())
                metavario.read(out_vario + "/" + setVario.getValue());
            else
                metavario.read(out_filename + ".json");

            if(metavario.getProcessing().normal_score.compare("NO") == 0)
            {
                std::cerr << "\033[0;31mERROR: normal score not exists in meta@vario.\033[0m" << std::endl;
                exit(1);
            }

            // 1. Definition of input/output format
            std::string ext = ".dat";
            if(csvConversion.isSet() == true)
                ext = ".csv";


            std::string name_json = "/" + Variable.getValue();
            if(subDataset.isSet())
                name_json += "_" + subDataset.getValue();

            if(!filesystem::exists(var_path))
                filesystem::create_directory(var_path);

            std::string norm_json = norm_path + name_json + ".json";

            if(!filesystem::exists(var_path + name_json + ".json"))
                filesystem::copy(norm_json, var_path + name_json + ".json");

            metacompute.read(var_path + name_json + ".json");

            std::vector<std::string> deps_ns;
            if(norm_json.find("out/") != std::string::npos)
                deps_ns.push_back(norm_json.substr(norm_json.find("out/"), norm_json.length()));

            std::vector<std::string> excommands;
            excommands.push_back(command);

            metacompute.setDependencies(deps_ns);
            metacompute.setCommands(excommands);

            ComputeMeta::Simulation sim = metacompute.getSimulation();
            sim.back_normal_score = true;
            sim.extrapolation_type = setExtrType.getValue();
            sim.min_extrapolation_value = setMinExtr.getValue();
            sim.max_extrapolation_value = setMaxExtr.getValue();

            if(setFile.isSet())
            {
                std::vector<double> result_sgs;
                load1d_xyzfile(setFile.getValue(), result_sgs);
                std::string filename = setFile.getValue().substr(setFile.getValue().find_last_of("/")+1, setFile.getValue().length()); //nome progetto
                filename = get_basename(filename);

                std::vector<double> trasformed_values = back_normal_score(result_sgs, normal_val, setExtrType.getValue(), setMinExtr.getValue(), setMaxExtr.getValue());
                export1d_xyz (var_path + "/" + filename + ext, trasformed_values);

                // sim.est_mean = mean(trasformed_values);
                // sim.est_var = variance(trasformed_values);

                if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
                {
                    MUSE::Metadata data;
                    data.read(l + "/metadata/" + Variable.getValue() + ".json");

                    MUSE::Data encode_output = data.getData(0);
                    encode_output.setComments(app_name + "OUTPUT-BNS");
                    //encode_output.setDescription(filename + "_ESTvarspace");
                    encode_output.write(var_path + "/" + filename + ".json");
                }
            }
            else
            {
                timing_logger.start();

                std::cout << norm_path << std::endl;
                std::vector<std::string> list_csv_all = get_files(norm_path, ext, true);
                if(list_csv_all.empty())
                {
                    std::cerr << "== ERROR. Searching file with extension " << ext << ": No valid files found." << std::endl;
                    exit(1);
                }

                // Check su file list nella cartella di compute con nome variabile + "_" + subdataset (riferito alle simulazioni) e se non esistono, prendi tutti i file con nome variabile + "_" (riferito alle simulazioni effettuate in varspace)
                std::vector<std::string> list_csv;
                for(const std::string &f : list_csv_all)
                {
                    std::string basename = get_basename(get_filename(f));
                    // Deve iniziare con il prefisso e finire con _DDDD (4 cifre)
                    if(basename.rfind(Variable.getValue() + "_", 0) == 0)
                    {
                        std::string suffix = basename.substr(Variable.getValue().size() + 1); // parte dopo "variabile_"
                        if(!suffix.empty() && std::all_of(suffix.begin(), suffix.end(), ::isdigit))
                            list_csv.push_back(f);
                    }
                }
                if(list_csv.empty())
                {
                    std::cerr << "== ERROR. Searching simulation file with following format name: " << Variable.getValue() << "_<xxxx>. No valid files found." << std::endl;
                    exit(1);
                }

                if(!filesystem::exists(var_path))
                    filesystem::create_directory(var_path);

                std::string norm_json = norm_path + name_json + ".json";

                MUSE::Metadata data;
                data.read(l + "/metadata/" + Variable.getValue() + ".json");

                MUSE::Metadata meta_output;
                meta_output.setProject(Project);
                meta_output.setCommands(excommands);
                meta_output.setDependencies(deps_ns);

                std::vector<MUSE::Data> multi_output;

                if(!filesystem::exists(var_path + name_json + ".json"))
                    filesystem::copy(norm_json, var_path + name_json + ".json");

                for(const std::string &s:list_csv)
                {
                    std::vector<double> result_sgs;
                    result_sgs.clear();
                    load1d_xyzfile(s, result_sgs);

                    int id_sim = std::stoi(get_basename(get_filename(s)).substr(get_basename(get_filename(s)).find_last_of('_')+1, get_basename(get_filename(s)).length()));
                    std::cout << "### ID SIM: " << id_sim << std::endl;

                    std::vector<double> trasformed_values = back_normal_score(result_sgs, normal_val, setExtrType.getValue(), setMinExtr.getValue(), setMaxExtr.getValue());
                    export1d_xyz (var_path + "/" + get_basename(get_filename(s)) + ext, trasformed_values);

                    // sim.est_mean = mean(trasformed_values);
                    // sim.est_var = variance(trasformed_values);

                    MUSE::Data sim_output = data.getData(0);
                    sim_output.setComments(app_name + "OUTPUT-BNS");
                    //sim_output.setDescription(get_basename(get_filename(s)) + "_ESTvarspace");
                    multi_output.push_back(sim_output);

                    std::cout << std::endl;
                }
                meta_output.setMultiData(multi_output);
                meta_output.write(var_path + "/_" + Variable.getValue() + ".json");
            }
            metacompute.setSimulation(sim);
            metacompute.write(var_path + name_json + ".json");

            std::cout << FGRN("Back Normal Score transformation ... COMPLETED.") << std::endl;
        }

        prefix_timing = "back_normalscore";
        timing_logger.stop(prefix_timing);
    }




    //---------------------------------------------------
    //---------------------------------------------------
    //---------------------------------------------------
    //---> CREATION DATABASE SWITCH ARGUMENT
    //---------------------------------------------------
    //---------------------------------------------------
    //---------------------------------------------------
    if(createDatabase.isSet())
    {
        ComputeMeta metacompute;

        if(!Variable.isSet())
        {
            std::cout << FRED("ERROR. Variable is NOT set!") << std::endl;
            exit(1);
        }

        if(!geomModel.isSet())
        {
            std::cout << FRED("ERROR. Geometry support is NOT set!") << std::endl;
            //exit(1);
        }

        std::string var = Variable.getValue(); //nome variabile

        app_folder += "/" + var;
        if(subDataset.isSet())
            app_folder += "_" + subDataset.getValue();

        app_folder += "_" + varioDirection.getValue() + varioDimension.getValue();

//        // 1. Define filename vario
//        if(varioDirection.getValue().compare("OMNI") == 0)
//            app_folder += "_omni";
//        else if(varioDirection.getValue().compare("DIR") == 0)
//            app_folder += "_dir";

        app_folder += "_" + get_basename(get_filename(geomModel.getValue()));
        if(!filesystem::exists(app_folder))
            filesystem::create_directory(app_folder);

//        if(!filesystem::exists(app_folder))
//        {
//            std::cerr << "ERROR: Folder not exists!" << std::endl;
//            exit(1);
//        }

        std::string database_path = app_folder + "/_normspace";
        //database_path += "/_normspace";
        if(setSpace.getValue().compare("NORMAL") != 0)
            database_path += "/../_varspace";
        std::string db_filename = database_path + "/" + Variable.getValue();
        if(subDataset.isSet())
            db_filename += "_" + subDataset.getValue();
        db_filename += ".db";

        std::string out_filename = database_path + "/" + Variable.getValue();
        if(subDataset.isSet())
            out_filename += "_" + subDataset.getValue();
        metacompute.read(out_filename + ".json");


        std::cout << FMAG("############################################################") << std::endl;
        std::cout << FMAG("RICORDA: ogni volta che utilizzo il comando per la creazione del database, il database esistente non verrà sovrascritto, ma esteso con i nuovi dati da inserire.") << std::endl;
        std::cout << FMAG("Tutti i dati si accoderanno al database esistente, anche se già presenti.") << std::endl;
        std::cout << FMAG("Questa versione non prevede infatti un controllo dei duplicati.") << std::endl;
        std::cout << FMAG("Attualmente CONVIENE cancellare il database esistente e rilanciare il comando.") << std::endl;
        std::cout << FMAG("TO DO: FUNZIONE PER CONTROLLO DUPLICATI!!") << std::endl;
        std::cout << FMAG("############################################################") << std::endl;
        std::cout << std::endl;

        sqlite3 *db = nullptr;

        open_or_create_db(db_filename.c_str(), &db);

        std::vector<std::string> values (sim_table_cols.size()-1);

        std::vector<std::vector<std::string> > values_list;
        for (int s=0; s < metacompute.getSimulation().n_iterations; s++) // For each Simulation
        {
            stringstream sgs_file;
            sgs_file << database_path << "/" << Variable.getValue() << "_" << std::setw(4) << std::setfill('0') << s;

            std::vector<double> val;
            load1d_xyzfile(sgs_file.str() + ".csv", val);

            for (uint c=0; c < metacompute.getSimulation().n_elements; c++) // For each Cell
            {
                values.at(0) = std::to_string(s);           // # Simulation
                values.at(1) = std::to_string(c);           // # Cell
                values.at(2) = var;                         // Variable name
                values.at(3) = std::to_string(val.at(c));   // Variable Value

                values_list.push_back(values);
            }
        }
        insert_in_db(db, values_list);

        sqlite3_close(db);

        std::cout << FGRN("Database creation ... COMPLETED.") << std::endl;
    }

    // Print and export timing summary
    timing_logger.print_summary();
    if(setDebug.isSet())
        timing_logger.export_to_csv(prefix_timing + "-timing.csv");




    //---------------------------------------------------
    //---------------------------------------------------
    //---------------------------------------------------
    //---> COMPUTE SWITCH ARGUMENT (MODE SET - FIXED)
    //---------------------------------------------------
    //---------------------------------------------------
    //---------------------------------------------------
    // Questo comando in modalità FIXED permette di calcolare le simulazioni su una variable specificata (n° campioni originali),
    // considerando un variogramma calcolato sulla stessa variabile ma con un numero diverso di campioni (n° campioni originali + campioni aggiunti)
    // (Vedi esempio 20_DGM_SMargherita)
    if(interpolationCompute.isSet() && modeCompute.isSet())
    {
        if(!Variable.isSet())
        {
            std::cout << FRED("ERROR. Variable is NOT set!") << std::endl;
            exit(1);
        }

        if(!geomModel.isSet())
        {
            std::cout << FRED("ERROR. Geometry support is NOT set!") << std::endl;
            exit(1);
        }

        if(!filesystem::exists(app_folder))
            filesystem::create_directory(app_folder);

        // Start timing
        timing_logger.start();
        timing_logger.set_output_folder(app_folder);

        /////////////////////////////
        //////////SUMMARY CSV FOR FRAMES
        ///
        ///
        std::cout << "Save summary of multi-frame variography analysis ... " << std::endl;
        std::ofstream file_summary;
        file_summary.open(app_folder + "/" + Variable.getValue() + "_SGSsummary.csv", std::fstream::out);
        if(!file_summary.is_open())
        {
            std::cerr << "\033[0;31mError in file opening: " << app_folder + "/" + Variable.getValue() + "_SGSsummary.csv" << "\033[0m" << std::endl;
            exit(1);
        }
        std::string delimiter = ";";

        std::vector<std::string> vec_csv;
        vec_csv.push_back("frame_name");
        vec_csv.push_back("domain");
        vec_csv.push_back(Variable.getValue()+"_mean");
        vec_csv.push_back(Variable.getValue()+"_var");
        vec_csv.push_back(Variable.getValue()+"_mean_zscore");
        vec_csv.push_back(Variable.getValue()+"_var_zscore");
        vec_csv.push_back("model_type");
        vec_csv.push_back("nugget");
        vec_csv.push_back("sill");
        vec_csv.push_back("partial_sill");
        vec_csv.push_back("range");
        vec_csv.push_back("range_max");
        vec_csv.push_back("range_min");
        vec_csv.push_back("range_z");
        for(uint col =0; col < vec_csv.size(); col++)
        {
            file_summary << vec_csv.at(col);
            if(col != vec_csv.size() - 1)
                file_summary << delimiter; // No comma at end of line
        }
        file_summary << "\n";
        ///////////////////////////////////////////////////////////////////////

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
            vec_csv.clear();

            count_frame++;

            filesystem::path dir = l;
            filesystem::path rel_datadir = filesystem::relative(dir, abs_datadir);
            vec_csv.push_back(rel_datadir.string());
            if(subDataset.isSet())
                vec_csv.push_back(subDataset.getValue());
            else
                vec_csv.push_back("");

            app_folder.clear();
            app_folder = out_folder + "/" + app_name;

            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);

            out_vario.clear();
            out_vario = out_folder + "/" + app_vario;

            if(rel_datadir.string().compare(".") != 0)
            {
                app_folder += "/" + rel_datadir.string();
                out_vario += "/" + rel_datadir.string();
                filesystem::create_directory(app_folder);

                std::cout << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << "### NUMBER OF TIME FRAMES: " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME N° " << count_frame << " ON " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME NAME: " << rel_datadir.string() << std::endl;
            }

            std::vector<std::string> list_json = get_files(l, ".json");
            if(list_json.size() > 1)
            {
                std::cerr << "ERROR. Only a file JSON is expected!" << std::endl;
                exit(1);
            }


            /// FIXED VARIOGRAM!!
            VarioMeta metavario;
            metavario.read(out_vario + "/" + setVario.getValue());
            std::cout << "### Set fixed variogram from JSON file: " << setVario.getValue() << std::endl;


            /// CREATION OF COMPUTE METADATA
            ComputeMeta metacompute;
            metacompute.setProject(Project);

            std::vector<std::string> excommands;
            excommands.push_back(command);
            metacompute.setCommands(excommands);

            app_folder += "/" + Variable.getValue();
            if(subDataset.isSet())
                app_folder += "_" + subDataset.getValue();

            app_folder += "_" + metavario.getInfoVariogram().direction + metavario.getInfoVariogram().dimension;

            app_folder += "_" + get_basename(get_filename(geomModel.getValue()));
            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);







            // // 0) Define meta for vario - dependencies
            // std::vector<std::string> deps;
            // filesystem::path rel_variopath = filesystem::relative(out_vario, Project.folder);
            // if(subDataset.isSet())
            //     rel_variopath += "/" + Variable.getValue() + "_" + subDataset.getValue() + ".json";
            // else
            //     rel_variopath += "/" + Variable.getValue() + ".json";
            // deps.push_back(rel_variopath);

            // filesystem::path rel_geompath = filesystem::relative(geomModel.getValue(), Project.folder);
            // deps.push_back(get_basename(rel_geompath) + ".json");

            // metacompute.setDependencies(deps);





            // 2. Storing json information into class Data
            MUSE::Metadata meta_input;
            meta_input.read(l + "/metadata/" + Variable.getValue() + ".json");
            Data data = meta_input.getData(0);

            data.setType(data.flag);
            readTextValues(l + "/data/" + Variable.getValue() + ".dat", data.text_values);

            //DataSummary(data); //da sistemare
            DataSummary sumdata;
            sumdata.setSummary(data);




            std::vector<std::string> id;
            std::vector<double> xCoord, yCoord, zCoord;


            if(metavario.getManipulate().stratigraphic_transf.compare("NO") == 0) //Condizione di default
            {
                std::cout << "\033[0;33mWARNING: No stratigraphic trasformation is set. The coordinate system remains unchanged.\033[0m" << std::endl;

                if(metavario.getInfoData().id_name.compare("Unknown") != 0)
                    readTextValues(l + "/data/" + metavario.getInfoData().id_name + ".dat", id);
                else
                    std::cerr << "ERROR reading ID: " << l + "/data/" + metavario.getInfoData().id_name + ".dat" << " NOT found." << std::endl;

                if(metavario.getInfoData().x_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + metavario.getInfoData().x_name + ".dat", xCoord);
                else
                    std::cerr << "ERROR reading X coordinate: " << l + "/data/" + metavario.getInfoData().x_name + ".dat" << " NOT found." << std::endl;

                if(metavario.getInfoData().y_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + metavario.getInfoData().y_name + ".dat", yCoord);
                else
                    std::cerr << "ERROR reading Y coordinate: " << l + "/data/" + metavario.getInfoData().y_name + ".dat" << " NOT found." << std::endl;


                if(metavario.getInfoData().z_name.compare("Unknown") != 0)
                    readCoordinate(l + "/data/" + metavario.getInfoData().z_name + ".dat", zCoord);
                else
                {
                    zCoord.resize(xCoord.size());
                    std::fill(zCoord.begin(), zCoord.end(), 0.0);

                    std::cerr << "ERROR reading Z coordinate: " << l + "/data/" + metavario.getInfoData().z_name + ".dat" << " NOT found." << std::endl;
                    std::cout << "\033[0;33mWARNING: Z coordinate is Unknown. Set -z --name <variable> for setting the variable.\033[0;0m" << std::endl;
                }
            }
            else
            {
                exit(1);
                //TO ENABLE ...
                std::cout << "\033[0;33mWARNING: Stratigraphic transformation is set on " << metavario.getManipulate().stratigraphic_transf << ". Variogram is computed in stratigraphic coordinate system.\033[0m" << std::endl;

                std::cout << "Stratigraphic coordinates are located in " << out_man + "/" << std::endl;
                //load_xyzfile(out_man + "/" + metavario.getManipulate().filename + ".xyz", xCoord, yCoord, zCoord);
            }

            metacompute.setInfoData(metavario.getInfoData());



            // String to double Conversion
            std::vector<std::string> corr_id;
            std::vector<double> conv_values, corr_x, corr_y, corr_z; //sampled data

            if(metavario.getManipulate().stratigraphic_transf.compare("NO") == 0) //Se non sono in coordinate stratigr
            {
                if(setRotAxis.isSet())
                {
                    MUSE::Rotation dataRotation_vario;

                    dataRotation_vario.rotation = true;
                    dataRotation_vario.rotation_axis = setRotAxis.getValue();
                    dataRotation_vario.rotation_center_x = setRotCenterX.getValue();
                    dataRotation_vario.rotation_center_y = setRotCenterY.getValue();
                    dataRotation_vario.rotation_center_z = setRotCenterZ.getValue();
                    dataRotation_vario.rotation_angle = setRotAngle.getValue();

                    std::cout << std::endl;
                    std::cout << "Rotation is activate on data ... " << dataRotation_vario.rotation << std::endl;
                    std::cout << "Rotation axis: " << dataRotation_vario.rotation_axis << std::endl;
                    std::cout << "Rotation center: [" << dataRotation_vario.rotation_center_x << "; " << dataRotation_vario.rotation_center_y << "; " << dataRotation_vario.rotation_center_z << "]" <<  std::endl;
                    std::cout << "Rotation angle (degree): " << dataRotation_vario.rotation_angle << std::endl;
                    std::cout << std::endl;

                    for(uint i=0; i< xCoord.size(); i++)
                    {
                        //rotazione coordinate all'inidice i
                        cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));
                        cinolib::vec3d axis = set_rotation_axis(dataRotation_vario.rotation_axis);
                        cinolib::vec3d c (dataRotation_vario.rotation_center_x, dataRotation_vario.rotation_center_y, dataRotation_vario.rotation_center_z);
                        sample = point_rotation(sample, axis, dataRotation_vario.rotation_angle, c);

                        xCoord.at(i) = sample.x();
                        yCoord.at(i) = sample.y();
                        zCoord.at(i) = sample.z();
                    }
                    metacompute.setRotation(dataRotation_vario);
                    std::cout << FGRN("Rotation on data ... COMPLETED.") << std::endl;
                }

                //Dopo aver caricato i dati grezzi, posso considerare un sottodataset o la totalità
                if(subDataset.isSet()) //sotto dataset da manipulate
                {
                    exit(1);
                    //TO ENABLE ...

                    // if(subDataset.getValue().compare(metavario.getManipulate().domain) != 0)
                    // {
                    //     std::cerr << "ERROR: vario is compute for another subdataset!" << std::endl;
                    //     exit(1);
                    // }

                    // MUSE::ExtractionMeta extrmeta;
                    // extrmeta.read(out_man + "/" + metavario.getManipulate().domain + ".json");
                    // std::cout << "Extraction sub-dataset is set. Reading ... " << out_man + "/" + metavario.getManipulate().domain + ".json" << std::endl;

                    // //1) VERIFICARE ROTAZIONE DATI
                    // MUSE::Rotation dataRotation = extrmeta.getRotation();
                    // if(dataRotation.rotation == true)
                    // {
                    //     std::cout << std::endl;
                    //     std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                    //     std::cout << "Rotation axis: " << dataRotation.rotation_axis << std::endl;
                    //     std::cout << "Rotation center: [" << dataRotation.rotation_center_x << "; " << dataRotation.rotation_center_y << "; " << dataRotation.rotation_center_z << "]" <<  std::endl;
                    //     std::cout << "Rotation angle (degree): " << dataRotation.rotation_angle << std::endl;
                    //     std::cout << std::endl;

                    //     for(uint i=0; i< xCoord.size(); i++)
                    //     {
                    //         //rotazione coordinate all'inidice i
                    //         cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));
                    //         cinolib::vec3d axis = set_rotation_axis(dataRotation.rotation_axis);
                    //         cinolib::vec3d c (dataRotation.rotation_center_x, dataRotation.rotation_center_y, dataRotation.rotation_center_z);
                    //         sample = point_rotation(sample, axis, dataRotation.rotation_angle, c);

                    //         xCoord.at(i) = sample.x();
                    //         yCoord.at(i) = sample.y();
                    //         zCoord.at(i) = sample.z();
                    //     }

                    //     metacompute.setRotation(dataRotation);
                    //     std::cout << FGRN("Rotation on data ... COMPLETED.") << std::endl;
                    // }

                    // //2) ESTRARRE SOTTODATASET DA INDICI
                    // if(extrmeta.getDataExtraction().id_points.size() == 0)
                    // {
                    //     std::cout << FRED("Vector of index is empty.") << std::endl;
                    //     exit(1);
                    // }

                    // string_to_double_conversion_vectors(extrmeta.getDataExtraction().id_points, data.text_values, id, xCoord, yCoord, zCoord, conv_values, corr_id, corr_x, corr_y, corr_z);
                    // std::cout << FGRN("Extraction sub-dataset ... COMPLETED.") << std::endl;
                }
                else
                    string_to_double_conversion_vectors(data.text_values, id, xCoord, yCoord, zCoord, conv_values, corr_id, corr_x, corr_y, corr_z);
            }
            else
            {
                if(setRotAxis.isSet())
                {
                    std::cout << FRED("Data rotation from cmdline is not active!") << std::endl;
                    exit(1);
                }

                if(subDataset.isSet()) //sotto dataset da manipulate
                {
                    exit(1);
                    //TO ENABLE ...

                    // if(subDataset.getValue().compare(metavario.getManipulate().domain) != 0)
                    // {
                    //     std::cerr << "ERROR: vario is compute for another subdataset!" << std::endl;
                    //     exit(1);
                    // }

                    // MUSE::ExtractionMeta extrmeta;
                    // extrmeta.read(out_man + "/" + metavario.getManipulate().domain + ".json");
                    // std::cout << "Extraction sub-dataset is set. Reading ... " << out_man + "/" + metavario.getManipulate().domain + ".json" << std::endl;

                    // //1) VERIFICARE ROTAZIONE DATI
                    // MUSE::Rotation dataRotation = extrmeta.getRotation();
                    // if(dataRotation.rotation == true)
                    // {
                    //     std::cout << std::endl;
                    //     std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                    //     std::cout << "Rotation axis: " << dataRotation.rotation_axis << std::endl;
                    //     std::cout << "Rotation center: [" << dataRotation.rotation_center_x << "; " << dataRotation.rotation_center_y << "; " << dataRotation.rotation_center_z << "]" <<  std::endl;
                    //     std::cout << "Rotation angle (degree): " << dataRotation.rotation_angle << std::endl;
                    //     std::cout << std::endl;

                    //     //NON DEVO RUOTARE I DATI DI NUOVO, MA SOLO COPIARE NEL JSON LE INFORMAZIONI DI ROTAZIONE DA MANIPULATE
                    //     metacompute.setRotation(dataRotation);
                    // }

                    // //2) ESTRARRE SOTTODATASET DA INDICI
                    // if(extrmeta.getDataExtraction().id_points.size() == 0)
                    // {
                    //     std::cout << FRED("Vector of index is empty.") << std::endl;
                    //     exit(1);
                    // }

                    // for(uint i:extrmeta.getDataExtraction().id_points)
                    // {
                    //     std::string val_tmp = data.text_values.at(i);
                    //     if(!val_tmp.empty() && val_tmp.compare("nd")!=0)
                    //     {
                    //         if(val_tmp.compare("*")!=0)
                    //         {
                    //             double val = std::stod(val_tmp);
                    //             conv_values.push_back(val);

                    //             if(id.size() > 0)
                    //                 corr_id.push_back(id.at(i));
                    //         }
                    //     }
                    // }

                    // corr_x = xCoord;
                    // corr_y = yCoord;
                    // corr_z = zCoord;

                    // std::cout << FGRN("Extraction sub-dataset ... COMPLETED.") << std::endl;
                }
                else
                    string_to_double_conversion_vectors(data.text_values, id, xCoord, yCoord, zCoord, conv_values, corr_id, corr_x, corr_y, corr_z);
            }


            int n_conv_samples = conv_values.size(); //numero campioni convertiti da stringa a double
            if(n_conv_samples == 0)
            {
                std::cerr << "ERROR: All values are invalid!" << std::endl;
                exit(1);
            }
            else
            {
                std::cout << "Data Statistical Summary ..." << std::endl;
                summary(conv_values);
                vec_csv.push_back(to_string(mean(conv_values)));
                vec_csv.push_back(to_string(variance(conv_values)));
            }

            std::cout << "\033[0;32mReading MUSE format and data analysis... COMPLETED.\033[0m" << std::endl;
            std::cout << std::endl;

            ComputeMeta::Processing infovar;
            infovar.v_name = Variable.getValue();
            infovar.normal_score = metavario.getProcessing().normal_score;
            infovar.declustering = metavario.getProcessing().declustering;
            metacompute.setProcessing(infovar);

            switch (data.type)
            {
            case varType::CATEGORIC_TEXT:
            {
                std::cout << std::endl;
                std::cout << FGRN("### VARTYPE CHECK: The variable is categoric (textual).") << std::endl;
                std::cout << std::endl;

                std::cout << FRED("ERROR: THE IMPLEMENTATION IS NOT COMPLETED!") << std::endl;
                exit(1);

                // ................................................... TO DO

                break;
            }
            case varType::CATEGORIC:
            {
                std::cout << std::endl;
                std::cout << FGRN("### VARTYPE CHECK: The variable is categoric.") << std::endl;
                std::cout << "### Only Indicator Kriging is active for categoric variables" << std::endl;
                std::cout << FMAG("### RICORDA - In questo caso mi aspetto più file json, uno per ogni categoria") << std::endl;
                std::cout << std::endl;


                std::cout << FRED("ERROR: THE IMPLEMENTATION IS NOT COMPLETED!") << std::endl;
                exit(1);

                // ................................................... TO DO

                break;
            }
            case varType::NUMBER: //VARIABILE CONTINUA
            {
                //4. Starting simulations (CHOSEN THE INTERPOLATION METHOD: KRIGING, SGS??)
                if(setCRIT.getValue().compare("SGS") != 0)
                {
                    std::cerr << "ERROR. Algorithm "<< setCRIT.getValue() << " is not available for continous variable. Set --crit SGS to interpolate continuous variable." << std::endl;
                    exit(1);
                }

                //NORMALSCORE
                normalscore normal_values;
                if(metavario.getProcessing().declustering.compare("YES") == 0)
                {
                    std::cout << "2D declustering ..." << std::endl;
                    std::cout << "### Cell size for declustering is set on " << setCellSize.getValue() << std::endl;
                    std::cout << "### Number of step for declustering (grid translation) is set on " << setNStep.getValue() << std::endl;
                    std::vector<double> decl_weight = decluster2d(corr_x, corr_y, setCellSize.getValue(), setNStep.getValue());
                    std::cout << "2D declustering ... COMPLETED." << std::endl;

                    normal_values = normal_score(conv_values, decl_weight); //RICORDA!! c'è un terzo parametro da considerare nella normal score, settato di default su false
                    export1d_xyz(app_folder + "/" + Variable.getValue() + "_weight.dat", decl_weight);
                }
                else
                    normal_values = normal_score(conv_values);

                vec_csv.push_back(to_string(mean(normal_values.values)));
                vec_csv.push_back(to_string(variance(normal_values.values)));

                export1d_xyz(app_folder + "/" + Variable.getValue() + "_convval.dat", conv_values);
                export3d_xyz(app_folder + "/" + Variable.getValue() + "_nscore.dat", normal_values.values, normal_values.x, normal_values.nsco);

                std::cout << "### Computing normal score for variable: " << Variable.getValue() << " ... COMPLETED." << std::endl;



                // 2. Read fitted variogram model from json (into vario folder)

                VarioDirection dir;
                convert_from_str(metavario.getInfoVariogram().direction, dir);

                ComputeMeta::InfoVariogram info_vario;
                info_vario.dimension = metavario.getInfoVariogram().dimension;
                info_vario.direction = metavario.getInfoVariogram().direction;
                metacompute.setInfoVariogram(info_vario);


                Variogram fvm;
                MUSE::variogram_methods fitvariov;
                switch (dir)
                {
                case VarioDirection::OMNI:
                {
                    fvm.set_range(metavario.getFitExpVariog(0).range);
                    fvm.nugget = metavario.getFitExpVariog(0).nugget;

                    //MODIFICATO COME 1 - NUGGET!!!!!!!!!!!!!!!!!
                    fvm.sill = metavario.getFitExpVariog(0).sill - fvm.nugget;

                    variogram_type type;
                    convert_from_str(metavario.getFitExpVariog(0).type, type);
                    fvm.type = type;

                    //for json
                    fitvariov.setNugget(fvm.nugget);
                    fitvariov.setSill(fvm.sill);
                    fitvariov.set_range(metavario.getFitExpVariog(0).range);
                    fitvariov.setType(metavario.getFitExpVariog(0).type);

                    break;
                }
                case VarioDirection::DIR:
                {
                    std::cout << "Range min is set on: " << metavario.getSummary().min_semiaxis << std::endl;
                    std::cout << "Range max is set on: " << metavario.getSummary().max_semiaxis << std::endl;

                    if(setZRange.isSet())
                    {
                        std::cout << "Range in Z direction is set on: " << setZRange.getValue() << std::endl;
                        fvm.set_range(metavario.getSummary().min_semiaxis, metavario.getSummary().max_semiaxis, setZRange.getValue());
                    }
                    else
                        fvm.set_range(metavario.getSummary().min_semiaxis, metavario.getSummary().max_semiaxis);


                    fvm.set_azimuth(metavario.getSummary().max_direction);
                    std::cout << "Azimuth is set on max continuity direction: " << fvm.get_azimuth() << " degree from North" << std::endl;

                    //Settati sulla massima direzione, ma non cambiano (per costruzione -> calcolo automatico del vario direzionale)
                    fvm.nugget = metavario.getFitExpVariog(0).nugget;
                    std::cout << "Nugget is set on: " << fvm.nugget << std::endl;

                    //MODIFICATO COME 1 - NUGGET!!!!!!!!!!!!!!!!!
                    fvm.sill = metavario.getFitExpVariog(0).sill - fvm.nugget; //che deve essere ovviamente = 1
                    std::cout << "Sill is set on: " << fvm.sill << std::endl;

                    //anche il tipo è uguale tra tutti, quindi prendo quello a modello in dir 0
                    variogram_type type;
                    convert_from_str(metavario.getFitExpVariog(0).type, type);
                    fvm.type = type;
                    std::cout << "Type is set on: " << metavario.getFitExpVariog(0).type << std::endl;
                    std::cout << std::endl;



                    //for json
                    fitvariov.setNugget(fvm.nugget);
                    fitvariov.setSill(fvm.sill);
                    fitvariov.range_max = fvm.get_maxrange();
                    fitvariov.range_min = fvm.get_minrange();
                    fitvariov.setRangeZ(fvm.get_zrange());
                    fitvariov.setType(metavario.getFitExpVariog(0).type);

                    break;
                }
                }
                metacompute.setFitExpVariog(fitvariov);

                vec_csv.push_back(fitvariov.type);
                vec_csv.push_back(to_string(fitvariov.nugget));
                vec_csv.push_back(to_string(1.0));
                vec_csv.push_back(to_string(fitvariov.sill));
                vec_csv.push_back(to_string(fitvariov.range));
                vec_csv.push_back(to_string(fitvariov.range_max));
                vec_csv.push_back(to_string(fitvariov.range_min));
                vec_csv.push_back(to_string(fitvariov.getRangeZ()));


                // 3. Load geometry model
                std::string geom_name = geomModel.getValue().substr(geomModel.getValue().find_last_of("/")+1, geomModel.getValue().length());
                std::string ext_mesh = get_extension(geom_name);


                SGSResults sgs_output;
                std::vector<std::vector<point3d>> sgs;

                ComputeMeta::Simulation sim;
                sim.geometry = geom_name;
                sim.sim_criterion = setCRIT.getValue();
                sim.n_iterations = setNsim.getValue();


                std::cout << "#########################"<< std::endl;
                std::cout << "### SGS Output type is set on " << setSGSoutput.getValue() << std::endl;

                bool back_normal_score_inSGS = false; //SGSresults non terranno in conto della back normal score (farla a posteriori)
                if(setBackNormalScore.isSet())
                {
                    back_normal_score_inSGS = true;
                    std::cout << "### Back Normal Score trasformation is performed into parallel_sgs algorithm." << std::endl;
                }
                else
                {
                    std::cout << "### Back Normal Score transformation is not performed into parallel_sgs algorithm." << std::endl;
                    std::cout << "### Simulation results have a normal distribution!" << std::endl;
                }
                std::cout << std::endl;

                std::cout << "### Number of input samples is set on " << setInputSamples.getValue() << std::endl;
                std::cout << "### Number of simulated points in SGS algorithm is set on " << setSimulatedPoints.getValue() << std::endl;
                std::cout << "### Scale factor for search radius is set on " << setScaleRadius.getValue() << std::endl;
                std::cout << "### Search by octant is set on " << doOctantSearch.getValue() << std::endl;
                std::cout << std::endl;
                std::cout << "######################### START SIMULATIONS ..."<< std::endl;

                //APPLICARE LA BACK NORMAL SCORE (SE NECESSARIO)
                if(!back_normal_score_inSGS)
                    sim.back_normal_score = false;
                else
                    sim.back_normal_score = true;

                sim.extrapolation_type = setExtrType.getValue();
                sim.min_extrapolation_value = setMinExtr.getValue();
                sim.max_extrapolation_value = setMaxExtr.getValue();
                // sim.est_mean_zscore = mean(normal_values.values);
                // sim.est_var_zscore = variance(normal_values.values);

                //std::vector<MUSE::Data> vec_encode_output;
                MUSE::Metadata meta_output;
                std::vector<MUSE::Data> multi_output;

                //Distinguo le mesh surf/vol in base all'estensione
                if(ext_mesh.compare(".off") == 0 || ext_mesh.compare(".obj") == 0)
                {
                    std::cout << "Mesh is surface." << std::endl;

                    MUSE::SurfaceMesh<> surf_mesh;
                    surf_mesh.load(geomModel.getValue().c_str());

                    sim.n_elements = surf_mesh.num_polys();

                    std::cout << std::endl;
                    std::cout << FMAG("############################################################") << std::endl;
                    std::cout << FMAG("PER CONTROLLO (PRIMA DELLE SIMULAZIONI):") << std::endl;
                    std::cout << FMAG("La funzione delle SGS considera il VARIO DIREZIONALE con i seguenti parametri: ") << std::endl;
                    std::string string_type;
                    convert_to_str(string_type, fvm.type);
                    std::cout << "Type = " << string_type << std::endl;
                    std::cout << "Dir max (azimuth) = " << fvm.get_azimuth() << " degree from North." << std::endl;
                    std::cout << "Range max = " << fvm.get_maxrange() << std::endl;
                    std::cout << "Range min = " << fvm.get_minrange() << std::endl;
                    std::cout << "Range z = " << fvm.get_zrange() << std::endl;
                    std::cout << "Nugget = " << fvm.nugget << std::endl;
                    std::cout << "Sill = " << fvm.sill << std::endl;
                    std::cout << FMAG("############################################################") << std::endl;
                    std::cout << std::endl;


                    if(setSGSoutput.getValue().compare("VECSIM") == 0)
                    {
                        #ifdef DEBUG
                        std::cout << std::endl;
                        std::cout << FMAG("### SGS OUTPUT - VECSIM: vector of simulation results in the normal space -> a CSV file for each simulation") << std::endl;
                        std::cout << FMAG("### Back normal score is managed by using the command -B") << std::endl;
                        std::cout << std::endl;
                        #endif

                        app_folder += "/_normspace";
                        if(!filesystem::exists(app_folder))
                            filesystem::create_directory(app_folder);

                        sgs = parallel_sgs2 (surf_mesh, normal_values.values, corr_x, corr_y, corr_z, fvm, setNsim.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue(), doOctantSearch.getValue());

                        // Record SGS simulation time
                        prefix_timing = "SGS_simulation";
                        timing_logger.stop(prefix_timing);

                        std::cout << std::endl;
                        for(uint it=0; it< setNsim.getValue(); it++)
                        {
                            std::vector<double> results_per_sim;

                            for(uint pid=0; pid < surf_mesh.num_polys(); pid++)
                            {
                                double v = sgs.at(it).at(pid).get_value(0);
                                results_per_sim.push_back(v);
                            }

                            stringstream filename_sim;
                            filename_sim << std::setw(4) << std::setfill('0') << it;
                            //filename_sim << data.name << "_sgs_" << std::setw(4) << std::setfill('0') << it;
                            //filename_sim << data.name << "_" << std::setw(4) << std::setfill('0') << it;
                            export1d_xyz (app_folder + "/" + data.getName() + "_" + filename_sim.str() + ".csv", results_per_sim);

                            if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
                            {
                                MUSE::Data encode_output = data;
                                encode_output.setFlag("R");
                                encode_output.setDescription(filename_sim.str());
                                encode_output.setComments(app_name + "OUTPUT-SIM");

                                multi_output.push_back(encode_output);
                            }
                            std::cout << FGRN("### Saving files for simulation ... ") << it << FGRN(" COMPLETED.") << std::endl;
                        }

                        std::string out_filename = app_folder + "/" + data.name;
                        if(subDataset.isSet())
                            out_filename += "_" + subDataset.getValue();

                        metacompute.setSimulation(sim);
                        metacompute.write(out_filename + ".json");

                        std::vector<std::string> deps_sgs;
                        filesystem::path realpath_sgs = filesystem::relative(out_filename + ".json", abspath);
                        deps_sgs.push_back(realpath_sgs);
                        meta_output.setDependencies(deps_sgs);
                    }
                    else if(setSGSoutput.getValue().compare("MEAN") == 0)
                    {
                        std::cout << std::endl;
                        std::cout << FMAG("### SGS OUTPUT - MEAN: mean of estimates in the variable space -> unique CSV file") << std::endl;
                        std::cout << FMAG("### Back normal score is managed in the SGS algorithm") << std::endl;
                        std::cout << std::endl;

                        sgs_output = parallel_sgs2 (surf_mesh, normal_values.values, corr_x, corr_y, corr_z, fvm, setNsim.getValue(), normal_values,
                                                   back_normal_score_inSGS, setExtrType.getValue(), setMinExtr.getValue(), setMaxExtr.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue(), doOctantSearch.getValue());

                        // Record SGS simulation time
                        prefix_timing = "SGS_simulation";
                        timing_logger.stop(prefix_timing);



                        std::string out_filename = app_folder + "/" + data.name;
                        if(subDataset.isSet())
                            out_filename += "_" + subDataset.getValue();

                        export1d_xyz (out_filename + ".csv", sgs_output.estimates);
                        export1d_xyz (out_filename + "_unc.csv", sgs_output.uncertainties);

                        metacompute.setSimulation(sim);
                        metacompute.write(out_filename + ".json");

                        double max_unc =  -DBL_MAX;
                        uint id_max = 0;
                        for(uint pid=0; pid<surf_mesh.num_polys(); pid++)
                        {
                            if(sgs_output.uncertainties.at(pid) >= max_unc)
                            {
                                max_unc = sgs_output.uncertainties.at(pid);
                                id_max = pid;
                            }
                        }
                        std::cout << "### Max uncertainty: " << max_unc << "at point " << surf_mesh.poly_centroid(id_max) << std::endl;



                        //MESH IN COORDINATE STRATIGRAFICHE
                        if(!filesystem::exists(app_folder + "/_fordebug"))
                            filesystem::create_directory(app_folder + "/_fordebug");

                        std::ofstream file_out1;
                        file_out1.open(app_folder + "/_fordebug/" + Variable.getValue() + "centr_sim" + to_string(setNsim.getValue()) + ".dat", std::fstream::out);
                        if(!file_out1.is_open())
                        {
                            std::cerr << "\033[0;31mError in file opening: \033[0m" << std::endl;
                            exit(1);
                        }

                        else
                        {
                            for(uint pid = 0; pid < surf_mesh.num_polys(); pid++)
                            {
                                cinolib::vec3d centroid = surf_mesh.poly_centroid(pid);
                                file_out1 << std::setprecision(4) << centroid.x() << " " << centroid.y() << " " << centroid.z() << " " << sgs_output.estimates.at(pid) << std::endl;
                            }
                            file_out1.close();
                        }
                        std::cout << FGRN("Saving files in _fordebug folder ... COMPLETED.") << std::endl;

                        if (metavario.getManipulate().stratigraphic_transf.compare("NO") != 0)
                        {
                            //MESH IN COORDINATE CARTESIANE
                            //cinolib::Trimesh<> surf_mesh_piega;
                            MUSE::SurfaceMesh<> surf_mesh_piega;
                            std::string surf_name = geom_name.substr(geom_name.find_first_of("_")+1, Project.folder.length()); //nome progetto
                            std::string surf_filename = out_folder + "/geometry/surf/" + surf_name;
                            surf_mesh_piega.load(surf_filename.c_str());

                            std::ofstream file_out2;
                            file_out2.open(app_folder + "/_fordebug/" + Variable.getValue() + "centr_sim" + to_string(setNsim.getValue()) + "_original.dat", std::fstream::out);
                            if(!file_out2.is_open())
                            {
                                std::cerr << "\033[0;31mError in file opening: \033[0m" << std::endl;
                                exit(1);
                            }

                            else
                            {
                                for(uint pid = 0; pid < surf_mesh_piega.num_polys(); pid++)
                                {
                                    cinolib::vec3d centroid = surf_mesh_piega.poly_centroid(pid);
                                    file_out2 << std::setprecision(4) << centroid.x() << " " << centroid.y() << " " << centroid.z() << " " << sgs_output.estimates.at(pid) << std::endl;
                                }
                                file_out2.close();
                            }
                            std::cout << FGRN("Saving files (stratigraphic condition - original model) in _fordebug folder ... COMPLETED.") << std::endl;
                        }
                    }
                }


                else if (ext_mesh.compare(".mesh") == 0 || ext_mesh.compare(".vtk") == 0)
                {
                    std::cout << std::endl;
                    std::cout << FMAG("############################################################") << std::endl;
                    std::cout << FMAG("PER CONTROLLO (PRIMA DELLE SIMULAZIONI):") << std::endl;
                    std::cout << FMAG("La funzione delle SGS considera il VARIO DIREZIONALE con i seguenti parametri: ") << std::endl;
                    std::string string_type;
                    convert_to_str(string_type, fvm.type);
                    std::cout << "Type = " << string_type << std::endl;
                    std::cout << "Dir max (azimuth) = " << fvm.get_azimuth() << " degree from North." << std::endl;
                    std::cout << "Range max = " << fvm.get_maxrange() << std::endl;
                    std::cout << "Range min = " << fvm.get_minrange() << std::endl;
                    std::cout << "Range z = " << fvm.get_zrange() << std::endl;
                    std::cout << "Nugget = " << fvm.nugget << std::endl;
                    std::cout << "Sill = " << fvm.sill << std::endl;
                    std::cout << FMAG("############################################################") << std::endl;
                    std::cout << std::endl;

                    std::cout << "Mesh is volumetric." << std::endl;

                    MUSE::VolumeMesh<> vol_mesh;
                    vol_mesh.load(geomModel.getValue().c_str());

                    sim.n_elements = vol_mesh.num_polys();


                    if(setSGSoutput.getValue().compare("VECSIM") == 0)
                    {
                        #ifdef DEBUG
                        std::cout << std::endl;
                        std::cout << FMAG("### SGS OUTPUT - VECSIM: vector of simulation results in the normal space -> a CSV file for each simulation") << std::endl;
                        std::cout << FMAG("### Back normal score is managed by using the command -B") << std::endl;
                        std::cout << std::endl;
                        #endif

                        app_folder += "/_normspace";
                        if(!filesystem::exists(app_folder))
                            filesystem::create_directory(app_folder);

                        sgs = parallel_sgs2 (vol_mesh, normal_values.values, corr_x, corr_y, corr_z, fvm, setNsim.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue(), doOctantSearch.getValue());

                        for(uint it=0; it< setNsim.getValue(); it++)
                        {
                            std::vector<double> results_per_sim;

                            for(uint pid=0; pid < vol_mesh.num_polys(); pid++)
                            {
                                double v = sgs.at(it).at(pid).get_value(0);
                                results_per_sim.push_back(v);
                            }

                            stringstream filename_sim;
                            filename_sim << std::setw(4) << std::setfill('0') << it;
                            export1d_xyz (app_folder + "/" + data.getName() + "_" + filename_sim.str() + ".csv", results_per_sim);

                            if(setFormat.getValue().compare("YES") == 0) //se è settato, insieme da ogni file di output, restituisci la codifica in data format (JSON)
                            {
                                MUSE::Data encode_output = data;
                                encode_output.setFlag("R");
                                encode_output.setDescription(filename_sim.str());
                                encode_output.setComments(app_name + "OUTPUT-SIM");

                                multi_output.push_back(encode_output);
                            }
                            std::cout << FGRN("### Saving files for simulation ... ") << it << FGRN(" COMPLETED.") << std::endl;
                        }

                        std::string out_filename = app_folder + "/" + data.name;
                        if(subDataset.isSet())
                            out_filename += "_" + subDataset.getValue();

                        metacompute.setSimulation(sim);
                        metacompute.write(out_filename + ".json");

                        std::vector<std::string> deps_sgs;
                        filesystem::path realpath_sgs = filesystem::relative(out_filename + ".json", abspath);
                        deps_sgs.push_back(realpath_sgs);
                        meta_output.setDependencies(deps_sgs);

                    }
                    else if(setSGSoutput.getValue().compare("MEAN") == 0)
                    {
                        std::cout << std::endl;
                        std::cout << FMAG("### SGS OUTPUT - MEAN: mean of estimates in the variable space -> unique CSV file") << std::endl;
                        std::cout << FMAG("### Back normal score is managed in the SGS algorithm") << std::endl;
                        std::cout << std::endl;

                        sgs_output = parallel_sgs2 (vol_mesh, normal_values.values, corr_x, corr_y, corr_z, fvm, setNsim.getValue(), normal_values,
                                                   back_normal_score_inSGS, setExtrType.getValue(), setMinExtr.getValue(), setMaxExtr.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue(), doOctantSearch.getValue());

                        std::string out_filename = app_folder + "/" + data.name;
                        if(subDataset.isSet())
                            out_filename += "_" + subDataset.getValue();

                        export1d_xyz (out_filename + ".csv", sgs_output.estimates);
                        export1d_xyz (out_filename + "_unc.csv", sgs_output.uncertainties);

                        metacompute.setSimulation(sim);
                        metacompute.write(out_filename + ".json");


                        //MESH IN COORDINATE STRATIGRAFICHE
                        if(!filesystem::exists(app_folder + "/_fordebug"))
                            filesystem::create_directory(app_folder + "/_fordebug");

                        std::ofstream file_out1;
                        file_out1.open(app_folder + "/_fordebug/" + Variable.getValue() + "centr_sim" + to_string(setNsim.getValue()) + ".dat", std::fstream::out);
                        if(!file_out1.is_open())
                        {
                            std::cerr << "\033[0;31mError in file opening: \033[0m" << std::endl;
                            exit(1);
                        }
                        else
                        {
                            for(uint pid = 0; pid < vol_mesh.num_polys(); pid++)
                            {
                                cinolib::vec3d centroid = vol_mesh.poly_centroid(pid);
                                file_out1 << std::setprecision(4) << centroid.x() << " " << centroid.y() << " " << centroid.z() << " " << sgs_output.estimates.at(pid) << std::endl;
                            }
                            file_out1.close();
                        }
                        std::cout << FGRN("Saving files in _fordebug folder ... COMPLETED.") << std::endl;

                        if (metavario.getManipulate().stratigraphic_transf.compare("NO") != 0)
                        {
                            std::cout << FYEL("SAVE fordebug file for model in stratigraphic coordinate ... TO DO!!") << std::endl;

                            //MESH IN COORDINATE CARTESIANE
                            MUSE::VolumeMesh<> vol_mesh_piega;
                            std::string surf_name = geom_name.substr(geom_name.find_first_of("_")+1, Project.folder.length()); //nome progetto
                            std::string surf_filename = out_folder + "/geometry/volume/" + surf_name;
                            vol_mesh_piega.load(surf_filename.c_str());

                            std::ofstream file_out2;
                            file_out2.open(app_folder + "/_fordebug/" + Variable.getValue() + "centr_sim" + to_string(setNsim.getValue()) + "_original.dat", std::fstream::out);
                            if(!file_out2.is_open())
                            {
                                std::cerr << "\033[0;31mError in file opening: \033[0m" << std::endl;
                                exit(1);
                            }

                            else
                            {
                                for(uint pid = 0; pid < vol_mesh_piega.num_polys(); pid++)
                                {
                                    cinolib::vec3d centroid = vol_mesh_piega.poly_centroid(pid);
                                    file_out2 << std::setprecision(4) << centroid.x() << " " << centroid.y() << " " << centroid.z() << " " << sgs_output.estimates.at(pid) << std::endl;
                                }
                                file_out2.close();
                            }
                            std::cout << FGRN("Saving files (stratigraphic condition - original model) in _fordebug folder ... COMPLETED.") << std::endl;
                        }
                    }
                }
                else
                {
                    std::cerr << "ERROR: Mesh format is not supported." << std::endl;
                    exit(1);
                }

                meta_output.setProject(Project);
                //meta_output.setDependencies(deps);
                meta_output.setCommands(excommands);
                meta_output.setMultiData(multi_output);
                meta_output.write(app_folder + "/_" + data.name + ".json");

                for(uint col =0; col < vec_csv.size(); col++)
                {
                    file_summary << vec_csv.at(col);
                    if(col != vec_csv.size() - 1)
                        file_summary << delimiter; // No comma at end of line
                }
                file_summary << "\n";

                break;


            }
            case COORDINATE:
            case ERROR:
            case ID:
            case TEXT:
                break;
            }
        }
        file_summary.close();// Close the file
        std::cout << FGRN("Save summary of multi-frame variography analysis ... COMPLETED.") << std::endl;
    }

    } catch (ArgException &e)  // catch exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

}
