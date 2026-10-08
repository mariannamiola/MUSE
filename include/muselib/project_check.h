#ifndef MUSE_PROJECT_CHECK_H
#define MUSE_PROJECT_CHECK_H

#include <filesystem>
#include <iostream>
#include <string>
#include <cstdlib>

namespace MUSE
{

/// @brief true if folder is a MUSE project directory created by muse_project
/// (it contains in/ and out/<project_name>.json)
inline bool is_project_folder (const std::filesystem::path &folder)
{
    std::error_code ec;
    const std::string name = folder.filename().string();
    return std::filesystem::is_directory(folder / "in", ec) &&
           std::filesystem::exists(folder / "out" / (name + ".json"), ec);
}

/// @brief Stops with a clear error message if --pdir is not a MUSE project directory
inline void check_project_folder (const std::string &pdir)
{
    std::filesystem::path folder = std::filesystem::path(pdir).lexically_normal();
    if(folder.filename().empty()) //trailing "/"
        folder = folder.parent_path();

    if(is_project_folder(folder))
        return;

    std::cerr << "\033[0;31mERROR: " << pdir << " is not a MUSE project directory.\033[0m" << std::endl;

    // Common mistake: -p points to the parent folder used with muse_project
    std::error_code ec;
    if(std::filesystem::is_directory(folder, ec))
        for(const auto &entry : std::filesystem::directory_iterator(folder, ec))
            if(entry.is_directory(ec) && is_project_folder(entry.path()))
                std::cerr << "Did you mean: -p " << entry.path().string() << " ?" << std::endl;

    std::cerr << "Create the project first with: muse_project -N -p <dir> --name <name>" << std::endl;
    std::cerr << "then use -p <dir>/<name> in all the other MUSE commands." << std::endl;
    exit(1);
}

}

#endif // MUSE_PROJECT_CHECK_H
