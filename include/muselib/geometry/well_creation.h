#pragma once

#ifndef MUSELIB_GEOMETRY_WELL_CREATION_H
#define MUSELIB_GEOMETRY_WELL_CREATION_H

#include <string>
#include <vector>

struct CreateWellsConfig {
    std::string input_file = "";
    std::string generate_box = "";
    std::string output_file = "";
    std::string volmesh_format = ".mesh";
    std::vector<std::string> well_strings;
    double target_edge_length = -1.0;
    bool verbose = false;
    bool generate_tet = false;
    double max_tet_volume = -1.0;
    bool save_no_wells = false;
    bool save_only_wells = false;
    std::string tetgen_flags = "pq20";
    bool refine_cylinders = false;
    double cylinder_edge_scale = 1.0;
};

int create_tetmesh_with_wells(const CreateWellsConfig& config);

#ifndef STATIC_MUSELIB
#include "well_creation.cpp"
#endif

#endif // MUSELIB_GEOMETRY_WELL_CREATION_H
