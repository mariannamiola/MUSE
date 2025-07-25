#ifndef LOAD_XYZ_H
#define LOAD_XYZ_H

#include <string>
#include <vector>
#include "muselib/data_structures/point.h"

using namespace MUSE;

int extractNumber   (const std::string& filename);
int load1d_xyzfile  (const std::string filename, std::vector<double> &points);
int load_xyzfile    (const std::string filename, std::vector<Point3D> &points);
int load_xyzfile    (const std::string filename, std::vector<double> &vecx, std::vector<double> &vecy, std::vector<double> &vecz);

void export1d_xyz   (const std::string filename, std::vector<double> &points, const int &precision = 9);
void export2d_xyz   (const std::string filename, std::vector<Point2D> &points, const int &precision = 9);
void export3d_xyz   (const std::string filename, std::vector<Point3D> &points, const int &precision = 9);
void export3d_xyz   (const std::string filename, const std::vector<double> &vecx, const std::vector<double> &vecy, const std::vector<double> &vecz, const int &precision = 9);
void export_idxyz   (const std::string filename, std::vector<Point3D> &points, const int &precision = 9);
void export_idxyzv  (const std::string filename, std::vector<double> &x, std::vector<double> &y, std::vector<double> &z, std::vector<double> &v, const int &precision = 9);
void export_idxyzv  (const std::string filename, std::vector<std::string> &id, std::vector<double> &x, std::vector<double> &y, std::vector<double> &z, std::vector<double> &v, const int &precision = 9);


#ifndef STATIC_MUSELIB
#include "load_xyz.cpp"
#endif

#endif // LOAD_XYZ_H
