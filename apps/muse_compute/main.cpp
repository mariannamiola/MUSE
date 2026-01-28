#include <iostream>
#include <filesystem>
#include <string>

#include <tclap/CmdLine.h>

#include "geostatslib/statistics/decluster.h"
#include "muselib/metadata/extraction_meta.h"
#include "muselib/utils.h"
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
    std::cout << "########### STARTING MUSE-COMPUTE ..." << std::endl;
    std::cout << std::endl;

    std::string app_name = "compute"; //app name
    std::string app_vario = "vario"; //app vario name
    std::string app_data = "data"; //app data name
    std::string app_manipulate = "manipulate";


    try {
    CmdLine cmd("MUSE = Modelling of Uncertainty as a Support of Environment; Compute tool", ' ', "version 0.0");


    // ---------------------------------------------------------------------------------------------------------
    // MAIN FUNCTIONALITIES:

    // Option 0. New project creation
    /**
     * @brief Enable computation mode for MUSE
     * @param compute Flag to enable computation mode
     */
    SwitchArg interpolationCompute      ("C", "compute", "Creation new project", cmd, false); //booleano
    
    /**
     * @brief Set computation mode
     * @param mode Computation mode setting (default: AUTO)
     */
    ValueArg<std::string> modeCompute   ("", "mode", "Set mode for compute", false, "string", "AUTO", cmd);
    
    /**
     * @brief Specify project directory
     * @param pdir Path to the project directory
     */
    ValueArg<std::string> projectFolder ("p", "pdir", "Project directory", false, "Directory", "path", cmd);
    
    /**
     * @brief Specify variable name to analyze
     * @param var Name of the variable to process
     */
    ValueArg<std::string> Variable      ("v", "var", "Variable", false, "name_var", "string", cmd);
    
    /**
     * @brief Specify geometry model name
     * @param geom Name of the geometry model to use
     */
    ValueArg<std::string> geomModel     ("m", "geom", "Geometry model", false, "name_geometry", "string", cmd);

    /**
     * @brief Extract sub-dataset based on geometry
     * @param sub Path to sub-dataset extraction directory
     */
    ValueArg<std::string> subDataset    ("", "sub", "Extraction sub dataset basing on geometry", false, "Directory", "path", cmd);

    // Option 0a. Data rotation
    /**
     * @brief Set rotation axis for data transformation
     * @param rotaxis Axis for rotation (default: NO)
     */
    ValueArg<std::string> setRotAxis    ("", "rotaxis", "Set rotation axis", false, "NO", "rot_axis", cmd);
    
    /**
     * @brief Set rotation angle in degrees (clockwise)
     * @param rotangle Rotation angle in degrees
     */
    ValueArg<double> setRotAngle        ("", "rotangle", "Set rotation angle (clockwise)", false, 0.0, "double", cmd);
    
    /**
     * @brief Set X coordinate of rotation center
     * @param rotcx X coordinate of rotation center
     */
    ValueArg<double> setRotCenterX      ("", "rotcx", "Set rotation center x", false, 0.0, "double", cmd);
    
    /**
     * @brief Set Y coordinate of rotation center
     * @param rotcy Y coordinate of rotation center
     */
    ValueArg<double> setRotCenterY      ("", "rotcy", "Set rotation center y", false, 0.0, "double", cmd);
    
    /**
     * @brief Set Z coordinate of rotation center
     * @param rotcz Z coordinate of rotation center
     */
    ValueArg<double> setRotCenterZ      ("", "rotcz", "Set rotation center z", false, 0.0, "double", cmd);


    //ValueArg<std::string> filenameStrat ("f", "filestrat", "Set filename of samples in stratigraphic coordinates", false, "filename", "path", cmd);
    
    /**
     * @brief Set fixed variogram parameters
     * @param vario Path to variogram configuration file
     */
    ValueArg<std::string> setVario      ("", "vario", "Set fixed variogram", false, "string", "path", cmd);


    // Option: types of variogram directions
    std::vector<std::string> allowedVarioDir = {"OMNI","DIR"};
    ValuesConstraint<std::string> allowedValsVD(allowedVarioDir);
    
    /**
     * @brief Set variogram direction type
     * @param dir Type of variogram direction (OMNI or DIR)
     */
    ValueArg<std::string> varioDirection ("", "dir", "type of variogram direction", false, "OMNI", &allowedValsVD, cmd);

    // Option: types of variogram dimensions
    std::vector<std::string> allowedVarioDim = {"3D","3Dxy","3Dz","2D","1Dz","1D"};
    ValuesConstraint<std::string> allowedValsVDm(allowedVarioDim);
    
    /**
     * @brief Set variogram dimension type
     * @param dim Type of variogram dimension (3D, 3Dxy, 3Dz, 2D, 1Dz, 1D)
     */
    ValueArg<std::string> varioDimension ("", "dim", "type of variogram dimension", false, "3D", &allowedValsVDm, cmd);
    
    /**
     * @brief Set range in Z direction
     * @param zrange Range value in Z direction
     */
    ValueArg<double> setZRange           ("", "zrange", "Set range in Z direction", false, 1, "double", cmd);

    // Option: set interpolation criteria
    std::vector<std::string> allowedCRIT = {"SGS","IK","SISIM"};
    ValuesConstraint<std::string> allowedValsCRIT(allowedCRIT);
    
    /**
     * @brief Set interpolation algorithm
     * @param crit Interpolation algorithm (SGS, IK, or SISIM)
     */
    ValueArg<std::string> setCRIT       ("", "crit", "Set interpolation algorithm", false, "SGS", &allowedValsCRIT, cmd);

    // Option 0b. Parameters for simulations
    /**
     * @brief Enable back normal score transformation integrated into SGS
     * @param bnscore Flag to enable back normal score transformation
     */
    SwitchArg setBackNormalScore        ("", "bnscore", "Set to do back normal score integrated into SGS", cmd, false); //booleano
    
    /**
     * @brief Set extrapolation type
     * @param extr Type of extrapolation (default: none)
     */
    ValueArg<std::string> setExtrType   ("", "extr", "Set extrapolation type", false, "none", "string", cmd); //di default settata su "none"
    
    /**
     * @brief Set minimum value for extrapolation
     * @param minextr Minimum extrapolation value
     */
    ValueArg<double> setMinExtr         ("", "minextr", "Min value for extrapolation", false, 0.0, "double", cmd); //n. di simulazioni = 10 di default
    
    /**
     * @brief Set maximum value for extrapolation
     * @param maxextr Maximum extrapolation value
     */
    ValueArg<double> setMaxExtr         ("", "maxextr", "Max value for extrapolation", false, 100000.0, "double", cmd); //n. di simulazioni = 10 di default

    /**
     * @brief Set number of simulation iterations
     * @param nsim Number of simulation iterations (default: 10)
     */
    ValueArg<uint> setNsim              ("", "nsim", "Number of iterations", false, 10, "uint", cmd); //n. di simulazioni = 10 di default

    // Option: set 2D declustering on data
    /**
     * @brief Set cell size for 2D declustering
     * @param csize Cell size for 2D declustering
     */
    ValueArg<double> setCellSize            ("", "csize", "Set cell size for 2D declustering", false, 0.0, "double", cmd);
    
    /**
     * @brief Set number of steps for 2D declustering grid translation
     * @param nstep Number of steps for 2D declustering
     */
    ValueArg<int> setNStep                  ("", "nstep", "Set n steps for 2D declustering (grid translation)", false, 0, "int", cmd);

    // Option 1. Statistical analysis on simulation results
    /**
     * @brief Enable statistical analysis on simulation results
     * @param stats Flag to compute statistical analysis
     */
    SwitchArg statisticalAnalysis       ("S", "stats", "Compute statistical analysis on simulation results", cmd, false); //booleano
    
    // Option: set interpolation criteria
    std::vector<std::string> allowedSPACE = {"NORMAL","VAR"};
    ValuesConstraint<std::string> allowedValsSPACE(allowedSPACE);
    
    /**
     * @brief Set space type for analysis
     * @param space Space type (NORMAL or VAR)
     */
    ValueArg<std::string> setSpace      ("", "space", "Set space", false, "NORMAL", &allowedValsSPACE, cmd);


    // Option 2. Back normal score
    /**
     * @brief Enable back normal score transformation
     * @param bns Flag to perform back normal score transformation
     */
    SwitchArg doBackNormalScore         ("B", "bns", "Do back normal score", cmd, false); //booleano
    
    /**
     * @brief Set input file path
     * @param file Path to input file
     */
    ValueArg<std::string> setFile       ("f", "file", "Set file", false, "Directory", "path", cmd);

    //ValueArg<std::string> setJSON       ("", "json", "Set JSON file", false, "Directory", "path", cmd);


    // Option 3. Database creation to store simulation results
    /**
     * @brief Enable database creation from simulations
     * @param db Flag to create database from simulation results
     */
    SwitchArg createDatabase            ("D", "db", "Create database from simulations", cmd, false); //booleano


    // ---------------------------------------------------------------------------------------------------------
    // ADDITIONAL FUNCTIONALITIES:

    /**
     * @brief Set number of input samples
     * @param input Number of input samples to use (default: 4)
     */
    ValueArg<uint> setInputSamples      ("", "input", "Set number of input samples", false, 4, "int", cmd);
    
    /**
     * @brief Set number of simulated points
     * @param simulated Number of points to simulate (default: 3)
     */
    ValueArg<uint> setSimulatedPoints   ("", "simulated", "Set number of simulated points", false, 3, "int", cmd);
    
    /**
     * @brief Set scale factor of search radius
     * @param scaleradius Scale factor for search radius (default: 1.0)
     */
    ValueArg<double> setScaleRadius     ("", "scaleradius", "Set scale factor of search radius", false, 1.0, "double", cmd);
    
    /**
     * @brief Enable octant search in SGS algorithm
     * @param octant Flag to enable octant search
     */
    SwitchArg doOctantSearch            ("", "octant", "Do octant search in SGS algorithm", cmd, false); //booleano

    /**
     * @brief Enable CSV format for output files
     * @param csv Flag to save files in CSV format
     */
    SwitchArg csvConversion             ("", "csv", "Saving file as csv", cmd, false); //booleano

    std::vector<std::string> allowedSGS = {"MEAN","VECSIM"};
    ValuesConstraint<std::string> allowedValsSGS(allowedSGS);
    
    /**
     * @brief Set type of SGS output
     * @param out Type of SGS output (MEAN or VECSIM)
     */
    ValueArg<std::string> setSGSoutput  ("", "out", "Set type of SGS output", false, "MEAN", &allowedValsSGS, cmd); //di default settata su "none"

    /**
     * @brief Set data format encoding for output
     * @param format Enable data format encoding (default: YES)
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


    // ---------------------------------------------------------------------------------------------------------
    // STARTS:

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
                        std::cout << std::endl;
                        std::cout << FMAG("### SGS OUTPUT - VECSIM: vector of simulation results in the normal space -> a CSV file for each simulation") << std::endl;
                        std::cout << FMAG("### Back normal score is managed by using the command -B") << std::endl;
                        std::cout << std::endl;

                        app_folder += "/_normspace";
                        if(!filesystem::exists(app_folder))
                            filesystem::create_directory(app_folder);

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
                        std::cout << std::endl;
                        std::cout << FMAG("### SGS OUTPUT - VECSIM: vector of simulation results in the normal space -> a CSV file for each simulation") << std::endl;
                        std::cout << FMAG("### Back normal score is managed by using the command -B") << std::endl;
                        std::cout << std::endl;

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



            // 2. Storing json information into class Data
            MUSE::Metadata meta_input;
            meta_input.read(l + "/metadata/" + Variable.getValue() + ".json");
            Data data = meta_input.getData(0);

            data.setType(data.flag);
            readTextValues(l + "/data/" + Variable.getValue() + ".dat", data.text_values);

            //DataSummary(data); //da sistemare
            DataSummary sumdata;
            sumdata.setSummary(data);


            // 3. Check on json vario files
            std::vector<std::string> vario_files = get_files(out_vario, ".json");
            if(vario_files.size() == 0)
            {
                std::cerr << "ERROR: vario folder is empty!" << std::endl;
                exit(1);
            }

            if (data.type == varType::NUMBER)
            {
                if(vario_files.size() > 1)
                {
                    std::cerr << "ERROR: Number of JSON file is major than 1. Only a JSON is accepted for numerical type variable." << std::endl;
                    exit(1);
                }
            }


            VarioMeta metavario;
            metavario.read(vario_files.at(0));

            std::cout << std::endl;
            std::cout << FMAG("RICORDA - Le informazioni sul variogramma sono estratte dal primo file json!! Anche in condizioni di file json multipli (come può succedere in caso di variabili categoriche)") << std::endl;
            std::cout << std::endl;

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
                std::cout << "\033[0;33mWARNING: Stratigraphic transformation is set on " << metavario.getManipulate().stratigraphic_transf << ". Variogram is computed in stratigraphic coordinate system.\033[0m" << std::endl;

                std::cout << "Stratigraphic coordinates are located in " << out_man + "/" << std::endl;
                load_xyzfile(out_man + "/" + metavario.getManipulate().filename + ".xyz", xCoord, yCoord, zCoord);
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
                    //processingData.sub_dataset = subDataset.getValue();

                    if(subDataset.getValue().compare(metavario.getManipulate().domain) != 0)
                    {
                        std::cerr << "ERROR: vario is compute for another subdataset!" << std::endl;
                        exit(1);
                    }

                    MUSE::ExtractionMeta extrmeta;
                    extrmeta.read(out_man + "/" + metavario.getManipulate().domain + ".json");
                    std::cout << "Extraction sub-dataset is set. Reading ... " << out_man + "/" + metavario.getManipulate().domain + ".json" << std::endl;

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

                        metacompute.setRotation(dataRotation);
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
                    //processingData.sub_dataset = subDataset.getValue();

                    if(subDataset.getValue().compare(metavario.getManipulate().domain) != 0)
                    {
                        std::cerr << "ERROR: vario is compute for another subdataset!" << std::endl;
                        exit(1);
                    }

                    MUSE::ExtractionMeta extrmeta;
                    extrmeta.read(out_man + "/" + metavario.getManipulate().domain + ".json");
                    std::cout << "Extraction sub-dataset is set. Reading ... " << out_man + "/" + metavario.getManipulate().domain + ".json" << std::endl;

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
                        metacompute.setRotation(dataRotation);
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
            if(metavario.getProcessing().v_name != Variable.getValue())
            {
                std::cout << FRED("ERROR on JSON content! Check <v_name> field in JSON file and set the correct variable name in the command line!") << std::endl;
                exit(1);
            }
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
                //std::cout << "### Only Indicator Kriging is active for categoric variables" << std::endl;
                std::cout << FMAG("### RICORDA - In questo caso mi aspetto più file json, uno per ogni categoria") << std::endl;
                std::cout << std::endl;


                // 1) Codifica ad indicatori prima di passare al variogramma!!
                std::vector<int> categ = categories_extraction(conv_values);

                //La trasformazione agli indicatori viene effettuata all'interno della funzione del kriging

                // 2) Preparazione dati di input -> vector<point3d>
                std::vector<point3d> input;
                for(uint i=0; i< conv_values.size(); i++)
                    input.push_back(point3d({xCoord.at(i), yCoord.at(i), zCoord.at(i)}, {conv_values.at(i)}));


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

                std::vector<Variogram> variograms;

                VarioDirection dir;
                convert_from_str(metavario.getInfoVariogram().direction, dir);

                ComputeMeta::InfoVariogram info_vario;
                info_vario.dimension = metavario.getInfoVariogram().dimension;
                info_vario.direction = metavario.getInfoVariogram().direction;
                metacompute.setInfoVariogram(info_vario);


                std::cout << std::endl;
                std::cout << FMAG("L'ordine dei file corrisponde al vettore delle categorie!! Matching json-categoria rispettato per COSTRUZIONE!") << std::endl;
                std::cout << std::endl;

                for(uint c=0; c<categ.size(); c++)
                {
                    std::cout << "### Category ID: " << c << std::endl;
                    std::cout << "### Category VALUE: " << categ.at(c) << std::endl;

                    std::string vario_name = out_vario + "/" + Variable.getValue() + std::to_string(categ.at(c));
                    if(subDataset.isSet())
                        vario_name += "_" + subDataset.getValue() + ".json";
                    else
                        vario_name += ".json";

                    std::cout << "### JSON: " << vario_name << std::endl;

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
                        //std::cout << FRED("ERROR: DIRECTIONAL CASE - TO BE IMPLEMENTED!") << std::endl;

                        std::cout << "Range min is set on: " << metavario_cat.getSummary().min_semiaxis << std::endl;
                        std::cout << "Range max is set on: " << metavario_cat.getSummary().max_semiaxis << std::endl;
                        //fvm_cat.set_range(metavario_cat.getSummary().min_semiaxis, metavario_cat.getSummary().max_semiaxis);

                        if(setZRange.isSet())
                        {
                            std::cout << "Range in Z direction is set on: " << setZRange.getValue() << std::endl;
                            fvm_cat.set_range(metavario_cat.getSummary().min_semiaxis, metavario_cat.getSummary().max_semiaxis, setZRange.getValue());
                        }
                        else
                            fvm_cat.set_range(metavario_cat.getSummary().min_semiaxis, metavario_cat.getSummary().max_semiaxis);


                        fvm_cat.set_azimuth(metavario_cat.getSummary().max_direction);
                        std::cout << "Azimuth is set on max continuity direction: " << fvm_cat.get_azimuth() << " degree from North" << std::endl;

                        //Settati sulla massima direzione, ma non cambiano (per costruzione -> calcolo automatico del vario direzionale)
                        fvm_cat.nugget = metavario_cat.getFitExpVariog(0).nugget;
                        std::cout << "Nugget is set on: " << fvm_cat.nugget << std::endl;

                        //MODIFICATO COME 1 - NUGGET!!!!!!!!!!!!!!!!!
                        fvm_cat.sill = metavario_cat.getFitExpVariog(0).sill - fvm_cat.nugget;
                        std::cout << "Sill is set on: " << fvm_cat.sill << std::endl;

                        //anche il tipo è uguale tra tutti, quindi prendo quello a modello in dir 0
                        variogram_type type;
                        convert_from_str(metavario_cat.getFitExpVariog(0).type, type);
                        fvm_cat.type = type;
                        std::cout << "Type is set on: " << metavario_cat.getFitExpVariog(0).type << std::endl;
                        std::cout << std::endl;


                        //for json
                        fitvariov.setNugget(fvm_cat.nugget);
                        fitvariov.setSill(fvm_cat.sill);
                        fitvariov.range_max = fvm_cat.get_maxrange();
                        fitvariov.range_min = fvm_cat.get_minrange();
                        fitvariov.setType(metavario_cat.getFitExpVariog(0).type);

                        break;
                    }
                    }

                    if(fvm_cat.type == GAUSSIAN)
                    {
                        std::cout << "### Check on nugget for gaussian model ..." << std::endl;
                        if(fvm_cat.nugget == 0.0)
                        {
                            std::cout << "Instability problems are encountered with a Gaussian model with no nugget effect." << std::endl;
                            fvm_cat.nugget = fvm_cat.nugget + 0.001;
                            fvm_cat.sill = fvm_cat.sill - 0.001;

                            std::cout << "Nugget value is perturbed as: " << fvm_cat.nugget << std::endl;
                            std::cout << "Updating sill value as: " << fvm_cat.sill << std::endl;
                        }
                    }

                    variograms.push_back(fvm_cat);
                }


                //4. Starting simulations (CHOSEN THE INTERPOLATION METHOD: KRIGING, SGS??)
                if(setCRIT.getValue().compare("IK") == 0)
                {
                    indicator_kriging(nodes, input, categ, variograms, setInputSamples.getValue(), doOctantSearch.getValue() ,setScaleRadius.getValue());
                    app_folder += "/kriging";

                    std::cout << FGRN("Indicator Kriging ... COMPLETED.") << std::endl;
                }
                else if(setCRIT.getValue().compare("SISIM") == 0)
                {

                    if(ext_mesh == ".off" || ext_mesh == ".obj")
                    {


                        parallel_sis(nodes, surf_mesh ,input, categ, variograms, setNsim.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue());

                    }
                    else if(ext_mesh == ".mesh" || ext_mesh == ".vtk")
                    {

                        parallel_sis(nodes, vol_mesh ,input, categ, variograms, setNsim.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue());

                    }

                   // parallel_sis(nodes, mesh ,input, categ, variograms, setNsim.getValue(), setInputSamples.getValue(), setSimulatedPoints.getValue(), setScaleRadius.getValue());

                    app_folder += "/_stats"; //sisim";
                    if(!filesystem::exists(app_folder))
                        filesystem::create_directory(app_folder);

                    std::cout << FGRN("Indicator Simulation ... COMPLETED.") << std::endl;
                }
                else
                {
                    std::cerr << "ERROR. Default algorithm is not available for categorical variable. Set --crit SGS to specify algorthm." << std::endl;
                    exit(1);
                }

                std::vector<double> results_x, results_y, results_z, results_v;
                for(uint n=0; n < nodes.size(); n++)
                {
                    results_x.push_back(nodes.at(n).get(0));
                    results_y.push_back(nodes.at(n).get(1));
                    results_z.push_back(nodes.at(n).get(2));
                    results_v.push_back(nodes.at(n).get_value(0));
                }
                export_idxyzv (app_folder + "/" + data.getName() + "_best_withlocations.csv", results_x, results_y, results_z, results_v);
                export1d_xyz (app_folder + "/" + data.getName() + "_best.csv", results_v);

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

                if(metavario.getProcessing().normal_score.compare("YES") != 0)
                {
                    //VARIABILE NON NORMALE!!!
                    std::cerr << "Normal Score Transformation is required for SGS algorithm." << std::endl;
                    exit(1);
                }


                normalscore normal_values;
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
                    std::cout << "Mesh is surface." << std::endl;


                    MUSE::SurfaceMesh<> surf_mesh;
                    surf_mesh.load(geomModel.getValue().c_str());

                    sim.n_elements = surf_mesh.num_polys();


                    //Funzione vecchia a cui gli passo il range!
                    //Anche sfruttando le funzioni sul range che tengono conto di min/max, NON va bene, perchè all'interno c'è il set_range(range), ovvero unico valore di range!
                    //VARIO FUNZIONANTE:
    //                fvm.set_range(3.1, 23.5);
    //                fvm.set_azimuth(88);
    //                fvm.nugget = 0.51;
    //                fvm.type = SPHERIC;


                    /*std::cout << std::endl;
                    std::cout << FMAG("############################################################") << std::endl;
                    std::cout << FMAG("PER CONTROLLO (PRIMA DELLE SIMULAZIONI):") << std::endl;
                    std::cout << FMAG("La funzione delle SGS considera il VARIO OMNIDIREZIONALE con i seguenti parametri: ") << std::endl;
                    std::string string_type;
                    convert_to_str(string_type, fvm.type);
                    std::cout << "Type = " << string_type << std::endl;
                    std::cout << "Dir max = " << fvm.get_azimuth() << " degree from North." << std::endl;
                    std::cout << "Range max = " << fvm.get_range(fvm.get_radians(fvm.get_azimuth())) << std::endl;
                    std::cout << "Nugget = " << fvm.nugget << std::endl;
                    std::cout << "Sill = " << fvm.sill << std::endl;
                    std::cout << FMAG("############################################################") << std::endl;
                    std::cout << std::endl;

                    sgs_output = parallel_sgs2 (surf_mesh, normal_values.values, corr_x, corr_y, corr_z,
                                                                fvm.get_range(fvm.get_radians(fvm.get_azimuth())), fvm.sill, fvm.nugget, fvm.type,
                                                                setNsim.getValue(), normal_values,
                                                                back_normal_score_inSGS, setExtrType.getValue(), setMinExtr.getValue(), setMaxExtr.getValue());*/



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

                    if(fvm.type == GAUSSIAN)
                    {
                        std::cout << "### Check on nugget for gaussian model ..." << std::endl;
                        if(fvm.nugget == 0.0)
                        {
                            std::cout << "Instability problems are encountered with a Gaussian model with no nugget effect." << std::endl;
                            fvm.nugget = fvm.nugget + 0.001;
                            fvm.sill = fvm.sill - 0.001;

                            std::cout << "Nugget value is perturbed as: " << fvm.nugget << std::endl;
                            std::cout << "Updating sill value as: " << fvm.sill << std::endl;
                        }
                    }


                    if(setSGSoutput.getValue().compare("VECSIM") == 0)
                    {
                        std::cout << std::endl;
                        std::cout << FMAG("### SGS OUTPUT - VECSIM: vector of simulation results in the normal space -> a CSV file for each simulation") << std::endl;
                        std::cout << FMAG("### Back normal score is managed by using the command -B") << std::endl;
                        std::cout << std::endl;

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
                        std::cout << std::endl;
                        std::cout << FMAG("### SGS OUTPUT - VECSIM: vector of simulation results in the normal space -> a CSV file for each simulation") << std::endl;
                        std::cout << FMAG("### Back normal score is managed by using the command -B") << std::endl;
                        std::cout << std::endl;

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

        // /////////////////////////////
        // //////////SUMMARY CSV FOR FRAMES
        // ///
        // ///
        // std::cout << "Save summary of multi-frame variography analysis ... " << std::endl;

        // std::ofstream file_statssummary(out_folder + "/" + app_name + "/" + Variable.getValue() + "_" + setSpace.getValue() + "summary.csv");
        // std::string delimiter = ";";

        // std::vector<std::string> vec_csv;
        // vec_csv.push_back("frame_name");
        // vec_csv.push_back("domain");
        // if(setSpace.getValue().compare("NORMAL") == 0)
        // {
        //     vec_csv.push_back("est_mean_zscore");
        //     vec_csv.push_back("est_var_zscore");
        // }
        // else
        // {
        //     vec_csv.push_back("est_mean");
        //     vec_csv.push_back("est_var");
        // }

        // for(uint col =0; col < vec_csv.size(); col++)
        // {
        //     file_statssummary << vec_csv.at(col);
        //     if(col != vec_csv.size() - 1)
        //         file_statssummary << delimiter; // No comma at end of line
        // }
        // file_statssummary << "\n";
        // ///////////////////////////////////////////////////////////////////////


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
                        std::cout << filename << std::endl;
                    }
                }
            }
            if (file_list.size() == 0)
            {
                std::cerr << "ERROR: " << app_folder << " is empty!" << std::endl;
                exit(1);
            }

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


            //     file_statssummary.is_open();
            //     for(uint col =0; col < vec_csv.size(); col++)
            //     {
            //         file_statssummary << vec_csv.at(col);
            //         if(col != vec_csv.size() - 1)
            //             file_statssummary << delimiter; // No comma at end of line
            //     }
            //     file_statssummary << "\n";
        }
        // file_statssummary.close();// Close the file
        // std::cout << FGRN("Save summary of multi-frame variography analysis ... COMPLETED.") << std::endl;
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
                std::cout << norm_path << std::endl;
                std::vector<std::string> list_csv = get_files(norm_path, ext, true);
                if(list_csv.size() == 0)
                {
                    std::cerr << "ERROR: Only .CSV or .DAT are accepted." << std::endl;
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



    } catch (ArgException &e)  // catch exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

}
