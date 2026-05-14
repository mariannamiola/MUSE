#include <iostream>
#include <filesystem>
#include <utility>
#include <limits.h>

#include <tclap/CmdLine.h>

#include "muselib/utils.h"
#include "muselib/colors.h"

#include "muselib/data_structures/project.h"
#include "muselib/data_structures/data.h"
#include "muselib/data_structures/info_data.h"

#include "muselib/input/load_xyz.h"

#include "muselib/flag/flag.h"

#include "muselib/metadata/metadata.h"
#include "muselib/metadata/vario_meta.h"
#include "muselib/metadata/vario_methods.h"
#include "muselib/metadata/data_meta.h"
#include "muselib/metadata/data_summary.h"
#include "muselib/metadata/vario_multiframe.h"
#include "muselib/metadata/manipulate_meta.h"

#include "muselib/plot/plots.h"
#include "muselib/interpolation/plane.h"

#include "geostatslib/statistics/data_structures.h"
#include "geostatslib/statistics/normal_score.h"
#include "geostatslib/statistics/variogram.h"
#include "geostatslib/statistics/decluster.h"

#include "muselib/geostatistics/utils.h"
#include "muselib/geostatistics/vario.h"
#include "muselib/geostatistics/fitvario.h"

#include "muselib/geostatistics/indicator.h"
#include "muselib/geostatistics/fitindvario.h"

#include "muselib/geometry/tools.h"
#include "muselib/stratigraphic_trasformation/coord_transf.h"



#include "ellipse_fit.h"

//for filesystem
#ifdef __APPLE__
    using namespace std::__fs;
#else
    using namespace std;
#endif


using namespace MUSE;
using namespace TCLAP;


///////////////////////////////////////////////////
// Function to convert string to weightsType enum
weightsType stringToWeightsType(const std::string& criterion) {
    if (criterion == "CRESSIE") return CRESSIE;
    if (criterion == "CRESSIE_WEIGHTED") return CRESSIE_WEIGHTED;
    if (criterion == "CRESSIE_WEIGHTED_MODIFIED") return CRESSIE_WEIGHTED_MODIFIED;
    if (criterion == "DEFAULT") return CRESSIE_WEIGHTED_MODIFIED;
    // Default fallback
    return CRESSIE_WEIGHTED_MODIFIED;
}


