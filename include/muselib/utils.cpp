#include "utils.h"

#include <algorithm>
#include <cmath>
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


int check_points_to_remove(const std::vector<std::vector<std::string>> &matrix_data,
                           int col_x,
                           int col_y,
                           int col_z,      // -1 se 2D
                           double tol_dist,
                           double z_weight = 1.0)
{
    std::cout << std::endl;
    std::cout << "########################################" << std::endl;
    std::cout << "### DUPLICATE POINTS (TO BE REMOVED) ###" << std::endl;
    std::cout << "########################################" << std::endl;

    size_t n = matrix_data.size();
    bool use_z = (col_z >= 0);

    std::vector<double> x(n), y(n), z(n, 0.0);
    std::vector<bool> to_remove(n, false);

    // Parsing
    for(size_t i = 0; i < n; i++)
    {
        try
        {
            x[i] = std::stod(matrix_data[i][col_x]);
            y[i] = std::stod(matrix_data[i][col_y]);
            if(use_z)
                z[i] = std::stod(matrix_data[i][col_z]);
        }
        catch(...)
        {
            std::cerr << "WARNING: parse error at row " << i << std::endl;
            to_remove[i] = true;
        }
    }

    int n_marked = 0;

    // Logica IDENTICA alla remove
    for(size_t i = 0; i < n; i++)
    {
        if(to_remove[i]) continue;

        for(size_t j = i + 1; j < n; j++)
        {
            if(to_remove[j]) continue;

            double dx = x[i] - x[j];
            double dy = y[i] - y[j];

            double dist;
            if(use_z)
            {
                double dz = (z[i] - z[j]) * z_weight;
                dist = std::sqrt(dx*dx + dy*dy + dz*dz);
            }
            else
            {
                dist = std::sqrt(dx*dx + dy*dy);
            }

            if(dist <= tol_dist)
            {
                to_remove[j] = true;
            }
        }
    }

    // 🔥 STAMPA PUNTI REALMENTE RIMOSSI
    std::cout << "### Points that would be removed:" << std::endl;

    for(size_t i = 0; i < n; i++)
    {
        if(to_remove[i])
        {
            std::cout << "Row " << i << " -> x=" << x[i] << " y=" << y[i];
            if(use_z) std::cout << " z=" << z[i];
            std::cout << std::endl;

            n_marked++;
        }
    }

    std::cout << std::endl;
    std::cout << "### Total points to remove: " << n_marked << std::endl;
    std::cout << "### Points remaining:      " << (n - n_marked) << std::endl;
    std::cout << "########################################" << std::endl;
    std::cout << std::endl;

    return n_marked;
}


int check_duplicate_coords(const std::vector<std::vector<std::string>> &matrix_data,
                           int col_x,
                           int col_y,
                           int col_z,          // <-- -1 se non hai Z
                           double tol_dist,
                           double z_weight = 1.0,
                           bool verbose = true)
{
    std::cout << std::endl;
    std::cout << "########################################" << std::endl;
    std::cout << "#### DUPLICATES CHECK (AUTO 2D/3D) #####" << std::endl;
    std::cout << "########################################" << std::endl;

    size_t n = matrix_data.size();
    bool use_z = (col_z >= 0);

    std::cout << "### Mode: " << (use_z ? "3D" : "2D") << std::endl;
    std::cout << "### Input points: " << n << std::endl;
    std::cout << "### Distance tolerance: " << tol_dist << std::endl;

    std::vector<double> x(n), y(n), z(n, 0.0);
    int n_parse_errors = 0;

    // Parsing coordinate
    for(size_t i = 0; i < n; i++)
    {
        try
        {
            x[i] = std::stod(matrix_data[i][col_x]);
            y[i] = std::stod(matrix_data[i][col_y]);

            if(use_z)
                z[i] = std::stod(matrix_data[i][col_z]);
        }
        catch(...)
        {
            n_parse_errors++;
            if(verbose)
                std::cerr << "WARNING: Cannot parse coordinates at row " << i << std::endl;
        }
    }

    int n_duplicates = 0;

    // Check duplicati
    for(size_t i = 0; i < n; i++)
    {
        for(size_t j = i + 1; j < n; j++)
        {
            double dx = x[i] - x[j];
            double dy = y[i] - y[j];

            double dist;

            if(use_z)
            {
                double dz = (z[i] - z[j]) * z_weight;
                dist = std::sqrt(dx*dx + dy*dy + dz*dz);
            }
            else
            {
                dist = std::sqrt(dx*dx + dy*dy);
            }

            if(dist <= tol_dist)
            {
                n_duplicates++;

                if(verbose)
                {
                    std::cout << "### DUPLICATE FOUND (" << (use_z ? "3D" : "2D") << "):" << std::endl;
                    std::cout << "    Row " << i << " -> x=" << x[i] << " y=" << y[i];
                    if(use_z) std::cout << " z=" << z[i];
                    std::cout << std::endl;

                    std::cout << "    Row " << j << " -> x=" << x[j] << " y=" << y[j];
                    if(use_z) std::cout << " z=" << z[j];
                    std::cout << std::endl;

                    std::cout << "    Distance: " << dist << std::endl;
                }
            }
        }
    }

    std::cout << std::endl;
    std::cout << "### Total duplicates found: " << n_duplicates << std::endl;
    std::cout << "### Parsing errors:         " << n_parse_errors << std::endl;

    if(n_duplicates > 0)
    {
        std::cout << "### WARNING: Duplicate coordinates detected!" << std::endl;
        std::cout << "### Consider cleaning the dataset." << std::endl;
    }
    else
    {
        std::cout << "### No duplicates found." << std::endl;
    }

    std::cout << "########################################" << std::endl;
    std::cout << std::endl;

    return n_duplicates;
}


