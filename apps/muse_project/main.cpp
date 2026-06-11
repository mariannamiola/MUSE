#include <tclap/CmdLine.h>
#include <json.hpp>
#include <filesystem>

#include "muselib/utils.h"
#include "muselib/data_structures/project.h"

//for filesystem
#ifdef __APPLE__
    using namespace std::__fs;
#else
    //#include <experimental/filesystem>
    using namespace std;
#endif

//Detecting operating system
std::string detect_os()
    {
#ifdef _WIN32
        return "Windows";
#elif __APPLE__
        return "macOS";
#elif __linux__
    return "Linux";
#else
    return "Unknown OS";
#endif
    }

//Detecting architecture
std::string detect_arch() {
#if defined(__x86_64__) || defined(_M_X64)
        return "x86_64";
#elif defined(__aarch64__)
        return "ARM64";
#elif defined(__i386__)
        return "x86";
#else
        return "Unknown Arch";
#endif
    }

//Detecting compiler
std::string compiler_info() {
#ifdef __clang__
    return "Clang " + std::to_string(__clang_major__);
#elif defined(__GNUC__)
    return "GCC " + std::to_string(__GNUC__);
#elif defined(_MSC_VER)
    return "MSVC " + std::to_string(_MSC_VER);
#else
    return "Unknown compiler";
#endif
}

//Detecting timestamp
std::string timestamp_now()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}


using namespace MUSE;
using namespace TCLAP;