int main(int argc, char** argv)
{
    std::cout << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "================== STARTING MUSE-VARIO ===================" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;

    std::string app_name = "vario"; //app name
    std::string app_data = "data";
    std::string app_manipulate = "manipulate";

    try {
    CmdLine cmd("MUSE - Modelling Uncertainty as a Support of Environment. MUSE-vario application", ' ', "version 0.0");


    // ---------------------------------------------------------------------------------------------------------
    // MAIN FUNCTIONALITIES:

    // Option 0. Compute varioagram for a variable
    /**
     * @brief Compute variogram for a specified variable. This option allows you to compute the variogram for a specified variable within a project directory. The variogram can be computed using either the experimental method or by fitting a model, depending on the parameters specified. This functionality is essential for analyzing spatial continuity and variability of the variable data, and it serves as a fundamental step in geostatistical analysis and modeling.
     * @note When computing variograms, the following parameters are mandatory:
     * - --pdir: Project directory (mandatory)
     * - --var: Variable name (mandatory)
     * OPTIONAL but commonly used:
     * - --vario: Variogram type (EXPERIMENTAL, MODEL)
     * - --dir: Direction type (OMNI, DIR)
     * - --dim: Dimension (3D, 2D, 1Dz, etc.)
     * @example muse_vario -V --pdir /path/to/project --var T --vario MODEL --dir OMNI --dim 3D
     */
    SwitchArg varioCompute                  ("V", "variogram", "Compute variogram for a specified variable", cmd, false); //booleano

    /**
     * @brief Project directory containing the data for variogram computation. This option allows you to specify the directory where the project data is located, which is essential for performing variogram computation. The specified directory should contain the necessary data for variogram computation, including the variable specified with --var. When using this flag, ensure that the directory structure and data files are properly organized to facilitate successful variogram computation.
     * @required Must be specified when using --variogram flag. 
     * @format /path/to/project/name-project
     * @note The specified directory must contain the necessary data for variogram computation, including the variable specified with --var.
     * @example --pdir /path/to/project/name-project
     */
    ValueArg<std::string> projectFolder     ("p", "pdir", "Project directory containing the data for variogram computation", true, "/path/to/project/name-project", "string", cmd);

    /**
     * @brief Set variable name to compute variogram. This option allows you to specify the name of the variable for which you want to compute the variogram. The variable name should match one of the variables present in the project data, and it is essential for identifying the specific variable to be analyzed in the variogram computation. When using this flag, ensure that the specified variable exists in the project data to avoid errors during computation.
     * @format variable_name
     * @required Must be specified when using --variogram flag. 
     * @note Use this flag to specify the variable for which you want to compute the variogram. The variable name should match one of the variables present in the project data.
     * @example --var T
     */
    ValueArg<std::string> Variable          ("v", "var", "Variable", true, "Set variable name to compute variogram", "string", cmd);

    /**
     * @brief Set debug mode to save additional support files. When debug mode is enabled, additional support files are saved during computation for troubleshooting and analysis. This may include intermediate results, logs, and diagnostic information. Use this flag when you want to investigate the computation process in more detail or when encountering issues.
     * @example muse_vario -V --pdir /path/to/project --var T --debug
     */
    SwitchArg setDebug ("", "debug", "Set debug mode to save additional support files", cmd, false); //booleano

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
     * @brief Set performing normal score transformation on continuous variables. This option allows you to specify whether to apply a normal score transformation to continuous variables before computing the variogram. Normal score transformation is a common technique used in geostatistics to transform data to follow a normal distribution, which can improve the performance of variogram modeling and kriging.
     * @default NO (no normal score transformation is applied). 
     * @note When using normal score transformation, requires:
     * - --var: Variable name (mandatory)
     * @example --var T --nscore YES
     */
    ValueArg<std::string> setNormalScore    ("", "nscore", "Set normal score transformation (only for continuous variables)", false, "NO", "string", cmd);

    /**
     * @brief Set 2D declustering for variogram computation. This option allows you to specify whether to apply 2D declustering to the data before computing the variogram. Declustering is a technique used to reduce the influence of clustered data points in variogram computation by assigning weights to data points based on their spatial distribution. When 2D declustering is enabled, additional parameters such as cell size and number of grid translation steps can be specified to control the declustering process.
     * @default false (no declustering is applied).
     * @format boolean
     * @note When using declustering (--decl), requires:
     * - --csize: Cell size for declustering 
     * - --nstep: Number of grid translation steps for declustering
     * @example --decl --csize 100 --nstep 5
     */
    SwitchArg setDeclustering2d             ("", "decl", "Set 2D declustering for variogram computation", cmd, false); //booleano

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


    // Option: variography on stratigraphic coordinates model (ref. manipulate)
    std::vector<std::string> allowedStratigraphicCondition = {"PROPORTIONAL","TRUNCATION","ONLAP","COMBINATION"};
    ValuesConstraint<std::string> allowedValsSC(allowedStratigraphicCondition);
    /**
     * @brief Set stratigraphic condition for coordinate transformation. This option allows you to specify the stratigraphic condition type (PROPORTIONAL, TRUNCATION, ONLAP, COMBINATION) to be used when performing variography on stratigraphic coordinates.
     * @param sttype Flag to set stratigraphic condition for coordinate transformation (PROPORTIONAL, TRUNCATION, ONLAP, COMBINATION)
     * @default NO (no stratigraphic coordinate transformation is performed -- from muse-manipulate)
     * @format selected string from allowed options
     * @values PROPORTIONAL, TRUNCATION, ONLAP, COMBINATION
     * @note When using stratigraphic transformation, requires
     * - --filestrat: filename containing samples converted in stratigraphic coordinate system (from muse-manipulate)
     * Available conditions, derived from reservoir modeling: PROPORTIONAL, TRUNCATION, ONLAP, COMBINATION
     * - PROPORTIONAL: The stratigraphic coordinates are computed proportionally to the original spatial coordinates, preserving the relative distances between points while transforming them into a stratigraphic framework.
     * - TRUNCATION: The stratigraphic coordinates are computed by truncating the original spatial coordinates based on stratigraphic boundaries, resulting in a transformation that reflects the stratigraphic layering of the data.
     * - ONLAP: The stratigraphic coordinates are computed by considering the onlap relationships between stratigraphic layers, where points are transformed based on their position relative to the onlapping layers in the stratigraphic sequence.
     * - COMBINATION: No correlation surface can be explicity defined. The coordinate transformation is not possible.
     * @example --sttype PROPORTIONAL --filestrat /path/to/strat_coords.dat
     */
    ValueArg<std::string> stratCondition    ("", "sttype", "Set stratigraphic condition for coordinate transformation", false, "NO", &allowedValsSC, cmd);
    
    /**
     * @brief Set filename of samples in stratigraphic coordinates. This option allows you to specify the path to the file containing the samples that have been converted into stratigraphic coordinates. This file is generated from muse-manipulate and is required when performing variography on stratigraphic coordinates using the --sttype flag.
     * @param filestrat Flag to set filename of samples converted in stratigraphic coordinate system (from muse-manipulate)
     * @required Must be specified when using --sttype flag for stratigraphic transformation.
     * @example --sttype PROPORTIONAL --filestrat /path/to/samples-strat.dat
     */
    ValueArg<std::string> filenameStrat     ("f", "filestrat", "Set filename containing samples converted in stratigraphic coordinate system (generated by muse-manipulate)", false, "path/to/samples-strat.dat", "string", cmd);


    std::vector<std::string> allowedVarioType = {"EXPERIMENTAL","MODEL"};
    ValuesConstraint<std::string> allowedValsVT(allowedVarioType);
    /**
     * @brief Set variogram type to compute (experimental or model). This option allows you to specify the type of variogram step to compute, either experimental or model. The experimental variogram is computed directly from the data, while the model variogram involves fitting a theoretical variogram model to the experimental variogram.
     * @param vario Flag to set variogram type to compute (EXPERIMENTAL, MODEL)
     * @default EXPERIMENTAL
     * @format selected string from allowed options
     * @values EXPERIMENTAL, MODEL
     * @note When computing variograms, use --vario to specify the type of variogram to compute. Available options are:
     * - EXPERIMENTAL: Computes the experimental variogram directly from the data, which represents the empirical spatial variability of the variable being analyzed. This is the default option and is typically the first step in variogram analysis.
     * - MODEL: Computes the model variogram by fitting a theoretical variogram model to the experimental variogram. This involves selecting a variogram model type (e.g., spherical, exponential, Gaussian) and estimating the model parameters to best fit the experimental variogram. The model variogram is used for spatial interpolation and kriging, as it provides a mathematical representation of the spatial continuity of the variable.
     * @example --vario MODEL
     */
    ValueArg<std::string> varioType         ("", "vario", "Set variogram type to compute (EXPERIMENTAL, MODEL)", false, "EXPERIMENTAL", &allowedValsVT, cmd);

    // Option: types of variogram directions
    std::vector<std::string> allowedVarioDir = {"OMNI","DIR"};
    ValuesConstraint<std::string> allowedValsVD(allowedVarioDir);
    /**
     * @brief Set variogram direction type (omnidirectional or directional). This option allows you to specify the type of variogram direction to compute, either omnidirectional or directional. The omnidirectional variogram considers all pairs of points regardless of their direction, while the directional variogram analyzes spatial continuity in specific directions.
     * @default OMNI
     * @format selected string from allowed options
     * @values OMNI, DIR
     * @note When computing variograms, use --dir to specify the type of variogram analysis. Available options are:
     * - OMNI: Computes the omnidirectional variogram, which considers all pairs of points regardless of their direction. This is the default option.
     * - DIR: Computes directional variograms, which analyze spatial continuity in specific directions. When using DIR for directional variogram computation, additional parameters become important to define the directional analysis. These parameters include:
     * - --deg: Degree step (default: 45°) - This parameter defines the angular step size for directional analysis. For example, a degree step of 30° would compute variograms at 0°, 30°, 60°, etc.
     * - --degtol: Tolerance (default: 45°) - This parameter specifies the angular tolerance for including point pairs in the directional variogram. For example, with a tolerance of 15°, point pairs that are within 15° of the specified direction would be included in the analysis.
     * - --zdegtol: Vertical tolerance (default: 22.5°) - This parameter is used for 3D directional variogram analysis to specify the vertical angular tolerance for including point pairs.
     * @example --dir DIR --deg 30 --degtol 15 --zdegtol 10
     */
    ValueArg<std::string> varioDirection    ("", "dir", "Set variogram direction type", false, "OMNI", &allowedValsVD, cmd);

    // Option: types of variogram dimensions
    std::vector<std::string> allowedVarioDim = {"3D","3Dxy","3Dz","2D","1Dz","1D"};
    ValuesConstraint<std::string> allowedValsVDm(allowedVarioDim);
    /**
     * @brief Set variogram dimension type (3D, 3Dxy, 3Dz, 2D, 1Dz)
     * @default 3D
     * @format selected string from allowed options
     * @values 3D, 3Dxy, 3Dz, 2D, 1Dz, 1D
     * @note When computing variograms, use --dim to specify the dimension type for variogram analysis. Available options are:
     * - 3D: Computes the variogram considering all three spatial dimensions (X, Y, Z). This is the default option.
     * - 3Dxy: Computes the variogram considering only the horizontal dimensions (X and Y), ignoring vertical differences. This is useful for analyzing spatial continuity in the horizontal plane.
     * - 3Dz: Computes the variogram considering only the vertical dimension (Z), ignoring horizontal differences. This is useful for analyzing spatial continuity in the vertical direction.
     * - 2D: Computes the variogram in a two-dimensional space, typically in the horizontal plane (X and Y), without considering vertical differences. This is commonly used for surface data or when vertical continuity is not of interest.
     * - 1Dz: Computes the variogram in a one-dimensional vertical space (Z), ignoring horizontal differences. This is useful for analyzing vertical continuity, such as in stratigraphic or depth-related analyses.
     * @example --dir DIR --dim 3Dxy
     */
    ValueArg<std::string> varioDimension    ("", "dim", "Set variogram dimension type", false, "3D", &allowedValsVDm, cmd);

    // Option: compute variogram with variable/constant lag spacing
    std::vector<std::string> allowedLag = {"VARIABLE","CONSTANT","FIXED"};
    ValuesConstraint<std::string> allowedValsLag(allowedLag);
    /**
     * @brief Set lag spacing type (variable, constant or fixed). This option allows you to specify the type of lag spacing to be used when computing the experimental variogram. Lag spacing refers to the distance intervals at which the experimental variogram points are computed, and different types of lag spacing can be used depending on the characteristics of the data and the analysis goals.
     * @default VARIABLE
     * @format selected string from allowed options
     * @values VARIABLE, CONSTANT, FIXED
     * @note When computing the experimental variogram, use --lagspac to specify the type of lag spacing. Available options are:
     * - VARIABLE: The lag spacing is variable, meaning that the distance intervals between variogram points can vary based on the distribution of data pairs. This approach can provide a more detailed variogram in areas with dense data and a broader overview in areas with sparse data. Refer to Equations 3-5 in the correlated paper.
     * - CONSTANT: The lag spacing is constant, meaning that the distance intervals between variogram points are fixed and uniform across the variogram. This approach provides a consistent spacing of variogram points, which can be useful for comparing variograms across different datasets or for fitting theoretical models. Refer to Equation 2 in the correlated paper.
     * - FIXED: The lag spacing is fixed, meaning that the distance intervals between variogram points are predetermined and do not change based on the data distribution. This approach is similar to constant lag spacing but emphasizes that the intervals are set in advance and are not influenced by the data. This can be useful for specific applications where a predefined set of lag distances is required for analysis or model fitting.
     * @example --lagspac CONSTANT
     */
    ValueArg<std::string> setLagType        ("", "lagspac", "Set lag spacing type", false, "VARIABLE", &allowedValsLag, cmd);
    
    /**
     * @brief Set lag spacing samples for vertical variogram computation, combined with --lagspac FIXED. This option allows you to specify the spacing of samples in the vertical direction when computing the variogram, particularly for 3D or 1Dz variogram analyses. The spacing of samples can influence the resolution and accuracy of the variogram, especially in the vertical dimension where data may be more sparse or have different spatial characteristics compared to the horizontal dimensions.
     * @default 0.0 (no specific spacing defined).
     * @format double value (spacing of samples in the vertical direction)
     * @note Used in combination with --lagspac FIXED for vertical variogram computation. When using this option, the specified spacing should be provided in the same units as the spatial coordinates of the data (e.g., meters) to ensure proper spacing of variogram points in the vertical direction. This can help to improve the representation of spatial continuity in the vertical dimension, especially when analyzing stratigraphic or depth-related data.
     * @example --lagspac FIXED --spac 10
     */
    ValueArg<double> setSpacingSamples      ("", "spac", "Set lag spacing samples for vertical variogram computation", false, 0.0, "double", cmd);

    // Option: set parameters for directional variogram computation
    /**
     * @brief Set degree step (in degree) for directional variogram computation. This option allows you to specify the angular step size in degrees for computing directional variograms when using the --dir DIR option. The degree step defines the angles at which the directional variograms will be computed, allowing for analysis of spatial continuity in specific directions.
     * @default 45.0 degrees
     * @format double value (angular step size in degrees)
     * @note Used with --dir DIR for directional variogram computation to specify the angular step size for directional analysis. 
     * For example, a degree step of 30° would compute variograms at 0°, 30°, 60°, etc., in counter-clockwise from north, allowing for a more detailed analysis of spatial continuity in specific directions. 
     * The choice of degree step can influence the resolution of the directional variogram and should be selected based on the spatial characteristics of the data and the analysis goals.
     * @example --dir DIR --deg 30
     */
    ValueArg<double> setDegree              ("", "deg", "Set degree step (in degree) for directional variogram computation", false, 45.0, "double", cmd);
    
    /**
     * @brief Set tolerance (in degree) for directional variogram computation. This option allows you to specify the angular tolerance in degrees for including point pairs in the directional variogram analysis when using the --dir DIR option. The tolerance defines the angular range around the specified direction within which point pairs will be included in the computation of the directional variogram.
     * @default 45.0 degrees
     * @format double value (angular tolerance in degrees)
     * @note Used with --dir DIR for directional variogram computation to specify the angular tolerance for including point pairs in the directional variogram analysis. 
     * For example, with a tolerance of 15°, point pairs that are within 15° of the specified direction would be included in the analysis, allowing for a more flexible and inclusive approach to directional variogram computation. 
     * The choice of tolerance can influence the results of the directional variogram and should be selected based on the spatial characteristics of the data and the desired level of directional specificity in the analysis.
     */
    ValueArg<double> setTol                 ("", "degtol", "Set tolerance (in degree) for directional variogram computation", false, 45.0, "double", cmd);
    
    /**
     * @brief Set vertical tolerance (in degree) for directional variogram computation. This option allows you to specify the vertical angular tolerance in degrees for including point pairs in the directional variogram analysis when using the --dir DIR option, particularly for 3D directional variogram computation. The vertical tolerance defines the angular range in the vertical direction within which point pairs will be included in the computation of the directional variogram, allowing for a more comprehensive analysis of spatial continuity in three-dimensional space.
     * @default 22.5 degrees
     * @format double value (vertical angular tolerance in degrees)
     * @note Used with --dir DIR for directional variogram computation. This parameter is particularly important for 3D directional variogram analysis, as it allows for the inclusion of point pairs that are within a specified vertical angular range, providing a more complete representation of spatial continuity in three-dimensional space.
     * @example --dir DIR --zdegtol 15
     */
    ValueArg<double> setVertTol             ("", "zdegtol", "Set vertical tolerance (in degree) for directional variogram computation", false, 22.5, "double", cmd);

    /**
     * @brief Set discrete directions (in degree) for directional variogram computation. This option allows you to specify a list of discrete directions in degrees for computing directional variograms when using the --dir DIR option. Instead of using a fixed degree step, you can provide specific angles at which the directional variograms will be computed, allowing for a more customized analysis of spatial continuity in specific directions.
     * @default "dir0,dir1"
     * @format comma-separated list of double values (angles in degrees)
     * @note Used with --dir DIR for directional variogram computation. When using this option, you can provide a comma-separated list of angles in degrees (e.g., "0,45,90") to specify the discrete directions for which the directional variograms will be computed. This allows for a more targeted analysis of spatial continuity in specific directions that may be of particular interest based on the spatial characteristics of the data or the analysis goals. 
     * The specified directions should be provided in degrees and will be used to compute the directional variograms at those specific angles, providing insights into the spatial continuity in those directions.
     * @example --dir DIR --dirs 0,45,90
     */
    ValueArg<std::string> loadDirs          ("", "dirs", "Set discrete directions (in degree) for directional variogram computation", false, "dir0,dir1", "string", cmd);

    /**
     * @brief Set bandwidth for directional variogram computation. This option allows you to specify the bandwidth for directional variogram computation when using the --dir DIR option. The bandwidth defines the spatial range within which point pairs will be included in the computation of the directional variogram, allowing for a more flexible analysis of spatial continuity in specific directions.
     * @param bandw Flag to set bandwidth for directional variogram computation
     * @default DBL_MAX
     * @format double value (spatial range in the same units as the data coordinates)
     * @note Used with --dir DIR for directional variogram computation to specify the bandwidth for including point pairs in the directional variogram analysis. 
     * The bandwidth is typically provided in the same units as the spatial coordinates of the data (e.g., meters) and defines the maximum distance between point pairs that will be included in the computation of the directional variogram.
     * A smaller bandwidth can help to focus the analysis on more local spatial continuity, while a larger bandwidth can provide a broader overview of spatial continuity in the specified directions.
     * @example --dir DIR --bandw 100
     */
    ValueArg<double> setBandwidth           ("", "bandwidth", "Set bandwidth for directional variogram computation", false, DBL_MAX, "double", cmd);
    
    /**
     * @brief Set vertical bandwidth for directional variogram computation. This option allows you to specify the vertical bandwidth for directional variogram computation when using the --dir DIR option, particularly for 3D directional variogram analysis. The vertical bandwidth defines the spatial range in the vertical direction within which point pairs will be included in the computation of the directional variogram, allowing for a more comprehensive analysis of spatial continuity in three-dimensional space.
     * @param vertbandw Flag to set vertical bandwidth for directional variogram computation
     * @default DBL_MAX
     * @format double value (vertical spatial range in the same units as the data coordinates)
     * @note Default value is DBL_MAX. Used with --dir DIR for directional variogram computation to specify the vertical bandwidth for including point pairs in the directional variogram analysis.
     * The vertical bandwidth is typically provided in the same units as the spatial coordinates of the data (e.g., meters) and defines the maximum vertical distance between point pairs that will be included in the computation of the directional variogram.
     * A smaller vertical bandwidth can help to focus the analysis on more local spatial continuity in the vertical direction, while a larger vertical bandwidth can provide a broader overview of spatial continuity in the vertical dimension, especially when analyzing stratigraphic or depth-related data.
     * @example --dir DIR --vertbandw 50
     */
    ValueArg<double> setVertBandwidth       ("", "vertical-bandwidth", "Set vertical bandwidth for directional variogram computation", false, DBL_MAX, "double", cmd);

    // Option: types of permissible models
    std::vector<std::string> allowedModel = {"AUTO","SPHERICAL","GAUSSIAN","EXPONENTIAL","LINEAR","DEFAULT"};
    ValuesConstraint<std::string> allowedValsMl(allowedModel);
    /**
     * @brief Set type of model variogram to fit to the experimental variogram. This option allows you to specify the type of theoretical variogram model to be fitted to the experimental variogram when using the --vario MODEL option.
     * @default "AUTO"
     * @format selected string from allowed options
     * @values AUTO, SPHERICAL, GAUSSIAN, EXPONENTIAL, LINEAR, DEFAULT
     * @note When using --vario MODEL, this flag becomes important for fitting a theoretical variogram model to the experimental variogram. Available model types include:
     * - AUTO: The model type is automatically selected based on the characteristics of the experimental variogram. The fitting process will evaluate different model types and select the one that best fits the experimental variogram based on the specified MSE criterion (with --mse).
     * - SPHERICAL: Fits a spherical variogram model to the experimental variogram.
     * - GAUSSIAN: Fits a Gaussian variogram model to the experimental variogram.
     * - EXPONENTIAL: Fits an exponential variogram model to the experimental variogram.
     * - LINEAR: Fits a linear variogram model to the experimental variogram.
     * - DEFAULT: Uses a default variogram model (which may be predefined in the software) for fitting to the experimental variogram.
     * @example --vario MODEL --type SPHERICAL
     */
    ValueArg<std::string> setModel          ("", "type", "Set type of model variogram to fit to the experimental variogram", false, "AUTO", &allowedValsMl, cmd);

    // Option: set model parameters
    /**
     * @brief Set nugget for model variogram fitting. This option allows you to specify the nugget parameter for fitting a theoretical variogram model to the experimental variogram when using the --vario MODEL option. The nugget represents the variance at zero distance and can account for measurement error or short-scale variability in the data.
     * @default 0.0 (no nugget forcing is applied).
     * @format nugget!valcat
     * @note When using --vario MODEL, this flag becomes important for fitting a theoretical variogram model to the experimental variogram. The nugget parameter can influence the shape of the fitted variogram model and is often used in conjunction with the sill and range parameters to fully specify the variogram model.
     * @example --vario MODEL --nugget 0.1
     */
    ValueArg<double> setNugget              ("", "nugget", "Set nugget for model variogram fitting", false, 0.0, "double", cmd);

    /**
     * @brief Set sill for model variogram fitting. This option allows you to specify the sill parameter for fitting a theoretical variogram model to the experimental variogram when using the --vario MODEL option. The sill represents the variance at large distances and is often used in conjunction with the nugget and range parameters to fully specify the variogram model.
     * @param sill Flag to set sill for model variogram fitting (NOT ENABLED)
     */
    ValueArg<double> setSill                ("", "sill", "Set sill for model variogram fitting (NOT ENABLED)", false, 0.0, "double", cmd);
   
    //ValueArg<double> setRange               ("", "range", "Set range for model variogram fitting", false, 0.0, "double", cmd);


    // Option: set model parameters - INDICATOR VARIOGRAMS
    /**
     * @brief Set type of model variogram for indicators. This option allows you to specify the type of theoretical variogram model to be fitted to the experimental variogram for indicator variables when using the --vario MODEL option. The category for indicators is specified by its value (it must be an integer), which allows for fitting different variogram models for different categories of indicator variables.
     * @format type!valcat
     * @note When using --vario MODEL for indicator variables, this flag becomes important for fitting a theoretical variogram model to the experimental variogram for each category of indicator variables. 
     * The category is specified by its value (it must be an integer), allowing for the fitting of different variogram models for different categories of indicator variables, which can be useful for capturing the spatial continuity of different classes or categories in the data.
     * @example --vario MODEL --itype SPHERICAL!1 --itype GAUSSIAN!2
     */
    MultiArg<std::string> setIndModel       ("", "itype", "Set type of model variogram for indicators. The category is specified by its value (it must be an integer).", false, "type!valcat", cmd);
    
    /**
     * @brief Set nugget for indicators. This option allows you to specify the nugget parameter for fitting a theoretical variogram model to the experimental variogram for indicator variables when using the --vario MODEL option. The category for indicators is specified by its value (it must be an integer), which allows for fitting different nugget values for different categories of indicator variables.
     * @format nugget!valcat
     * @note When using --vario MODEL for indicator variables, this flag becomes important for fitting a theoretical variogram model to the experimental variogram for each category of indicator variables. The category is specified by its value (it must be an integer), allowing for the fitting of different nugget values for different categories of indicator variables, which can be useful for capturing the spatial continuity and variability of different classes or categories in the data, especially when different categories may have different levels of measurement error or short-scale variability that can be accounted for with different  nugget values.
     * @example --vario MODEL --indnugget 0.1!1 --indnugget 0.2!2
     */
    MultiArg<std::string> setIndNugget      ("", "inugget", "Set nugget for indicators. The category is specified by its value (it must be an integer).", false, "0.0!valcat", cmd);
    
    /**
     * @brief Set max distance for computing variogram for indicators. This option allows you to specify the maximum distance to be considered when computing the experimental variogram for indicator variables when using the --vario MODEL option. The category for indicators is specified by its value (it must be an integer), which allows for setting different maximum distances for different categories of indicator variables.
     * @format maxdist!valcat
     * @note When using --vario MODEL for indicator variables, this flag becomes important for computing the experimental variogram for each category of indicator variables. The category is specified by its value (it must be an integer), allowing for setting different maximum distances for different categories of indicator variables, which can be useful for managing the variogram coverage and ensuring that the variogram is computed over a meaningful spatial range for each category, especially when different categories may have different spatial distributions or ranges of influence in the data.        
     * @example --vario MODEL --indmaxdist 100!1 --indmaxdist 200!2
     */
    MultiArg<std::string> setIndMaxDistance ("", "imaxdist", "Set max distance for computing variogram for indicators (NOT ENABLE)", false, "-DBL_MAX!namecat", cmd);
    
    //MultiArg<std::string> setIndSill        ("", "isill", "Set sull for indicators", false, "0.0|ncat", cmd);


    // Option: different criteria for fitting variogram models for mse computation (active only for continuous variables)
    std::vector<std::string> allowedMSEcrit = {"CRESSIE","CRESSIE_WEIGHTED","CRESSIE_WEIGHTED_MODIFIED","DEFAULT"};
    ValuesConstraint<std::string> allowedValsMSE(allowedMSEcrit);
    /**
     * @brief Set criterion for fitting variogram models for mse computation (active only for continuous variables). This option allows you to specify the criterion to be used for fitting theoretical variogram models to the experimental variogram based on mean squared error (MSE) computation when using the --vario MODEL option for continuous variables. The choice of criterion can influence the selection of the best-fitting variogram model and can help to achieve a more accurate representation of spatial continuity in the data.
     * @default CRESSIE_WEIGHTED_MODIFIED. 
     * @format selected string from allowed options
     * @values CRESSIE, CRESSIE_WEIGHTED, CRESSIE_WEIGHTED_MODIFIED, DEFAULT
     * @note When using --vario MODEL for continuous variables, this flag becomes important for fitting theoretical variogram models to the experimental variogram based on mean squared error (MSE) computation. Available criteria include:
     * - CRESSIE: Uses the Cressie criterion for fitting variogram models, which is a commonly used approach that weights the squared differences between the experimental and model variogram values by the number of point pairs contributing to each variogram point, providing a more robust fitting that accounts for the variability in the number of point pairs across different lag distances.
     * - CRESSIE_WEIGHTED: Uses a weighted version of the Cressie criterion for fitting variogram models, which further emphasizes the influence of variogram points with a higher number of point pairs, providing an even more robust fitting that can help to mitigate the influence of noisy variogram points that are computed with a low number of point pairs.
     * - CRESSIE_WEIGHTED_MODIFIED: Uses a modified weighted version of the Cressie criterion for fitting variogram models, which incorporates additional adjustments to the weighting scheme to further enhance the robustness of the fitting process, particularly in cases where the experimental variogram may have a high level of noise or variability in the number of point pairs across different lag distances.
     * - DEFAULT: Uses a default criterion for fitting variogram models based on MSE computation, which may be predefined in the software and can provide a standard approach to variogram model fitting without requiring the user to specify a particular criterion, allowing for a more streamlined analysis when the specific choice of criterion is not a primary concern for the user.
     * @example --vario MODEL --mse CRESSIE_WEIGHTED
     */
    ValueArg<std::string> setMSEcriterion ("", "mse", "Set mse criterion for fitting variogram. (Active only for continuos variables).", false, "CRESSIE_WEIGHTED_MODIFIED", &allowedValsMSE, cmd);


    allowedStratigraphicCondition.clear();
    allowedVarioType.clear();
    allowedVarioDir.clear();
    allowedVarioDim.clear();
    allowedLag.clear();
    allowedModel.clear();
    allowedMSEcrit.clear();


    // ---------------------------------------------------------------------------------------------------------
    // ADDITIONAL FUNCTIONALITIES:

    // Option: set parameters
    /**
     * @brief Set number of experimental variogram points (bins). This option allows you to specify the number of points to be used for discretizing the experimental variogram when using the --vario EXPERIMENTAL option. The number of points can influence the level of detail and noise in the variogram estimation, and adjusting this parameter can help to achieve a more robust variogram estimation based on the specific characteristics of the data.
     * @default 15
     * @format integer value
     * @note Optional parameter used for experimental variogram computation. Default value is 15. 
     * This parameter sets the number of points used for discretizing the experimental variogram, which can help to manage the trade-off between detail and noise in the variogram estimation. A higher number of points can provide a more detailed variogram but may also introduce more noise, while a lower number of points can result in a smoother variogram but may miss important features. 
     * Adjusting this parameter can help to achieve a more robust variogram estimation based on the specific characteristics of the data.
     * @example --npoints 20
     */
    ValueArg<uint> setPointsVario           ("", "npoints", "Set number of experimental variogram points (bins)", false, 15, "int", cmd); //n. di punti per la discretizzazione del variogramma sperimentale = 15 di default

    /**
     * @brief Set number of variogram points pairs to compute variogram experimental points avoiding noisy variogram points. This option allows you to specify the minimum number of point pairs required for computing each experimental variogram point when using the --vario EXPERIMENTAL option. Setting a minimum number of point pairs can help to avoid including noisy variogram points that are computed with a low number of point pairs, which can lead to a more robust variogram estimation.
     * @default 0
     * @format integer value
     * @example --vclean 10
     */
    ValueArg<uint> setVarioCleanPoints      ("", "vclean", "Set number of variogram points pairs to compute variogram experimental points avoiding noisy variogram points", false, 0, "int", cmd); //n. di punti minimo (clean variogram) = 10 di default

    /**
     * @brief Set range step for computing variogram model. This option allows you to specify the range step for computing the variogram model when using the --vario MODEL option. The range step defines the distance intervals at which the variogram model will be computed, and adjusting this parameter can help to achieve a more detailed or smoother representation of the fitted variogram model based on the specific characteristics of the data and the analysis goals.
     * @default 100.0
     * @format double value
     * @example --rangestep 50.0
     */
    ValueArg<double> setRangeStep           ("", "rangestep", "Set range step for computing variogram model", false, 100.0, "double", cmd);

    /**
     * @brief Set nugget step for computing variogram model. This option allows you to specify the nugget step for computing the variogram model when using the --vario MODEL option. The nugget step defines the intervals at which the nugget parameter will be evaluated during the fitting of the variogram model, and adjusting this parameter can help to achieve a more accurate representation of the spatial variability at short distances, especially when there is a significant nugget effect in the data.
     * @default 100.0
     * @format double value
     * @example --nugstep 50.0
     */
    ValueArg<double> setNuggetStep          ("", "nugstep", "Set nugget step for computing variogram model", false, 100.0, "double", cmd);

    /**
     * @brief Set maximum points distance to flexibly manage the variogram coverage (dc). When compute experimental variogram, a maximum value of coverage is set to half of maximum distance between pairs of sampled points, as the reference distance at which the variogram can be considered meaningful (dc=dmax/2). This option allows you to specify a maximum distance for computing the experimental variogram, which can help to manage the variogram coverage and ensure that the variogram is computed over a meaningful spatial range, especially when there are pairs of points that are very far apart and may not contribute meaningful information to the variogram estimation.
     * @default -DBL_MAX
     * @format double value
     * @example --maxdist 10
     */
    ValueArg<double> setMaxDistance         ("", "maxdist", "Set maximum points distance for computing experimental variogram", false, -DBL_MAX, "double", cmd);

    /**
     * @brief Set tolerance factor for lag tolerance to compute experimental variogram (tolerance = lag spacing/tolfac). This option allows you to specify a tolerance factor for computing the experimental variogram, which can help to manage the trade-off between detail and noise in the variogram estimation. The tolerance factor is used to calculate the lag tolerance (tol) based on the lag spacing, and adjusting this parameter can help to achieve a more robust variogram estimation by controlling the inclusion of point pairs in the computation of the experimental variogram points.
     * @default 2.0
     * @format double value
     * @example --tolfac 1.5
     */
    ValueArg<double> setToleranceFactor     ("", "tolfac", "Set tolerance factor for computing experimental variogram", false, 2.0, "double", cmd);

    /**
     * @brief Set lag growth factor (lgf) for computing variable lag spacing for experimental variogram. When using variable lag spacing, this parameter can be used to set a lag growth factor (lgf) that controls how the lag spacing increases with distance. A higher lgf will result in a more rapid increase in lag spacing, while a lower lgf will result in a more gradual increase. Adjusting this parameter can help to achieve a more robust variogram estimation by managing the trade-off between detail and noise in the variogram points, especially at larger distances where data may be sparser.
     * @default 1.0
     * @format double value
     * @example --lgf 1.5
     */
    ValueArg<double> setFactor              ("", "lgf", "Set lag growth factor (lgf) for computing variable lag spacing for experimental variogram", false, 1.0, "double", cmd);

    /**
     * @brief Set weight on nugget. This option allows you to specify a weight on the nugget effect when computing directional variograms, particularly for 2D directional variogram analysis. The weight on the nugget can influence the contribution of the nugget effect to the overall variogram estimation, and adjusting this parameter can help to achieve a more accurate representation of spatial continuity in specific directions, especially when there is a significant nugget effect in the data that may vary across different directions.
     * @default false (no weight on nugget).
     * @format boolean flag
     * @note Used with --dir DIR for directional variogram computation. 
     * @example --dir DIR --weight
     */
    SwitchArg setWeight                     ("", "weight", "Set weight on nugget to compute directional variogram", cmd, false); //booleano

    //  Option: plotting tools
    /**
     * @brief Set eps for plot centering. This option allows you to specify an epsilon value for centering the variogram plot when visualizing the experimental and model variograms. 
     * @default 0.0
     * @format double value
     * @note Enable for centering anisotropy ellipse plot on the origin.
     * @example --eps 0.1
     */
    ValueArg<double> setEps                 ("", "eps", "Set eps for plot centering", false, 0.0, "double", cmd);

    /**
     * @brief Set number of points for variogram model plot - only for visualization purposes, it does not affect the variogram computation. This parameter can be used to set the number of points used for plotting the variogram model, which can help to achieve a smoother or more detailed visualization of the fitted model. A higher number of points will result in a smoother curve, while a lower number of points may result in a more jagged curve that follows the experimental variogram more closely.
     * @default 100
     * @format integer value
     * @note This parameter does not affect the variogram computation, but only the visualization of the fitted model.
     * @example --n 500
     */
    ValueArg<size_t> setNxvis ("", "vm-npoints", "Set number of points for variogram model plot (only for visualization purposes)", false, 100, "size_t", cmd);
    
    /**
     * @brief Set flag to compute variogram diagnostics. When set, this flag enables the computation of variogram diagnostics, which can include various analyses to assess the quality and characteristics of the computed variogram. The diagnostics can help to evaluate the robustness of the variogram estimation and identify potential issues or insights based on the original variable data.
     * @default false
     * @format boolean flag
     * @note Using this flag requires --load flag to load experimental variogram points to be checked and --setVariableFile flag to specify the input file for the original value.
     * - --load: Experimental variogram points (mandatory)
     * - --file: Original values (mandatory)
     * @example -D --load /path/to/experimental/variogram/points.txt --setVariableFile /path/to/variable_input.dat
     */
    SwitchArg setVarioDiagnose ("D", "diagnose", "Compute variogram diagnostics", cmd, false); //booleano

    /**
     * @brief Load experimental variogram by txt file to set internal experimental variogram data structure and store it in json file.
     * @default false
     * @format string value (path to txt file containing experimental variogram points)
     * @note This flag allows to load an experimental variogram from a txt file, which can be useful to set the internal experimental variogram data structure without the need to recompute the experimental points.
     * This is used for fitting a model variogram to an already computed experimental variogram, or for plotting the experimental variogram without recomputing it.
     * @example --load /path/to/experimental/variogram/points.txt
     */
    ValueArg<std::string> loadExpVario ("", "load", "Load experimental variogram by txt file to set internal experimental variogram data structure and store a json file", false, "filename", "string", cmd);

    /**
     * @brief Set variable input file to perform variogram diagnostics based on the original variable data. This option allows you to specify an input file for the variable to be analyzed in the variogram computation, which can be particularly useful when performing variogram diagnostics to assess the quality and characteristics of the computed variogram. The input file should contain the necessary data for the specified variable (an unique column of sampled values), and using this flag in combination with the --diagnose flag can provide insights into the spatial continuity and variability of the original variable data, which can help to evaluate the robustness of the variogram estimation and identify potential issues or insights based on the original variable data.
     * @default false
     * @format string value (path to variable input file)
     * @example --setVariableFile /path/to/variable_input.dat
     */
    ValueArg<std::string> setVariableFile ("", "file", "Set variable input file", false, "filename-variable", "string", cmd);


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
    std::string out_folder      = Project.folder + "/out";
    std::string metadata_folder = out_folder + "/" + app_data + "/metadata";
    std::string data_folder     = out_folder + "/" + app_data + "/data";



    // 0) Define meta for vario
    VarioMeta metavario;
    metavario.setProject(Project);

    std::vector<std::string> excommands;
    excommands.push_back(command);
    metavario.setCommands(excommands);

    // 0) Define meta for vario - dependencies
    std::vector<std::string> depsvario;



    // ---------------------------------------------------------------------------------------------------------
    // STARTS:

    if(varioCompute.isSet())
    {
        std::vector<MUSE::VarioFrame> vec_vario_frame;

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
            MUSE::VarioFrame vario_frame;
            MUSE::VarioFrame::StatisticsInfo stats;

            count_frame++;

            depsvario.clear();

            filesystem::path dir = l;
            filesystem::path rel_datadir = filesystem::relative(dir, abs_datadir);

            std::vector<std::string> list_json = get_files(l, ".json");
            if(list_json.size() > 1)
            {
                std::cerr << "ERROR. Only a file JSON is expected!" << std::endl;
                exit(1);
            }

            MUSE::DataMeta datameta;
            datameta.read(list_json.at(0));

            MUSE::InfoData infodata = datameta.getInfoData();
            metavario.setInfoData(infodata);

            VarioMeta::Processing infovar;
            infovar.v_name = Variable.getValue();
            if(setDeclustering2d.isSet())
                infovar.declustering = "YES";
            infovar.normal_score = setNormalScore.getValue();
            metavario.setProcessing(infovar);

            std::string man_folder = out_folder + "/" + app_manipulate;
            std::string app_folder = out_folder + "/" + app_name;
            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);

            if(rel_datadir.string().compare(".") != 0)
            {
                app_folder += "/" + rel_datadir.string();
                man_folder += "/" + rel_datadir.string();

                std::cout << std::endl;
                std::cout << "###########################" << std::endl;
                std::cout << "### NUMBER OF TIME FRAMES: " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME N° " << count_frame << " ON " << list_dir.size() << std::endl;
                std::cout << "### TIME FRAME NAME: " << rel_datadir.string() << std::endl;
                std::cout << std::endl;
            }

            if(!filesystem::exists(app_folder))
                filesystem::create_directory(app_folder);

            app_folder += "/" + infovar.v_name;
            if(subDataset.isSet())
                app_folder += "_" + subDataset.getValue();

            app_folder += "_" + varioDirection.getValue() + varioDimension.getValue();


            filesystem::create_directory(app_folder);
            std::string vario_filename = app_folder + "/" + infovar.v_name;


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
            
            // ================================
            // 1. LOADING COORDINATES AND ID
            // ================================
            if(!stratCondition.isSet()) //Condizione di default
            {
                std::cout << "\033[0;33m=== WARNING: No stratigraphic trasformation is set.\033[0m" << std::endl;

                if(datameta.getInfoData().id_name.compare("Unknown") != 0)
                {
                    readTextValues(l + "/data/" + datameta.getInfoData().id_name + ".dat", id);
                    depsvario.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().id_name + ".json", Project.folder));
                    std::cout << "=== Loaded ... coordinates points ID - size: " << id.size() << std::endl;
                }
                else
                    std::cerr << "ERROR reading ID: " << l + "/data/" + datameta.getInfoData().id_name + ".dat" << " NOT found." << std::endl;

                if(datameta.getInfoData().x_name.compare("Unknown") != 0)
                {
                    readCoordinate(l + "/data/" + datameta.getInfoData().x_name + ".dat", xCoord);
                    depsvario.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().x_name + ".json", Project.folder));
                    std::cout << "=== Loaded ... coordinates points X - size: " << xCoord.size() << std::endl;
                }
                else
                    std::cerr << "ERROR reading X coordinate: " << l + "/data/" + datameta.getInfoData().x_name + ".dat" << " NOT found." << std::endl;

                if(datameta.getInfoData().y_name.compare("Unknown") != 0)
                {
                    readCoordinate(l + "/data/" + datameta.getInfoData().y_name + ".dat", yCoord);
                    depsvario.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().y_name + ".json", Project.folder));
                    std::cout << "=== Loaded ... coordinates points Y - size: " << yCoord.size() << std::endl;
                }
                else
                    std::cerr << "ERROR reading Y coordinate: " << l + "/data/" + datameta.getInfoData().y_name + ".dat" << " NOT found." << std::endl;

                if(datameta.getInfoData().z_name.compare("Unknown") != 0)
                {
                    readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                    depsvario.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().z_name + ".json", Project.folder));
                    std::cout << "=== Loaded ... coordinates points Z - size: " << zCoord.size() << std::endl;
                }
                else
                {
                    zCoord.resize(xCoord.size(), 0.0);
                    std::cout << "=== Z coordinate is missing. Set it as 0.0 for all points." << std::endl;
                }
                std::cout << std::endl;
            }
            else
            {
                std::cout << "\033[0;33m=== WARNING: Stratigraphic transformation is set on " << stratCondition.getValue() << ". Variogram is computed in stratigraphic coordinate system.\033[0m" << std::endl;
                std::cout << "=== Stratigraphic coordinates are located in " << out_folder + "/" + app_manipulate + "/" << std::endl;
                load_xyzfile(man_folder + "/" + filenameStrat.getValue() + ".xyz", xCoord, yCoord, zCoord);
                depsvario.push_back(filesystem::relative(man_folder + "/" + filenameStrat.getValue() + ".json", Project.folder));
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

            if(setDebug.isSet())
            {
                std::cout << "=== Debug: Check loaded coordinates points (x,y,z) ... " << std::endl;
                for(size_t i=0; i< 10 && i< xCoord.size(); i++)
                {
                    std::cout << "Point " << i << ": (" << xCoord.at(i) << ", " << yCoord.at(i) << ", " << zCoord.at(i) << ")" << std::endl;
                }
                std::cout << "=== Debug: Check loaded coordinates points (x,y,z) ... COMPLETED." << std::endl;
            }

            VarioMeta::Manipulate processingData;
            processingData.stratigraphic_transf = stratCondition.getValue();
            processingData.filename = filenameStrat.getValue();


            // ================================
            // 3. LOAD VALUES
            // ================================
            // Storing json information into class Data
            MUSE::Metadata meta_input;
            meta_input.read(l + "/metadata/" + infovar.v_name + ".json");
            depsvario.push_back(filesystem::relative(l + "/metadata/" + infovar.v_name + ".json", Project.folder));

            MUSE::Data data = meta_input.getData(0); //gli input hanno un unico header!!
            data.setType(data.flag);
            readTextValues(l + "/data/" + infovar.v_name + ".dat", data.text_values);


            DataSummary sumdata;
            sumdata.setSummary(data);

            size_t n_sample = data.text_values.size();

            // Check if the number of samples in the coordinate vectors matches the number of samples in the variable data
            if(xCoord.size() != n_sample)
            {
                std::cerr << "=== WARNING in loading data. The number of samples in the coordinate vectors does not match the number of samples in the variable data." << std::endl;
                std::cerr << "=== WARNING: mismatch coordinates (" << xCoord.size() << " vs " << n_sample << ")." << std::endl;
            }

            if(!stratCondition.isSet() && setRotAxis.isSet()) //Se non sono in coordinate stratigrafiche, posso applicare una rotazione ai dati grezzi prima di qualsiasi altra operazione
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
                metavario.setRotation(dataRotation_vario);
                std::cout << FGRN("=== Rotation on data ... COMPLETED.") << std::endl;
            }

            std::vector<std::string> corr_id;
            std::vector<double> conv_values, corr_x, corr_y, corr_z; //sampled data 
            if(subDataset.isSet())
            {
                std::cout << "=== Sub-dataset is set on " << subDataset.getValue() << ". Variogram is computed on the sub-dataset selected by muse-manipulate." << std::endl;
                std::cout << "=== Reading ... " << man_folder + "/" + subDataset.getValue() + ".json" << std::endl;

                processingData.sub_dataset = "YES";
                processingData.domain = subDataset.getValue();

                MUSE::ExtractionMeta extrmeta;
                extrmeta.read(man_folder + "/" + subDataset.getValue() + ".json");              
                depsvario.push_back(filesystem::relative(man_folder + "/" + subDataset.getValue() + ".json", Project.folder));

                //2) ESTRARRE SOTTODATASET DA INDICI
                const auto& indices = extrmeta.getDataExtraction().id_points;
                if(indices.empty())
                {
                    std::cerr << "=== ERROR: Vector of index is empty." << std::endl;
                    std::cerr << "=== Please check the JSON file for the sub-dataset extraction: " << man_folder + "/" + subDataset.getValue() + ".json" << std::endl;
                    std::cerr << "=== or use muse-manipulate (-E command)." << std::endl;
                    exit(1);
                }

                if(stratCondition.isSet())
                {
                    //Le coordinate sono già filtrate e ordinate secondo il vettore 'indices' 
                    //(prodotte da muse-manipulate): xCoord, yCoord, zCoord hanno già dimensione indices.size()
                    //Bisogna solo allinere i valori della variabile usando il vettore indices come riferimento
                    if(xCoord.size() != indices.size())
                    {
                        std::cerr << "=== ERROR: Mismatch between coordinate vectors and index vector for sub-dataset extraction." << std::endl;
                        std::cerr << "=== Coordinate vectors size: " << xCoord.size() << ", Index vector size: " << indices.size() << std::endl;
                        std::cerr << "=== Please check the JSON file for the sub-dataset extraction: " << man_folder + "/" + subDataset.getValue() + ".json" << std::endl;
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
                    std::cout << "=== Checking rotation on data for sub-dataset extraction ... " << std::endl;
                    MUSE::Rotation dataRotation = extrmeta.getRotation();
                    if(dataRotation.rotation == true)
                    {
                        std::cout << std::endl;
                        std::cout << "Rotation is activate on data ... " << dataRotation.rotation << std::endl;
                        std::cout << "Rotation axis: " << dataRotation.rotation_axis << std::endl;
                        std::cout << "Rotation center: [" << dataRotation.rotation_center_x << "; " << dataRotation.rotation_center_y << "; " << dataRotation.rotation_center_z << "]" <<  std::endl;
                        std::cout << "Rotation angle (degree): " << dataRotation.rotation_angle << std::endl;
                        std::cout << std::endl;

                        // Applicare la rotazione ai dati originali prima di filtrare con gli indici
                        apply_rotation(dataRotation);
                        metavario.setRotation(dataRotation);
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

            metavario.setDependencies(depsvario); //added dependencies
            metavario.setManipulate(processingData);

            int n_conv_samples = conv_values.size(); //numero campioni convertiti da stringa a double
            if(n_conv_samples == 0)
            {
                std::cerr << "=== ERROR: No valid sample is available for variogram computation after conversion. Please check the variable data and the sub-dataset extraction." << std::endl;
                exit(1);
            }
            else
            {
                std::cout << "=== Data Statistical Summary ..." << std::endl;
                if(n_sample > n_conv_samples)
                    std::cout << "N (original)" << n_sample << std::endl;
                summary(conv_values);

                stats.mean = mean(conv_values);
                stats.var = variance(conv_values);
            }

            std::cout << "\033[0;32mReading MUSE format and data analysis... COMPLETED.\033[0m" << std::endl;
            std::cout << std::endl;

            xCoord.clear();
            yCoord.clear();
            zCoord.clear();

            if(setDebug.isSet())
            {
                std::cout << "=== Debug: Check loaded coordinates points (x,y,z) to pass at variogram computation ... " << std::endl;
                for(size_t i=0; i< 10 && i< corr_x.size(); i++)
                {
                    std::cout << "Point " << i << ": (" << corr_x.at(i) << ", " << corr_y.at(i) << ", " << corr_z.at(i) << ")" << std::endl;
                }
                std::cout << "=== Debug: Check loaded coordinates points (x,y,z) to pass at variogram computation ... COMPLETED." << std::endl;
            }

            // ================================
            // STARTING VARIO COMPUTATION
            // ================================
            if (data.type == varType::CATEGORIC)
            {
                std::cout << std::endl;
                std::cout << FGRN("=== VARTYPE CHECK: The variable is categoric.") << std::endl;
                std::cout << std::endl;

                // 1) Codifica ad indicatori prima di passare al variogramma!!
                std::vector<int> categ = categories_extraction(conv_values);
                std::vector<std::vector<int>> indicator_mat = indicator_transformation (conv_values, categ);

                // 2) Ciclo sulle categorie: per ogni categoria, ho un vettore adi indicatori e per quello calcolo il variogramma
                for(size_t cat=0; cat<indicator_mat.size(); cat++)
                {
                    std::vector<double> indicator_var;
                    for(size_t j=0; j< indicator_mat.at(cat).size(); j++)
                        indicator_var.push_back(static_cast<double>(indicator_mat.at(cat).at(j)));

                    std::cout << "### Category ID: " << cat << std::endl;
                    std::cout << "### Category VALUE: " << categ.at(cat) << std::endl;
                    double var_cat = variance(indicator_var);
                    std::cout << "### Data variance related to category " << categ.at(cat) << " is " << var_cat << std::endl;


                    bool dist_setcat = false;
                    double fix_dist = -DBL_MAX;
                    if(setIndMaxDistance.isSet())
                    {
                        for(size_t cd=0; cd< setIndMaxDistance.getValue().size(); cd++)
                        {
                            std::pair<std::string,std::string> distpair = split_string_pair(setIndMaxDistance.getValue().at(cd), '!');

                            if(std::stoi(distpair.second) == categ.at(cat))
                            {
                                dist_setcat = true;
                                fix_dist = std::stod(distpair.first);
                                break;
                            }
                        }
                    }

                    bool nug_setcat = false;
                    double fix_nug = 0.0;
                    if(setIndNugget.isSet())
                    {
                        for(size_t cn=0; cn< setIndNugget.getValue().size(); cn++)
                        {
                            //std::cout << setIndNugget.getValue().at(cn) << std::endl;
                            std::pair<std::string,std::string> nugpair = split_string_pair(setIndNugget.getValue().at(cn), '!');

                            if(std::stoi(nugpair.second) == categ.at(cat))
                            {
                                std::cout << "### Specifying nugget for category ID: " << cat << "; category NAME:" << categ.at(cat) << std::endl;
                                std::cout << "### Set nugget equal to: " << nugpair.first << " for category NAME: " << nugpair.second << std::endl;

                                nug_setcat = true;
                                fix_nug = std::stod(nugpair.first);
                                break;
                            }
                        }
                    }

                    bool mod_setcat = false;
                    std::string fix_mod = " ";
                    if(setIndModel.isSet())
                    {
                        for(size_t cm=0; cm< setIndModel.getValue().size(); cm++)
                        {
                            //std::pair<int,std::string> modforcat;
                            //std::cout << setIndModel.getValue().at(cm) << std::endl;
                            std::pair<std::string,std::string> modpair = split_string_pair(setIndModel.getValue().at(cm), '!');

                            if(std::stoi(modpair.second) == categ.at(cat))
                            {
                                std::cout << "### Specifying variogram model for category ID: " << cat << "; category NAME:" << categ.at(cat) << std::endl;
                                std::cout << "### Set variogram model equal to: " << modpair.first << " for category NAME: " << modpair.second << std::endl;

                                mod_setcat = true;
                                fix_mod = modpair.first;
                                break;
                            }
                        }
                    }


                    // 3) Per il calcolo del fitting mi serve il valore della varianza dei dati -> calcolo varianza degli indicatori per ogni cateogoria
                    // (NON LAVORO CON VARIABILE NORMALE INFATTI, LA CUI VARIANZA È UNITARIA, ovvero corrispondente all sill massimo raggiungibile nel vario modello)

                    VarioType option;
                    convert_from_str(varioType.getValue(), option);

                    VarioDirection dir;
                    convert_from_str(varioDirection.getValue(), dir);

                    switch (option)
                    {
                    case VarioType::EXPERIMENTAL: //solo variogramma sperimentale: può essere omnidirezionale/direzionale
                    {
                        switch (dir)
                        {
                        case VarioDirection::OMNI:
                        {
                            exp_variog exp_var;

                            // Compute the experimenatal variogram - OMNIDIRECTIONAL
                            if(varioDimension.getValue().compare("3D") == 0)
                            {
                                exp_var = exp_variogram (indicator_var, corr_x, corr_y, corr_z, setLagType.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                std::cout << "Computing 3D experimental variogram ... COMPLETED." << std::endl;
                            }
                            else if(varioDimension.getValue().compare("2D") == 0)
                            {
                                std::cout << "ERROR: NO 2D experimental variogram implementation... " << std::endl;
                                exit(1);
                            }
                            else if(varioDimension.getValue().compare("1D") == 0)
                            {
                                std::cout << "ERROR: NO 1D experimental variogram implementation... " << std::endl;
                                exit(1);
                            }
                            else
                            {
                                std::cout << "ERROR: Nothing to do ... " << std::endl;
                                exit(1);
                            }


                            VarioMeta::InfoVariogram info_vario;
                            info_vario.dimension = varioDimension.getValue();
                            info_vario.direction = varioDirection.getValue();

                            if(setVarioCleanPoints.isSet())
                                info_vario.clean_is_set = true;
                            info_vario.n_min_points_for_clean = setVarioCleanPoints.getValue();
                            metavario.setInfoVariogram(info_vario);


                            if(setVarioCleanPoints.isSet())
                            {
                                std::cout << "Clean experimental variogram ..." << std::endl;
                                exp_var = clean_exp_variogram (exp_var, setVarioCleanPoints.getValue());
                                std::cout << "Clean experimental variogram ... COMPLETED." << std::endl;
                            }


                            std::vector<MUSE::exp_variog_methods> variov;

                            MUSE::exp_variog_methods variovv;

                            variovv.setDirection(0.0);
                            variovv.setExpVariog_N(exp_var.N);
                            variovv.setExpVariog_h(exp_var.h);
                            variovv.setExpVariog_gamma(exp_var.gamma);

                            variov.push_back(variovv);

                            metavario.setDirExpVariog(variov);

                            std::cout << std::endl;



                            // Plotting experimental variogram
                            PlotStruct gamma_h;
                            for(size_t i=0; i < exp_var.gamma.size(); i++)
                            {
                                gamma_h.x.push_back(exp_var.h.at(i));          //distanze
                                gamma_h.y.push_back(exp_var.gamma.at(i));      //gamma (variogramma)
                            }
                            biv_plot(gamma_h, "Experimental Variogram", "Lag distance (m)", "Semivariogram");
                            matplot::save(app_folder + "/" + data.getName() + std::to_string(categ.at(cat)), "jpeg");
                            matplot::cla();
                            std::cout << std::endl;

                            break;
                        }

                        case VarioDirection::DIR:
                        {
                            VarioMeta::InfoVariogram info_vario;
                            info_vario.dimension = varioDimension.getValue();
                            info_vario.direction = varioDirection.getValue();
                            //info_vario.n_directions = 180/setDegree.getValue();


                            // Compute the experimenatal variogram - DIRECTIONAL

                            std::vector<exp_variog> dir_ex_var;
                            std::vector<double> directions(1,0);

                            if(varioDimension.getValue().compare("3D") == 0)
                            {
                                std::cout << "Computing 3D directional experimental variogram ... " << std::endl;
                                std::cout << FRED("ERROR: NO 3D directional experimental variogram implementation... ") << std::endl;
                                exit(1);
                            }
                            else if(varioDimension.getValue().compare("3Dxy") == 0)
                            {
                                std::cout << "Computing 3Dxy directional experimental variogram ... " << std::endl;
                                std::cout << FRED("ERROR: NO 3Dxy directional experimental variogram implementation... ") << std::endl;
                                exit(1);
                            }
                            else if(varioDimension.getValue().compare("2D") == 0)
                            {
                                if(!loadDirs.isSet())
                                {
                                    info_vario.n_directions = 180/setDegree.getValue();
                                    info_vario.degree_step = setDegree.getValue();
                                    info_vario.degree_tolerance = setTol.getValue();

                                    for(int i=1; i< info_vario.n_directions; i++)
                                        directions.push_back(directions.at(i-1) + setDegree.getValue());
                                }
                                else
                                {
                                    //std::cout << loadDirs.getValue() << std::endl;
                                    std::vector<std::string> direc = split_string(loadDirs.getValue(), ',');

                                    directions.at(0) = std::stod(direc.at(0));
                                    for(size_t i=1; i< direc.size(); i++)
                                        directions.push_back(std::stod(direc.at(i)));

                                    info_vario.n_directions = directions.size();
                                }
                                dir_ex_var = dir_exp_variogram (indicator_var, corr_x, corr_y, setLagType.getValue(), directions, setTol.getValue(), setBandwidth.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());

                                std::cout << "Computing 2D directional experimental variogram... COMPLETED." << std::endl;
                            }
                            else if(varioDimension.getValue().compare("1D") == 0)
                            {
                                std::cout << "Computing 1D directional experimental variogram ... " << std::endl;
                                std::cout << FRED("ERROR: NO 1D directional experimental variogram implementation... ") << std::endl;
                                exit(1);
                            }
                            else
                            {
                                std::cout << "ERROR: Nothing to do ... " << std::endl;
                                exit(1);
                            }


                            if(setVarioCleanPoints.isSet())
                                info_vario.clean_is_set = true;
                            info_vario.n_min_points_for_clean = setVarioCleanPoints.getValue();
                            metavario.setInfoVariogram(info_vario);


                            // 2) Clean directional variograms
                            int nzeros = 0;
                            if(setVarioCleanPoints.isSet())
                            {
                                std::cout << "Clean directional experimental variogram ..." << std::endl;
                                for(uint i=0; i< dir_ex_var.size(); i++)
                                    dir_ex_var.at(i) = clean_exp_variogram (dir_ex_var.at(i), setVarioCleanPoints.getValue(), nzeros);
                                std::cout << "Clean directional experimental variogram ... COMPLETED." << std::endl;
                            }

                            std::vector<MUSE::exp_variog_methods> variov;
                            for(size_t i=0; i<dir_ex_var.size(); i++)
                            {
                                MUSE::exp_variog_methods variovv;

                                variovv.setDirection(i*setDegree.getValue());
                                variovv.setExpVariog_N(dir_ex_var.at(i).N);
                                variovv.setExpVariog_h(dir_ex_var.at(i).h);
                                variovv.setExpVariog_gamma(dir_ex_var.at(i).gamma);

                                variov.push_back(variovv);

                            }
                            metavario.setDirExpVariog(variov);


                            // 3) Plot directional variograms
                            int i_dir = 0;
                            for(const exp_variog &v:dir_ex_var)
                            {
                                PlotStruct dir_gamma_h;
                                for(size_t j=0; j < v.gamma.size(); j++)
                                {
                                    dir_gamma_h.x.push_back(v.h.at(j));          //distanze
                                    dir_gamma_h.y.push_back(v.gamma.at(j));      //gamma (variogramma)
                                }
                                biv_plot(dir_gamma_h, "Experimental Variogram", "Lag distance (m)", "Semivariogram");
                                //matplot::save(app_folder + "/" + data.getName() + std::to_string(cat) +  "_dir" + std::to_string(i_dir), "jpeg");
                                matplot::save(app_folder + "/" + data.getName() + std::to_string(categ.at(cat)) +  "_dir" + std::to_string(i_dir), "jpeg");
                                matplot::cla();

                                i_dir++;
                            }

                            break;
                        }
                        }

                        break;
                    }
                    case VarioType::MODEL:
                    {
                        //se ho già lo sperimentale, lo leggo da json?!

                        //Set MSE criterion for fitting
                        weightsType w_type = weightsType::CRESSIE_WEIGHTED_MODIFIED;
                        if(setMSEcriterion.isSet())
                            w_type = stringToWeightsType(setMSEcriterion.getValue());
                        std::cout << "=== MSE criterion for fitting is set on: " << setMSEcriterion.getValue() << std::endl;


                        switch (dir)
                        {
                        case VarioDirection::OMNI:
                        {
                            variogram fitted_exp_var;

                            exp_variog exp_var;

                            if(loadExpVario.isSet())
                            {
                                //leggi un file .dat e riempi la struttura dati exp_var;
                                load_exp_variogram(loadExpVario.getValue(), exp_var);
                            }
                            else
                            {
                                //Compute experimental variogram
                                if(varioDimension.getValue().compare("3D") == 0)
                                {
                                    exp_var = exp_variogram (indicator_var, corr_x, corr_y, corr_z, setLagType.getValue(), setFactor.getValue(), setPointsVario.getValue(), fix_dist, setToleranceFactor.getValue());
                                    //else
                                        //exp_var = exp_variogram (indicator_var, corr_x, corr_y, corr_z, setLagType.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                    std::cout << "Computing 3D experimental variogram ... COMPLETED." << std::endl;
                                }
                                else if(varioDimension.getValue().compare("2D") == 0)
                                {
                                    std::cout << "ERROR: NO 2D experimental variogram implementation... " << std::endl;
                                    exit(1);
                                }

                                else if(varioDimension.getValue().compare("1D") == 0)
                                {
                                    std::cout << "ERROR: NO 1D experimental variogram implementation... " << std::endl;
                                    exit(1);
                                }
                                else
                                {
                                    std::cout << "ERROR: Nothing to do ... " << std::endl;
                                    exit(1);
                                }
                            }

                            VarioMeta::InfoVariogram info_vario;
                            info_vario.dimension = varioDimension.getValue();
                            info_vario.direction = varioDirection.getValue();

                            if(setVarioCleanPoints.isSet())
                                info_vario.clean_is_set = true;
                            info_vario.n_min_points_for_clean = setVarioCleanPoints.getValue();
                            metavario.setInfoVariogram(info_vario);


                            if(setVarioCleanPoints.isSet())
                            {
                                std::cout << "Clean experimental variogram ..." << std::endl;
                                exp_var = clean_exp_variogram (exp_var, setVarioCleanPoints.getValue());
                                std::cout << "Clean experimental variogram ... COMPLETED." << std::endl;
                            }


                            std::vector<MUSE::exp_variog_methods> variov;

                            MUSE::exp_variog_methods variovv;

                            variovv.setDirection(0.0);
                            variovv.setExpVariog_N(exp_var.N);
                            variovv.setExpVariog_h(exp_var.h);
                            variovv.setExpVariog_gamma(exp_var.gamma);

                            variov.push_back(variovv);

                            metavario.setDirExpVariog(variov);


                            std::cout << std::endl;

                            if ((setIndModel.isSet() && mod_setcat) && (setIndNugget.isSet() && nug_setcat))
                            {
                                std::cout << "Fit variogram with fixed model type: " << fix_mod << " and fixed nugget: " << fix_nug << std::endl;
                                variogram_type model_type;
                                convert_from_str(fix_mod, model_type);

                                fitted_exp_var = fit_ind_variogram_2par (exp_var, setRangeStep.getValue(), setNugget.getValue(), var_cat, model_type, true, w_type);
                            }
                            else if(setIndNugget.isSet() && nug_setcat)
                            {
                                std::cout << "Fit variogram with fixed nugget: " << fix_nug << std::endl;
                                fitted_exp_var = fit_ind_variogram_1par (exp_var, setRangeStep.getValue(), fix_nug, var_cat, w_type);
                            }
                            else if(setIndModel.isSet() && mod_setcat)
                            {
                                std::cout << "Fit variogram with fixed model type: " << fix_mod << std::endl;
                                variogram_type model_type;
                                convert_from_str(fix_mod, model_type);

                                fitted_exp_var = fit_ind_variogram_1par (exp_var, setRangeStep.getValue(), setNuggetStep.getValue(), var_cat, model_type, w_type);
                            }
                            else if (setSill.isSet())
                            {
                                std::cout << "Fit variogram with fixed sill: " << setSill.getValue() << std::endl;
                                std::cout << FRED("COMMAND NOT ACTIVE!!") << std::endl;
                                exit(1);
                            }
                            else
                            {
                                std::cout << "Automatic fitting of experimental variogram ... " << std::endl;
                                fitted_exp_var = fit_ind_variogram(exp_var, setRangeStep.getValue(), setNuggetStep.getValue(), var_cat, w_type);
                            }

                            std::vector<MUSE::variogram_methods> fitvario;

                            MUSE::variogram_methods fitvariov;

                            fitvariov.setDirection(0.0);
                            fitvariov.setNugget(fitted_exp_var.nugget);
                            fitvariov.setSill(fitted_exp_var.sill);
                            fitvariov.set_range(fitted_exp_var.range);
                            fitvariov.setType(fitted_exp_var.type);

                            fitvario.push_back(fitvariov);

                            metavario.setFitExpVariog(fitvario);




                            // Plotting experimental variogram + model
                            PlotStruct gamma_h;
                            for(size_t i=0; i < exp_var.gamma.size(); i++)
                            {
                                gamma_h.x.push_back(exp_var.h.at(i));          //distanze
                                gamma_h.y.push_back(exp_var.gamma.at(i));      //gamma (variogramma)
                            }
                            variogram_plot(gamma_h, fitted_exp_var, "Fitted Experimental Variogram Model", "Lag distance", "Semivariogram", setNxvis.getValue());



                            if(subDataset.isSet())
                                matplot::save(app_folder + "/" + data.getName() + std::to_string(categ.at(cat)) + "_" + subDataset.getValue(), "jpeg");
                            else
                                matplot::save(app_folder + "/" + data.getName() + std::to_string(categ.at(cat)), "jpeg");

                            matplot::cla();
                            std::cout << std::endl;

                            //FINE PLOT

                            break;
                        }

                        case VarioDirection::DIR:
                        {
                            VarioMeta::InfoVariogram info_vario;
                            info_vario.dimension = varioDimension.getValue();
                            info_vario.direction = varioDirection.getValue();

                            std::vector<exp_variog> dir_ex_var;
                            std::vector<double> directions(1,0);

                            if(!loadDirs.isSet())
                            {
                                info_vario.n_directions = 180/setDegree.getValue();
                                info_vario.degree_step = setDegree.getValue();
                                info_vario.degree_tolerance = setTol.getValue();

                                for(int i=1; i< info_vario.n_directions; i++)
                                    directions.push_back(directions.at(i-1) + setDegree.getValue());
                            }
                            else
                            {
                                info_vario.set_directions = loadDirs.getValue();
                                std::vector<std::string> direc = split_string(loadDirs.getValue(), ',');

                                directions.at(0) = std::stod(direc.at(0));
                                for(size_t i=1; i< direc.size(); i++)
                                    directions.push_back(std::stod(direc.at(i)));

                                info_vario.n_directions = directions.size();
                            }

                            if(varioDimension.getValue().compare("3D") == 0)
                            {
                                std::cout << "Computing 3D directional experimental variogram ... " << std::endl;
                                std::cout << FRED("ERROR: NO 3D directional experimental variogram implementation... ") << std::endl;
                                exit(1);
                            }
                            else if(varioDimension.getValue().compare("3Dxy") == 0)
                            {
                                std::cout << "Computing 3Dxy directional experimental variogram ... " << std::endl;
                                dir_ex_var = dir3DH_exp_variogram (indicator_var, corr_x, corr_y, corr_z, setLagType.getValue(), directions, setTol.getValue(), setVertTol.getValue(), setBandwidth.getValue(), setVertBandwidth.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());

                                std::cout << "Computing 3D HORIZONTAL directional experimental variogram... COMPLETED." << std::endl;

                                //std::cout << FRED("ERROR: NO 3Dxy directional experimental variogram implementation... ") << std::endl;
                                //exit(1);
                            }
                            else if(varioDimension.getValue().compare("3Dz") == 0)
                            {
                                directions.clear();
                                directions.push_back(0.0);
                                std::cout << "### Calculation direction is fixed on 0 degree from Z axis." << std::endl;

                                dir_ex_var = dir3DV_exp_variogram (indicator_var, corr_x, corr_y, corr_z, setLagType.getValue(), setSpacingSamples.getValue(), directions, setVertTol.getValue(), setBandwidth.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                std::cout << "Computing 3D VERTICAL directional experimental variogram... COMPLETED." << std::endl;
                            }
                            else if(varioDimension.getValue().compare("2D") == 0)
                            {
                                dir_ex_var = dir_exp_variogram (indicator_var, corr_x, corr_y, setLagType.getValue(), directions, setTol.getValue(), setBandwidth.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                std::cout << "Computing 2D directional experimental variogram... COMPLETED." << std::endl;
                            }
                            else if(varioDimension.getValue().compare("1D") == 0)
                            {
                                std::cout << "Computing 1D directional experimental variogram ... " << std::endl;
                                std::cout << FRED("ERROR: NO 1D directional experimental variogram implementation... ") << std::endl;
                                exit(1);
                            }
                            else
                            {
                                std::cout << "ERROR: Nothing to do ... " << std::endl;
                                exit(1);
                            }



                            if(setVarioCleanPoints.isSet())
                                info_vario.clean_is_set = true;
                            info_vario.n_min_points_for_clean = setVarioCleanPoints.getValue();
                            metavario.setInfoVariogram(info_vario);


                            // 2) Clean directional variograms
                            std::vector<int> zeros;
                            if(setVarioCleanPoints.isSet())
                            {
                                for(uint i=0; i< dir_ex_var.size(); i++)
                                {
                                    int nzeros = 0;
                                    dir_ex_var.at(i) = clean_exp_variogram(dir_ex_var.at(i), setVarioCleanPoints.getValue(), nzeros);

                                    zeros.push_back(nzeros); //prima nzeros-1
                                }
                                std::cout << FGRN("Clean directional experimental variogram ... COMPLETED.") << std::endl;
                                std::cout << std::endl;
                            }
                            else
                            {
                                std::cout << FYEL("WARNING: Clean directional experimental variogram is NOT set. Vector of zeros is not computed.") << std::endl;
                                zeros.resize(dir_ex_var.size(), 0.0);
                            }


                            std::vector<MUSE::exp_variog_methods> variov;
                            for(size_t i=0; i<dir_ex_var.size(); i++)
                            {
                                MUSE::exp_variog_methods variovv;

                                //variovv.setDirection(i*setDegree.getValue());
                                variovv.setDirection(directions.at(i));

                                variovv.setExpVariog_N(dir_ex_var.at(i).N);
                                variovv.setExpVariog_h(dir_ex_var.at(i).h);
                                variovv.setExpVariog_gamma(dir_ex_var.at(i).gamma);

                                variov.push_back(variovv);

                            }
                            metavario.setDirExpVariog(variov);



                            std::vector<double> weight;
                            weight.resize(zeros.size(), 1);
                            if(!setVarioCleanPoints.isSet() && setWeight.isSet())
                            {
                                std::cerr << "ERROR: Penalty function is associated with number of zeros detected by variogram cleaning funtion!" << std::endl;
                                std::cerr << "Set --vclean <npoints> to enable --weight command." << std::endl;
                                exit(1);
                            }

                            if(setVarioCleanPoints.isSet() && setWeight.isSet())
                            {
                                weight.clear();

                                if(zeros.size() == 0)
                                {
                                    std::cout << FRED("ERROR: Vector of zeros is empty!") << std::endl;
                                    exit(1);
                                }

                                std::cout << std::endl;
                                std::cout << FYEL("WARNING. Penalty function on: 1-(n_zeros(dir))*0.1") << std::endl;
                                for(size_t k=0; k<zeros.size(); k++)
                                {
                                    //double w = 1.0/(zeros.at(k)+1);
                                    double w = 1.0-(zeros.at(k))*0.1; //penalty function

                                    std::cout << "dir = " << directions.at(k) << "; n_zeros = " << zeros.at(k) << "; weight = " << w << std::endl;
                                    weight.push_back(w);
                                }
                                std::cout << FGRN("Penalty function (based on number of zeros in exmperimental variogram computation for each direction)... COMPLETED.") << std::endl;
                                std::cout << std::endl;
                            }


                            vector<variogram> vv;
                            try
                            {
                                if ((setIndModel.isSet() && mod_setcat) && (setIndNugget.isSet() && nug_setcat))
                                {
                                    std::cout << "Fit variogram with fixed model type: " << fix_mod << " and fixed nugget: " << fix_nug << std::endl;
                                    std::cout << FRED("Weight on nugget is not active!") << std::endl;
                                    std::cout << FMAG("Il peso non è attivo poichè non faccio la media dei nugget sulle direzioni (ovvero dove applico i pesi), ma fisso il nugget da cmd") << std::endl;
                                    variogram_type model_type;
                                    convert_from_str(fix_mod, model_type);

                                    vv = fit_ind_dir_variogram_2par (dir_ex_var, directions, setRangeStep.getValue(), fix_nug, var_cat, model_type, true, w_type);
                                }
                                else if(setIndNugget.isSet() && nug_setcat)
                                {
                                    std::cout << "Fit variogram with fixed nugget: " << fix_nug << std::endl;
                                    std::cout << FRED("Weight on nugget is not active!") << std::endl;
                                    std::cout << FMAG("Il peso non è attivo poichè non faccio la media dei nugget sulle direzioni (ovvero dove applico i pesi), ma fisso il nugget da cmd") << std::endl;
                                    vv = fit_ind_dir_variogram_1par (dir_ex_var, directions, setTol.getValue(), setRangeStep.getValue(), fix_nug, var_cat, true, w_type);
                                }
                                else if(setIndModel.isSet() && mod_setcat)
                                {
                                    std::cout << "Fitting of directional variogram is set using fixed model type: " << fix_mod << std::endl;
                                    variogram_type model_type;
                                    convert_from_str(fix_mod, model_type);

                                    vv = fit_ind_dir_variogram_1par (dir_ex_var, directions, setTol.getValue(), setRangeStep.getValue(), setNuggetStep.getValue(), var_cat, model_type, weight, true, w_type);
                                }
                                else if (setSill.isSet())
                                {
                                    std::cout << "Fit variogram with fixed sill: " << setSill.getValue() << std::endl;
                                    std::cout << FRED("COMMAND NOT ACTIVE!!") << std::endl;
                                    exit(1);
                                }
                                else
                                {
                                    std::cout << "Automatic fitting of directional variograms is set ..." << std::endl;
                                    vv = fit_ind_dir_variogram (dir_ex_var, directions, setTol.getValue(), setRangeStep.getValue(), setNuggetStep.getValue(), var_cat, weight, true, w_type);
                                }
                            }
                            catch (exception e)
                            {
                                std::cerr << "ERROR Computing directional variogram modeling ... " << e.what() << std::endl;
                            }


                            if(vv.empty())
                                std::cout << "fitting vector is empty ###########################" << std::endl;

                            std::vector<MUSE::variogram_methods> fitvario;
                            for(size_t i=0; i<vv.size(); i++)
                            {
                                variogram v = vv.at(i);

                                MUSE::variogram_methods fitvariov;

                                //fitvariov.setDirection(i*setDegree.getValue());
                                fitvariov.setDirection(directions.at(i));

                                fitvariov.setNugget(v.nugget);
                                fitvariov.setSill(v.sill);
                                fitvariov.set_range(v.range);
                                fitvariov.setType(v.type);

                                fitvario.push_back(fitvariov);
                            }
                            metavario.setFitExpVariog(fitvario);


                            // 3) Plot directional variograms
                            int i_dir = 0;
                            for(const exp_variog &v:dir_ex_var)
                            {
                                PlotStruct dir_gamma_h;
                                for(size_t j=0; j < v.gamma.size(); j++)
                                {
                                    dir_gamma_h.x.push_back(v.h.at(j));          //distanze
                                    dir_gamma_h.y.push_back(v.gamma.at(j));      //gamma (variogramma)
                                }

                                variogram_plot(dir_gamma_h, vv.at(i_dir), "Fitted Directional Variogram Model", "Lag distance", "Semivariogram");

                                matplot::save(app_folder + "/" + data.getName() + std::to_string(categ.at(cat)) + "_dir" + std::to_string(i_dir), "jpeg");
                                matplot::cla();

                                i_dir++;
                            }


                            //PLOT OF RANGES AND ELLIPSE FITTING (ON PLANE X-Y)
                            if(varioDimension.getValue().compare("3Dz") !=0)
                            {
                                PlotStruct h_plot;
                                for(size_t i=0; i<vv.size(); i++)
                                {
                                    variogram v = vv.at(i);

                                    h_plot.x.push_back(get_rangex(v.range, directions.at(i)));
                                    h_plot.y.push_back(get_rangey(v.range, directions.at(i)));

                                    h_plot.x.push_back(get_rangex(v.range, 180 + directions.at(i)));
                                    h_plot.y.push_back(get_rangey(v.range, 180 + directions.at(i)));
                                }


                                EllipseParameter summary;
                                fit_anisotropy_ellipse(h_plot.x, h_plot.y, summary);

                                std::cout << FMAG("phi in gradi = ") << get_degrees(summary.phi_rad) << std::endl;
                                summary.max_direction = 90 - get_degrees(summary.phi_rad);
                                if(summary.max_direction < 0)
                                    summary.max_direction = 180 + summary.max_direction;

                                summary.min_direction = summary.max_direction - 90;
                                if(summary.min_direction < 0)
                                    summary.min_direction = 180 + summary.min_direction;

                                                            std::cout << "Computing: MAX direction "<< summary.max_direction << " degree from North; MIN direction "<< summary.min_direction << " degree from North."<< std::endl;
                                std::cout << FMAG("NOTA BENE: Questa soluzione sottostima il range massimo!!") << std::endl;
                                std::cout << std::endl;

                                auto fig = biv_plot_leg(h_plot, "Rose Diagram of Ranges", "hx", "hy", false, "Dir degree");

                                matplot::hold(matplot::on);
                                ellipse_plot(fig, summary, setEps.getValue());

                                matplot::save(app_folder + "/" + data.getName() + std::to_string(categ.at(cat)) + "_RangesDiagram", "jpeg");
                                matplot::cla();

                                metavario.setSummary(summary);
                            }

                            Variogram fvm;
                            if(varioDimension.getValue().compare("3Dz") !=0)
                                fvm.set_range(metavario.getSummary().min_semiaxis, metavario.getSummary().max_semiaxis);
                            else
                                fvm.set_range(vv.at(0).range);
                            fvm.set_azimuth(metavario.getSummary().max_direction);
                            fvm.nugget = metavario.getFitExpVariog(0).nugget;
                            fvm.sill = metavario.getFitExpVariog(0).sill - fvm.nugget; //che deve essere ovviamente = 1

                            MUSE::variogram_methods fitvariov;
                            fitvariov.setNugget(fvm.nugget);
                            fitvariov.setSill(fvm.sill);
                            fitvariov.range_max = fvm.get_maxrange();
                            fitvariov.range_min = fvm.get_minrange();
                            fitvariov.setRangeZ(fvm.get_zrange());
                            fitvariov.setType(metavario.getFitExpVariog(0).type);

                            //vario_frame.setVario(fitvariov);

                            break;
                        }
                        }

                        break;
                    }
                    }

                    vario_filename.clear();
                    vario_filename = app_folder + "/" + data.getName() + std::to_string(categ.at(cat));

                    if(subDataset.isSet())
                        vario_filename += "_" + subDataset.getValue();
                    metavario.write(vario_filename + ".json");
                }
            }




            else
            {
                // VARIO - VARIABILE CONTINUA
                if(!setNormalScore.isSet() || (setNormalScore.isSet() && setNormalScore.getValue().compare("YES") != 0))
                {
                    std::cout << FRED("=== ERROR: Normal Score Transformation is not set!") << std::endl;
                    std::cout << FRED("=== Set --normalscore YES to enable normal score transformation on values.") << std::endl;
                    exit(1);
                }

//                if(setNormalScore.getValue().compare("YES") == 0)
//                {
                    normalscore normal_values;
                    std::cout << "=== Set normal score trasformation from geostatslib (MUSE-submodule) ..." << std::endl;
                    if(setDeclustering2d.isSet())
                    {
                        std::cout << "=== Set 2D declustering ..." << std::endl;
                        std::cout << "=== Cell size for declustering is set on " << setCellSize.getValue() << std::endl;
                        std::cout << "=== Number of step for declustering (grid translation) is set on " << setNStep.getValue() << std::endl;
                        std::vector<double> decl_weight = decluster2d(corr_x, corr_y, setCellSize.getValue(), setNStep.getValue());
                        std::cout << "=== 2D declustering ... COMPLETED." << std::endl;

                        normal_values = normal_score(conv_values, decl_weight); //RICORDA!! c'è un terzo parametro da considerare nella normal score, settato di default su false
                        export1d_xyz(app_folder + "/" + Variable.getValue() + "_weight.dat", decl_weight);
                    }
                    else
                    {
                        #ifdef DEBUG
                        std::cout << BOLD(FMAG("#################################")) << std::endl;
                        std::cout << BOLD(FMAG("UTILIZZO NORMAL SCORE (GEOSTATLIB)")) << std::endl;
                        std::cout << FMAG("Implementazione analoga a GSLIB, eccetto per la funzione di NORMALCDFINVERSE (espressa con altri coefficienti).") << std::endl;
                        std::cout << FMAG("L'argomento della NORMALCDFINVERSE viene calcolato secondo l'equazione: //https://en.wikipedia.org/wiki/Normal_probability_plot") << std::endl;
                        std::cout << FMAG("Salvataggio normalscore in file:") << std::endl;
                        std::cout << FMAG("col1) values = score assegnati alla variabile (NON ordinata)") << std::endl;
                        std::cout << FMAG("col2) x = variabile ordinata") << std::endl;
                        std::cout << FMAG("col3) nsco = score assegnati alla variabile ordinata") << std::endl;
                        std::cout << FMAG("OSSERVAZIONE: nel caso di dati duplicati, il punteggio assegnato a variabili uguali dovrebbe essere uguale, anche per garantire il passaggio inverso effettuato con la back normal score.") << std::endl;
                        std::cout << FMAG("In tale implementazione però si considera che ogni elemento duplicato sia un evento indipendente dall'altro (come se fosse un valore diverso in pratica)") << std::endl;
                        std::cout << FMAG("Questa assunzione giustifica l'assegnazione di uno score diverso a variabili con lo stesso valore.") << std::endl;
                        std::cout << FMAG("Il passaggio inverso (back normal score) viene comunque garantito: la back ha all'interno una funzione di interpolazione (equazione lineare - retta), che quando incontra i due duplicati, restituisce il valore unico della variabile.") << std::endl;
                        std::cout << FMAG("La correlazione quindi viene rispettata.") << std::endl;
                        std::cout << BOLD(FMAG("#################################")) << std::endl;
                        std::cout << std::endl;
                        #endif

                        normal_values = normal_score(conv_values);
                    }
                    export1d_xyz(app_folder + "/" + Variable.getValue() + "_convval.dat", conv_values);
                    export3d_xyz(app_folder + "/" + Variable.getValue() + "_nscore.dat", normal_values.values, normal_values.x, normal_values.nsco);

                    stats.mean_zscore = mean(normal_values.values);
                    stats.var_zscore = variance(normal_values.values);
                    vario_frame.setStatisticsInfo(stats);

                    metavario.setProcessing(infovar);

                    VarioType option;
                    convert_from_str(varioType.getValue(), option);

                    VarioDirection dir;
                    convert_from_str(varioDirection.getValue(), dir);

                    std::cout << std::endl;
                    std::cout << "=== Starting variogram computation ..." << std::endl;
                    switch (option)
                    {
                    case VarioType::EXPERIMENTAL: //solo variogramma sperimentale: può essere omnidirezionale/direzionale
                    {
                        std::cout << "=== Experimental points computation ..." << std::endl;
                        switch (dir)
                        {
                        case VarioDirection::OMNI:
                        {
                            std::cout << "=== Omnidirectional analysis ..." << std::endl;
                            std::cout << "=== Lag type is set on: " << setLagType.getValue() << std::endl;
                            
                            // Compute the experimenatal variogram - OMNIDIRECTIONAL
                            exp_variog exp_var;

                            std::cout << "=== Variogram dimension is set on: " << varioDimension.getValue() << std::endl;
                            if(varioDimension.getValue().compare("3D") == 0)
                            {
                                exp_var = exp_variogram(normal_values.values, corr_x, corr_y, corr_z, setLagType.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                std::cout << "=== Computing 3D experimental variogram ... COMPLETED." << std::endl;
                            }
                            else if(varioDimension.getValue().compare("2D") == 0)
                            {
                                std::cout << FRED("ERROR: NO 2D experimental variogram implementation... ") << std::endl;
                                exit(1);
                            }
                            else if(varioDimension.getValue().compare("1D") == 0)
                            {
                                std::cout << "Computing 1D experimental variogram ... " << std::endl;
                                std::cout << "ERROR: NO 1D experimental variogram implementation... " << std::endl;
                                exit(1);
                            }


                            VarioMeta::InfoVariogram info_vario;
                            info_vario.dimension = varioDimension.getValue();
                            info_vario.direction = varioDirection.getValue();


                            if(setVarioCleanPoints.isSet())
                                info_vario.clean_is_set = true;
                            info_vario.n_min_points_for_clean = setVarioCleanPoints.getValue();
                            metavario.setInfoVariogram(info_vario);


                            if(setVarioCleanPoints.isSet())
                            {
                                std::cout << "=== Clean experimental variogram ..." << std::endl;
                                std::cout << "=== Min pair of points for clean: " << setVarioCleanPoints.getValue() << std::endl;
                                exp_var = clean_exp_variogram (exp_var, setVarioCleanPoints.getValue());
                                std::cout << "=== Clean experimental variogram ... COMPLETED." << std::endl;
                            }


                            // Plotting experimental variogram
                            std::cout << "=== Plotting experimental variogram ..." << std::endl;
                            PlotStruct gamma_h;
                            for(size_t i=0; i < exp_var.gamma.size(); i++)
                            {
                                gamma_h.x.push_back(exp_var.h.at(i));          //distanze
                                gamma_h.y.push_back(exp_var.gamma.at(i));      //gamma (variogramma)
                            }
                            biv_plot(gamma_h, "Experimental Variogram", "Lag distance (m)", "Semivariogram");

                            matplot::save(app_folder + "/" + data.getName(), "jpeg");
                            std::cout << std::endl;

                            break;
                        }

                        case VarioDirection::DIR:
                        {
                            std::cout << "=== Directional analysis ..." << std::endl;
                            std::cout << "=== Lag type is set on: " << setLagType.getValue() << std::endl;

                            VarioMeta::InfoVariogram info_vario;
                            info_vario.dimension = varioDimension.getValue();
                            info_vario.direction = varioDirection.getValue();

                            std::vector<exp_variog> dir_ex_var;
                            std::vector<double> directions(1,0);
                            if(!loadDirs.isSet())
                            {
                                std::cout << "=== Automatic directions are computed ..." << std::endl;
                                info_vario.n_directions = 180/setDegree.getValue();
                                info_vario.degree_step = setDegree.getValue();
                                info_vario.degree_tolerance = setTol.getValue();

                                for(int i=1; i< info_vario.n_directions; i++)
                                    directions.push_back(directions.at(i-1) + setDegree.getValue());
                            }
                            else
                            {
                                std::cout << "=== Discrete directions are set ..." << std::endl;
                                info_vario.set_directions = loadDirs.getValue();
                                std::vector<std::string> direc = split_string(loadDirs.getValue(), ',');

                                directions.at(0) = std::stod(direc.at(0));
                                for(size_t i=1; i< direc.size(); i++)
                                    directions.push_back(std::stod(direc.at(i)));

                                info_vario.n_directions = directions.size();
                            }

                            // 1) Compute directional experimental variograms

                            if(varioDimension.getValue().compare("3D") == 0)
                            {
                                //std::cout << "Computing 3D directional experimental variogram ... " << std::endl;
                                std::cout << FRED("ERROR: NO 3D directional experimental variogram implementation... ") << std::endl;
                                exit(1);
                            }
                            else if(varioDimension.getValue().compare("3Dxy") == 0)
                            {
                                dir_ex_var = dir3DH_exp_variogram (normal_values.values, corr_x, corr_y, corr_z, setLagType.getValue(), directions, setTol.getValue(), setVertTol.getValue(), setBandwidth.getValue(), setVertBandwidth.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                std::cout << "Computing 3D HORIZONTAL directional experimental variogram... COMPLETED." << std::endl;
                            }
                            else if(varioDimension.getValue().compare("3Dz") == 0)
                            {
                                directions.clear();
                                directions.push_back(0.0);
                                std::cout << "=== Calculation direction is fixed on 0 degree from Z axis." << std::endl;

                                dir_ex_var = dir3DV_exp_variogram (normal_values.values, corr_x, corr_y, corr_z, setLagType.getValue(), setSpacingSamples.getValue(), directions, setVertTol.getValue(), setBandwidth.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                std::cout << "Computing 3D VERTICAL directional experimental variogram... COMPLETED." << std::endl;
                            }
                            else if(varioDimension.getValue().compare("2D") == 0)
                            {
                                dir_ex_var = dir_exp_variogram (normal_values.values, corr_x, corr_y, setLagType.getValue(), directions, setTol.getValue(), setBandwidth.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                std::cout << "Computing 2D directional experimental variogram... COMPLETED." << std::endl;
                            }

                            else if(varioDimension.getValue().compare("1D") == 0)
                            {
                                //std::cout << "Computing 1D directional experimental variogram ... " << std::endl;
                                std::cout << "ERROR: NO 1D directional experimental variogram implementation... " << std::endl;
                                exit(1);
                            }

                            if(setVarioCleanPoints.isSet())
                                info_vario.clean_is_set = true;
                            info_vario.n_min_points_for_clean = setVarioCleanPoints.getValue();
                            metavario.setInfoVariogram(info_vario);


                            // 2) Clean directional variograms
                            int nzeros = 0;
                            if(setVarioCleanPoints.isSet())
                            {
                                std::cout << "=== Clean directional experimental variogram ..." << std::endl;
                                std::cout << "=== Min pair of points for clean: " << setVarioCleanPoints.getValue() << std::endl;
                                for(uint i=0; i< dir_ex_var.size(); i++)
                                    dir_ex_var.at(i) = clean_exp_variogram (dir_ex_var.at(i), setVarioCleanPoints.getValue(), nzeros);
                                std::cout << "=== Clean directional experimental variogram ... COMPLETED." << std::endl;
                            }

                            std::vector<MUSE::exp_variog_methods> variov;
                            for(size_t i=0; i<dir_ex_var.size(); i++)
                            {
                                MUSE::exp_variog_methods variovv;

                                variovv.setDirection(i*setDegree.getValue());
                                variovv.setExpVariog_N(dir_ex_var.at(i).N);
                                variovv.setExpVariog_h(dir_ex_var.at(i).h);
                                variovv.setExpVariog_gamma(dir_ex_var.at(i).gamma);

                                variov.push_back(variovv);
                            }
                            metavario.setDirExpVariog(variov);


                            // 3) Plot directional variograms
                            std::cout << "=== Plotting experimental variogram ..." << std::endl;
                            int i_dir = 0;
                            for(const exp_variog &v:dir_ex_var)
                            {
                                PlotStruct dir_gamma_h;
                                for(size_t j=0; j < v.gamma.size(); j++)
                                {
                                    dir_gamma_h.x.push_back(v.h.at(j));          //distanze
                                    dir_gamma_h.y.push_back(v.gamma.at(j));      //gamma (variogramma)
                                }
                                biv_plot(dir_gamma_h, "Experimental Variogram", "Lag distance (m)", "Semivariogram");
                                matplot::save(app_folder + "/" + data.getName() + "_dir" + std::to_string(i_dir), "jpeg");
                                matplot::cla();

                                i_dir++;
                            }

                            break;
                        }
                        }

                        break;
                    } //close case EXPERIMENTAL


                    case VarioType::MODEL:
                    {
                        std::cout << "=== Experimental points fitting ..." << std::endl;
                        std::string plot_title = "Experimental Variogram Fitting";

                        //Set MSE criterion for fitting
                        weightsType w_type = weightsType::CRESSIE_WEIGHTED_MODIFIED;
                        if(setMSEcriterion.isSet())
                            w_type = stringToWeightsType(setMSEcriterion.getValue());
                        std::cout << "=== MSE criterion for fitting is set on: " << setMSEcriterion.getValue() << std::endl;

                        //se ho già lo sperimentale, lo leggo da json?!
                        switch (dir)
                        {
                        case VarioDirection::OMNI:
                        {
                            exp_variog exp_var;

                            if(loadExpVario.isSet())
                            {
                                //leggi un file .dat e riempi la struttura dati exp_var;
                                std::cout << "=== Load experimental variogram from json file: " << loadExpVario.getValue() << std::endl;
                                load_exp_variogram(loadExpVario.getValue(), exp_var);
                            }
                            else
                            {
                                //Compute experimental variogram
                                std::cout << "=== Computing experimental variogram ..." << std::endl;
                                std::cout << "=== Variogram dimension is set on: " << varioDimension.getValue() << std::endl;
                                if(varioDimension.getValue().compare("3D") == 0)
                                {
                                    exp_var = exp_variogram(normal_values.values, corr_x, corr_y, corr_z, setLagType.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                    std::cout << "=== Computing 3D experimental variogram ... COMPLETED." << std::endl;
                                }
                                else if(varioDimension.getValue().compare("2D") == 0)
                                {
                                    //std::cout << "Computing 2D experimental variogram ... COMPLETED." << std::endl;
                                    std::cout << FRED("ERROR: NO 2D experimental variogram implementation... ") << std::endl;
                                    exit(1);
                                }
                                else if(varioDimension.getValue().compare("1Dz") == 0)
                                {
                                    exp_var = exp_variogram(normal_values.values, corr_z, setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());

                                    plot_title.clear();
                                    plot_title = "Vertical Experimental Variogram Fitting";
                                    std::cout << "Computing 1Dz experimental variogram ... COMPLETED." << std::endl;
                                }

                                else if(varioDimension.getValue().compare("1D") == 0)
                                {
                                    //std::cout << "Computing 1D experimental variogram ... " << std::endl;
                                    std::cout << FRED("ERROR: NO 1D experimental variogram implementation... ") << std::endl;
                                    exit(1);
                                }
                            }


                            VarioMeta::InfoVariogram info_vario;
                            info_vario.dimension = varioDimension.getValue();
                            info_vario.direction = varioDirection.getValue();

                            if(setVarioCleanPoints.isSet())
                                info_vario.clean_is_set = true;
                            info_vario.n_min_points_for_clean = setVarioCleanPoints.getValue();
                            metavario.setInfoVariogram(info_vario);


                            if(setVarioCleanPoints.isSet())
                            {
                                std::cout << "Clean experimental variogram ..." << std::endl;
                                std::cout << "Min pair of points for clean: " << setVarioCleanPoints.getValue() << std::endl;
                                exp_var = clean_exp_variogram (exp_var, setVarioCleanPoints.getValue());
                                std::cout << "Clean experimental variogram ... COMPLETED." << std::endl;
                            }


                            std::vector<MUSE::exp_variog_methods> variov;

                            MUSE::exp_variog_methods variovv;

                            variovv.setDirection(0.0);
                            variovv.setExpVariog_N(exp_var.N);
                            variovv.setExpVariog_h(exp_var.h);
                            variovv.setExpVariog_gamma(exp_var.gamma);

                            variov.push_back(variovv);

                            metavario.setDirExpVariog(variov);

                            std::cout << std::endl;

                            variogram fitted_exp_var;


                            if (setModel.isSet() && setNugget.isSet())
                            {
                                std::cout << "=== Fit variogram with fixed model type: " << setModel.getValue() << " and fixed nugget: " << setNugget.getValue() << std::endl;
                                variogram_type model_type;
                                convert_from_str(setModel.getValue(), model_type);

                                fitted_exp_var = fit_variogram (exp_var, setRangeStep.getValue(), model_type, setNugget.getValue(), true, w_type);
                            }
                            else if(setNugget.isSet())
                            {
                                std::cout << "=== Fit variogram with fixed nugget: " << setNugget.getValue() << std::endl;
                                fitted_exp_var = fit_variogram_1par (exp_var, setRangeStep.getValue(), setNugget.getValue(), w_type);
                            }
                            else if(setModel.isSet())
                            {
                                std::cout << "=== Fit variogram with fixed model type: " << setModel.getValue() << std::endl;
                                variogram_type model_type;
                                convert_from_str(setModel.getValue(), model_type);

                                fitted_exp_var = fit_variogram (exp_var, setRangeStep.getValue(), setNuggetStep.getValue(), model_type, w_type);
                            }
                            else if (setSill.isSet())
                            {
                                //std::cout << "=== Fit variogram with fixed sill: " << setSill.getValue() << std::endl;
                                std::cout << "=== MUSE-vario operates on continuos normal-score values. Sill is assumed as 1.0." << std::endl;
                                std::cout << FRED("COMMAND NOT ENABLED!!") << std::endl;
                                exit(1);
                            }
                            else
                            {
                                std::cout << "Automatic fitting of experimental variogram ... " << std::endl;
                                fitted_exp_var = fit_variogram(exp_var, setRangeStep.getValue(), setNuggetStep.getValue(), w_type);
                            }




                            std::vector<MUSE::variogram_methods> fitvario;

                            MUSE::variogram_methods fitvariov;

                            fitvariov.setDirection(0.0);
                            fitvariov.setNugget(fitted_exp_var.nugget);
                            fitvariov.setSill(fitted_exp_var.sill);
                            fitvariov.set_range(fitted_exp_var.range);
                            fitvariov.setType(fitted_exp_var.type);

                            fitvario.push_back(fitvariov);

                            metavario.setFitExpVariog(fitvario);
                            vario_frame.setVario(fitvariov);



                            // Plotting experimental variogram + model
                            PlotStruct gamma_h;
                            for(size_t i=0; i < exp_var.gamma.size(); i++)
                            {
                                gamma_h.x.push_back(exp_var.h.at(i));          //distanze
                                gamma_h.y.push_back(exp_var.gamma.at(i));      //gamma (variogramma)
                            }
                            variogram_plot(gamma_h, fitted_exp_var, plot_title, "Lag distance", "Semivariogram", setNxvis.getValue());


                            if(subDataset.isSet())
                                matplot::save(app_folder + "/" + data.getName() + "_" + subDataset.getValue(), "jpeg");
                            else
                                matplot::save(app_folder + "/" + data.getName(), "jpeg");

                            matplot::cla();
                            std::cout << std::endl;

                            //FINE PLOT


                        break;
                        } //close case OMNI

                        case VarioDirection::DIR:
                        {
                            VarioMeta::InfoVariogram info_vario;
                            info_vario.dimension = varioDimension.getValue();
                            info_vario.direction = varioDirection.getValue();

                            // Compute the experimenatal variogram - DIRECTIONAL
                            std::vector<exp_variog> dir_ex_var;
                            std::vector<double> directions(1,0);
                            if(!loadDirs.isSet())
                            {
                                info_vario.n_directions = 180/setDegree.getValue();
                                info_vario.degree_step = setDegree.getValue();
                                info_vario.degree_tolerance = setTol.getValue();

                                for(int i=1; i< info_vario.n_directions; i++)
                                    directions.push_back(directions.at(i-1) + setDegree.getValue());
                            }
                            else
                            {
                                info_vario.set_directions = loadDirs.getValue();
                                std::vector<std::string> direc = split_string(loadDirs.getValue(), ',');

                                directions.at(0) = std::stod(direc.at(0));
                                for(size_t i=1; i< direc.size(); i++)
                                    directions.push_back(std::stod(direc.at(i)));

                                info_vario.n_directions = directions.size();
                            }


                            if(varioDimension.getValue().compare("3D") == 0)
                            {
                                //std::cout << "Computing 3D directional experimental variogram ... " << std::endl;
                                std::cout << FRED("ERROR: NO 3D directional experimental variogram implementation ... ") << std::endl;
                                exit(1);
                            }
                            else if(varioDimension.getValue().compare("3Dxy") == 0)
                            {
                                dir_ex_var = dir3DH_exp_variogram (normal_values.values, corr_x, corr_y, corr_z, setLagType.getValue(), directions, setTol.getValue(), setVertTol.getValue(), setBandwidth.getValue(), setVertBandwidth.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                std::cout << "Computing 3D HORIZONTAL directional experimental variogram ... COMPLETED." << std::endl;

                            }
                            else if(varioDimension.getValue().compare("3Dz") == 0)
                            {
                                directions.clear();
                                directions.push_back(0.0);
                                std::cout << "### Calculation direction is fixed on 0 degree from Z axis." << std::endl;

                                dir_ex_var = dir3DV_exp_variogram (normal_values.values, corr_x, corr_y, corr_z, setLagType.getValue(), setSpacingSamples.getValue(), directions, setVertTol.getValue(), setBandwidth.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                std::cout << "Computing 3D VERTICAL directional experimental variogram... COMPLETED." << std::endl;
                            }
                            else if(varioDimension.getValue().compare("2D") == 0)
                            {
                                dir_ex_var = dir_exp_variogram (normal_values.values, corr_x, corr_y, setLagType.getValue(), directions, setTol.getValue(), setBandwidth.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());

                                //dir_ex_var = dir_exp_variogram_lagvar (normal_values.values, corr_x, corr_y, directions, setTol.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());

                                //dir_ex_var = dir_exp_variogram_lagvar(normal_values.values, corr_x, corr_y, setDegree.getValue(), setTol.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                //dir_ex_var = dir_exp_variogram (normal_values.values, corr_x, corr_y, setDegree.getValue(), setTol.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());

                                std::cout << "Computing 2D directional experimental variogram... COMPLETED." << std::endl;
                            }
                            else if(varioDimension.getValue().compare("1D") == 0)
                            {
                                //std::cout << "Computing 1D directional experimental variogram ... " << std::endl;
                                std::cout << FRED("ERROR: NO 1D directional experimental variogram implementation... ") << std::endl;
                                exit(1);
                            }

                            std::cout << std::endl;


                            if(setVarioCleanPoints.isSet())
                            {
                                info_vario.clean_is_set = true;
                                std::cout << "################################################" << std::endl;
                                std::cout << "Clean directional experimental variogram is set!" << std::endl;
                            }
                            info_vario.n_min_points_for_clean = setVarioCleanPoints.getValue();


                            // 2) Clean directional variograms
                            std::vector<int> zeros;
                            if(setVarioCleanPoints.isSet())
                            {
                                for(uint i=0; i< dir_ex_var.size(); i++)
                                {
                                    int nzeros = 0;
                                    std::cout << "### Clean directional experimental variogram - DIR (degree): " << directions.at(i) << std::endl;
                                    dir_ex_var.at(i) = clean_exp_variogram(dir_ex_var.at(i), setVarioCleanPoints.getValue(), nzeros);

                                    zeros.push_back(nzeros); //prima nzeros-1
                                }
                                std::cout << FGRN("Clean directional experimental variogram ... COMPLETED.") << std::endl;
                                std::cout << std::endl;
                            }
                            else
                            {
                                std::cout << FYEL("WARNING: Clean directional experimental variogram is NOT set. Vector of zeros is not computed.") << std::endl;
                                zeros.resize(dir_ex_var.size(), 0);
                            }


                            std::vector<MUSE::exp_variog_methods> variov;
                            for(size_t i=0; i<dir_ex_var.size(); i++)
                            {
                                MUSE::exp_variog_methods variovv;

                                //variovv.setDirection(i*setDegree.getValue());
                                variovv.setDirection(directions.at(i));

                                variovv.setExpVariog_N(dir_ex_var.at(i).N);
                                variovv.setExpVariog_h(dir_ex_var.at(i).h);
                                variovv.setExpVariog_gamma(dir_ex_var.at(i).gamma);

                                variov.push_back(variovv);

                            }
                            metavario.setDirExpVariog(variov);



                            std::vector<double> weight;
                            weight.resize(zeros.size(), 1);
                            if(!setVarioCleanPoints.isSet() && setWeight.isSet())
                            {
                                std::cerr << "ERROR: Penalty function is associated with number of zeros detected by variogram cleaning funtion!" << std::endl;
                                std::cerr << "Set --vclean <npoints> to enable --weight command." << std::endl;
                                exit(1);
                            }

                            if(setVarioCleanPoints.isSet() && setWeight.isSet())
                            {
                                weight.clear();
                                if(zeros.size() == 0)
                                {
                                    std::cout << FRED("ERROR: Vector of zeros is empty!") << std::endl;
                                    exit(1);
                                }

                                std::cout << std::endl;
                                std::cout << FYEL("WARNING. Penalty function on: 1-(n_zeros(dir))*0.1") << std::endl;
                                for(size_t k=0; k<zeros.size(); k++)
                                {
                                    double w = 1.0-(zeros.at(k))*0.1; //penalty function

                                    std::cout << "dir = " << directions.at(k) << "; n_zeros = " << zeros.at(k) << "; weight = " << w << std::endl;
                                    weight.push_back(w);
                                }
                                std::cout << FGRN("Penalty function (based on number of zeros in exmperimental variogram computation for each direction)... COMPLETED.") << std::endl;
                                std::cout << std::endl;

                                info_vario.penalty_function = true;
                            }
                            metavario.setInfoVariogram(info_vario);


                            vector<variogram> vv;
                            try
                            {
                                if (setModel.isSet() && setNugget.isSet())
                                {
                                    std::cout << "Fit variogram with fixed model type: " << setModel.getValue() << " and fixed nugget: " << setNugget.getValue() << std::endl;
                                    std::cout << FRED("Weight on nugget is not active!") << std::endl;
                                    std::cout << FMAG("Il peso non è attivo poichè non faccio la media dei nugget sulle direzioni (ovvero dove applico i pesi), ma fisso il nugget da cmd") << std::endl;
                                    variogram_type model_type;
                                    convert_from_str(setModel.getValue(), model_type);

                                    vv = fit_dir_variogram (dir_ex_var, directions, setRangeStep.getValue(), model_type, setNugget.getValue(), true, w_type);
                                }
                                else if(setNugget.isSet())
                                {
                                    std::cout << "Fit variogram with fixed nugget: " << setNugget.getValue() << std::endl;
                                    std::cout << FRED("Weight on nugget is not active!") << std::endl;
                                    std::cout << FMAG("Il peso non è attivo poichè non faccio la media dei nugget sulle direzioni (ovvero dove applico i pesi), ma fisso il nugget da cmd") << std::endl;
                                    vv = fit_dir_variogram (dir_ex_var, directions, setTol.getValue(), setRangeStep.getValue(), setNugget.getValue(), true, w_type);
                                }
                                else if(setModel.isSet())
                                {
                                    std::cout << "Fitting of directional variogram is set using fixed model type: " << setModel.getValue() << std::endl;
                                    variogram_type model_type;
                                    convert_from_str(setModel.getValue(), model_type);

                                    vv = fit_dir_variogram (dir_ex_var, directions, setTol.getValue(), setRangeStep.getValue(), setNuggetStep.getValue(), model_type, weight, true, w_type);
                                }
                                else if (setSill.isSet())
                                {
                                    std::cout << "Fit variogram with fixed sill: " << setSill.getValue() << std::endl;
                                    std::cout << FRED("COMMAND NOT ACTIVE!!") << std::endl;
                                    exit(1);
                                }
    //                            else if (varioDimension.getValue().compare("3Dz") == 0)
    //                            {
    //                                variogram v = fit_variogram(dir_ex_var.at(0), setRangeStep.getValue(), setNuggetStep.getValue());
    //                                vv.push_back(v);
    //                            }
                                else
                                {
                                    std::cout << "Automatic fitting of directional variograms is set ..." << std::endl;
                                    vv = fit_dir_variogram (dir_ex_var, directions, setTol.getValue(), setRangeStep.getValue(), setNuggetStep.getValue(), weight, true, w_type);
                                }
                            }
                            catch (exception e)
                            {
                                std::cerr << "ERROR Directional variogram " << e.what() << std::endl;
                            }



                            std::vector<MUSE::variogram_methods> fitvario;
                            for(size_t i=0; i<vv.size(); i++)
                            {
                                variogram v = vv.at(i);

                                MUSE::variogram_methods fitvariov;

                                //fitvariov.setDirection(i*setDegree.getValue());
                                fitvariov.setDirection(directions.at(i));

                                fitvariov.setNugget(v.nugget);
                                fitvariov.setSill(v.sill);

                                fitvariov.set_range(v.range);
                                fitvariov.setRangeZ(v.range);

                                fitvariov.setType(v.type);

                                fitvario.push_back(fitvariov);
                            }
                            metavario.setFitExpVariog(fitvario);


                            // 3) Plot directional variograms
                            int i_dir = 0;
                            for(const exp_variog &v:dir_ex_var)
                            {
                                PlotStruct dir_gamma_h;
                                for(size_t j=0; j < v.gamma.size(); j++)
                                {
                                    dir_gamma_h.x.push_back(v.h.at(j));          //distanze
                                    dir_gamma_h.y.push_back(v.gamma.at(j));      //gamma (variogramma)
                                }

                                variogram_plot(dir_gamma_h, vv.at(i_dir), "Fitted Directional Variogram Model", "Lag distance", "Semivariogram", setNxvis.getValue());

                                matplot::save(app_folder + "/" + data.getName() + "_dir" + std::to_string(i_dir), "jpeg");
                                matplot::cla();

                                i_dir++;
                            }

                            //PLOT OF RANGES AND ELLIPSE FITTING (ON PLANE X-Y)
                            if(varioDimension.getValue().compare("3Dz") !=0)
                            {
                                PlotStruct h_plot;
                                for(size_t i=0; i<vv.size(); i++)
                                {
                                    variogram v = vv.at(i);

                                    h_plot.x.push_back(get_rangex(v.range, directions.at(i)));
                                    h_plot.y.push_back(get_rangey(v.range, directions.at(i)));

                                    h_plot.x.push_back(get_rangex(v.range, 180 + directions.at(i)));
                                    h_plot.y.push_back(get_rangey(v.range, 180 + directions.at(i)));
                                }


                                EllipseParameter summary;
                                fit_anisotropy_ellipse(h_plot.x, h_plot.y, summary);

                                std::cout << FMAG("phi in gradi = ") << get_degrees(summary.phi_rad) << std::endl;
                                summary.max_direction = 90 - get_degrees(summary.phi_rad);
                                if(summary.max_direction < 0)
                                    summary.max_direction = 180 + summary.max_direction;

                                summary.min_direction = summary.max_direction - 90;
                                if(summary.min_direction < 0)
                                    summary.min_direction = 180 + summary.min_direction;

                                std::cout << "Computing: MAX direction "<< summary.max_direction << " degree from North; MIN direction "<< summary.min_direction << " degree from North."<< std::endl;
                                //std::cout << FMAG("NOTA BENE: Questa soluzione sottostima il range massimo!!") << std::endl;
                                std::cout << std::endl;

                                auto fig = biv_plot_leg(h_plot, "Rose Diagram of Ranges", "hx", "hy", false, "Dir degree");

                                matplot::hold(matplot::on);
                                ellipse_plot(fig, summary, setEps.getValue());

                                matplot::save(app_folder + "/" + data.getName() + "_RangesDiagram", "jpeg");
                                matplot::cla();

                                metavario.setSummary(summary);
                                vario_frame.setSummary(summary);
                            }

                            Variogram fvm;
                            if(varioDimension.getValue().compare("3Dz") !=0)
                                fvm.set_range(metavario.getSummary().min_semiaxis, metavario.getSummary().max_semiaxis);
                            else
                                fvm.set_range(vv.at(0).range);
                            fvm.set_azimuth(metavario.getSummary().max_direction);
                            fvm.nugget = metavario.getFitExpVariog(0).nugget;
                            fvm.sill = metavario.getFitExpVariog(0).sill - fvm.nugget; //che deve essere ovviamente = 1

                            MUSE::variogram_methods fitvariov;
                            fitvariov.setNugget(fvm.nugget);
                            fitvariov.setSill(fvm.sill);
                            fitvariov.range_max = fvm.get_maxrange();
                            fitvariov.range_min = fvm.get_minrange();
                            fitvariov.setRangeZ(fvm.get_zrange());
                            fitvariov.setType(metavario.getFitExpVariog(0).type);

                            vario_frame.setVario(fitvariov);

                            break;
                        } //close case DIR

                        } //close switch dir

                        break;
                    } //close type MODEL

                    } //close switch option

                    if(subDataset.isSet())
                        vario_filename += "_" + subDataset.getValue();
                    metavario.write(vario_filename + ".json");
                // }

                // else
                // {
                //     std::cout << "\033[0;31mWARNING: Set Normal Score Transformation --nscore\033[0m" << std::endl;
                //     exit(1);
                // }
            }

            vario_frame.setFrameName(rel_datadir.string());
            vec_vario_frame.push_back(vario_frame);
        }



        /////////////////////////////
        //////////SUMMARY FOR FRAMES
        ///
        ///
        MUSE::VarioMultiFrame vario_mf;
        vario_mf.setMultiFrame(vec_vario_frame);

        if(subDataset.isSet())
            vario_mf.write(out_folder + "/" + app_name + "/"+ Variable.getValue() + "_" + subDataset.getValue() + "_" + varioDirection.getValue() + varioDimension.getValue() +"_multiframe.json");
        else
            vario_mf.write(out_folder + "/" + app_name + "/"+ Variable.getValue() + "_" + varioDirection.getValue() + varioDimension.getValue() + "_multiframe.json");
    }


    if(setVarioDiagnose.isSet())
    {
        std::cout << "=== Variogram diagnose ..." << std::endl;
        std::cout << "=== TO BE TESTED!" << std::endl;
        if(!loadExpVario.isSet())
        {
            std::cout << FRED("=== ERROR: Set --load <file.txt> to enable variogram diagnose!") << std::endl;
            std::cout << FRED("=== The file must be composed of three columns in this order: N, h, gamma(h)") << std::endl;
            exit(1);
        }

        if(setVariableFile.isSet())
        {
            std::cout << FRED("=== ERROR: Set --file <var.dat> to enable variogram diagnose respect to such a dataset!") << std::endl;
            std::cout << FRED("=== The file must be composed of one column, representing original data values") << std::endl;
            exit(1);
        }

        std::cout << "=== Load experimental variogram from txt file: " << loadExpVario.getValue() << std::endl;
        exp_variog exp_var;
        load_exp_variogram(loadExpVario.getValue(), exp_var);

        std::vector<double> values;
        load1d_xyzfile (setVariableFile.getValue(), values);
        
        diagnose_exp_variogram (exp_var, values);
        std::cout << "=== Variogram diagnose ... COMPLETED." << std::endl;
    }



    } catch (ArgException &e)  // catch exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

}


