#include "well_creation.h"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <set>
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
#include <cinolib/merge_meshes_at_coincident_vertices.h>
#include <cinolib/octree.h>
#include <cinolib/tetgen_wrap.h>
#include <cinolib/triangle_wrap.h>
#include <cinolib/winding_number.h>

struct WellSpec {
    double x, y, z;
    double height;
    double radius;
    double half_size_x = 0.0;
    double half_size_y = 0.0;
    bool project_bottom_to_surface = false;
    std::vector<double> z_subdivisions;
    enum class Shape {
        Cylinder,
        Box
    } shape = Shape::Cylinder;
};

struct WellParseDefaults {
    double height = 0.0;
};

enum class WellFormat {
    XYZHR,
    XYHR,
    XYR
};

enum class SurfaceSheet {
    Top,
    Bottom
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

static std::vector<double> parseWellValues(const std::string& well_str)
{
    std::vector<double> values;
    std::stringstream ss(well_str);
    std::string token;

    while (std::getline(ss, token, ','))
    {
        values.push_back(std::stod(token));
    }

    return values;
}

static std::string normalizeWellFormatString(std::string format)
{
    std::transform(format.begin(), format.end(), format.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return format;
}

struct ParsedWellSpec {
    WellSpec::Shape shape;
    WellFormat format;
    std::string values;
};

static WellFormat parseWellFormat(const std::string& format_str)
{
    std::string format = normalizeWellFormatString(format_str);

    if (format == "XYZHR")
    {
        return WellFormat::XYZHR;
    }
    if (format == "XYHR")
    {
        return WellFormat::XYHR;
    }
    if (format == "XYR")
    {
        return WellFormat::XYR;
    }

    throw std::runtime_error("Unsupported well format: " + format_str + ". Supported formats are XYZHR, XYHR, XYR");
}

static ParsedWellSpec parseWellSpecifier(const std::string& well_str)
{
    const size_t first_separator = well_str.find(':');
    if (first_separator == std::string::npos)
    {
        throw std::runtime_error("Well specification must start with CYL:FORMAT:... or BOX:FORMAT:..., for example CYL:XYZHR:10,20,0,50,2 or BOX:XYHR:10,20,50,4,6");
    }

    const std::string first_token = normalizeWellFormatString(well_str.substr(0, first_separator));
    const std::string remainder = well_str.substr(first_separator + 1);

    if (first_token == "CYL" || first_token == "BOX")
    {
        const size_t second_separator = remainder.find(':');
        if (second_separator == std::string::npos)
        {
            throw std::runtime_error("Well specification must use SHAPE:FORMAT:values, for example CYL:XYHR:10,20,50,2 or BOX:XYR:10,20,4,6");
        }

        ParsedWellSpec parsed;
        parsed.shape = (first_token == "CYL") ? WellSpec::Shape::Cylinder : WellSpec::Shape::Box;
        parsed.format = parseWellFormat(remainder.substr(0, second_separator));
        parsed.values = remainder.substr(second_separator + 1);
        return parsed;
    }

    ParsedWellSpec parsed;
    parsed.shape = WellSpec::Shape::Cylinder;
    parsed.format = parseWellFormat(first_token);
    parsed.values = remainder;
    return parsed;
}

static double parseWellRadius(const std::string& well_str)
{
    const ParsedWellSpec parsed = parseWellSpecifier(well_str);
    std::vector<double> values = parseWellValues(parsed.values);

    if (parsed.shape == WellSpec::Shape::Cylinder)
    {
        switch (parsed.format)
        {
            case WellFormat::XYZHR:
            {
                if (values.size() < 5)
                {
                    throw std::runtime_error("Cylinder XYZHR must have at least 5 values: x,y,z,height,radius[,z_sub1,z_sub2,...]");
                }
                return values[4];
            }
            case WellFormat::XYHR:
            {
                if (values.size() < 4)
                {
                    throw std::runtime_error("Cylinder XYHR must have at least 4 values: x,y,height,radius[,z_sub1,z_sub2,...]");
                }
                return values[3];
            }
            case WellFormat::XYR:
            {
                if (values.size() != 3)
                {
                    throw std::runtime_error("Cylinder XYR must have exactly 3 values: x,y,radius");
                }
                return values[2];
            }
        }
    }

    switch (parsed.format)
    {
        case WellFormat::XYZHR:
            if (values.size() < 6)
            {
                throw std::runtime_error("Box XYZHR must have at least 6 values: x,y,z,height,diag_x,diag_y[,z_sub1,z_sub2,...]");
            }
            return 0.5 * std::min(std::abs(values[4]), std::abs(values[5]));
        case WellFormat::XYHR:
            if (values.size() < 5)
            {
                throw std::runtime_error("Box XYHR must have at least 5 values: x,y,height,diag_x,diag_y[,z_sub1,z_sub2,...]");
            }
            return 0.5 * std::min(std::abs(values[3]), std::abs(values[4]));
        case WellFormat::XYR:
            if (values.size() != 4)
            {
                throw std::runtime_error("Box XYR must have exactly 4 values: x,y,diag_x,diag_y");
            }
            return 0.5 * std::min(std::abs(values[2]), std::abs(values[3]));
    }
    throw std::runtime_error("Unsupported well format");
}

static cinolib::Octree buildSurfaceOctree(const cinolib::Trimesh<>& surface)
{
    cinolib::Octree octree;
    for (uint fid = 0; fid < surface.num_polys(); ++fid)
    {
        octree.push_triangle(fid, surface.poly_vert(fid, 0), surface.poly_vert(fid, 1), surface.poly_vert(fid, 2));
    }
    octree.build();
    return octree;
}

static double projectWellZToSurface(const cinolib::Trimesh<>& surface, const cinolib::Octree& octree, const double x, const double y)
{
    const double bbox_height = surface.bbox().delta_z();
    const double z_offset = std::max(1.0, bbox_height * 0.01);
    const cinolib::vec3d ray_origin(x, y, surface.bbox().max.z() + z_offset);
    const cinolib::vec3d ray_dir(0, 0, -1);

    double hit_t = 0.0;
    uint hit_id = 0;
    if (!octree.intersects_ray(ray_origin, ray_dir, hit_t, hit_id))
    {
        throw std::runtime_error("Could not project well at x=" + std::to_string(x) + ", y=" + std::to_string(y) + " onto the input surface");
    }

    (void)hit_id;
    return ray_origin.z() - hit_t;
}

static double projectPointZToTopSurface(const cinolib::Trimesh<>& surface, const cinolib::Octree& octree, const double x, const double y)
{
    return projectWellZToSurface(surface, octree, x, y);
}

static std::pair<double, double> projectWellSpanToSurface(const cinolib::Trimesh<>& surface, const cinolib::Octree& octree, const double x, const double y)
{
    const double bbox_height = surface.bbox().delta_z();
    const double z_offset = std::max(1.0, bbox_height * 0.01);
    const cinolib::vec3d ray_origin(x, y, surface.bbox().max.z() + z_offset);
    const cinolib::vec3d ray_dir(0, 0, -1);

    std::set<std::pair<double, uint>> intersections;
    if (!octree.intersects_ray(ray_origin, ray_dir, intersections) || intersections.empty())
    {
        throw std::runtime_error("Could not project well at x=" + std::to_string(x) + ", y=" + std::to_string(y) + " onto the input surface");
    }

    const double z_first = ray_origin.z() - intersections.begin()->first;
    const double z_last = ray_origin.z() - intersections.rbegin()->first;
    if (std::abs(z_first - z_last) <= std::numeric_limits<double>::epsilon())
    {
        throw std::runtime_error("Could not derive a vertical span for well at x=" + std::to_string(x) + ", y=" + std::to_string(y) + " from the input surface");
    }
    return { z_first, z_last };
}

static double projectPointZToSurfaceSheet(const cinolib::Trimesh<>& surface,
                                          const cinolib::Octree& octree,
                                          const double x,
                                          const double y,
                                          const SurfaceSheet sheet)
{
    if (sheet == SurfaceSheet::Top)
    {
        return projectPointZToTopSurface(surface, octree, x, y);
    }

    return projectWellSpanToSurface(surface, octree, x, y).second;
}

static cinolib::vec2d toXY(const cinolib::vec3d& p)
{
    return cinolib::vec2d(p.x(), p.y());
}

static double signedArea2D(const std::vector<cinolib::vec3d>& loop)
{
    double area = 0.0;
    for (size_t i = 0; i < loop.size(); ++i)
    {
        const cinolib::vec3d& a = loop[i];
        const cinolib::vec3d& b = loop[(i + 1) % loop.size()];
        area += a.x() * b.y() - b.x() * a.y();
    }
    return 0.5 * area;
}

static bool pointInPolygonXY(const cinolib::vec2d& p, const std::vector<cinolib::vec3d>& polygon)
{
    bool inside = false;
    for (size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++)
    {
        const cinolib::vec3d& pi = polygon[i];
        const cinolib::vec3d& pj = polygon[j];
        const bool crosses = ((pi.y() > p.y()) != (pj.y() > p.y()));
        if (crosses)
        {
            const double x_intersection = (pj.x() - pi.x()) * (p.y() - pi.y()) / (pj.y() - pi.y()) + pi.x();
            if (p.x() < x_intersection)
            {
                inside = !inside;
            }
        }
    }
    return inside;
}

static double pointSegmentDistanceSquared2D(const cinolib::vec2d& p, const cinolib::vec2d& a, const cinolib::vec2d& b)
{
    const cinolib::vec2d ab = b - a;
    const double denom = ab.dot(ab);
    if (denom <= std::numeric_limits<double>::epsilon())
    {
        return (p - a).dot(p - a);
    }

    const double t = std::max(0.0, std::min(1.0, (p - a).dot(ab) / denom));
    const cinolib::vec2d projection = a + ab * t;
    return (p - projection).dot(p - projection);
}

static double pointPolylineDistanceSquared2D(const cinolib::vec2d& p, const std::vector<cinolib::vec3d>& polyline)
{
    if (polyline.empty())
    {
        return std::numeric_limits<double>::max();
    }

    double best = std::numeric_limits<double>::max();
    for (size_t i = 0; i < polyline.size(); ++i)
    {
        best = std::min(best, pointSegmentDistanceSquared2D(p, toXY(polyline[i]), toXY(polyline[(i + 1) % polyline.size()])));
    }
    return best;
}

static bool pointOnSegment2D(const cinolib::vec2d& p,
                             const cinolib::vec2d& a,
                             const cinolib::vec2d& b,
                             const double tolerance_sq,
                             double& t_out)
{
    const cinolib::vec2d ab = b - a;
    const double denom = ab.dot(ab);
    if (denom <= std::numeric_limits<double>::epsilon())
    {
        t_out = 0.0;
        return (p - a).dot(p - a) <= tolerance_sq;
    }

    const double t = (p - a).dot(ab) / denom;
    if (t < 0.0 || t > 1.0)
    {
        t_out = t;
        return false;
    }

    const cinolib::vec2d projection = a + ab * t;
    t_out = t;
    return (p - projection).dot(p - projection) <= tolerance_sq;
}

static std::vector<cinolib::vec3d> augmentRingBoundaryWithExistingVertices(const std::vector<cinolib::vec3d>& ring_boundary,
                                                                           const std::vector<cinolib::vec3d>& original_patch_points,
                                                                           const double merge_tol)
{
    if (ring_boundary.size() < 3)
    {
        return ring_boundary;
    }

    const double merge_tol_sq = merge_tol * merge_tol;
    std::vector<std::vector<std::pair<double, cinolib::vec3d>>> segment_points(ring_boundary.size());

    for (const cinolib::vec3d& p : original_patch_points)
    {
        const cinolib::vec2d p_xy = toXY(p);
        int best_segment = -1;
        double best_t = 0.0;
        double best_dist_sq = std::numeric_limits<double>::max();

        for (size_t i = 0; i < ring_boundary.size(); ++i)
        {
            const cinolib::vec2d a = toXY(ring_boundary[i]);
            const cinolib::vec2d b = toXY(ring_boundary[(i + 1) % ring_boundary.size()]);
            double t = 0.0;
            if (!pointOnSegment2D(p_xy, a, b, merge_tol_sq, t))
            {
                continue;
            }

            const double dist_sq = pointSegmentDistanceSquared2D(p_xy, a, b);
            if (dist_sq < best_dist_sq)
            {
                best_dist_sq = dist_sq;
                best_segment = static_cast<int>(i);
                best_t = t;
            }
        }

        if (best_segment < 0)
        {
            continue;
        }

        const cinolib::vec2d a = toXY(ring_boundary[best_segment]);
        const cinolib::vec2d b = toXY(ring_boundary[(best_segment + 1) % ring_boundary.size()]);
        const double segment_length = std::sqrt((b - a).dot(b - a));
        if (segment_length <= std::numeric_limits<double>::epsilon())
        {
            continue;
        }

        const double endpoint_tolerance = std::min(0.25, merge_tol / segment_length);
        if (best_t <= endpoint_tolerance || best_t >= 1.0 - endpoint_tolerance)
        {
            continue;
        }

        bool duplicate = false;
        for (const auto& item : segment_points[best_segment])
        {
            const cinolib::vec2d q_xy = toXY(item.second);
            if ((p_xy - q_xy).dot(p_xy - q_xy) <= merge_tol_sq)
            {
                duplicate = true;
                break;
            }
        }
        if (!duplicate)
        {
            segment_points[best_segment].push_back({ best_t, p });
        }
    }

    std::vector<cinolib::vec3d> augmented_boundary;
    augmented_boundary.reserve(ring_boundary.size() + original_patch_points.size());

    for (size_t i = 0; i < ring_boundary.size(); ++i)
    {
        augmented_boundary.push_back(ring_boundary[i]);
        auto& inserts = segment_points[i];
        std::sort(inserts.begin(), inserts.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs.first < rhs.first;
        });

        for (const auto& insert : inserts)
        {
            const cinolib::vec2d candidate_xy = toXY(insert.second);
            const cinolib::vec2d last_xy = toXY(augmented_boundary.back());
            if ((candidate_xy - last_xy).dot(candidate_xy - last_xy) > merge_tol_sq)
            {
                augmented_boundary.push_back(insert.second);
            }
        }
    }

    if (augmented_boundary.size() >= 2)
    {
        const cinolib::vec2d first_xy = toXY(augmented_boundary.front());
        const cinolib::vec2d last_xy = toXY(augmented_boundary.back());
        if ((first_xy - last_xy).dot(first_xy - last_xy) <= merge_tol_sq)
        {
            augmented_boundary.pop_back();
        }
    }

    return augmented_boundary;
}

static bool pointInTriangleXY(const cinolib::vec2d& p, const cinolib::vec2d& a, const cinolib::vec2d& b, const cinolib::vec2d& c)
{
    const auto cross2d = [](const cinolib::vec2d& u, const cinolib::vec2d& v)
    {
        return u.x() * v.y() - u.y() * v.x();
    };

    const double d1 = cross2d(b - a, p - a);
    const double d2 = cross2d(c - b, p - b);
    const double d3 = cross2d(a - c, p - c);

    const bool has_neg = (d1 < 0.0) || (d2 < 0.0) || (d3 < 0.0);
    const bool has_pos = (d1 > 0.0) || (d2 > 0.0) || (d3 > 0.0);
    return !(has_neg && has_pos);
}

static int orientation2D(const cinolib::vec2d& a, const cinolib::vec2d& b, const cinolib::vec2d& c)
{
    const double value = (b.y() - a.y()) * (c.x() - b.x()) - (b.x() - a.x()) * (c.y() - b.y());
    const double eps = 1e-12;
    if (std::abs(value) <= eps)
    {
        return 0;
    }
    return value > 0.0 ? 1 : 2;
}

static bool pointOnSegmentXY(const cinolib::vec2d& p, const cinolib::vec2d& a, const cinolib::vec2d& b)
{
    const double eps = 1e-12;
    return p.x() <= std::max(a.x(), b.x()) + eps &&
           p.x() + eps >= std::min(a.x(), b.x()) &&
           p.y() <= std::max(a.y(), b.y()) + eps &&
           p.y() + eps >= std::min(a.y(), b.y());
}

static bool segmentsIntersectXY(const cinolib::vec2d& p1,
                                const cinolib::vec2d& q1,
                                const cinolib::vec2d& p2,
                                const cinolib::vec2d& q2)
{
    const int o1 = orientation2D(p1, q1, p2);
    const int o2 = orientation2D(p1, q1, q2);
    const int o3 = orientation2D(p2, q2, p1);
    const int o4 = orientation2D(p2, q2, q1);

    if (o1 != o2 && o3 != o4)
    {
        return true;
    }

    if (o1 == 0 && pointOnSegmentXY(p2, p1, q1))
    {
        return true;
    }
    if (o2 == 0 && pointOnSegmentXY(q2, p1, q1))
    {
        return true;
    }
    if (o3 == 0 && pointOnSegmentXY(p1, p2, q2))
    {
        return true;
    }
    if (o4 == 0 && pointOnSegmentXY(q1, p2, q2))
    {
        return true;
    }

    return false;
}

static bool triangleIntersectsPolygonXY(const cinolib::vec3d& v0,
                                        const cinolib::vec3d& v1,
                                        const cinolib::vec3d& v2,
                                        const std::vector<cinolib::vec3d>& polygon)
{
    const cinolib::vec2d p0 = toXY(v0);
    const cinolib::vec2d p1 = toXY(v1);
    const cinolib::vec2d p2 = toXY(v2);

    if (pointInPolygonXY(p0, polygon) ||
        pointInPolygonXY(p1, polygon) ||
        pointInPolygonXY(p2, polygon))
    {
        return true;
    }

    for (const cinolib::vec3d& p : polygon)
    {
        if (pointInTriangleXY(toXY(p), p0, p1, p2))
        {
            return true;
        }
    }

    for (size_t i = 0; i < polygon.size(); ++i)
    {
        const cinolib::vec2d a = toXY(polygon[i]);
        const cinolib::vec2d b = toXY(polygon[(i + 1) % polygon.size()]);
        if (segmentsIntersectXY(p0, p1, a, b) ||
            segmentsIntersectXY(p1, p2, a, b) ||
            segmentsIntersectXY(p2, p0, a, b))
        {
            return true;
        }
    }

    return false;
}

static double footprintRadiusXY(const WellSpec& well)
{
    if (well.shape == WellSpec::Shape::Cylinder)
    {
        return well.radius;
    }

    return std::sqrt(well.half_size_x * well.half_size_x + well.half_size_y * well.half_size_y);
}

static bool triangleMatchesSurfaceSheet(const cinolib::Trimesh<>& surface,
                                        const cinolib::Trimesh<>& reference_surface,
                                        const cinolib::Octree& reference_octree,
                                        const uint pid,
                                        const SurfaceSheet sheet,
                                        const double tolerance)
{
    const cinolib::vec3d centroid = surface.poly_centroid(pid);
    try
    {
        const double projected_z = projectPointZToSurfaceSheet(reference_surface, reference_octree, centroid.x(), centroid.y(), sheet);
        return std::abs(centroid.z() - projected_z) <= tolerance;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

static std::set<uint> selectConnectedPatchComponent(const cinolib::Trimesh<>& surface,
                                                    const std::set<uint>& candidate_triangles,
                                                    const cinolib::vec2d& center)
{
    if (candidate_triangles.empty())
    {
        return candidate_triangles;
    }

    uint seed = *candidate_triangles.begin();
    double best_dist_sq = std::numeric_limits<double>::max();
    for (uint pid : candidate_triangles)
    {
        const cinolib::vec2d cxy = toXY(surface.poly_centroid(pid));
        const double dist_sq = (cxy - center).dot(cxy - center);
        if (dist_sq < best_dist_sq)
        {
            best_dist_sq = dist_sq;
            seed = pid;
        }
    }

    std::set<uint> component;
    std::queue<uint> frontier;
    component.insert(seed);
    frontier.push(seed);

    while (!frontier.empty())
    {
        const uint curr = frontier.front();
        frontier.pop();

        for (uint nbr : surface.adj_p2p(curr))
        {
            if (candidate_triangles.count(nbr) == 0)
            {
                continue;
            }
            if (component.insert(nbr).second)
            {
                frontier.push(nbr);
            }
        }
    }

    return component;
}

static std::vector<std::vector<uint>> collectBoundaryLoops(const cinolib::Trimesh<>& surface,
                                                           const std::set<uint>& selected_triangles)
{
    std::cout << "Collecting boundary edges from the selected patch triangles..." << std::endl;
    std::map<std::pair<uint, uint>, int> edge_counts;
    for (uint pid : selected_triangles)
    {
        for (uint offset = 0; offset < 3; ++offset)
        {
            uint a = surface.poly_vert_id(pid, offset);
            uint b = surface.poly_vert_id(pid, (offset + 1) % 3);
            if (a > b)
            {
                std::swap(a, b);
            }
            edge_counts[{ a, b }]++;
        }
    }

    std::cout << "Building adjacency for boundary edges..." << std::endl;
    std::map<uint, std::vector<uint>> boundary_adjacency;
    for (const auto& item : edge_counts)
    {
        if (item.second == 1)
        {
            boundary_adjacency[item.first.first].push_back(item.first.second);
            boundary_adjacency[item.first.second].push_back(item.first.first);
        }
    }

    std::cout << "Extracting boundary loops from adjacency..." << std::endl;
    std::set<std::pair<uint, uint>> visited_edges;
    std::vector<std::vector<uint>> loops;
    const size_t max_walk_steps = std::max<size_t>(1, boundary_adjacency.size()) * 4;
    for (const auto& item : boundary_adjacency)
    {
        const uint start = item.first;
        for (uint next : item.second)
        {
            const std::pair<uint, uint> edge_key = std::minmax(start, next);
            if (visited_edges.count(edge_key) > 0)
            {
                continue;
            }

            std::vector<uint> loop;
            uint prev = std::numeric_limits<uint>::max();
            uint curr = start;
            uint candidate = next;
            bool closed_loop = false;
            size_t walk_steps = 0;
            while (true)
            {
                if (++walk_steps > max_walk_steps)
                {
                    std::cout << "Boundary loop walk guard triggered (non-manifold or invalid boundary around vertex "
                              << curr << ")" << std::endl;
                    break;
                }

                loop.push_back(curr);
                const std::pair<uint, uint> walk_edge = std::minmax(curr, candidate);
                if (visited_edges.count(walk_edge) > 0)
                {
                    break;
                }
                visited_edges.insert(walk_edge);

                prev = curr;
                curr = candidate;
                if (curr == start)
                {
                    closed_loop = true;
                    break;
                }

                const auto& neighbors = boundary_adjacency.at(curr);
                bool found_next = false;
                for (uint neigh : neighbors)
                {
                    if (neigh == prev)
                    {
                        continue;
                    }

                    const std::pair<uint, uint> next_edge = std::minmax(curr, neigh);
                    if (visited_edges.count(next_edge) == 0)
                    {
                        candidate = neigh;
                        found_next = true;
                        break;
                    }
                }

                if (!found_next)
                {
                    for (uint neigh : neighbors)
                    {
                        const std::pair<uint, uint> next_edge = std::minmax(curr, neigh);
                        if (visited_edges.count(next_edge) == 0)
                        {
                            candidate = neigh;
                            found_next = true;
                            break;
                        }
                    }
                }

                if (!found_next)
                {
                    break;
                }
            }

            if (closed_loop && loop.size() >= 3)
            {
                loops.push_back(loop);
            }
        }
    }

    std::cout << "Identified " << loops.size() << " boundary loops." << std::endl;
    return loops;
}

static std::vector<std::vector<uint>> collectMeshBoundaryLoops(const cinolib::Trimesh<>& mesh)
{
    std::map<std::pair<uint, uint>, int> edge_counts;
    for (uint pid = 0; pid < mesh.num_polys(); ++pid)
    {
        for (uint offset = 0; offset < 3; ++offset)
        {
            uint a = mesh.poly_vert_id(pid, offset);
            uint b = mesh.poly_vert_id(pid, (offset + 1) % 3);
            if (a > b)
            {
                std::swap(a, b);
            }
            edge_counts[{ a, b }]++;
        }
    }

    std::map<uint, std::vector<uint>> boundary_adjacency;
    for (const auto& item : edge_counts)
    {
        if (item.second == 1)
        {
            boundary_adjacency[item.first.first].push_back(item.first.second);
            boundary_adjacency[item.first.second].push_back(item.first.first);
        }
    }

    std::set<std::pair<uint, uint>> visited_edges;
    std::vector<std::vector<uint>> loops;
    const size_t max_walk_steps = std::max<size_t>(1, boundary_adjacency.size()) * 4;
    for (const auto& item : boundary_adjacency)
    {
        const uint start = item.first;
        for (uint next : item.second)
        {
            const std::pair<uint, uint> edge_key = std::minmax(start, next);
            if (visited_edges.count(edge_key) > 0)
            {
                continue;
            }

            std::vector<uint> loop;
            uint prev = std::numeric_limits<uint>::max();
            uint curr = start;
            uint candidate = next;
            bool closed_loop = false;
            size_t walk_steps = 0;
            while (true)
            {
                if (++walk_steps > max_walk_steps)
                {
                    break;
                }

                loop.push_back(curr);
                const std::pair<uint, uint> walk_edge = std::minmax(curr, candidate);
                if (visited_edges.count(walk_edge) > 0)
                {
                    break;
                }
                visited_edges.insert(walk_edge);

                prev = curr;
                curr = candidate;
                if (curr == start)
                {
                    closed_loop = true;
                    break;
                }

                const auto& neighbors = boundary_adjacency.at(curr);
                bool found_next = false;
                for (uint neigh : neighbors)
                {
                    if (neigh == prev)
                    {
                        continue;
                    }

                    const std::pair<uint, uint> next_edge = std::minmax(curr, neigh);
                    if (visited_edges.count(next_edge) == 0)
                    {
                        candidate = neigh;
                        found_next = true;
                        break;
                    }
                }

                if (!found_next)
                {
                    break;
                }
            }

            if (closed_loop && loop.size() >= 3)
            {
                loops.push_back(loop);
            }
        }
    }

    return loops;
}

static void closeBoundaryHoles(cinolib::Trimesh<>& mesh,
                              const cinolib::vec2d& center,
                              const double local_radius)
{
    const auto loops = collectMeshBoundaryLoops(mesh);
    if (loops.empty())
    {
        return;
    }

    int added_faces = 0;
    int closed_loops = 0;
    int skipped_loops = 0;
    const double local_radius_sq = local_radius * local_radius;
    for (const auto& loop : loops)
    {
        if (loop.size() < 3)
        {
            continue;
        }

        cinolib::vec2d loop_centroid(0.0, 0.0);
        for (uint vid : loop)
        {
            const cinolib::vec2d p = toXY(mesh.vert(vid));
            loop_centroid = loop_centroid + p;
        }
        loop_centroid = loop_centroid / static_cast<double>(loop.size());

        const double dist_sq = (loop_centroid - center).dot(loop_centroid - center);
        if (dist_sq > local_radius_sq)
        {
            ++skipped_loops;
            continue;
        }

        for (size_t i = 1; i + 1 < loop.size(); ++i)
        {
            mesh.poly_add(loop[0], loop[i], loop[i + 1]);
            ++added_faces;
        }
        ++closed_loops;
    }

    if (closed_loops > 0)
    {
        std::cout << "Closed " << closed_loops << " local open boundary loops by adding "
                  << added_faces << " repair triangles." << std::endl;
    }
    if (skipped_loops > 0)
    {
        std::cout << "Skipped " << skipped_loops
                  << " non-local boundary loops during repair to avoid modifying distant geometry." << std::endl;
    }
}

static std::vector<cinolib::vec3d> chooseOuterPatchBoundary(const cinolib::Trimesh<>& surface,
                                                            const std::vector<std::vector<uint>>& loops,
                                                            const cinolib::vec2d& center)
{
    double best_area = -1.0;
    std::vector<cinolib::vec3d> best_loop;
    for (const auto& loop : loops)
    {
        std::vector<cinolib::vec3d> polygon;
        polygon.reserve(loop.size());
        for (uint vid : loop)
        {
            polygon.push_back(surface.vert(vid));
        }

        if (!pointInPolygonXY(center, polygon))
        {
            continue;
        }

        const double area = std::abs(signedArea2D(polygon));
        if (area > best_area)
        {
            best_area = area;
            best_loop = polygon;
        }
    }

    if (best_loop.empty())
    {
        throw std::runtime_error("Could not identify a closed top boundary loop for the local well patch");
    }

    if (signedArea2D(best_loop) < 0.0)
    {
        std::reverse(best_loop.begin(), best_loop.end());
    }
    return best_loop;
}

static cinolib::Trimesh<> rebuildSurfaceWithoutTriangles(const cinolib::Trimesh<>& surface, const std::set<uint>& removed_triangles)
{
    std::vector<cinolib::vec3d> kept_vertices;
    std::vector<std::vector<uint>> kept_faces;
    std::map<uint, uint> vertex_map;

    for (uint pid = 0; pid < surface.num_polys(); ++pid)
    {
        if (removed_triangles.count(pid) > 0)
        {
            continue;
        }

        std::vector<uint> face;
        face.reserve(3);
        for (uint offset = 0; offset < 3; ++offset)
        {
            const uint old_vid = surface.poly_vert_id(pid, offset);
            auto it = vertex_map.find(old_vid);
            if (it == vertex_map.end())
            {
                const uint new_vid = static_cast<uint>(kept_vertices.size());
                vertex_map[old_vid] = new_vid;
                kept_vertices.push_back(surface.vert(old_vid));
                face.push_back(new_vid);
            }
            else
            {
                face.push_back(it->second);
            }
        }
        kept_faces.push_back(face);
    }

    return cinolib::Trimesh<>(kept_vertices, kept_faces);
}

static std::vector<uint> findEmbeddedRingVertexIds(const cinolib::Trimesh<>& surface,
                                                   const std::vector<cinolib::vec3d>& ring,
                                                   const double tolerance)
{
    const double tolerance_sq = tolerance * tolerance;
    std::vector<uint> vertex_ids;
    vertex_ids.reserve(ring.size());

    for (const cinolib::vec3d& p : ring)
    {
        int best_vid = -1;
        double best_dist_sq = std::numeric_limits<double>::max();
        for (uint vid = 0; vid < surface.num_verts(); ++vid)
        {
            const cinolib::vec3d delta = surface.vert(vid) - p;
            const double dist_sq = delta.dot(delta);
            if (dist_sq <= tolerance_sq && dist_sq < best_dist_sq)
            {
                best_dist_sq = dist_sq;
                best_vid = static_cast<int>(vid);
            }
        }

        if (best_vid < 0)
        {
            throw std::runtime_error("Could not locate embedded ring vertex on remeshed surface");
        }

        vertex_ids.push_back(static_cast<uint>(best_vid));
    }

    return vertex_ids;
}

static std::vector<cinolib::vec3d> buildProjectedFootprintRing(const WellSpec& well,
                                                               const cinolib::Trimesh<>& reference_surface,
                                                               const cinolib::Octree& reference_octree,
                                                               const SurfaceSheet sheet,
                                                               const double target_edge_length)
{
    std::vector<cinolib::vec3d> ring;

    auto append_projected_point = [&](const double x, const double y)
    {
        ring.emplace_back(x, y, projectPointZToSurfaceSheet(reference_surface, reference_octree, x, y, sheet));
    };

    if (well.shape == WellSpec::Shape::Cylinder)
    {
        const double safe_edge_length = std::max(target_edge_length, 1e-6);
        const double circumference = 2.0 * M_PI * well.radius;
        const int radial_segments = std::max(6, static_cast<int>(std::ceil(circumference / safe_edge_length)));
        ring.reserve(radial_segments);
        for (int i = 0; i < radial_segments; ++i)
        {
            const double angle = 2.0 * M_PI * i / radial_segments;
            append_projected_point(well.x + well.radius * std::cos(angle), well.y + well.radius * std::sin(angle));
        }
        return ring;
    }

    const double min_x = well.x - well.half_size_x;
    const double max_x = well.x + well.half_size_x;
    const double min_y = well.y - well.half_size_y;
    const double max_y = well.y + well.half_size_y;
    const std::vector<cinolib::vec2d> corners = {
        cinolib::vec2d(min_x, min_y),
        cinolib::vec2d(max_x, min_y),
        cinolib::vec2d(max_x, max_y),
        cinolib::vec2d(min_x, max_y)
    };

    const double safe_edge_length = std::max(target_edge_length, 1e-6);
    for (size_t i = 0; i < corners.size(); ++i)
    {
        const cinolib::vec2d a = corners[i];
        const cinolib::vec2d b = corners[(i + 1) % corners.size()];
        const double length = std::sqrt((b - a).dot(b - a));
        const int segments = std::max(1, static_cast<int>(std::ceil(length / safe_edge_length)));
        for (int step = 0; step < segments; ++step)
        {
            const double t = static_cast<double>(step) / static_cast<double>(segments);
            append_projected_point(a.x() + (b.x() - a.x()) * t, a.y() + (b.y() - a.y()) * t);
        }
    }

    return ring;
}

static cinolib::Trimesh<> triangulateSurfacePatchWithConstraints(const std::vector<cinolib::vec3d>& outer_boundary,
                                                                 std::vector<cinolib::vec3d> ring_boundary,
                                                                 const std::vector<cinolib::vec3d>& original_patch_points,
                                                                 const cinolib::Trimesh<>& reference_surface,
                                                                 const cinolib::Octree& reference_octree,
                                                                 const SurfaceSheet sheet,
                                                                 const double target_edge_length)
{
    if (signedArea2D(ring_boundary) > 0.0)
    {
        std::reverse(ring_boundary.begin(), ring_boundary.end());
    }

    std::vector<double> points_in;
    std::vector<unsigned int> segments_in;
    std::vector<double> point_z;
    const double merge_tol = std::max(1e-6, target_edge_length * 1e-3);
    const double merge_tol_sq = merge_tol * merge_tol;

    ring_boundary = augmentRingBoundaryWithExistingVertices(ring_boundary, original_patch_points, merge_tol);

    auto append_unique_point = [&](const cinolib::vec3d& p, const bool prefer_existing_z)
    {
        const cinolib::vec2d p_xy = toXY(p);
        for (size_t i = 0; i < point_z.size(); ++i)
        {
            const cinolib::vec2d q(points_in[2 * i], points_in[2 * i + 1]);
            if ((p_xy - q).dot(p_xy - q) <= merge_tol_sq)
            {
                if (!prefer_existing_z)
                {
                    point_z[i] = p.z();
                }
                return static_cast<unsigned int>(i);
            }
        }

        points_in.push_back(p.x());
        points_in.push_back(p.y());
        point_z.push_back(p.z());
        return static_cast<unsigned int>(point_z.size() - 1);
    };

    for (const cinolib::vec3d& p : original_patch_points)
    {
        const cinolib::vec2d p_xy = toXY(p);
        if (!pointInPolygonXY(p_xy, outer_boundary))
        {
            continue;
        }
        if (pointPolylineDistanceSquared2D(p_xy, outer_boundary) <= merge_tol_sq ||
            pointPolylineDistanceSquared2D(p_xy, ring_boundary) <= merge_tol_sq)
        {
            continue;
        }
        append_unique_point(p, true);
    }

    const auto append_loop = [&](const std::vector<cinolib::vec3d>& loop)
    {
        std::vector<unsigned int> ids;
        ids.reserve(loop.size());
        for (const cinolib::vec3d& p : loop)
        {
            ids.push_back(append_unique_point(p, false));
        }

        for (size_t i = 0; i < ids.size(); ++i)
        {
            const unsigned int a = ids[i];
            const unsigned int b = ids[(i + 1) % ids.size()];
            if (a != b)
            {
                segments_in.push_back(a);
                segments_in.push_back(b);
            }
        }
    };

    append_loop(outer_boundary);
    append_loop(ring_boundary);

    std::vector<double> points_out;
    std::vector<unsigned int> tris_out;
    cinolib::triangle_wrap(points_in, segments_in, {}, "Q", points_out, tris_out);

    std::vector<double> coords3d;
    coords3d.reserve(points_out.size() / 2 * 3);
    for (size_t i = 0; i < points_out.size(); i += 2)
    {
        const double x = points_out[i];
        const double y = points_out[i + 1];
        coords3d.push_back(x);
        coords3d.push_back(y);
        if (i / 2 < point_z.size())
        {
            coords3d.push_back(point_z[i / 2]);
        }
        else
        {
            coords3d.push_back(projectPointZToSurfaceSheet(reference_surface, reference_octree, x, y, sheet));
        }
    }

    return cinolib::Trimesh<>(coords3d, tris_out);
}

static cinolib::Trimesh<> embedRingIntoSurfacePatch(const cinolib::Trimesh<>& surface,
                                                    const cinolib::Trimesh<>& reference_surface,
                                                    const cinolib::Octree& reference_octree,
                                                    const WellSpec& well,
                                                    const std::vector<cinolib::vec3d>& ring,
                                                    const SurfaceSheet sheet,
                                                    const double target_edge_length)
{
    static int patch_debug_counter = 0;
    const cinolib::vec2d center(well.x, well.y);
    const double patch_radius = footprintRadiusXY(well) + std::max(1e-6, target_edge_length * 1e-3);
    const double sheet_tolerance = std::max(1e-6, target_edge_length * 0.25);

    std::set<uint> patch_triangles;
    for (uint pid = 0; pid < surface.num_polys(); ++pid)
    {
        if (!triangleMatchesSurfaceSheet(surface, reference_surface, reference_octree, pid, sheet, sheet_tolerance))
        {
            continue;
        }

        if (triangleIntersectsPolygonXY(surface.poly_vert(pid, 0), surface.poly_vert(pid, 1), surface.poly_vert(pid, 2), ring))
        {
            patch_triangles.insert(pid);
        }
    }

    patch_triangles = selectConnectedPatchComponent(surface, patch_triangles, center);

    if (patch_triangles.empty())
    {
        throw std::runtime_error("Could not find a surface patch to remesh around the well ring");
    }

    if (patch_triangles.size() >= surface.num_polys())
    {
        throw std::runtime_error("Patch selection consumed the full surface; aborting local remeshing to avoid losing the input mesh");
    }

    std::cout << "Identified " << patch_triangles.size() << " triangles in the initial patch around the well ring." << std::endl;

    std::cout << "Collecting patch boundary loops..." << std::endl;
    const auto loops = collectBoundaryLoops(surface, patch_triangles);

    std::cout << "Choosing outer patch boundary loop..." << std::endl;
    const std::vector<cinolib::vec3d> outer_boundary = chooseOuterPatchBoundary(surface, loops, center);

    std::cout << "Embedding ring vertices into the patch..." << std::endl;
    std::set<uint> patch_vertices;
    for (uint pid : patch_triangles)
    {
        patch_vertices.insert(surface.poly_vert_id(pid, 0));
        patch_vertices.insert(surface.poly_vert_id(pid, 1));
        patch_vertices.insert(surface.poly_vert_id(pid, 2));
    }

    std::cout << "Found " << patch_vertices.size() << " unique vertices in the initial patch." << std::endl;
    std::vector<cinolib::vec3d> patch_points;
    patch_points.reserve(patch_vertices.size());
    for (uint vid : patch_vertices)
    {
        patch_points.push_back(surface.vert(vid));
    }


    std::cout << "Triangulating the patch with the embedded ring as a constraint..." << std::endl;
    const cinolib::Trimesh<> remeshed_patch = triangulateSurfacePatchWithConstraints(outer_boundary, ring, patch_points, reference_surface, reference_octree, sheet, target_edge_length);
    
    std::cout << "Merging the remeshed patch back into the original surface..." << std::endl;
    const cinolib::Trimesh<> cut_surface = rebuildSurfaceWithoutTriangles(surface, patch_triangles);

    const std::string cut_surface_filename = std::string("cut_surface_")
                                           + (sheet == SurfaceSheet::Top ? "top_" : "bottom_")
                                           + std::to_string(++patch_debug_counter) + ".obj";
    try
    {
        cinolib::Trimesh<> cut_surface_copy = cut_surface;
        cut_surface_copy.save(cut_surface_filename.c_str());
        std::cout << "Saved cut surface before patch reinsertion to: " << cut_surface_filename << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Warning: failed to save cut surface debug mesh: " << e.what() << std::endl;
    }

    if (cut_surface.num_polys() == 0)
    {
        throw std::runtime_error("Patch removal emptied the input surface; aborting local remeshing");
    }


    std::cout << "Merging meshes at coincident vertices..." << std::endl;
    cinolib::Trimesh<> embedded_surface;
    cinolib::merge_meshes_at_coincident_vertices(cut_surface, remeshed_patch, embedded_surface);

    closeBoundaryHoles(embedded_surface, center, std::max(target_edge_length * 2.0, patch_radius * 2.0));

    if (embedded_surface.num_polys() < surface.num_polys() / 2)
    {
        throw std::runtime_error("Remeshing produced an unexpectedly small surface; aborting to avoid dropping the input geometry");
    }

    std::cout << "Successfully embedded the well ring into the surface patch." << std::endl;
    return embedded_surface;
}

static WellSpec parseWellString(const std::string& well_str,
                                const WellParseDefaults& defaults,
                                const cinolib::Trimesh<>& input_surface,
                                const cinolib::Octree& surface_octree)
{
    const ParsedWellSpec parsed = parseWellSpecifier(well_str);
    std::vector<double> values = parseWellValues(parsed.values);

    WellSpec well;
    well.shape = parsed.shape;

    if (parsed.shape == WellSpec::Shape::Cylinder)
    {
        switch (parsed.format)
        {
            case WellFormat::XYZHR:
            {
                if (values.size() < 5)
                {
                    throw std::runtime_error("Cylinder XYZHR must have at least 5 values: x,y,z,height,radius[,z_sub1,z_sub2,...]");
                }
                well.x = values[0];
                well.y = values[1];
                well.z = values[2];
                well.height = values[3];
                well.radius = values[4];
                for (size_t i = 5; i < values.size(); ++i)
                {
                    well.z_subdivisions.push_back(values[i]);
                }
                break;
            }
            case WellFormat::XYHR:
            {
                if (values.size() < 4)
                {
                    throw std::runtime_error("Cylinder XYHR must have at least 4 values: x,y,height,radius[,z_sub1,z_sub2,...]");
                }
                well.x = values[0];
                well.y = values[1];
                well.z = projectWellZToSurface(input_surface, surface_octree, well.x, well.y);
                well.height = values[2];
                well.radius = values[3];
                for (size_t i = 4; i < values.size(); ++i)
                {
                    well.z_subdivisions.push_back(values[i]);
                }
                break;
            }
            case WellFormat::XYR:
            {
                if (values.size() != 3)
                {
                    throw std::runtime_error("Cylinder XYR must have exactly 3 values: x,y,radius");
                }
                well.x = values[0];
                well.y = values[1];
                const auto span = projectWellSpanToSurface(input_surface, surface_octree, well.x, well.y);
                well.z = span.first;
                well.height = span.second - span.first;
                well.project_bottom_to_surface = true;
                well.radius = values[2];
                break;
            }
        }
    }
    else
    {
        switch (parsed.format)
        {
            case WellFormat::XYZHR:
            {
                if (values.size() < 6)
                {
                    throw std::runtime_error("Box XYZHR must have at least 6 values: x,y,z,height,diag_x,diag_y[,z_sub1,z_sub2,...]");
                }
                well.x = values[0];
                well.y = values[1];
                well.z = values[2];
                well.height = values[3];
                well.half_size_x = 0.5 * std::abs(values[4]);
                well.half_size_y = 0.5 * std::abs(values[5]);
                for (size_t i = 6; i < values.size(); ++i)
                {
                    well.z_subdivisions.push_back(values[i]);
                }
                break;
            }
            case WellFormat::XYHR:
            {
                if (values.size() < 5)
                {
                    throw std::runtime_error("Box XYHR must have at least 5 values: x,y,height,diag_x,diag_y[,z_sub1,z_sub2,...]");
                }
                well.x = values[0];
                well.y = values[1];
                well.z = projectWellZToSurface(input_surface, surface_octree, well.x, well.y);
                well.height = values[2];
                well.half_size_x = 0.5 * std::abs(values[3]);
                well.half_size_y = 0.5 * std::abs(values[4]);
                for (size_t i = 5; i < values.size(); ++i)
                {
                    well.z_subdivisions.push_back(values[i]);
                }
                break;
            }
            case WellFormat::XYR:
            {
                if (values.size() != 4)
                {
                    throw std::runtime_error("Box XYR must have exactly 4 values: x,y,diag_x,diag_y");
                }
                well.x = values[0];
                well.y = values[1];
                const auto span = projectWellSpanToSurface(input_surface, surface_octree, well.x, well.y);
                well.z = span.first;
                well.height = span.second - span.first;
                well.project_bottom_to_surface = true;
                well.half_size_x = 0.5 * std::abs(values[2]);
                well.half_size_y = 0.5 * std::abs(values[3]);
                break;
            }
        }
    }

    if (well.shape == WellSpec::Shape::Box)
    {
        well.radius = 0.0;
    }

    if (well.shape == WellSpec::Shape::Cylinder && well.radius <= 0.0)
    {
        throw std::runtime_error("Well radius must be > 0");
    }

    if (well.shape == WellSpec::Shape::Box && (well.half_size_x <= 0.0 || well.half_size_y <= 0.0))
    {
        throw std::runtime_error("BOX diagonal components in x and y must be > 0");
    }

    if (std::abs(well.height) <= std::numeric_limits<double>::epsilon())
    {
        throw std::runtime_error("Well height must be non-zero");
    }

    // z_subdivisions are absolute z coordinates and must lie strictly inside the well z-interval
    double z0 = well.z;
    double z1 = well.z + well.height;
    double z_min = std::min(z0, z1);
    double z_max = std::max(z0, z1);
    for (double z_sub : well.z_subdivisions)
    {
        if (z_sub <= z_min || z_sub >= z_max)
        {
            throw std::runtime_error("Z subdivision must be between " + std::to_string(z_min) + " and " + std::to_string(z_max) + ": " + std::to_string(z_sub));
        }
    }

    std::sort(well.z_subdivisions.begin(), well.z_subdivisions.end());

    // Remove duplicate/near-duplicate subdivision levels to avoid zero-thickness segments.
    {
        std::vector<double> unique_subdivisions;
        unique_subdivisions.reserve(well.z_subdivisions.size());
        const double eps = 1e-9;
        for (double z_sub : well.z_subdivisions)
        {
            if (unique_subdivisions.empty() || std::abs(z_sub - unique_subdivisions.back()) > eps)
            {
                unique_subdivisions.push_back(z_sub);
            }
        }
        well.z_subdivisions.swap(unique_subdivisions);
    }

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

static cinolib::Trimesh<> attachExtrudedWalls(cinolib::Trimesh<>& surface_mesh,
                                              const WellSpec& well,
                                              double target_edge_length,
                                              const std::vector<cinolib::vec3d>& footprint_ring,
                                              const std::vector<uint>& top_ring_vids,
                                              const std::vector<uint>* bottom_ring_vids,
                                              bool verbose = false)
{
    const int ring_segments = static_cast<int>(top_ring_vids.size());
    if (ring_segments < 3 || static_cast<size_t>(ring_segments) != footprint_ring.size())
    {
        throw std::runtime_error("Embedded footprint ring is invalid or inconsistent with the extrusion path");
    }

    if (verbose)
    {
        if (well.shape == WellSpec::Shape::Cylinder)
        {
            std::cout << "  - Radius: " << well.radius << ", Height: " << well.height << std::endl;
        }
        else
        {
            std::cout << "  - Half-size x/y: " << well.half_size_x << "/" << well.half_size_y << ", Height: " << well.height << std::endl;
        }
        std::cout << "  - Using " << ring_segments << " footprint segments" << std::endl;
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

    std::vector<cinolib::vec3d> wall_vertices;
    std::vector<std::vector<uint>> wall_faces;

    std::vector<double> critical_z_levels;
    critical_z_levels.push_back(well.z);
    for (double z_sub : well.z_subdivisions)
    {
        critical_z_levels.push_back(z_sub);
    }
    critical_z_levels.push_back(well.z + well.height);
    std::sort(critical_z_levels.begin(), critical_z_levels.end());

    // Ensure strictly increasing z levels to prevent degenerate side quads.
    {
        std::vector<double> unique_levels;
        unique_levels.reserve(critical_z_levels.size());
        const double eps = 1e-9;
        for (double z : critical_z_levels)
        {
            if (unique_levels.empty() || std::abs(z - unique_levels.back()) > eps)
            {
                unique_levels.push_back(z);
            }
        }
        critical_z_levels.swap(unique_levels);
    }

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
    const double top_z = well.z;
    const double bottom_z = well.z + well.height;
    for (size_t level = 0; level < z_levels.size(); ++level)
    {
        double z = z_levels[level];
        if (std::abs(z - top_z) < eps)
        {
            for (int i = 0; i < ring_segments; ++i)
            {
                ring_vertex_map[{ static_cast<int>(level), i }] = top_ring_vids.at(i);
            }
            continue;
        }

        if (bottom_ring_vids && std::abs(z - bottom_z) < eps)
        {
            for (int i = 0; i < ring_segments; ++i)
            {
                ring_vertex_map[{ static_cast<int>(level), i }] = bottom_ring_vids->at(i);
            }
            continue;
        }

        for (int i = 0; i < ring_segments; ++i)
        {
            const double x = footprint_ring[static_cast<size_t>(i)].x();
            const double y = footprint_ring[static_cast<size_t>(i)].y();

            const uint new_vid = surface_mesh.vert_add(cinolib::vec3d(x, y, z));
            ring_vertex_map[{ static_cast<int>(level), i }] = new_vid;
            wall_vertices.push_back(cinolib::vec3d(x, y, z));
        }
    }

    for (size_t level = 0; level < z_levels.size() - 1; ++level)
    {
        for (int i = 0; i < ring_segments; ++i)
        {
            int next_i = (i + 1) % ring_segments;

            uint v0 = ring_vertex_map[{ static_cast<int>(level), i }];
            uint v1 = ring_vertex_map[{ static_cast<int>(level), next_i }];
            uint v2 = ring_vertex_map[{ static_cast<int>(level + 1), i }];
            uint v3 = ring_vertex_map[{ static_cast<int>(level + 1), next_i }];

            surface_mesh.poly_add(v0, v1, v2);
            surface_mesh.poly_add(v1, v3, v2);

            wall_faces.push_back({ v0, v1, v2 });
            wall_faces.push_back({ v1, v3, v2 });
        }
    }

    // Close the classifier mesh used by winding-number queries.
    auto append_ring_cap = [&](const std::vector<uint>& ring_vids, const bool invert_orientation)
    {
        if (ring_vids.size() < 3)
        {
            return;
        }

        for (size_t i = 1; i + 1 < ring_vids.size(); ++i)
        {
            if (!invert_orientation)
            {
                wall_faces.push_back({ ring_vids[0], ring_vids[i], ring_vids[i + 1] });
            }
            else
            {
                wall_faces.push_back({ ring_vids[0], ring_vids[i + 1], ring_vids[i] });
            }
        }
    };

    std::vector<uint> bottom_ring_for_caps;
    int bottom_level = -1;
    for (size_t level = 0; level < z_levels.size(); ++level)
    {
        if (std::abs(z_levels[level] - bottom_z) < eps)
        {
            bottom_level = static_cast<int>(level);
            break;
        }
    }
    if (bottom_level >= 0)
    {
        bottom_ring_for_caps.reserve(ring_segments);
        for (int i = 0; i < ring_segments; ++i)
        {
            bottom_ring_for_caps.push_back(ring_vertex_map[{ bottom_level, i }]);
        }
    }

    append_ring_cap(top_ring_vids, false);
    append_ring_cap(bottom_ring_for_caps, true);

    // For wells with an explicit H, the classifier is closed by the synthetic
    // bottom cap above, but TetGen only sees facets added to surface_mesh.
    // Add the same bottom plane to the PLC so extracted well tets conform to a
    // planar bottom instead of being reconstructed later by centroid labeling.
    if (!bottom_ring_vids && bottom_ring_for_caps.size() >= 3)
    {
        const uint bottom_center_vid = surface_mesh.vert_add(cinolib::vec3d(well.x, well.y, bottom_z));
        for (int i = 0; i < ring_segments; ++i)
        {
            const int next_i = (i + 1) % ring_segments;
            surface_mesh.poly_add(bottom_center_vid, bottom_ring_for_caps[next_i], bottom_ring_for_caps[i]);
        }
    }

    // Add internal horizontal caps at user-provided z levels so TetGen can honor
    // internal layer boundaries inside each well.
    int internal_cap_faces = 0;
    for (double z_sub : well.z_subdivisions)
    {
        int level = -1;
        for (size_t i = 0; i < z_levels.size(); ++i)
        {
            if (std::abs(z_levels[i] - z_sub) < eps)
            {
                level = static_cast<int>(i);
                break;
            }
        }

        if (level < 0)
        {
            continue;
        }

        const uint center_vid = surface_mesh.vert_add(cinolib::vec3d(well.x, well.y, z_sub));
        for (int i = 0; i < ring_segments; ++i)
        {
            const int next_i = (i + 1) % ring_segments;
            const uint v0 = ring_vertex_map[{ level, i }];
            const uint v1 = ring_vertex_map[{ level, next_i }];
            surface_mesh.poly_add(center_vid, v0, v1);
            ++internal_cap_faces;
        }
    }

    if (verbose)
    {
        std::cout << "  - Generated: " << wall_vertices.size() << " new wall vertices, " << wall_faces.size() << " wall faces" << std::endl;
        if (!well.z_subdivisions.empty())
        {
            std::cout << "  - Added " << internal_cap_faces << " internal critical-level cap faces" << std::endl;
        }
    }

    std::vector<cinolib::vec3d> all_vertices;
    all_vertices.reserve(surface_mesh.num_verts());
    for (uint vid = 0; vid < surface_mesh.num_verts(); ++vid)
    {
        all_vertices.push_back(surface_mesh.vert(vid));
    }

    return cinolib::Trimesh<>(all_vertices, wall_faces);
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

static bool saveMesh(const cinolib::Trimesh<>& mesh, const std::string& filename)
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
    return wn != 0;
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
                for (size_t i = 0; i < well_strings.size(); ++i)
                {
                    try
                    {
                        min_radius = std::min(min_radius, parseWellRadius(well_strings[i]));
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
        if (saveMesh(input_surface, box_filename) && verbose)
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

    WellParseDefaults well_defaults;
    well_defaults.height = input_surface.bbox().max.z() - input_surface.bbox().min.z();
    cinolib::Octree surface_octree = buildSurfaceOctree(input_surface);

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
            WellSpec well = parseWellString(well_strings[i], well_defaults, input_surface, surface_octree);
            wells.push_back(well);

            if (verbose)
            {
                std::cout << "Well " << (i + 1) << ": position=(" << well.x << "," << well.y << "," << well.z << "), height=" << well.height;
                if (well.shape == WellSpec::Shape::Cylinder)
                {
                    std::cout << ", radius=" << well.radius;
                }
                else
                {
                    std::cout << ", box_diagonal=(" << (2.0 * well.half_size_x) << "," << (2.0 * well.half_size_y) << "," << std::abs(well.height) << ")";
                }
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

    cinolib::Trimesh<> remeshed_surface = input_surface;
    for (size_t i = 0; i < wells.size(); ++i)
    {
        double cylinder_edge_length = target_edge_length;
        if (refine_cylinders)
        {
            cylinder_edge_length *= 0.5;
        }
        cylinder_edge_length *= cylinder_edge_scale;

        const std::vector<cinolib::vec3d> top_ring = buildProjectedFootprintRing(wells[i], input_surface, surface_octree, SurfaceSheet::Top, cylinder_edge_length);

        if (verbose)
        {
            std::cout << "Remeshing top surface for well " << (i + 1) << "..." << std::endl;
        }

        remeshed_surface = embedRingIntoSurfacePatch(remeshed_surface, input_surface, surface_octree, wells[i], top_ring, SurfaceSheet::Top, cylinder_edge_length);

        if (wells[i].project_bottom_to_surface)
        {
            const std::vector<cinolib::vec3d> bottom_ring = buildProjectedFootprintRing(wells[i], input_surface, surface_octree, SurfaceSheet::Bottom, cylinder_edge_length);

            if (verbose)
            {
                std::cout << "Remeshing bottom surface for well " << (i + 1) << "..." << std::endl;
            }

            remeshed_surface = embedRingIntoSurfacePatch(remeshed_surface, input_surface, surface_octree, wells[i], bottom_ring, SurfaceSheet::Bottom, cylinder_edge_length);
        }
    }

    const std::string remeshed_surface_filename = "remeshed_surface.obj";
    if (saveMesh(remeshed_surface, remeshed_surface_filename) && verbose)
    {
        std::cout << "Remeshed surface saved to: " << remeshed_surface_filename << std::endl;
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

        const double ring_match_tolerance = std::max(1e-6, cylinder_edge_length * 1e-3);
        const std::vector<cinolib::vec3d> top_ring = buildProjectedFootprintRing(wells[i], input_surface, surface_octree, SurfaceSheet::Top, cylinder_edge_length);
        const std::vector<uint> top_ring_vids = findEmbeddedRingVertexIds(remeshed_surface, top_ring, ring_match_tolerance);

        std::vector<cinolib::vec3d> bottom_ring;
        std::vector<uint> bottom_ring_vids;
        const std::vector<uint>* bottom_ring_vids_ptr = nullptr;
        if (wells[i].project_bottom_to_surface)
        {
            bottom_ring = buildProjectedFootprintRing(wells[i], input_surface, surface_octree, SurfaceSheet::Bottom, cylinder_edge_length);
            bottom_ring_vids = findEmbeddedRingVertexIds(remeshed_surface, bottom_ring, ring_match_tolerance);
            bottom_ring_vids_ptr = &bottom_ring_vids;
        }

        cinolib::Trimesh<> cylinder = attachExtrudedWalls(remeshed_surface, wells[i], cylinder_edge_length, top_ring, top_ring_vids, bottom_ring_vids_ptr, verbose);
        cylinder_meshes.push_back(cylinder);

        std::string cylinder_filename = "cylinder_" + std::to_string(i + 1) + ".obj";
        if (saveMesh(cylinder, cylinder_filename) && verbose)
        {
            std::cout << "Cylinder " << (i + 1) << " mesh saved to: " << cylinder_filename << std::endl;
        }
    }

    cinolib::Trimesh<> all_cylinders_mesh;
    if (cylinder_meshes.empty())
    {
        all_cylinders_mesh = cinolib::Trimesh<>();
    }
    else if (cylinder_meshes.size() == 1)
    {
        all_cylinders_mesh = cylinder_meshes[0];
    }
    else
    {
        all_cylinders_mesh = mergeMeshes(cylinder_meshes[0], std::vector<cinolib::Trimesh<>>(cylinder_meshes.begin() + 1, cylinder_meshes.end()));
    }

    std::string cylinders_filename = "cylinders.obj";
    if (saveMesh(all_cylinders_mesh, cylinders_filename))
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

    cinolib::Trimesh<> result_mesh = remeshed_surface;
    if (saveMesh(result_mesh, output_file) && verbose)
    {
        std::cout << "Combined mesh saved to: " << output_file << std::endl;
    }

    if (generate_tet)
    {
        if (verbose)
        {
            std::cout << "Generating tetrahedral mesh directly with TetGen on the remeshed surface..." << std::endl;
            std::cout << "#V = " << result_mesh.num_verts() << ", #F = " << result_mesh.num_polys() << std::endl;
        }

        cinolib::Tetmesh<> tet_mesh;
        try
        {
            if (verbose)
            {
                std::cout << "Running TetGen tetrahedral meshing with flags: " << tetgen_flags << std::endl;
            }

            cinolib::tetgen_wrap(result_mesh, tetgen_flags, tet_mesh);

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
        std::cout << "  - Cylinder " << (i + 1) << ": cylinder_" << (i + 1) << ".obj" << std::endl;
    }
    std::cout << "  - Combined mesh: " << output_file << std::endl;
    std::cout << "  - Combined cylinders: cylinders.obj" << std::endl;

    return 0;
}