int main(int argc, char** argv)
{
    try {
    CmdLine cmd("MUSE - Modelling Uncertainty as a Support of Environment. MUSE-project application", ' ', "version 0.0");

    std::cout << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "================== STARTING MUSE-PROJECT ===================" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;

    // ---------------------------------------------------------------------------------------------------------
    // MAIN FUNCTIONALITIES:

    /**
     * @brief Creation of a new project. With this flag that needs -p (--pdir) and -n (--name) options, MUSE-project create a filesystem space where organize hyerarchically all the information about input, output and metadata files.
     * @param new_project Flag to enable new project creation (mandatory)
     * @note When using --new_project, the following flags are typically used together:
     *       - --pdir (required): Specify project directory
     *       - --name (required): Specify project name
     *       - --setEPSG (optional): Set coordinate reference system
     * @example muse_project -N -p /path/to/project/dir -n 00_test --setEPSG EPSG:4326
     */
    SwitchArg projectCreation           ("N", "new_project", "New project creation", cmd, false); //booleano
    
    /**
     * @brief Specify the directory path where the project is created
     * @param pdir Path where the project is created (mandatory)
     * @note Required when using --new_project flag
     * @example -p /path/to/project/dir
     */
    ValueArg<std::string> projectFolder ("p", "pdir", "Project directory", true, "/path/to/project/dir", "string", cmd);
    
    /**
     * @brief Specify name of the new project
     * @param name Project name to be created (default: <project_name>). Check the project name and, if exists, the project is not created.
     * @note Required when using --new_project flag. The project name will be used as:
     *       - Directory name: /path/to/project/dir/project_name
     *       - JSON config file in output folder: /path/to/project/dir/project_name/out/project_name.json
     * @example -n 00_test
     */
    ValueArg<std::string> Name          ("n", "name", "Set project name", false, "project_name", "string", cmd);


    // ---------------------------------------------------------------------------------------------------------
    // ADDITIONAL FUNCTIONALITIES:

    /**
     * @brief Set project EPSG coordinate reference system in projected coordinates. At the moment, no geographic coordinates are permitted.
     * @param setEPSG EPSG authority code for the project coordinate system (mandatory)
     * @note Optional parameter, used with --new_project flag
     *       Common EPSG codes:
     *       - EPSG:4326 (WGS84 Geographic)
     *       - EPSG:3857 (Web Mercator)
     *       - EPSG:32633 (UTM Zone 33N)
     * @example --setEPSG EPSG:4326
     */
    ValueArg<std::string> setEPSG       ("", "setEPSG", "Set project EPSG code", false, "unknown", "EPSG:n", cmd);


    /**
     * @brief Check project name and, if exists, create a new folder as "project_YYYY-MM-DD_HH-MM-SS"
     * @param timestamp
     * @note Optional parameter
     * @example muse_project -N -p /path/to/project/dir -n 00_test --timestamp
     */
    SwitchArg timestamp                 ("", "timestamp", "Rename the project folder with timestamp", cmd, false);


    /**
     * @brief Check project name and overwrite the existing project
     * @param overwrite
     * @note Optional parameter
     * @example muse_project -N -p /path/to/project/dir -n 00_test --overwrite
     */
    SwitchArg overwrite                 ("", "overwrite", "Overwrite existing project", cmd, false);



    // ---------------------------------------------------------------------------------------------------------
    // PARSING:

    // Parse the argv array.
    cmd.parse(argc, argv);

    // ---------------------------------------------------------------------------------------------------------
    // SETTINGS:

    std::cout << "=== Build date: " << __DATE__ << " " << __TIME__ << "\n";
    std::cout << "=== OS: " << detect_os() << "\n";
    std::cout << "=== Arch: " << detect_arch() << "\n";
    std::cout << "=== Compiler: " << compiler_info() << "\n";
    std::cout << std::endl;


    // 0) Commands
    std::string command;
    filesystem::path abspath = argv[3];
    for(int i=1; i< argc; i++)
    {
        std::string string = argv[i];
        if(string.find(abspath) != std::string::npos)
        {
            //std::cout << "Path: " << argv[i] << std::endl;

            filesystem::path path = argv[i];
            filesystem::path relpath = filesystem::relative(path, abspath);

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
    std::cout << "=== Command line: " << command << std::endl;
    std::cout << "=== Number of command arguments: " << argc << std::endl;
    std::cout << "=== Absolute path: " << abspath << std::endl;
    std::cout << std::endl;

    // ---------------------------------------------------------------------------------------------------------
    // STARTS:

    // Option 0. Project creation and settings
    if(projectCreation.isSet())
    {
        MUSE::Project Project;
        Project.setFolder(projectFolder.getValue());
        Project.setName(Name.getValue());

        std::string path_project = projectFolder.getValue() + "/" + Name.getValue();

        if(std::filesystem::exists(path_project))
        {
            if(overwrite.isSet())
            {
                std::cout << "\033[0;33mOverwriting existing project " << Name.getValue() << "\033[0m\n";
                std::filesystem::remove_all(path_project);
            }
            else if(timestamp.isSet())
            {
                std::string project_name_tmp = Name.getValue() + "_" + timestamp_now();
                std::cout << "\033[0;32mProject <" << project_name_tmp << "> is created.\033[0m" << std::endl;
                path_project = projectFolder.getValue() + "/" + project_name_tmp;
                Project.setName(project_name_tmp);
            }
            else
            {
                std::cout << "\033[0;31mProject <" << Project.name << "> already exists.\033[0m" << std::endl;
                std::cout << "Use --timestamp to create a new folder as <" << Project.name << "_YYYY-MM-DD_HH-MM-SS>" << std::endl;
                std::cout << "Use --overwrite to overwrite the existing project." << std::endl;
                exit(1);
            }
        }

        // bool equal_name = check_folder_name(Project.name, Project.folder);
        // if(equal_name == true)
        // {
        //     std::cout << "\033[0;31mProject <" << Project.name << "> already exists.\033[0m" << std::endl;
        //     std::cout << "Use --timestamp to create a new folder as <" << Project.name << "_YYYY-MM-DD_HH-MM-SS>" << std::endl;
        //     std::cout << "Use --overwrite to overwrite the existing project." << std::endl;
        //     exit(1);
        // }



        // Output folder
        //Project.folder = Project.folder + "/" + Project.name;
        filesystem::create_directory(path_project);

        std::string in_dir = path_project + "/in";
        filesystem::create_directory(in_dir);

        std::string out_dir = path_project + "/out";
        filesystem::create_directory(out_dir);

        std::cout << "\033[0;32mCreation new project " << path_project << " ... COMPLETED.\033[0m" << std::endl;

        if(setEPSG.isSet())
            Project.setAuthority(setEPSG.getValue());

        //Project.write(out_dir + "/"+ Project.name + ".json");
        //std::cout << "\033[0;32mCreation of Project JSON into " << out_dir << " ... COMPLETED.\033[0m" << std::endl;

        MUSE::ProjectMeta::Env project_env;
        project_env.created = std::string(__DATE__) + " " + std::string(__TIME__);
        project_env.arch = detect_arch();
        project_env.os = detect_os();
        project_env.compiler = compiler_info();

        MUSE::ProjectMeta project_json;
        project_json.setProject(Project);
        project_json.setEnv(project_env);
        project_json.write(out_dir + "/"+ Project.name + ".json");
    }

    } catch (ArgException &e)  // catch exceptions
    { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

}
