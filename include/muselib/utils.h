#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>


bool check_folder_name (const std::string filename, std::vector<std::string> dirs);
bool check_filename (const std::string new_name, std::string project_folder);

std::vector<std::string> split_string    (const std::string &str, char delimiter);
std::pair<std::string,std::string> split_string_pair (const std::string &str, char delimiter);

std::string             get_path         (const std::string &complete_path);
std::string             get_filename     (const std::string &path);
std::string             get_basename     (const std::string &name);
std::string             get_extension    (const std::string &name);
std::string             get_extensionND  (const std::string &name); //extension without point
bool                    find_char        (const std::string &string, const char &c);

std::vector<std::string> get_directories (const std::string& project_dir);
std::vector<std::string> get_recursive_directories (const std::string &project_dir);

std::vector<std::string> get_files       (const std::string &project_dir);

int cmpfunc (const void * a, const void * b);
std::vector<std::string> get_files       (const std::string &project_dir, const std::string &ext, bool alphab_sort = false);

std::vector<std::string> get_vectorfiles (const std::string &project_dir);
std::vector<std::string> get_rasterfiles (const std::string &project_dir);

std::vector<std::string> get_shapefiles  (const std::string &project_dir);
std::vector<std::string> get_xyzfiles    (const std::string &project_dir);
std::vector<std::string> get_meshfiles   (const std::string &project_dir);

void cout_list                           (const std::vector<std::string> &list);


#ifndef STATIC_MUSELIB
#include "utils.cpp"
#endif

#endif // UTILS_H
