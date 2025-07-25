#include "utils.h"

#include <algorithm>
#include <stdlib.h>

//for filesystem
#ifdef __APPLE__
    #include <filesystem>
    using namespace std::__fs;
#else
    //#include <experimental/filesystem>
    #include <filesystem>
    using namespace std;
#endif

std::vector<std::string> split_string (const std::string &str, char delimiter)
{
    std::vector<std::string> v;
    std::stringstream ss(str);

    while (ss.good())
    {
        std::string substr;
        getline(ss, substr, delimiter);
        v.push_back(substr);
    }
    return v;
}

std::pair<std::string,std::string> split_string_pair (const std::string &str, char delimiter)
{
    std::pair<std::string,std::string> v;
    if(str.find(delimiter) != std::string::npos)
    {
        v.first = str.substr(0, str.find_last_of(delimiter));
        v.second = str.substr(str.find_last_of(delimiter)+1, str.length());
    }
    return v;
}

std::string get_path (const std::string &complete_path)
{
    std::string path;
    if(complete_path.find("/") != std::string::npos)
        path = complete_path.substr(0, complete_path.find_last_of("/"));
    return path;
}

std::string get_filename (const std::string &path)
{
    std::string filename;
    if(path.find("/") != std::string::npos)
        filename = path.substr(path.find_last_of("/")+1, path.length());
    return filename;
}

std::string get_basename (const std::string &name)
{
    std::string basename;
    if(name.find(".") != std::string::npos)
        basename = name.substr(0, name.find_last_of("."));
    return basename;
}

std::string get_extension (const std::string &name) //extension with point
{
    std::string extension;
    if(name.find(".") != std::string::npos)
        extension = name.substr(name.find_last_of("."), name.length());
    return extension;
}

std::string get_extensionND (const std::string &name) //extension without point
{
    std::string extension;
    if(name.find(".") != std::string::npos)
        extension = name.substr(name.find_last_of(".")+1, name.length());
    return extension;
}

bool find_char (const std::string &string, const char &c)
{
    if(string.find(c) != std::string::npos)
        return true;
    else
        return false;
}

std::vector<std::string> get_directories(const std::string &project_dir)
{
    std::vector<std::string> list;
    for(auto& p : filesystem::directory_iterator(project_dir))
        #ifdef __APPLE__
            if (p.is_directory())
                list.push_back(p.path().string());
        #else
            if (filesystem::is_directory(p))
                list.push_back(p.path().string());
            // {
            //     //std::string path = p.path();
            //     //std::cout << path << std::endl;
            //     //list.push_back(p.path());
            //     //list.push_back(p.path().string());
            //     list.push_back(path);
            // }
        #endif

    return list;
}

std::vector<std::string> get_recursive_directories (const std::string &project_dir)
{
    std::vector<std::string> list;
    for(auto& p : filesystem::recursive_directory_iterator(project_dir))
        #ifdef __APPLE__
        if (p.is_directory())
        #else
        if (filesystem::is_directory(p))
        #endif
            list.push_back(p.path().string());

    return list;
}

std::vector<std::string> get_files (const std::string &project_dir)
{
    std::vector<std::string> list;
    for(auto& p : filesystem::directory_iterator(project_dir))
        list.push_back(p.path());

    return list;
}

std::vector<std::string> get_files (const std::string &project_dir, const std::string &ext, bool alphab_sort)
{
    std::vector<std::string> list;

    for(auto& p : filesystem::directory_iterator(project_dir))
    {
        std::string path = p.path().string();
//        std::string filename = path.substr(path.find_last_of("/")+1, path.length());

//        std::string extname;
//        if(filename.find(".") != std::string::npos)
//            extname = filename.substr(filename.find_last_of("."), filename.length());

//        if(extname.compare(ext) == 0)
//            list.push_back(p.path());

        if(get_extension(path).compare(ext) == 0)
            list.push_back(p.path());
    }
    if(alphab_sort == true)
        std::sort(list.begin(), list.end());

    return list;
}



