#include <iostream>
#include <filesystem>
#include <utility>
#include <limits.h>

#include <tclap/CmdLine.h>
//#include <json.hpp>

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

//#include "muselib/interpolation/ellipse.h"

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



//TO DO: VARIO 1D + VARIO2D

#include "ellipse_fit.h"

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
    std::cout << "########### STARTING MUSE-VARIO ..." << std::endl;
    std::cout << std::endl;

    std::string app_name = "vario"; //app name
    std::string app_data = "data";
    std::string app_manipulate = "manipulate";

    try {
    CmdLine cmd("MUSE = Modelling of Uncertainty as a Support of Environment; Vario tool", ' ', "version 0.0");


    // ---------------------------------------------------------------------------------------------------------
    // MAIN FUNCTIONALITIES:

    // Option 0. Compute varioagram for a variable
    /**
     * @brief Compute variogram
     * @param variogram Flag to compute variogram
     * @note MANDATORY when computing variograms. Requires:
     * - --pdir: Project directory (mandatory)
     * - --var: Variable name (mandatory)
     * OPTIONAL but commonly used:
     * - --vario: Variogram type (EXPERIMENTAL, MODEL)
     * - --dir: Direction type (OMNI, DIR)
     * - --dim: Dimension (3D, 2D, 1Dz, etc.)
     * @example muse_vario -V --pdir /path/to/project --var temperature --vario EXPERIMENTAL --dir OMNI --dim 3D
     */

    SwitchArg varioCompute                  ("V", "variogram", "Compute variogram", cmd, false); //booleano
    /**

     * @brief Project directory

     * @param pdir Path to project directory

     */

    ValueArg<std::string> projectFolder     ("p", "pdir", "Project directory", true, "Directory", "path", cmd);
    //ValueArg<std::string> subFolder         ("", "subdir", "Project subdirectory", false, "Directory", "path", cmd);
    /**
     * @brief Variable
     * @param var Name of variable
     * @note MANDATORY when using -V/--variogram flag
     * The variable must exist in the project data
     */

    ValueArg<std::string> Variable          ("v", "var", "Variable", true, "variable to analyse", "name", cmd);

    // Option: set third coordinate (z), if necessary
    //ValueArg<std::string> setZcoord         ("z", "setZ", "Set Z Coordinate", false, "Unknown", "Name Z coordinate", cmd);

    // Option: use subdataset (related to a specified domain)
    /**

     * @brief Set extracted sub-dataset referring to specified geometry domain

     * @param sub extracted sub-dataset referring to specified geometry domain

     */

    ValueArg<std::string> subDataset        ("", "sub", "Set extracted sub-dataset referring to specified geometry domain", false, "Directory", "path", cmd);

    // Option: set data rotation
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

    // Option: compute normal score transformation on variable
    /**

     * @brief Set normal score transformation

     * @param nscore Number of set normal score transformation

     */

    ValueArg<std::string> setNormalScore    ("", "nscore", "Set normal score transformation", false, "NO", "string", cmd);

    // Option: set 2D declustering on data
    //ValueArg<std::string> setDeclustering2d ("", "decl", "Set 2D declustering", false, "NO", "string", cmd);
    /**
     * @brief Set 2D declustering
     * @param decl Enable set 2d declustering
     * @note When using declustering, requires:
     * - --csize: Cell size for declustering (mandatory)
     * - --nstep: Number of grid translation steps (mandatory)
     * @example --decl --csize 100 --nstep 5
     */

    SwitchArg setDeclustering2d             ("", "decl", "Set 2D declustering", cmd, false); //booleano
    /**
     * @brief Set cell size for 2D declustering
     * @param csize cell size for 2d declustering
     * @note Used together with --decl flag for 2D declustering
     */

    ValueArg<double> setCellSize            ("", "csize", "Set cell size for 2D declustering", false, 0.0, "double", cmd);
    /**
     * @brief Set n steps for 2D declustering (grid translation)
     * @param nstep Number of set n steps for 2d declustering (grid translation)
     * @note Used together with --decl flag for 2D declustering
     */

    ValueArg<int> setNStep                  ("", "nstep", "Set n steps for 2D declustering (grid translation)", false, 0, "int", cmd);


    // Option: variography on stratigraphic coordinates model (ref. manipulate)
    std::vector<std::string> allowedStratigraphicCondition = {"PROPORTIONAL","TRUNCATION","ONLAP","COMBINATION"};
    ValuesConstraint<std::string> allowedValsSC(allowedStratigraphicCondition);
    /**
     * @brief Set stratigraphic condition for coordinate transformation
     * @param sttype stratigraphic condition for coordinate transformation
     * @note When using stratigraphic transformation, requires:
     * - --filestrat: Filename of samples in stratigraphic coordinates (mandatory)
     * Available conditions: PROPORTIONAL, TRUNCATION, ONLAP, COMBINATION
     * @example --sttype PROPORTIONAL --filestrat /path/to/strat_coords.dat
     */

    ValueArg<std::string> stratCondition    ("", "sttype", "Set stratigraphic condition for coordinate transformation", false, "NO", &allowedValsSC, cmd);
    /**
     * @brief Set filename of samples in stratigraphic coordinates
     * @param filestrat Path to set filename of samples in stratigraphic coordinates
     * @note Required when using --sttype flag for stratigraphic transformation
     */

    ValueArg<std::string> filenameStrat     ("f", "filestrat", "Set filename of samples in stratigraphic coordinates", false, "filename", "path", cmd);



    // Option: set types of variograms
    std::vector<std::string> allowedVarioType = {"EXPERIMENTAL","MODEL"};
    ValuesConstraint<std::string> allowedValsVT(allowedVarioType);
    /**

     * @brief type of variogram

     * @param vario Name of type of variogram

     */

    ValueArg<std::string> varioType         ("", "vario", "type of variogram", false, "EXPERIMENTAL", &allowedValsVT, cmd);

    // Option: types of variogram directions
    std::vector<std::string> allowedVarioDir = {"OMNI","DIR"};
    ValuesConstraint<std::string> allowedValsVD(allowedVarioDir);
    /**
     * @brief type of variogram direction
     * @param dir Path to type of variogram direction
     * @note When using DIR (directional), directional parameters become important:
     * - --deg: Degree step (default: 45°)
     * - --degtol: Tolerance (default: 45°)
     * - --zdegtol: Vertical tolerance (default: 22.5°)
     * - --bandw: Bandwidth (optional)
     * - --vertbandw: Vertical bandwidth (optional)
     * @example --dir DIR --deg 30 --degtol 15 --zdegtol 10
     */

    ValueArg<std::string> varioDirection    ("", "dir", "type of variogram direction", false, "OMNI", &allowedValsVD, cmd);

    // Option: types of variogram dimensions
    std::vector<std::string> allowedVarioDim = {"3D","3Dxy","3Dz","2D","1Dz","1D"};
    ValuesConstraint<std::string> allowedValsVDm(allowedVarioDim);
    /**

     * @brief type of variogram dimension

     * @param dim type of variogram dimension

     */

    ValueArg<std::string> varioDimension    ("", "dim", "type of variogram dimension", false, "3D", &allowedValsVDm, cmd);

    // Option: compute variogram with variable/constant lag spacing
    std::vector<std::string> allowedLag = {"VARIABLE","CONSTANT","FIXED"};
    ValuesConstraint<std::string> allowedValsLag(allowedLag);
    /**

     * @brief Set lag spacing type

     * @param lagspac lag spacing type

     */

    ValueArg<std::string> setLagType        ("", "lagspac", "Set lag spacing type", false, "VARIABLE", &allowedValsLag, cmd);
    /**

     * @brief Set spacing samples for vertical variogram

     * @param spac spacing samples for vertical variogram

     */

    ValueArg<double> setSpacingSamples      ("", "spac", "Set spacing samples for vertical variogram", false, 0.0, "meters", cmd);

    // Option: set parameters for directional variogram computation
    /**
     * @brief Set degree step (in degree)
     * @param deg degree step (in degree)
     * @note Used with --dir DIR for directional variogram computation
     * Works together with --degtol and --zdegtol
     */

    ValueArg<double> setDegree              ("", "deg", "Set degree step (in degree)", false, 45.0, "degree", cmd);
    /**
     * @brief Set tolerance (in degree)
     * @param degtol tolerance (in degree)
     * @note Used with --dir DIR for directional variogram computation
     * Works together with --deg and --zdegtol for direction tolerance
     */

    ValueArg<double> setTol                 ("", "degtol", "Set tolerance (in degree)", false, 45.0, "degree", cmd);
    /**
     * @brief Set vertical tolerance (in degree)
     * @param zdegtol vertical tolerance (in degree)
     * @note Used with --dir DIR for directional variogram computation
     * Works together with --deg and --degtol for vertical direction tolerance
     */

    ValueArg<double> setVertTol             ("", "zdegtol", "Set vertical tolerance (in degree)", false, 22.5, "degree", cmd);
    /**

     * @brief Load discrete directions (in degree)

     * @param dirs Path to load discrete directions (in degree)

     */

    ValueArg<std::string> loadDirs          ("", "dirs", "Load discrete directions (in degree)", false, "dir0,dir1", "string", cmd);
    /**
     * @brief Set bandwidth
     * @param bandw bandwidth
     * @note Used with --dir DIR for directional variogram computation
     * Often used together with --vertbandw for 3D directional analysis
     */

    ValueArg<double> setBandwidth           ("", "bandw", "Set bandwidth", false, DBL_MAX, "double", cmd);
    /**
     * @brief Set vertical bandwidth
     * @param vertbandw vertical bandwidth
     * @note Used with --dir DIR for directional variogram computation
     * Often used together with --bandw for 3D directional analysis
     */

    ValueArg<double> setVertBandwidth       ("", "vertbandw", "Set vertical bandwidth", false, DBL_MAX, "double", cmd);



    // Option: types of permissible models
    std::vector<std::string> allowedModel = {"AUTO","SPHERICAL","GAUSSIAN","EXPONENTIAL","LINEAR","DEFAULT"};
    ValuesConstraint<std::string> allowedValsMl(allowedModel);
    /**
     * @brief Set type of model variogram
     * @param type type of model variogram
     * @note When using --vario MODEL, this flag becomes important
     * For model fitting, use together with:
     * - --nugget: Nugget value (optional)
     * - --sill: Sill value (optional, not fully enabled)
     * Available models: AUTO, SPHERICAL, GAUSSIAN, EXPONENTIAL, LINEAR, DEFAULT
     * @example --vario MODEL --type SPHERICAL --nugget 0.1
     */

    ValueArg<std::string> setModel          ("", "type", "Set type of model variogram", false, "AUTO", &allowedValsMl, cmd);

    // Option: set model parameters
    /**
     * @brief Nugget
     * @param nugget Number of nugget
     * @note Used with --vario MODEL and --type for model fitting
     * Often used together with --sill for complete model specification
     */

    ValueArg<double> setNugget              ("", "nugget", "Nugget", false, 0.0, "double", cmd);
    /**
     * @brief Sill (NOT ENABLE)
     * @param sill sill (not enable)
     * @note Used with --vario MODEL and --type for model fitting
     * Often used together with --nugget for complete model specification
     * WARNING: This parameter is not fully enabled in current version
     */

    ValueArg<double> setSill                ("", "sill", "Sill (NOT ENABLE)", false, 0.0, "double", cmd);
    //ValueArg<double> setRange               ("", "range", "Range (NOT ENABLE)", false, 0.0, "double", cmd);


    // Option: set model parameters - INDICATOR VARIOGRAMS
    MultiArg<std::string> setIndModel       ("", "itype", "Set type of model variogram for indicators. The category is specified by its value (it must be an integer).", false, "type!valcat", cmd);
    MultiArg<std::string> setIndNugget      ("", "inugget", "Set nugget for indicators. The category is specified by its value (it must be an integer).", false, "0.0!valcat", cmd);
    MultiArg<std::string> setIndMaxDistance ("", "imaxdist", "Set max distance for computing variogram for indicators (NOT ENABLE)", false, "-DBL_MAX!namecat", cmd);
    //MultiArg<std::string> setIndSill        ("", "isill", "Set sull for indicators", false, "0.0|ncat", cmd);




    allowedStratigraphicCondition.clear();
    allowedVarioType.clear();
    allowedVarioDir.clear();
    allowedVarioDim.clear();
    allowedLag.clear();
    allowedModel.clear();


    // ---------------------------------------------------------------------------------------------------------
    // ADDITIONAL FUNCTIONALITIES:

    // Option: set parameters
    /**

     * @brief Set number of experimental variogram points

     * @param npoints Number of set number of experimental variogram points

     */

    ValueArg<uint> setPointsVario           ("", "npoints", "Set number of experimental variogram points", false, 15, "int", cmd); //n. di punti per la discretizzazione del variogramma sperimentale = 15 di default
    //ValueArg<uint> setStartPointsVario      ("", "nstartpoints", "Number of discretized experimental variogram in coverage/2 extended zone (TO REMOVE)", false, 10, "int", cmd); //n. di punti per la discretizzazione del variogramma sperimentale = 15 di default
    //ValueArg<double> setPointStep           ("", "pstep", "Set points step (TO REMOVE)", false, 0.8, "double", cmd);

    /**


     * @brief Set cleaning variogram points


     * @param vclean cleaning variogram points


     */


    ValueArg<uint> setVarioCleanPoints      ("", "vclean", "Set cleaning variogram points", false, 0, "int", cmd); //n. di punti minimo (clean variogram) = 10 di default

    /**


     * @brief Set range step


     * @param rangestep range step


     */


    ValueArg<double> setRangeStep           ("", "rangestep", "Set range step", false, 100.0, "double", cmd);
    /**

     * @brief Set nugget step

     * @param nugstep Number of set nugget step

     */

    ValueArg<double> setNuggetStep          ("", "nugstep", "Set nugget step", false, 100.0, "double", cmd);

    /**
     * @brief Set maximum points distance to flexibly manage the variogram coverage (dc). When compute experimental variogram, a maximum value of coverage is set to half of maximum distance between pairs of sampled points, as the reference distance at which the variogram can be considered meaningful (dc=dmax/2).
     * @param maxdist maximum coverage distance for computing experimental variogram (default: -DBL_MAX)
     * @note Optional parameter used for experimental variogram computation.
     * If --maxdist is set on &lt;value&gt;, dc=&lt;value&gt;; otherwise dc=dmax/2, where dmax is computed in bacth mode.
     * @example --maxdist 10
     */
    ValueArg<double> setMaxDistance         ("", "maxdist", "Set maximum distance between points for computing experimental variogram", false, -DBL_MAX, "double", cmd);

    /**
     * @brief Set tolerance factor for lag tolerance to compute experimental variogram.
     * @param tolfac tolerance factor for computing experimental variogram
     * @note Optional parameter. Default: tolfac=2.0.
     * If --tolfac is set on &lt;value&gt;, tol=lag spacing/&lt;value&gt;
     * @example --tolfac 1.5
     */
    ValueArg<double> setToleranceFactor     ("", "tolfac", "Set tolerance factor for computing experimental variogram", false, 2.0, "double", cmd);

    /**


     * @brief Set multiplier factor for computing variable lag spacing for experimental variogram


     * @param fac multiplier factor for computing variable lag spacing for experimental variogram


     */


    ValueArg<double> setFactor              ("", "fac", "Set multiplier factor for computing variable lag spacing for experimental variogram", false, 1.0, "double", cmd);

    /**


     * @brief Set weight on nugget to compute directional variogram


     * @param weight Flag to set weight on nugget to compute directional variogram


     */


    SwitchArg setWeight                     ("", "weight", "Set weight on nugget to compute directional variogram", cmd, false); //booleano

    /**


     * @brief Load experimental variogram


     * @param expvario Name of load experimental variogram


     */


    ValueArg<std::string> loadExpVario      ("", "expvario", "Load experimental variogram", false, "filename", "string", cmd);


    //  Option: plotting tools
    /**

     * @brief Set eps for plot centering

     * @param eps eps for plot centering

     */

    ValueArg<double> setEps                 ("", "eps", "Set eps for plot centering", false, 0.0, "double", cmd);
    /**

     * @brief Set eps_y for plot on y axis

     * @param epsy eps_y for plot on y axis

     */

    ValueArg<double> setEps_y               ("", "epsy", "Set eps_y for plot on y axis", false, 0.05, "double", cmd);


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

            if(!stratCondition.isSet()) //Condizione di default
            {
                std::cout << "\033[0;33mWARNING: No stratigraphic trasformation is set. The coordinate system remains unchanged.\033[0m" << std::endl;

                if(datameta.getInfoData().id_name.compare("Unknown") != 0)
                {
                    readTextValues(l + "/data/" + datameta.getInfoData().id_name + ".dat", id);
                    depsvario.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().id_name + ".json", Project.folder));
                }
                else
                    std::cerr << "ERROR reading ID: " << l + "/data/" + datameta.getInfoData().id_name + ".dat" << " NOT found." << std::endl;

                if(datameta.getInfoData().x_name.compare("Unknown") != 0)
                {
                    readCoordinate(l + "/data/" + datameta.getInfoData().x_name + ".dat", xCoord);
                    depsvario.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().x_name + ".json", Project.folder));
                }
                else
                    std::cerr << "ERROR reading X coordinate: " << l + "/data/" + datameta.getInfoData().x_name + ".dat" << " NOT found." << std::endl;

                if(datameta.getInfoData().y_name.compare("Unknown") != 0)
                {
                    readCoordinate(l + "/data/" + datameta.getInfoData().y_name + ".dat", yCoord);
                    depsvario.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().y_name + ".json", Project.folder));
                }
                else
                    std::cerr << "ERROR reading Y coordinate: " << l + "/data/" + datameta.getInfoData().y_name + ".dat" << " NOT found." << std::endl;

                if(datameta.getInfoData().z_name.compare("Unknown") != 0)
                {
                    readCoordinate(l + "/data/" + datameta.getInfoData().z_name + ".dat", zCoord);
                    depsvario.push_back(filesystem::relative(l + "/metadata/" + datameta.getInfoData().z_name + ".json", Project.folder));
                }
                else
                {
                    zCoord.resize(xCoord.size(), 0.0);
                    //std::cerr << "ERROR reading Z coordinate: " << l + "/data/" + datameta.getInfoData().z_name + ".dat" << " NOT found." << std::endl;
                    //std::cout << "\033[0;33mWARNING: Z coordinate is Unknown. Set -z --name <variable> for setting the variable.\033[0;0m" << std::endl;
                }
                std::cout << std::endl;
            }
            else
            {
                std::cout << "\033[0;33mWARNING: Stratigraphic transformation is set on " << stratCondition.getValue() << ". Variogram is computed in stratigraphic coordinate system.\033[0m" << std::endl;
                std::cout << "Stratigraphic coordinates are located in " << out_folder + "/" + app_manipulate + "/" << std::endl;
                load_xyzfile(man_folder + "/" + filenameStrat.getValue() + ".xyz", xCoord, yCoord, zCoord);
                depsvario.push_back(filesystem::relative(man_folder + "/" + filenameStrat.getValue() + ".json", Project.folder));
            }

            if((xCoord.size() != yCoord.size()) || (xCoord.size() != zCoord.size()) || (yCoord.size() != zCoord.size()))
            {
                std::cerr << "ERROR in loading vector coordinates data!" << std::endl;
                    exit(1);
            }

            VarioMeta::Manipulate processingData;
            processingData.stratigraphic_transf = stratCondition.getValue();
            processingData.filename = filenameStrat.getValue();


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

            std::vector<std::string> corr_id;
            std::vector<double> conv_values, corr_x, corr_y, corr_z; //sampled data

            //NEWWWWWWWWWWWWWWWWWWWWW

            if(!stratCondition.isSet()) //Se non sono in coordinate stratigr
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
                    metavario.setRotation(dataRotation_vario);
                    std::cout << FGRN("Rotation on data ... COMPLETED.") << std::endl;
                }

                //Dopo aver caricato i dati grezzi, posso considerare un sottodataset o la totalità
                if(subDataset.isSet()) //sotto dataset da manipulate
                {
                    //depsvario.push_back("manipulate/" + subDataset.getValue() + ".json");

                    processingData.sub_dataset = "YES";
                    processingData.domain = subDataset.getValue();

                    MUSE::ExtractionMeta extrmeta;
                    extrmeta.read(man_folder + "/" + subDataset.getValue() + ".json");
                    std::cout << "Extraction sub-dataset is set. Reading ... " << man_folder + "/" + subDataset.getValue() + ".json" << std::endl;
                    depsvario.push_back(filesystem::relative(man_folder + "/" + subDataset.getValue() + ".json", Project.folder));


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

                        for(uint i=0; i< xCoord.size(); i++)
                        {
                            //rotazione coordinate all'inidice i
                            cinolib::vec3d sample (xCoord.at(i), yCoord.at(i), zCoord.at(i));
                            cinolib::vec3d axis = set_rotation_axis(dataRotation.rotation_axis);
                            cinolib::vec3d c (dataRotation.rotation_center_x, dataRotation.rotation_center_y, dataRotation.rotation_center_z);
                            sample = point_rotation(sample, axis, dataRotation.rotation_angle, c);

                            xCoord.at(i) = sample.x();
                            yCoord.at(i) = sample.y();
                            zCoord.at(i) = sample.z();
                        }

                        metavario.setRotation(dataRotation);
                        std::cout << FGRN("Rotation on data ... COMPLETED.") << std::endl;
                    }

                    //2) ESTRARRE SOTTODATASET DA INDICI
                    if(extrmeta.getDataExtraction().id_points.size() == 0)
                    {
                        std::cout << FRED("Vector of index is empty.") << std::endl;
                        exit(1);
                    }

                    string_to_double_conversion_vectors(extrmeta.getDataExtraction().id_points, data.text_values, id, xCoord, yCoord, zCoord, conv_values, corr_id, corr_x, corr_y, corr_z);
                    std::cout << FGRN("Extraction sub-dataset ... COMPLETED.") << std::endl;
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
                    //depsvario.push_back("manipulate/" + subDataset.getValue() + ".json");

                    processingData.sub_dataset = "YES";
                    processingData.domain = subDataset.getValue();

                    MUSE::ExtractionMeta extrmeta;
                    extrmeta.read(man_folder + "/" + subDataset.getValue() + ".json");
                    std::cout << "Extraction sub-dataset is set. Reading ... " << man_folder + "/" + subDataset.getValue() + ".json" << std::endl;
                    depsvario.push_back(filesystem::relative(man_folder + "/" + subDataset.getValue() + ".json", Project.folder));

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

                        //NON DEVO RUOTARE I DATI DI NUOVO, MA SOLO COPIARE NEL JSON LE INFORMAZIONI DI ROTAZIONE DA MANIPULATE
                        metavario.setRotation(dataRotation);
                    }

                    //2) ESTRARRE SOTTODATASET DA INDICI
                    if(extrmeta.getDataExtraction().id_points.size() == 0)
                    {
                        std::cout << FRED("Vector of index is empty.") << std::endl;
                        exit(1);
                    }

                    for(uint i:extrmeta.getDataExtraction().id_points)
                    {
                        std::string val_tmp = data.text_values.at(i);
                        if(!val_tmp.empty() && val_tmp.compare("nd")!=0)
                        {
                            if(val_tmp.compare("*")!=0)
                            {
                                double val = std::stod(val_tmp);
                                conv_values.push_back(val);

                                if(id.size() > 0)
                                    corr_id.push_back(id.at(i));
                            }
                        }
                    }

                    corr_x = xCoord;
                    corr_y = yCoord;
                    corr_z = zCoord;

                    std::cout << FGRN("Extraction sub-dataset ... COMPLETED.") << std::endl;
                }
                else
                    string_to_double_conversion_vectors(data.text_values, id, xCoord, yCoord, zCoord, conv_values, corr_id, corr_x, corr_y, corr_z);
            }
            //NEWWWWWWWWWWWWWWWWWWWWW

            std::cout << std::endl;

            metavario.setDependencies(depsvario); //added dependencies

            metavario.setManipulate(processingData);


            int n_conv_samples = conv_values.size(); //numero campioni convertiti da stringa a double
            if(n_conv_samples == 0)
            {
                std::cerr << "\033[0;31mERROR: All values are invalid!\033[0m" << std::endl;
                exit(1);
            }
            else
            {
                std::cout << "Data Statistical Summary ..." << std::endl;
                summary(conv_values);
                // infovar.mean = mean(conv_values);
                // infovar.var = variance(conv_values);

                stats.mean = mean(conv_values);
                stats.var = variance(conv_values);
            }

            std::cout << "\033[0;32mReading MUSE format and data analysis... COMPLETED.\033[0m" << std::endl;
            std::cout << std::endl;


            // if((xCoord.size() != n_conv_samples) || (yCoord.size() != n_conv_samples) || (zCoord.size() != n_conv_samples))
            // {
            //     std::cerr << "ERROR in loading vector data!" << std::endl;
            //     exit(1);
            // }
            xCoord.clear();
            yCoord.clear();
            zCoord.clear();



            if (data.type == varType::CATEGORIC_TEXT)
            {
                std::cout << std::endl;
                std::cout << FGRN("### VARTYPE CHECK: The variable is categoric (textual).") << std::endl;
                std::cout << std::endl;

                //la variabile categorica può essere anche testuale: in quel caso prevedere una trasformazione in indici/categorie numeriche e trattarla come indicatore
                std::vector<std::string> conv_values;

                //PUNTO DA MODIFICARE DATO CHE FACCIO LA CORRISPONDENZA DATI PRIMA!
                for(size_t i = 0; i<n_sample; i++)
                {
                    std::string val_tmp = data.text_values.at(i);
                    if(!val_tmp.empty() && val_tmp.compare("nd")!=0)
                    {
                        conv_values.push_back(val_tmp);

                        if(id.size() > 0)
                            corr_id.push_back(id.at(i));
                        corr_x.push_back(xCoord.at(i));
                        corr_y.push_back(yCoord.at(i));
                        corr_z.push_back(zCoord.at(i));
                    }
                }

                std::vector<std::string> categ_name = categories_extraction(conv_values);
                std::vector<double> categ_index;
                for(size_t i=0; i< categ_name.size(); i++)
                    categ_index.push_back(i+1);


                std::cout << FRED("ERROR: THE IMPLEMENTATION IS NOT COMPLETED!") << std::endl;
                exit(1);

                // ................................................... TO DO
            }





            else if (data.type == varType::CATEGORIC)
            {
                std::cout << std::endl;
                std::cout << FGRN("### VARTYPE CHECK: The variable is categoric.") << std::endl;
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

                                fitted_exp_var = fit_ind_variogram_2par (exp_var, setRangeStep.getValue(), setNugget.getValue(), var_cat, model_type, true);
                            }
                            else if(setIndNugget.isSet() && nug_setcat)
                            {
                                std::cout << "Fit variogram with fixed nugget: " << fix_nug << std::endl;
                                fitted_exp_var = fit_ind_variogram_1par (exp_var, setRangeStep.getValue(), fix_nug, var_cat);
                            }
                            else if(setIndModel.isSet() && mod_setcat)
                            {
                                std::cout << "Fit variogram with fixed model type: " << fix_mod << std::endl;
                                variogram_type model_type;
                                convert_from_str(fix_mod, model_type);

                                fitted_exp_var = fit_ind_variogram_1par (exp_var, setRangeStep.getValue(), setNuggetStep.getValue(), var_cat, model_type);
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
                                fitted_exp_var = fit_ind_variogram(exp_var, setRangeStep.getValue(), setNuggetStep.getValue(), var_cat);
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
                            variogram_plot(gamma_h, fitted_exp_var, "Fitted Experimental Variogram Model", "Lag distance", "Semivariogram", setEps_y.getValue());



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

                                    vv = fit_ind_dir_variogram_2par (dir_ex_var, directions, setRangeStep.getValue(), fix_nug, var_cat, model_type, true);
                                }
                                else if(setIndNugget.isSet() && nug_setcat)
                                {
                                    std::cout << "Fit variogram with fixed nugget: " << fix_nug << std::endl;
                                    std::cout << FRED("Weight on nugget is not active!") << std::endl;
                                    std::cout << FMAG("Il peso non è attivo poichè non faccio la media dei nugget sulle direzioni (ovvero dove applico i pesi), ma fisso il nugget da cmd") << std::endl;
                                    vv = fit_ind_dir_variogram_1par (dir_ex_var, directions, setTol.getValue(), setRangeStep.getValue(), fix_nug, var_cat, true);
                                }
                                else if(setIndModel.isSet() && mod_setcat)
                                {
                                    std::cout << "Fitting of directional variogram is set using fixed model type: " << fix_mod << std::endl;
                                    variogram_type model_type;
                                    convert_from_str(fix_mod, model_type);

                                    vv = fit_ind_dir_variogram_1par (dir_ex_var, directions, setTol.getValue(), setRangeStep.getValue(), setNuggetStep.getValue(), var_cat, model_type, weight, true);
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
                                    vv = fit_ind_dir_variogram (dir_ex_var, directions, setTol.getValue(), setRangeStep.getValue(), setNuggetStep.getValue(), var_cat, weight, true);
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

                                biv_plot_leg(h_plot, "Rose Diagram of Ranges", "hx", "hy", false, "Dir degree");

                                matplot::hold(matplot::on);
                                ellipse_plot(summary, setEps.getValue());

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

                if(setNormalScore.getValue().compare("YES") == 0)
                {
                    normalscore normal_values;

                    //if(setDeclustering2d.getValue().compare("YES") == 0)
                    if(setDeclustering2d.isSet())
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
                    {
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

                        normal_values = normal_score(conv_values);
                        //normal_values = normal_score2(conv_values);
                        //normal_values = normal_score3(conv_values);
                    }

                    export1d_xyz(app_folder + "/" + Variable.getValue() + "_convval.dat", conv_values);
                    export3d_xyz(app_folder + "/" + Variable.getValue() + "_nscore.dat", normal_values.values, normal_values.x, normal_values.nsco);

                    // infovar.mean_zscore = mean(normal_values.values);
                    // infovar.var_zscore = variance(normal_values.values);
                    stats.mean_zscore = mean(normal_values.values);
                    stats.var_zscore = variance(normal_values.values);
                    vario_frame.setStatisticsInfo(stats);

                    metavario.setProcessing(infovar);

                    std::cout << "\033[0;33mWARNING: Normal Score Transformation has been computed on values.\033[0m" << std::endl;
                    std::cout << std::endl;


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
                            //exp_variog exp_var = experimental_variogram_memory_optimization(normal_values.values, corr_x, corr_y, corr_z, setPointsVario.getValue());

                            //std::cout << "VARIO LAG VARIABILE - ORIGINALE" << std::endl;
                            //exp_variog exp_var = experimental_variogram_with_variable_lag(normal_values.values, corr_x, corr_y, corr_z);
                            //exp_variog exp_var = experimental_variogram_with_variable_lag_opt2(normal_values.values, corr_x, corr_y, corr_z, setPointsVario.getValue());
                            //exp_variog exp_var = experimental_variogram_varlag_opt (normal_values.values, corr_x, corr_y, corr_z, setPointsVario.getValue(), setMaxDistance.getValue());
                            //exit(1);

                            // Compute the experimenatal variogram - OMNIDIRECTIONAL
                            exp_variog exp_var;

                            std::cout << "Variogram dimension is set on: " << varioDimension.getValue() << std::endl;
                            if(varioDimension.getValue().compare("3D") == 0)
                            {
                                exp_var = exp_variogram(normal_values.values, corr_x, corr_y, corr_z, setLagType.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                //exp_var = exp_variogram (normal_values.values, corr_x, corr_y, corr_z, setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                std::cout << "Computing 3D experimental variogram ... COMPLETED." << std::endl;
                            }
                            else if(varioDimension.getValue().compare("2D") == 0)
                            {
                                //std::cout << "Computing 2D experimental variogram ... COMPLETED." << std::endl;
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
                                std::cout << "Clean experimental variogram ..." << std::endl;
                                exp_var = clean_exp_variogram (exp_var, setVarioCleanPoints.getValue());
                                std::cout << "Clean experimental variogram ... COMPLETED." << std::endl;
                            }


                            // Plotting experimental variogram
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
                                std::cout << "### Calculation direction is fixed on 0 degree from Z axis." << std::endl;

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
                        std::string plot_title = "Fitted Experimental Variogram Model";

                        //se ho già lo sperimentale, lo leggo da json?!
                        switch (dir)
                        {
                        case VarioDirection::OMNI:
                        {
                            exp_variog exp_var;

                            if(loadExpVario.isSet())
                            {
                                //leggi un file .dat e riempi la struttura dati exp_var;
                                load_exp_variogram(loadExpVario.getValue(), exp_var);
                            }
                            else
                            {
                                //Compute experimental variogram
                                std::cout << "Variogram dimension is set on: " << varioDimension.getValue() << std::endl;
                                if(varioDimension.getValue().compare("3D") == 0)
                                {
                                    exp_var = exp_variogram(normal_values.values, corr_x, corr_y, corr_z, setLagType.getValue(), setFactor.getValue(), setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                    //exp_var = exp_variogram (normal_values.values, corr_x, corr_y, corr_z, setPointsVario.getValue(), setMaxDistance.getValue(), setToleranceFactor.getValue());
                                    std::cout << "Computing 3D experimental variogram ... COMPLETED." << std::endl;
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
                                    plot_title = "Fitted Vertical Experimental Variogram Model";

    //                                PlotStruct vert_gamma_h;
    //                                for(size_t j=0; j < vertical_vario.gamma.size(); j++)
    //                                {
    //                                    vert_gamma_h.x.push_back(vertical_vario.h.at(j));          //distanze
    //                                    vert_gamma_h.y.push_back(vertical_vario.gamma.at(j));      //gamma (variogramma)
    //                                }

    //                                std::cout << "Automatic fitting of experimental variogram ... " << std::endl;
    //                                variogram fitted_vertical_var = fit_variogram(vertical_vario, setRangeStep.getValue(), setNuggetStep.getValue());

    //                                variogram_plot(vert_gamma_h, fitted_vertical_var, "Fitted Vertical Experimental Variogram Model", "Lag distance", "Semivariogram");

    //                                matplot::save(app_folder + "/" + data.getName() + "_vertical", "jpeg");
    //                                matplot::cla();
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
                                std::cout << "Fit variogram with fixed model type: " << setModel.getValue() << " and fixed nugget: " << setNugget.getValue() << std::endl;
                                variogram_type model_type;
                                convert_from_str(setModel.getValue(), model_type);

                                fitted_exp_var = fit_variogram (exp_var, setRangeStep.getValue(), model_type, setNugget.getValue(), true);
                            }
                            else if(setNugget.isSet())
                            {
                                std::cout << "Fit variogram with fixed nugget: " << setNugget.getValue() << std::endl;
                                fitted_exp_var = fit_variogram_1par (exp_var, setRangeStep.getValue(), setNugget.getValue());
                            }
                            else if(setModel.isSet())
                            {
                                std::cout << "Fit variogram with fixed model type: " << setModel.getValue() << std::endl;
                                variogram_type model_type;
                                convert_from_str(setModel.getValue(), model_type);

                                fitted_exp_var = fit_variogram (exp_var, setRangeStep.getValue(), setNuggetStep.getValue(), model_type);
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
                                fitted_exp_var = fit_variogram(exp_var, setRangeStep.getValue(), setNuggetStep.getValue());
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
                            variogram_plot(gamma_h, fitted_exp_var, plot_title, "Lag distance", "Semivariogram", setEps_y.getValue());


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
                                    //double w = 1.0/(zeros.at(k)+1);
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

                                    vv = fit_dir_variogram (dir_ex_var, directions, setRangeStep.getValue(), model_type, setNugget.getValue(), true);
                                }
                                else if(setNugget.isSet())
                                {
                                    std::cout << "Fit variogram with fixed nugget: " << setNugget.getValue() << std::endl;
                                    std::cout << FRED("Weight on nugget is not active!") << std::endl;
                                    std::cout << FMAG("Il peso non è attivo poichè non faccio la media dei nugget sulle direzioni (ovvero dove applico i pesi), ma fisso il nugget da cmd") << std::endl;
                                    vv = fit_dir_variogram (dir_ex_var, directions, setTol.getValue(), setRangeStep.getValue(), setNugget.getValue(), true);
                                }
                                else if(setModel.isSet())
                                {
                                    std::cout << "Fitting of directional variogram is set using fixed model type: " << setModel.getValue() << std::endl;
                                    variogram_type model_type;
                                    convert_from_str(setModel.getValue(), model_type);

                                    vv = fit_dir_variogram (dir_ex_var, directions, setTol.getValue(), setRangeStep.getValue(), setNuggetStep.getValue(), model_type, weight, true);
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
                                    vv = fit_dir_variogram (dir_ex_var, directions, setTol.getValue(), setRangeStep.getValue(), setNuggetStep.getValue(), weight, true);
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

                                variogram_plot(dir_gamma_h, vv.at(i_dir), "Fitted Directional Variogram Model", "Lag distance", "Semivariogram", setEps_y.getValue());

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

                                biv_plot_leg(h_plot, "Rose Diagram of Ranges", "hx", "hy", false, "Dir degree");

                                matplot::hold(matplot::on);
                                ellipse_plot(summary, setEps.getValue());

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
                }

                else
                {
                    std::cout << "\033[0;31mWARNING: Set Normal Score Transformation --nscore\033[0m" << std::endl;
                    exit(1);
                }
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



        // /////////////////////////////
        // //////////SUMMARY CSV FOR FRAMES
        // ///
        // ///
        // if(setMoreInfo.isSet() && varioDimension.getValue() != "3Dz")
        // {
        //     std::cout << "Save summary of multi-frame variography analysis ... " << std::endl;

        //     std::ofstream file_summary(out_folder + "/" + app_name + "/" + Variable.getValue() + "_summary.csv");
        //     std::string delimiter = ";";

        //     std::vector<std::string> vec_csv;
        //     vec_csv.push_back("frame_name");
        //     vec_csv.push_back("domain");
        //     vec_csv.push_back("mean");
        //     vec_csv.push_back("var");
        //     vec_csv.push_back("mean_zscore");
        //     vec_csv.push_back("var_zscore");
        //     vec_csv.push_back("model_type");
        //     vec_csv.push_back("nugget");
        //     vec_csv.push_back("sill");
        //     vec_csv.push_back("partial_sill");
        //     vec_csv.push_back("range");
        //     vec_csv.push_back("range_max");
        //     vec_csv.push_back("range_min");
        //     for(uint col =0; col < vec_csv.size(); col++)
        //     {
        //         file_summary << vec_csv.at(col);
        //         if(col != vec_csv.size() - 1)
        //             file_summary << delimiter; // No comma at end of line
        //     }
        //     file_summary << "\n";


        //     for(size_t id_frame=0; id_frame < vec_vario_frame.size(); id_frame++)
        //     {
        //         vec_csv.clear();
        //         vec_csv.push_back(vec_vario_frame.at(id_frame).frame_name);
        //         if(subDataset.isSet())
        //             vec_csv.push_back(subDataset.getValue());
        //         else
        //             vec_csv.push_back("");

        //         vec_csv.push_back(to_string(vec_vario_frame.at(id_frame).getStatisticsInfo().mean));
        //         vec_csv.push_back(to_string(vec_vario_frame.at(id_frame).getStatisticsInfo().var));
        //         vec_csv.push_back(to_string(vec_vario_frame.at(id_frame).getStatisticsInfo().mean_zscore));
        //         vec_csv.push_back(to_string(vec_vario_frame.at(id_frame).getStatisticsInfo().var_zscore));

        //         vec_csv.push_back(vec_vario_frame.at(id_frame).vario.type);
        //         vec_csv.push_back(to_string(vec_vario_frame.at(id_frame).vario.getNugget()));
        //         vec_csv.push_back(to_string(1.0));
        //         vec_csv.push_back(to_string(vec_vario_frame.at(id_frame).vario.getSill()));
        //         vec_csv.push_back(to_string(vec_vario_frame.at(id_frame).vario.range));
        //         vec_csv.push_back(to_string(vec_vario_frame.at(id_frame).vario.range_max));
        //         vec_csv.push_back(to_string(vec_vario_frame.at(id_frame).vario.range_min));

        //         for(uint col =0; col < vec_csv.size(); col++)
        //         {
        //             file_summary << vec_csv.at(col);
        //             if(col != vec_csv.size() - 1)
        //                 file_summary << delimiter; // No comma at end of line
        //         }
        //         file_summary << "\n";
        //     }
        //     file_summary.close();// Close the file
        //     std::cout << FGRN("Save summary of multi-frame variography analysis ... COMPLETED.") << std::endl;
        // }
    }



    } catch (ArgException &e)  // catch exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

}