int remove_duplicate_rows_by_coords(std::vector<std::vector<std::string>> &matrix_data,
                                     int col_x,
                                     int col_y,
                                     int col_z,      // <-- opzionale: usa -1 se assente
                                     double tol_dist,
                                     double z_weight = 1.0)
{
    std::cout << std::endl;
    std::cout << "########################################" << std::endl;
    std::cout << "#### REMOVE DUPLICATES (AUTO 2D/3D) ####" << std::endl;
    std::cout << "########################################" << std::endl;

    size_t n = matrix_data.size();

    std::vector<double> x(n), y(n), z(n, 0.0);
    std::vector<bool> to_remove(n, false);

    bool use_z = (col_z >= 0);

    std::cout << "### Mode: " << (use_z ? "3D" : "2D") << std::endl;
    std::cout << "### Tolerance: " << tol_dist << std::endl;

    // Parsing
    for(size_t i = 0; i < n; i++)
    {
        try
        {
            x[i] = std::stod(matrix_data[i][col_x]);
            y[i] = std::stod(matrix_data[i][col_y]);

            if(use_z)
                z[i] = std::stod(matrix_data[i][col_z]);
        }
        catch(...)
        {
            std::cerr << "WARNING: Cannot parse coordinates at row " << i << std::endl;
            to_remove[i] = true;
        }
    }

    int n_duplicates = 0;

    for(size_t i = 0; i < n; i++)
    {
        if(to_remove[i]) continue;

        for(size_t j = i + 1; j < n; j++)
        {
            if(to_remove[j]) continue;

            double dx = x[i] - x[j];
            double dy = y[i] - y[j];

            double dist;

            if(use_z)
            {
                double dz = (z[i] - z[j]) * z_weight;
                dist = std::sqrt(dx*dx + dy*dy + dz*dz);
            }
            else
            {
                dist = std::sqrt(dx*dx + dy*dy);
            }

            if(dist <= tol_dist)
            {
                n_duplicates++;

                std::cout << "### DUPLICATE FOUND (" << (use_z ? "3D" : "2D") << "): removing row " << j << std::endl;

                to_remove[j] = true;
            }
        }
    }

    // rebuild dataset
    std::vector<std::vector<std::string>> clean_matrix;
    clean_matrix.reserve(n);

    for(size_t i = 0; i < n; i++)
    {
        if(!to_remove[i])
            clean_matrix.push_back(matrix_data[i]);
    }

    matrix_data = clean_matrix;

    std::cout << "### Removed: " << n_duplicates << std::endl;
    std::cout << "### Remaining: " << matrix_data.size() << std::endl;
    std::cout << "########################################" << std::endl;
    std::cout << std::endl;

    return n_duplicates;
}