std::vector<std::string> get_vectorfiles (const std::string &project_dir)
{
    std::vector<std::string> list;
    for(auto& p : filesystem::directory_iterator(project_dir))
    {
        std::string path = p.path().string();
        std::string filename = path.substr(path.find_last_of("/")+1, path.length());

        std::string extname;
        if(filename.find(".") != std::string::npos)
            extname = filename.substr(filename.find_last_of("."), filename.length());

        if(extname.compare(".shp") == 0 || extname.compare(".gpkg") == 0)
            list.push_back(p.path());
    }

    return list;
}

std::vector<std::string> get_rasterfiles (const std::string &project_dir)
{
    std::vector<std::string> list;
    for(auto& p : filesystem::directory_iterator(project_dir))
    {
        std::string path = p.path().string();
        std::string filename = path.substr(path.find_last_of("/")+1, path.length());

        std::string extname;
        if(filename.find(".") != std::string::npos)
            extname = filename.substr(filename.find_last_of("."), filename.length());

        if(extname.compare(".asc") == 0 || extname.compare(".gpkg") == 0)
            list.push_back(p.path());
    }

    return list;
}


std::vector<std::string> get_shapefiles (const std::string &project_dir)
{
    std::vector<std::string> list;
    for(auto& p : filesystem::directory_iterator(project_dir))
    {
        std::string path = p.path().string();
        std::string filename = path.substr(path.find_last_of("/")+1, path.length());

        std::string extname;
        if(filename.find(".") != std::string::npos)
            extname = filename.substr(filename.find_last_of("."), filename.length());

        if(extname.compare(".shp") == 0)
            list.push_back(p.path());
    }

    return list;
}

std::vector<std::string> get_xyzfiles (const std::string &project_dir)
{
    std::vector<std::string> list;
    for(auto& p : filesystem::directory_iterator(project_dir))
    {
        std::string path = p.path().string();
        std::string filename = path.substr(path.find_last_of("/")+1, path.length());

        std::string extname;
        if(filename.find(".") != std::string::npos)
            extname = filename.substr(filename.find_last_of("."), filename.length());

        if(extname.compare(".dat") == 0 || extname.compare(".xyz") == 0 || extname.compare(".txt") == 0 || extname.compare(".csv") == 0)
            list.push_back(p.path());
    }

    return list;
}

std::vector<std::string> get_meshfiles (const std::string &project_dir)
{
    std::vector<std::string> list;
    for(auto& p : filesystem::directory_iterator(project_dir))
    {
        std::string path = p.path().string();
        std::string filename = path.substr(path.find_last_of("/")+1, path.length());

        std::string extname;
        if(filename.find(".") != std::string::npos)
            extname = filename.substr(filename.find_last_of("."), filename.length());

        if(extname.compare(".mesh") == 0 || extname.compare(".off") == 0)
            list.push_back(p.path());
    }

    return list;
}




bool check_folder_name (const std::string new_name, std::string project_folder)
{
    bool equal_name = false;

    //lista cartelle a tutti i livelli
    std::vector<std::string> dirs = get_directories(project_folder);

    for(size_t i=0; i< dirs.size(); i++)
    {
        //std::cout << "Dir: " << dirs.at(i) << std::endl;
        std::string existed_name = dirs.at(i).substr(dirs.at(i).find_last_of("/")+1, dirs.at(i).length());
        std::string basename = existed_name.substr(0, existed_name.find_last_of("."));

        if(new_name.compare(basename) == 0)
        {
            equal_name = true;
            break;
        }
    }
    return equal_name;
}

bool check_filename (const std::string new_name, std::string project_folder)
{
    bool equal_name = false;

    //lista cartelle a tutti i livelli
    std::vector<std::string> dirs = get_files(project_folder);

    for(size_t i=0; i< dirs.size(); i++)
    {
        //std::cout << "Dir: " << dirs.at(i) << std::endl;
        std::string existed_name = dirs.at(i).substr(dirs.at(i).find_last_of("/")+1, dirs.at(i).length());
        //std::string basename = existed_name.substr(0, existed_name.find_last_of("."));

        if(new_name.compare(existed_name) == 0)
        {
            equal_name = true;
            break;
        }
    }
    return equal_name;
}





void cout_list (const std::vector<std::string> &list)
{
    for(size_t i=0; i<list.size(); i++)
    {
        std::cout << "\033[0;32m" << list.at(i) << "\033[0m" <<std::endl;
    }
}


