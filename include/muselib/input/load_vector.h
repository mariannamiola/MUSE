#ifndef LOAD_VECTOR_H
#define LOAD_VECTOR_H

#include <string>
#include <vector>

#include "muselib/data_structures/point.h"

#define IOSUCCESS 0
#define IOERROR 1

using namespace MUSE;

int load_vectorfile             (const std::string filename, std::vector<Point3D> &boundary, std::vector<std::vector<Point3D>> &points, std::string &geometryType);

// Loading ShapeFile (.shp)
int load_shapefile_shp          (const std::string filename, std::vector<std::vector<Point3D>> &boundary, std::vector<std::vector<Point3D>> &points, std::string &geometry_type);
int load_shapefile_xyz          (const std::string filename, std::vector<std::vector<Point3D>> &boundaries);

int getnPoints_shapefile_shp    (const std::string filename);

// Loading GeoPackage File (.gpkg)
int load_gpkg                   (const std::string filename, std::vector<std::vector<Point3D>> &boundary, std::vector<std::vector<Point3D>> &points, std::string &geometry_type);
//int export_gpkg_attributes_to_csv(const std::string filename, const string &csv_filename);

int export_attributes_to_csv(const std::string filename, const std::string &csv_filename);



#ifndef STATIC_MUSELIB
#include "load_vector.cpp"
#endif

#endif // LOAD_VECTOR_H
