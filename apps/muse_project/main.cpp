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
    CmdLine cmd("MUSE - Modelling Uncertainty as a Support of Environment. MUSE-project application", ' ', "version 0.0");

    // ---------------------------------------------------------------------------------------------------------
    // MAIN FUNCTIONALITIES:

    /**
     * @brief Create a new project
     * @param new_project Flag to enable new project creation
     * @note When using --new_project, the following flags are typically used together:
     *       - --pdir (required): Specify project directory
     *       - --name (required): Specify project name
     *       - --setEPSG (optional): Set coordinate reference system
     * @example muse_project -N -p /path/to/project/dir --name 00_test --setEPSG EPSG:4326
     */
    SwitchArg projectCreation           ("N", "new_project", "New project creation", cmd, false); //booleano
    
    /**
     * @brief Specify project directory path
     * @param pdir Path where the project will be created
     * @note Required when using -N/--new_project flag
     * @example -p /path/to/project/dir
     */
    ValueArg<std::string> projectFolder ("p", "pdir", "Project directory", true, "/path/to/project/dir", "string", cmd);
    
    /**
     * @brief Specify name of the new project
     * @param name Project name to be created
     * @note Required when using --new_project flag. The project name will be used as:
     *       - Directory name: {pdir}/{name}
     *       - JSON config file: {pdir}/{name}/out/{name}.json
     * @example --name 00_test
     */
    ValueArg<std::string> Name          ("n", "name", "Set project name", false, "name", "string", cmd);


    // ---------------------------------------------------------------------------------------------------------
    // ADDITIONAL FUNCTIONALITIES:

    /**
     * @brief Set project EPSG coordinate reference system
     * @param setEPSG EPSG authority code for the project coordinate system
     * @note Optional parameter, used with --new_project flag
     *       Common EPSG codes:
     *       - EPSG:4326 (WGS84 Geographic)
     *       - EPSG:3857 (Web Mercator)
     *       - EPSG:32633 (UTM Zone 33N)
     * @example --setEPSG EPSG:4326
     */
    ValueArg<std::string> setEPSG       ("", "setEPSG", "Set project EPSG", false, "unknown", "EPSG:n", cmd);


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
            std::cout << "\033[0;31m" << Project.name << " already exists.\033[0m" << std::endl;

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

        Project.write(out_dir + "/"+ Project.name + ".json");

        std::cout << "\033[0;32mCreation of Project JSON into " << out_dir << " ... COMPLETED.\033[0m" << std::endl;
    }

    } catch (ArgException &e)  // catch exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

}
