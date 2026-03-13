#include "well_creation.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#include <process.h>
#else
#include <unistd.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <cinolib/meshes/tetmesh.h>
#include <cinolib/meshes/trimesh.h>
#include <cinolib/tetgen_wrap.h>
#include <cinolib/winding_number.h>

struct WellSpec {
    double x, y, z;
    double height;
    double radius;
    std::vector<double> z_subdivisions;
};

static int get_process_id()
{
#ifdef _WIN32
    return _getpid();
#else
    return getpid();
#endif
}

static std::string get_cwd_string()
{
#ifdef _WIN32
    char* cwd = _getcwd(nullptr, 0);
#else
    char* cwd = getcwd(nullptr, 0);
#endif
    if (!cwd)
    {
        return std::string();
    }
    std::string result(cwd);
    free(cwd);
    return result;
}

static std::string get_mesh_generator_path()
{
#ifdef VOLUME_MESHER_EXECUTABLE
    return VOLUME_MESHER_EXECUTABLE;
#else
#ifdef _WIN32
    std::string cwd = get_cwd_string();
    if (cwd.empty())
    {
        return std::string();
    }
    return cwd + "/external/VolumeMesher/build/Release/mesh_generator.exe";
#else
    return "./external/VolumeMesher/build/mesh_generator";
#endif
#endif
}

static WellSpec parseWellString(const std::string& well_str)
{
    std::vector<double> values;
    std::stringstream ss(well_str);
    std::string token;

    while (std::getline(ss, token, ','))
    {
        values.push_back(std::stod(token));
    }

    if (values.size() < 5)
    {
        throw std::runtime_error("Well specification must have at least 5 values: x,y,z,height,radius[,z_sub1,z_sub2,...]");
    }

    WellSpec well;
    well.x = values[0];
    well.y = values[1];
    well.z = values[2];
    well.height = values[3];
    well.radius = values[4];

    // z_subdivisions are absolute z coordinates and must lie strictly inside the well z-interval
    double z0 = well.z;
    double z1 = well.z + well.height;
    double z_min = std::min(z0, z1);
    double z_max = std::max(z0, z1);
    for (size_t i = 5; i < values.size(); ++i)
    {
        double z_sub = values[i];
        if (z_sub <= z_min || z_sub >= z_max)
        {
            throw std::runtime_error("Z subdivision must be between " + std::to_string(z_min) + " and " + std::to_string(z_max) + ": " + std::to_string(z_sub));
        }
        well.z_subdivisions.push_back(z_sub);
    }

    std::sort(well.z_subdivisions.begin(), well.z_subdivisions.end());
    return well;
}

