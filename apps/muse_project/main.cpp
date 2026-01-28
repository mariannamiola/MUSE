#include <tclap/CmdLine.h>
#include <json.hpp>
#include <filesystem>

#include "muselib/utils.h"
#include "muselib/colors.h"
#include "muselib/data_structures/project.h"

//for filesystem
#ifdef __APPLE__
    using namespace std::__fs;
#else
    //#include <experimental/filesystem>
    using namespace std;
#endif

using namespace MUSE;
using namespace TCLAP;

int main(int argc, char** argv)
{
    std::cout << std::endl;
    std::cout << "########### STARTING MUSE-PROJECT ..." << std::endl;
    std::cout << std::endl;

    //std::string app_name = "project";      //app name

    try {
    CmdLine cmd("MUSE = Modelling of Uncertainty as a Support of Environment; Muse Project tool", ' ', "version 0.0");

    // ---------------------------------------------------------------------------------------------------------
    // MAIN FUNCTIONALITIES:

    /**
     * @brief Create a new project
     * @param projectCreation Switch to enable new project creation
     *        - Usage: muse_project -N --pdir /path/to/projects --name MyProject
     */
    /**

     * @brief Creation new project

     * @param new_project Flag to creation new project

     */

    SwitchArg projectCreation           ("N", "new_project", "Creation new project", cmd, false); //booleano
    
    /**
     * @brief Directory path for the new project
     * @param projectFolder Path where the project will be created
     *        - Usage: muse_project --pdir /path/to/projects
     */
    /**

     * @brief Project directory

     * @param pdir Path to project directory

     */

    ValueArg<std::string> projectFolder ("p", "pdir", "Project directory", false, "Directory", "path", cmd);
    
    /**
     * @brief Name of the new project
     * @param Name Project name to be created
     *        - Usage: muse_project --name MyProject
     */
    /**

     * @brief Name new project

     * @param name Name of name new project

     */

    ValueArg<std::string> Name          ("n", "name", "Name new project", false, "File path", "file", cmd);

    //SwitchArg overwriteName             ("o", "ovrwrite_name", "Overwrite", cmd, false); //booleano


    // ---------------------------------------------------------------------------------------------------------
    // ADDITIONAL FUNCTIONALITIES:

    /**
     * @brief Set project EPSG code (coordinate reference system authority)
     * @param setEPSG EPSG code for the project coordinate system
     *        - Format: EPSG:####
     *        - Example: muse_project --setEPSG EPSG:4326
     *        - Common codes: EPSG:4326 (WGS84), EPSG:3857 (Web Mercator)
     */
    /**

     * @brief Set project EPSG

     * @param setEPSG project epsg

     */

    ValueArg<std::string> setEPSG       ("", "setEPSG", "Set project EPSG", false, "Unknown", "EPSG:n", cmd);


    // ---------------------------------------------------------------------------------------------------------
    // PARSING:

    // Parse the argv array.
    cmd.parse(argc, argv);

    // ---------------------------------------------------------------------------------------------------------
    // SETTINGS:

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

            // std::filesystem::path path = argv[i];
            // std::filesystem::path relpath;
            // #ifdef __APPLE__
            // relpath = filesystem::relative(path, abspath);
            // #else
            // relpath = path.relative_path();
            // #endif

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

    // ---------------------------------------------------------------------------------------------------------
    // STARTS:

    // Option 0. Project creation and settings
    if(projectCreation.isSet())
    {
        MUSE::Project Project;
        Project.folder = projectFolder.getValue();
        Project.name = Name.getValue();

        bool equal_name = check_folder_name(Project.name, Project.folder);
        if(equal_name == true)
        {
            std::cout << "\033[0;31m" << Project.name << " already exists.\033[0m" << std::endl;

//            if(!overwriteName.isSet())
//                Project.name = Project.name + "_01";
//            else
//                std::cout << "\033[0;32mOverwrited folder.\033[0m" << std::endl;
        }

        // cancella tutti gli output??

        // Output folder
        Project.folder = Project.folder + "/" + Project.name;
        filesystem::create_directory(Project.folder);

        std::string in_dir = Project.folder + "/in";
        filesystem::create_directory(in_dir);

        std::string out_dir = Project.folder + "/out";
        filesystem::create_directory(out_dir);

        std::cout << "\033[0;32mCreation new project " << Project.folder << " ... COMPLETED.\033[0m" << std::endl;


//        // Creation of Project Json
//        json project_metadata;
//        build_defaultJson(Project, project_metadata);

//        //Setting project EPSG
//        if(setEPSG.getValue().compare("Unknown") == 0)
//            project_metadata["Project"].push_back({"Authority", "Unknown"});
//        else
//            project_metadata["Project"].push_back({"Authority", setEPSG.getValue()});

        if(setEPSG.isSet())
            Project.setAuthority(setEPSG.getValue());

        //save_json(out_dir + "/"+ Project.name, project_metadata); //sovrascrivi il json aggiornato
        Project.write(out_dir + "/"+ Project.name + ".json");

        std::cout << "\033[0;32mCreation of Project JSON into " << out_dir << " ... COMPLETED.\033[0m" << std::endl;
    }

    //CHANGE DIR (nel caso di trasferimento progetto da pc a pc)
    //SALVO NEL JSON



    } catch (ArgException &e)  // catch exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

}
