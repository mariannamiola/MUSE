#ifndef PLANE_H
#define PLANE_H

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <muselib/data_structures/point.h>

using namespace MUSE;

struct fittedPlane
{
    double meanX;
    double meanY;
    double meanZ;
    double meanA0;
    double meanA1;
    double b;
};



fittedPlane fitPlane(const std::vector<Point3D> &points);

//void OK_interpolation (const std::vector<double> &residual, const std::vector<double> &coord_x, const std::vector<double> &coord_y, const std::vector<double> &coord_z,
  //                    std::vector<point3d> &points_to_est);

#ifndef STATIC_MUSELIB
#include "plane.cpp"
#endif

#endif // PLANE_H