static cinolib::Trimesh<> generateBoxMesh(double width, double height, double depth, double target_edge_length = -1.0)
{
    std::vector<cinolib::vec3d> vertices;
    std::vector<std::vector<uint>> faces;

    double hw = width / 2.0;
    double hh = height / 2.0;
    double hd = depth / 2.0;

    if (target_edge_length <= 0)
    {
        target_edge_length = std::min({ width, height, depth }) / 10.0;
    }

    auto calculateSubdivisions = [](double dimension, double target_edge)
    {
        return std::max(1, static_cast<int>(std::ceil(dimension / target_edge)));
    };

    int width_subdivs = calculateSubdivisions(width, target_edge_length);
    int height_subdivs = calculateSubdivisions(height, target_edge_length);
    int depth_subdivs = calculateSubdivisions(depth, target_edge_length);

    auto addFaceGrid = [&](cinolib::vec3d corner, cinolib::vec3d u_vec, cinolib::vec3d v_vec,
                           int u_subdivs, int v_subdivs, bool flip_normal = false)
    {
        uint start_vertex_idx = vertices.size();

        for (int j = 0; j <= v_subdivs; ++j)
        {
            for (int i = 0; i <= u_subdivs; ++i)
            {
                double u = static_cast<double>(i) / u_subdivs;
                double v = static_cast<double>(j) / v_subdivs;
                cinolib::vec3d vertex = corner + u_vec * u + v_vec * v;
                vertices.push_back(vertex);
            }
        }

        for (int j = 0; j < v_subdivs; ++j)
        {
            for (int i = 0; i < u_subdivs; ++i)
            {
                uint v00 = start_vertex_idx + j * (u_subdivs + 1) + i;
                uint v10 = start_vertex_idx + j * (u_subdivs + 1) + i + 1;
                uint v01 = start_vertex_idx + (j + 1) * (u_subdivs + 1) + i;
                uint v11 = start_vertex_idx + (j + 1) * (u_subdivs + 1) + i + 1;

                bool use_main_diagonal = ((i + j) % 2) == 0;

                if (!flip_normal)
                {
                    if (use_main_diagonal)
                    {
                        faces.push_back({ v00, v10, v11 });
                        faces.push_back({ v00, v11, v01 });
                    }
                    else
                    {
                        faces.push_back({ v00, v10, v01 });
                        faces.push_back({ v10, v11, v01 });
                    }
                }
                else
                {
                    if (use_main_diagonal)
                    {
                        faces.push_back({ v00, v11, v10 });
                        faces.push_back({ v00, v01, v11 });
                    }
                    else
                    {
                        faces.push_back({ v00, v01, v10 });
                        faces.push_back({ v10, v01, v11 });
                    }
                }
            }
        }
    };

    addFaceGrid(cinolib::vec3d(-hw, -hh, -hd), cinolib::vec3d(width, 0, 0), cinolib::vec3d(0, height, 0), width_subdivs, height_subdivs, true);
    addFaceGrid(cinolib::vec3d(-hw, -hh, hd), cinolib::vec3d(width, 0, 0), cinolib::vec3d(0, height, 0), width_subdivs, height_subdivs, false);
    addFaceGrid(cinolib::vec3d(-hw, -hh, -hd), cinolib::vec3d(width, 0, 0), cinolib::vec3d(0, 0, depth), width_subdivs, depth_subdivs, false);
    addFaceGrid(cinolib::vec3d(-hw, hh, -hd), cinolib::vec3d(width, 0, 0), cinolib::vec3d(0, 0, depth), width_subdivs, depth_subdivs, true);
    addFaceGrid(cinolib::vec3d(-hw, -hh, -hd), cinolib::vec3d(0, height, 0), cinolib::vec3d(0, 0, depth), height_subdivs, depth_subdivs, true);
    addFaceGrid(cinolib::vec3d(hw, -hh, -hd), cinolib::vec3d(0, height, 0), cinolib::vec3d(0, 0, depth), height_subdivs, depth_subdivs, false);

    return cinolib::Trimesh<>(vertices, faces);
}

static double calculateAverageEdgeLength(const cinolib::Trimesh<>& mesh)
{
    double total_length = 0.0;
    int edge_count = 0;

    for (uint eid = 0; eid < mesh.num_edges(); ++eid)
    {
        cinolib::vec3d v0 = mesh.vert(mesh.edge_vert_id(eid, 0));
        cinolib::vec3d v1 = mesh.vert(mesh.edge_vert_id(eid, 1));

        double length = (v1 - v0).norm();
        total_length += length;
        edge_count++;
    }

    return edge_count > 0 ? total_length / edge_count : 1.0;
}

