#ifndef POINT_H
#define POINT_H

#include <string>
#include <vector>
#include <iostream>

namespace MUSE
{
    class Point2D;
    class Point3D;
}

class MUSE::Point2D
{
    public:

        std::string id;
        double x;
        double y;
};

class MUSE::Point3D
{
    public:

        std::string id;
        double x;
        double y;
        double z;
        int index = 0;
        //Point3D(double a, double b, double c): x(a), y(b), z(c) {}
};


double  dist         (MUSE::Point2D p0, MUSE::Point2D p1);
double  sqrt_dist    (MUSE::Point2D p0, MUSE::Point2D p1);

double  dist3D       (MUSE::Point3D p0, MUSE::Point3D p1);
bool    comparePoint (MUSE::Point3D p0, MUSE::Point3D p1);

bool    equalPoint   (MUSE::Point3D p0, MUSE::Point3D p1);

MUSE::Point3D rotPoint (MUSE::Point3D p0, const std::string &rot_axis, const double &rot_angle);

std::vector<MUSE::Point3D> remove_duplicates (std::vector<MUSE::Point3D> &points, double threshold = 1e-6);
void remove_duplicates (std::vector<MUSE::Point3D> &points, std::vector<MUSE::Point3D> &unique_points, std::vector<int> &unique_points_id, double threshold = 1e-6);
void remove_duplicates (const std::vector<double> &points_x, const std::vector<double> &points_y, std::vector<int> &unique_points_id, std::vector<double> &unique_points_x, std::vector<double> &unique_points_y, double threshold = 1e-6);


MUSE::Point2D linear_interpolation (MUSE::Point2D p0, MUSE::Point2D p1, const double &t);
MUSE::Point2D pixel2world ( const int& x, const int& y, const double &raster_width, const double &raster_height);


#ifndef STATIC_MUSELIB
#include "point.cpp"
#endif


#endif // POINT_H