static cinolib::Trimesh<> generateCylinderMesh(const WellSpec& well, double target_edge_length, bool verbose = false)
{
    double circumference = 2 * M_PI * well.radius;
    int radial_segments = std::max(6, static_cast<int>(std::ceil(circumference / target_edge_length)));

    if (verbose)
    {
        std::cout << "  - Radius: " << well.radius << ", Height: " << well.height << std::endl;
        std::cout << "  - Using " << radial_segments << " radial segments" << std::endl;
        if (!well.z_subdivisions.empty())
        {
            std::cout << "  - Internal subdivisions at z: [";
            for (size_t i = 0; i < well.z_subdivisions.size(); ++i)
            {
                if (i > 0)
                    std::cout << ", ";
                std::cout << well.z_subdivisions[i];
            }
            std::cout << "]" << std::endl;
        }
    }

    std::vector<cinolib::vec3d> vertices;
    std::vector<std::vector<uint>> faces;

    // z_subdivisions are absolute z coordinates for internal critical levels

    std::vector<double> critical_z_levels;
    critical_z_levels.push_back(well.z);
    for (double z_sub : well.z_subdivisions)
    {
        critical_z_levels.push_back(z_sub);
    }
    critical_z_levels.push_back(well.z + well.height);
    std::sort(critical_z_levels.begin(), critical_z_levels.end());

    std::vector<double> z_levels;
    for (size_t i = 0; i < critical_z_levels.size() - 1; ++i)
    {
        double z_start = critical_z_levels[i];
        double z_end = critical_z_levels[i + 1];
        double span = z_end - z_start;

        int num_segments = std::max(1, static_cast<int>(std::ceil(span / target_edge_length)));

        z_levels.push_back(z_start);
        for (int j = 1; j < num_segments; ++j)
        {
            double t = static_cast<double>(j) / num_segments;
            z_levels.push_back(z_start + t * span);
        }
    }
    z_levels.push_back(critical_z_levels.back());

    if (verbose)
    {
        std::cout << "  - Total z-levels: " << z_levels.size() << " (height segments: " << (z_levels.size() - 1) << ")" << std::endl;
    }

    std::map<std::pair<int, int>, uint> ring_vertex_map;
    const double eps = 1e-9;
    double z_min = std::min(well.z, well.z + well.height);
    double z_max = std::max(well.z, well.z + well.height);

    for (size_t level = 0; level < z_levels.size(); ++level)
    {
        double z = z_levels[level];
        for (int i = 0; i < radial_segments; ++i)
        {
            double angle = 2.0 * M_PI * i / radial_segments;
            double x = well.x + well.radius * std::cos(angle);
            double y = well.y + well.radius * std::sin(angle);

            ring_vertex_map[{ static_cast<int>(level), i }] = vertices.size();
            vertices.push_back(cinolib::vec3d(x, y, z));
        }
    }

    for (size_t level = 0; level < z_levels.size() - 1; ++level)
    {
        for (int i = 0; i < radial_segments; ++i)
        {
            int next_i = (i + 1) % radial_segments;

            uint v0 = ring_vertex_map[{ static_cast<int>(level), i }];
            uint v1 = ring_vertex_map[{ static_cast<int>(level), next_i }];
            uint v2 = ring_vertex_map[{ static_cast<int>(level + 1), i }];
            uint v3 = ring_vertex_map[{ static_cast<int>(level + 1), next_i }];

            faces.push_back({ v0, v1, v2 });
            faces.push_back({ v1, v3, v2 });
        }
    }

    for (size_t level = 0; level < z_levels.size(); ++level)
    {
        double z = z_levels[level];
        bool is_end_cap = (std::abs(z - z_min) < eps) || (std::abs(z - z_max) < eps);
        if (!is_end_cap)
        {
            continue;
        }

        uint center_idx = vertices.size();
        vertices.push_back(cinolib::vec3d(well.x, well.y, z));

        bool is_bottom = (std::abs(z - z_min) < eps);

        for (int i = 0; i < radial_segments; ++i)
        {
            int next_i = (i + 1) % radial_segments;
            uint v0 = ring_vertex_map[{ static_cast<int>(level), i }];
            uint v1 = ring_vertex_map[{ static_cast<int>(level), next_i }];

            if (is_bottom)
            {
                faces.push_back({ center_idx, v1, v0 });
            }
            else
            {
                faces.push_back({ center_idx, v0, v1 });
            }
        }
    }

    if (verbose)
    {
        std::cout << "  - Generated: " << vertices.size() << " vertices, " << faces.size() << " faces" << std::endl;
    }

    return cinolib::Trimesh<>(vertices, faces);
}

static cinolib::Trimesh<> mergeMeshes(const cinolib::Trimesh<>& base_mesh, const std::vector<cinolib::Trimesh<>>& additional_meshes)
{
    std::vector<cinolib::vec3d> all_vertices;
    std::vector<std::vector<uint>> all_faces;

    for (uint vid = 0; vid < base_mesh.num_verts(); ++vid)
    {
        all_vertices.push_back(base_mesh.vert(vid));
    }

    for (uint fid = 0; fid < base_mesh.num_polys(); ++fid)
    {
        std::vector<uint> face_verts;
        for (uint i = 0; i < base_mesh.verts_per_poly(fid); ++i)
        {
            face_verts.push_back(base_mesh.poly_vert_id(fid, i));
        }
        all_faces.push_back(face_verts);
    }

    for (const auto& mesh : additional_meshes)
    {
        uint vertex_offset = all_vertices.size();

        for (uint vid = 0; vid < mesh.num_verts(); ++vid)
        {
            all_vertices.push_back(mesh.vert(vid));
        }

        for (uint fid = 0; fid < mesh.num_polys(); ++fid)
        {
            std::vector<uint> face_verts;
            for (uint i = 0; i < mesh.verts_per_poly(fid); ++i)
            {
                face_verts.push_back(mesh.poly_vert_id(fid, i) + vertex_offset);
            }
            all_faces.push_back(face_verts);
        }
    }

    return cinolib::Trimesh<>(all_vertices, all_faces);
}

static bool saveMeshToOFF(const cinolib::Trimesh<>& mesh, const std::string& filename)
{
    try
    {
        const_cast<cinolib::Trimesh<>&>(mesh).save(filename.c_str());
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

static bool isPointInsideCylinderWindingNumber(const cinolib::vec3d& point, const cinolib::Trimesh<>& cylinder_mesh)
{
    int wn = cinolib::winding_number(cylinder_mesh, point);
    return wn > 0;
}

static int getWellRegion(const cinolib::vec3d& point, const WellSpec& well)
{
    const double eps = 1e-9;
    double z_min = std::min(well.z, well.z + well.height);
    double z_max = std::max(well.z, well.z + well.height);
    if (point.z() < z_min - eps || point.z() > z_max + eps)
    {
        return -1;
    }

    double point_z = point.z();

    for (size_t i = 0; i < well.z_subdivisions.size(); ++i)
    {
        double z_level = well.z_subdivisions[i];
        if (point_z < z_level)
        {
            return static_cast<int>(i);
        }
    }

    return static_cast<int>(well.z_subdivisions.size());
}

static bool saveTetWellIdField(const cinolib::Tetmesh<>& tet_mesh,
                               const std::vector<cinolib::Trimesh<>>& cylinder_meshes,
                               const std::vector<WellSpec>& wells,
                               const std::string& filename,
                               bool verbose)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    int outside_count = 0;
    std::vector<int> inside_counts(cylinder_meshes.size(), 0);
    std::vector<std::vector<int>> region_counts(wells.size());
    for (size_t i = 0; i < wells.size(); ++i)
    {
        region_counts[i].resize(wells[i].z_subdivisions.size() + 1, 0);
    }

    for (uint pid = 0; pid < tet_mesh.num_polys(); ++pid)
    {
        std::vector<uint> tet_verts = tet_mesh.poly_verts_id(pid);
        cinolib::vec3d centroid(0, 0, 0);
        for (uint vid : tet_verts)
        {
            centroid += tet_mesh.vert(vid);
        }
        centroid /= static_cast<double>(tet_verts.size());

        int well_id = -1;
        for (size_t i = 0; i < cylinder_meshes.size(); ++i)
        {
            if (isPointInsideCylinderWindingNumber(centroid, cylinder_meshes[i]))
            {
                well_id = static_cast<int>(i) + 1;
                inside_counts[i]++;
                int region_id = getWellRegion(centroid, wells[i]);
                if (region_id >= 0)
                {
                    region_counts[i][region_id]++;
                }
                break;
            }
        }

        if (well_id == -1)
        {
            outside_count++;
        }

        file << well_id << "\n";
    }

    file.close();

    if (verbose)
    {
        std::cout << "Tet well id field saved to: " << filename << std::endl;
        std::cout << "  - Outside tets: " << outside_count << std::endl;
        for (size_t i = 0; i < inside_counts.size(); ++i)
        {
            std::cout << "  - Well " << (i + 1) << " tets: " << inside_counts[i];
            if (!wells[i].z_subdivisions.empty())
            {
                std::cout << " (";
                for (size_t r = 0; r < region_counts[i].size(); ++r)
                {
                    if (r > 0)
                        std::cout << ", ";
                    std::cout << "region " << r << ": " << region_counts[i][r];
                }
                std::cout << ")";
            }
            std::cout << std::endl;
        }
    }

    return true;
}

static bool saveTetRegionIdField(const cinolib::Tetmesh<>& tet_mesh,
                                 const std::vector<cinolib::Trimesh<>>& cylinder_meshes,
                                 const std::vector<WellSpec>& wells,
                                 const std::string& filename,
                                 bool verbose)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    for (uint pid = 0; pid < tet_mesh.num_polys(); ++pid)
    {
        std::vector<uint> tet_verts = tet_mesh.poly_verts_id(pid);
        cinolib::vec3d centroid(0, 0, 0);
        for (uint vid : tet_verts)
        {
            centroid += tet_mesh.vert(vid);
        }
        centroid /= static_cast<double>(tet_verts.size());

        int well_id = -1;
        int region_id = -1;

        for (size_t i = 0; i < cylinder_meshes.size(); ++i)
        {
            if (isPointInsideCylinderWindingNumber(centroid, cylinder_meshes[i]))
            {
                well_id = static_cast<int>(i) + 1;
                region_id = getWellRegion(centroid, wells[i]);
                break;
            }
        }

        file << well_id << " " << region_id << "\n";
    }

    file.close();

    if (verbose)
    {
        std::cout << "Tet region id field saved to: " << filename << std::endl;
        std::cout << "  Format: well_id region_id (e.g., \"1 0\" = Well 1 Region 0, \"2 1\" = Well 2 Region 1)" << std::endl;
    }

    return true;
}

static void removeTetsInsideWells(cinolib::Tetmesh<>& tet_mesh,
                                  const std::vector<cinolib::Trimesh<>>& cylinder_meshes,
                                  bool verbose)
{
    if (verbose)
    {
        std::cout << "Removing tetrahedra inside wells..." << std::endl;
    }

    std::vector<uint> removed_tets;

    for (uint pid = 0; pid < tet_mesh.num_polys(); ++pid)
    {
        std::vector<uint> tet_verts = tet_mesh.poly_verts_id(pid);
        cinolib::vec3d centroid(0, 0, 0);
        for (uint vid : tet_verts)
        {
            centroid += tet_mesh.vert(vid);
        }
        centroid /= tet_verts.size();

        bool tet_inside_well = false;
        for (size_t i = 0; i < cylinder_meshes.size(); ++i)
        {
            if (isPointInsideCylinderWindingNumber(centroid, cylinder_meshes[i]))
            {
                tet_inside_well = true;
                break;
            }
        }

        if (tet_inside_well)
        {
            removed_tets.push_back(pid);
        }
    }

    if (verbose)
    {
        std::cout << "Removing " << removed_tets.size() << " tetrahedra inside wells" << std::endl;
    }

    for (auto it = removed_tets.rbegin(); it != removed_tets.rend(); ++it)
    {
        tet_mesh.poly_remove(*it);
    }

    if (verbose)
    {
        std::cout << "No-wells mesh: " << tet_mesh.num_verts() << " vertices, " << tet_mesh.num_polys() << " tetrahedra" << std::endl;
    }
}

static void removeTetsOutsideWells(cinolib::Tetmesh<>& tet_mesh,
                                   const std::vector<cinolib::Trimesh<>>& cylinder_meshes,
                                   bool verbose)
{
    if (verbose)
    {
        std::cout << "Removing tetrahedra outside wells (label -1)..." << std::endl;
    }

    std::vector<uint> removed_tets;

    for (uint pid = 0; pid < tet_mesh.num_polys(); ++pid)
    {
        std::vector<uint> tet_verts = tet_mesh.poly_verts_id(pid);
        cinolib::vec3d centroid(0, 0, 0);
        for (uint vid : tet_verts)
        {
            centroid += tet_mesh.vert(vid);
        }
        centroid /= tet_verts.size();

        bool tet_inside_well = false;
        for (size_t i = 0; i < cylinder_meshes.size(); ++i)
        {
            if (isPointInsideCylinderWindingNumber(centroid, cylinder_meshes[i]))
            {
                tet_inside_well = true;
                break;
            }
        }

        if (!tet_inside_well)
        {
            removed_tets.push_back(pid);
        }
    }

    if (verbose)
    {
        std::cout << "Removing " << removed_tets.size() << " tetrahedra outside wells" << std::endl;
    }

    for (auto it = removed_tets.rbegin(); it != removed_tets.rend(); ++it)
    {
        tet_mesh.poly_remove(*it);
    }

    if (verbose)
    {
        std::cout << "Wells-only mesh: " << tet_mesh.num_verts() << " vertices, " << tet_mesh.num_polys() << " tetrahedra" << std::endl;
    }
}

int create_tetmesh_with_wells(const CreateWellsConfig& config)
{
    std::string input_file = config.input_file;
    std::string generate_box = config.generate_box;
    std::string output_file = config.output_file;
    std::string volmesh_format = config.volmesh_format;
    std::vector<std::string> well_strings = config.well_strings;
    double target_edge_length = config.target_edge_length;
    bool verbose = config.verbose;
    bool generate_tet = config.generate_tet;
    bool save_no_wells = config.save_no_wells;
    bool save_only_wells = config.save_only_wells;
    std::string tetgen_flags = config.tetgen_flags;
    bool refine_cylinders = config.refine_cylinders;
    double cylinder_edge_scale = config.cylinder_edge_scale;

    if (cylinder_edge_scale <= 0.0)
    {
        std::cerr << "Error: --cylinder-edge-scale must be > 0" << std::endl;
        return 1;
    }

    if (input_file.empty() && generate_box.empty())
    {
        std::cerr << "Error: Either input file (-i) or generate-box (--generate-box) must be specified." << std::endl;
        return 1;
    }

    if (verbose)
    {
        std::cout << "Input: " << (input_file.empty() ? "[generated box]" : input_file) << std::endl;
        std::cout << "Output: " << output_file << std::endl;
        std::cout << "Wells: " << well_strings.size() << std::endl;
    }

    cinolib::Trimesh<> input_surface;
    if (!generate_box.empty())
    {
        std::vector<double> dims;
        std::stringstream ss(generate_box);
        std::string token;
        while (std::getline(ss, token, ','))
        {
            dims.push_back(std::stod(token));
        }
        if (dims.size() != 3)
        {
            std::cerr << "Error: Box needs width,height,depth" << std::endl;
            return 1;
        }

        double box_target_edge_length = target_edge_length;
        if (box_target_edge_length <= 0)
        {
            box_target_edge_length = std::min({ dims[0], dims[1], dims[2] }) / 10.0;
            if (!well_strings.empty())
            {
                double min_radius = std::numeric_limits<double>::max();
                for (const auto& well_str : well_strings)
                {
                    try
                    {
                        WellSpec temp_well = parseWellString(well_str);
                        min_radius = std::min(min_radius, temp_well.radius);
                    }
                    catch (...)
                    {
                    }
                }
                if (min_radius < std::numeric_limits<double>::max())
                {
                    double cylinder_target = (2 * M_PI * min_radius) / 16.0;
                    box_target_edge_length = std::min(box_target_edge_length, cylinder_target * 1.5);
                }
            }
        }

        input_surface = generateBoxMesh(dims[0], dims[1], dims[2], box_target_edge_length);
        if (verbose)
        {
            std::cout << "Generated box: " << dims[0] << "x" << dims[1] << "x" << dims[2] << " (" << input_surface.num_verts() << " vertices, " << input_surface.num_polys() << " faces)" << std::endl;
        }

        std::string box_filename = "box_mesh.off";
        if (saveMeshToOFF(input_surface, box_filename) && verbose)
        {
            std::cout << "Box mesh saved to: " << box_filename << std::endl;
        }
    }
    else
    {
        try
        {
            input_surface.load(input_file.c_str());
            if (verbose)
            {
                std::cout << "Loaded mesh: " << input_surface.num_verts() << " vertices, " << input_surface.num_polys() << " faces" << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error loading mesh: " << e.what() << std::endl;
            return 1;
        }
    }

    if (target_edge_length <= 0)
    {
        target_edge_length = calculateAverageEdgeLength(input_surface);
        if (verbose)
        {
            std::cout << "Calculated target edge length: " << target_edge_length << std::endl;
        }
    }

    std::vector<WellSpec> wells;
    for (size_t i = 0; i < well_strings.size(); ++i)
    {
        try
        {
            WellSpec well = parseWellString(well_strings[i]);
            wells.push_back(well);

            if (verbose)
            {
                std::cout << "Well " << (i + 1) << ": position=(" << well.x << "," << well.y << "," << well.z << "), height=" << well.height << ", radius=" << well.radius;
                if (!well.z_subdivisions.empty())
                {
                    std::cout << ", z_subdivisions=[";
                    for (size_t j = 0; j < well.z_subdivisions.size(); ++j)
                    {
                        if (j > 0)
                            std::cout << ",";
                        std::cout << well.z_subdivisions[j];
                    }
                    std::cout << "]";
                }
                std::cout << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing well " << (i + 1) << ": " << e.what() << std::endl;
            return 1;
        }
    }

    std::vector<cinolib::Trimesh<>> cylinder_meshes;
    for (size_t i = 0; i < wells.size(); ++i)
    {
        if (verbose)
        {
            std::cout << "Generating cylinder mesh for well " << (i + 1) << "..." << std::endl;
        }

        double cylinder_edge_length = target_edge_length;
        if (refine_cylinders)
        {
            cylinder_edge_length *= 0.5;
        }
        cylinder_edge_length *= cylinder_edge_scale;

        cinolib::Trimesh<> cylinder = generateCylinderMesh(wells[i], cylinder_edge_length, verbose);
        cylinder_meshes.push_back(cylinder);

        std::string cylinder_filename = "cylinder_" + std::to_string(i + 1) + ".off";
        if (saveMeshToOFF(cylinder, cylinder_filename) && verbose)
        {
            std::cout << "Cylinder " << (i + 1) << " mesh saved to: " << cylinder_filename << std::endl;
        }
    }

    cinolib::Trimesh<> all_cylinders_mesh;
    if (cylinder_meshes.size() == 1)
    {
        all_cylinders_mesh = cylinder_meshes[0];
    }
    else
    {
        all_cylinders_mesh = mergeMeshes(cylinder_meshes[0], std::vector<cinolib::Trimesh<>>(cylinder_meshes.begin() + 1, cylinder_meshes.end()));
    }

    std::string cylinders_filename = "cylinders.off";
    if (saveMeshToOFF(all_cylinders_mesh, cylinders_filename))
    {
        if (verbose)
        {
            std::cout << "Combined cylinders mesh saved to: " << cylinders_filename << std::endl;
            std::cout << "Cylinders mesh stats: " << all_cylinders_mesh.num_verts() << " vertices, " << all_cylinders_mesh.num_polys() << " faces" << std::endl;
        }
    }
    else
    {
        std::cerr << "Error: Failed to save cylinders mesh" << std::endl;
        return 1;
    }

    cinolib::Trimesh<> result_mesh = mergeMeshes(input_surface, cylinder_meshes);
    if (saveMeshToOFF(result_mesh, output_file) && verbose)
    {
        std::cout << "Combined mesh saved to: " << output_file << std::endl;
    }

    if (generate_tet)
    {
        if (verbose)
        {
            std::cout << "Generating tetrahedral mesh using VolumeMesher boolean union + TetGen..." << std::endl;
        }

        std::string box_file = input_file.empty() ? "box_mesh.off" : input_file;

        // if it is a .obj, convert in.off since VolumeMesher doesn't support .obj input
        if (box_file.substr(box_file.find_last_of('.')) == ".obj")
        {
            cinolib::Trimesh<> input_for_meshing (box_file.c_str());
            box_file += ".off";

            input_for_meshing.save(box_file.c_str());
        }

        std::string cylinders_file = "cylinders.off";

        std::string mesh_gen_path = get_mesh_generator_path();
        if (mesh_gen_path.empty())
        {
            std::cerr << "Error: Failed to resolve VolumeMesher executable path" << std::endl;
            return 1;
        }

        std::string cmd = mesh_gen_path + " -b " + box_file + " U " + cylinders_file;
        if (verbose)
        {
            std::cout << "Running: " << cmd << std::endl;
        }

        int result = system(cmd.c_str());
        if (result != 0)
        {
            std::cerr << "Error: VolumeMesher failed with exit code: " << result << std::endl;
            return 1;
        }

        cinolib::Trimesh<> merged_surface;
        try
        {
            std::string black_faces_file = "black_faces.off";
            if (std::ifstream(black_faces_file).good())
            {
                merged_surface.load(black_faces_file.c_str());
                if (verbose)
                {
                    std::cout << "Loaded merged surface from black_faces.off: " << merged_surface.num_verts() << " vertices, " << merged_surface.num_polys() << " faces" << std::endl;
                }
            }
            else
            {
                throw std::runtime_error("black_faces.off was not created by VolumeMesher");
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error loading merged surface: " << e.what() << std::endl;
            return 1;
        }

        cinolib::Tetmesh<> tet_mesh;
        try
        {
            if (verbose)
            {
                std::cout << "Running TetGen tetrahedral meshing with flags: " << tetgen_flags << std::endl;
            }

            cinolib::tetgen_wrap(merged_surface, tetgen_flags, tet_mesh);

            if (verbose)
            {
                std::cout << "Generated " << tet_mesh.num_verts() << " vertices, " << tet_mesh.num_polys() << " tetrahedra" << std::endl;
            }

            std::string tet_filename = output_file.substr(0, output_file.find_last_of('.')) + volmesh_format; //".mesh";

            try
            {
                tet_mesh.save(tet_filename.c_str());
                if (verbose)
                {
                    std::cout << "Tetrahedral mesh saved to: " << tet_filename << std::endl;
                    std::cout << "Tet mesh stats: " << tet_mesh.num_verts() << " vertices, " << tet_mesh.num_polys() << " tetrahedra" << std::endl;
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error: Failed to save tetrahedral mesh: " << e.what() << std::endl;
            }
            
            std::string region_id_filename = output_file.substr(0, output_file.find_last_of('.')) + "_region_id.txt";
            if (!saveTetRegionIdField(tet_mesh, cylinder_meshes, wells, region_id_filename, verbose))
            {
                std::cerr << "Warning: Failed to save tet region id field" << std::endl;
            }
            
            if (save_no_wells)
            {
                try
                {
                    cinolib::Tetmesh<> no_wells_mesh = tet_mesh;
                    removeTetsInsideWells(no_wells_mesh, cylinder_meshes, verbose);

                    std::string no_wells_filename = output_file.substr(0, output_file.find_last_of('.')) + "_no_wells" + volmesh_format;
                    no_wells_mesh.save(no_wells_filename.c_str());

                    if (verbose)
                    {
                        std::cout << "No-wells mesh saved to: " << no_wells_filename << std::endl;
                        std::cout << "No-wells mesh stats: " << no_wells_mesh.num_verts() << " vertices, " << no_wells_mesh.num_polys() << " tetrahedra" << std::endl;
                    }
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Warning: Failed to save no-wells mesh: " << e.what() << std::endl;
                }
            }

            if (save_only_wells)
            {
                try
                {
                    cinolib::Tetmesh<> only_wells_mesh = tet_mesh;
                    removeTetsOutsideWells(only_wells_mesh, cylinder_meshes, verbose);

                    std::string only_wells_filename = output_file.substr(0, output_file.find_last_of('.')) + "_only_wells" + volmesh_format;
                    only_wells_mesh.save(only_wells_filename.c_str());

                    if (verbose)
                    {
                        std::cout << "Wells-only mesh saved to: " << only_wells_filename << std::endl;
                        std::cout << "Wells-only mesh stats: " << only_wells_mesh.num_verts() << " vertices, " << only_wells_mesh.num_polys() << " tetrahedra" << std::endl;
                    }
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Warning: Failed to save wells-only mesh: " << e.what() << std::endl;
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error generating tetrahedral mesh: " << e.what() << std::endl;
            return 1;
        }
    }

    std::cout << "Successfully generated mesh with " << wells.size() << " cylindrical wells";
    if (generate_tet)
    {
        std::cout << " (including tetrahedral mesh)";
    }
    std::cout << "." << std::endl;

    std::cout << "\nSaved mesh files:" << std::endl;
    if (!generate_box.empty())
    {
        std::cout << "  - Box mesh: box_mesh.off" << std::endl;
    }
    for (size_t i = 0; i < wells.size(); ++i)
    {
        std::cout << "  - Cylinder " << (i + 1) << ": cylinder_" << (i + 1) << ".off" << std::endl;
    }
    std::cout << "  - Combined mesh: " << output_file << std::endl;
    std::cout << "  - Combined cylinders: cylinders.off" << std::endl;

    return 0;
}
