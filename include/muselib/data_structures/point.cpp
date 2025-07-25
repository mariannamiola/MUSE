#include "point.h"

#include <math.h>

double dist (MUSE::Point2D p0, MUSE::Point2D p1)
{
    double d = 0.0;
    d = pow((p1.x-p0.x),2) + pow((p1.y-p0.y),2);

    return d;
}

double sqrt_dist (MUSE::Point2D p0, MUSE::Point2D p1)
{
    double d = 0.0;
    d = pow(pow((p1.x-p0.x),2) + pow((p1.y-p0.y),2), 0.5);

    return d;
}

double dist3D (MUSE::Point3D p0, MUSE::Point3D p1)
{
    double d = 0.0;
    d = pow((p1.x-p0.x),2) + pow((p1.y-p0.y),2) + pow((p1.z-p0.z),2);

    return d;
}

bool comparePoint (MUSE::Point3D p0, MUSE::Point3D p1)
{
    if (p0.x != p1.x)
        return p0.x > p1.x;
    else if (p0.y != p1.y)
        return  p0.y > p1.y;
    else
        return p0.z > p1.z;
}

bool equalPoint (MUSE::Point3D p0, MUSE::Point3D p1)
{
    if (dist3D(p0, p1) < 1e-6)
        return true;
    return false;
}

MUSE::Point3D rotPoint (MUSE::Point3D p0, const std::string &rot_axis, const double &rot_angle)
{
    MUSE::Point3D p;
    double rad = (rot_angle * M_PI)/180;

    if(rot_axis.compare("X") == 0)
    {
        //std::cout << "Rotation on X axis" << std::endl;

        p.x = p0.x;
        p.y = p0.y * cos(rad) - p0.z * sin(rad);
        p.z = p0.y * sin(rad) + p0.z * cos(rad);
    }
    else if(rot_axis.compare("Y") == 0)
        std::cout << "Rotation on Y axis" << std::endl;

    else if(rot_axis.compare("Z") == 0)
        std::cout << "Rotation on Z axis" << std::endl;

    return p;
}

std::vector<MUSE::Point3D> remove_duplicates (std::vector<MUSE::Point3D> &points, double threshold)
{
    std::vector<MUSE::Point3D> sorted_points = points;
    std::vector<MUSE::Point3D> unique_points;

    unique_points.push_back(sorted_points.at(0));
    for (uint i=1; i < sorted_points.size(); i++)
    {
        MUSE::Point2D p0, p1;
        p0.x = sorted_points.at(i-1).x;
        p0.y = sorted_points.at(i-1).y;
        p1.x = sorted_points.at(i).x;
        p1.y = sorted_points.at(i).y;

        if (dist(p0, p1) > threshold)
            unique_points.push_back(sorted_points.at(i));
    }

    std::cout << "Removing duplicated points ... COMPLETED. " << unique_points.size() << " left" << std::endl;
    return unique_points;
}


void remove_duplicates (std::vector<MUSE::Point3D> &points, std::vector<MUSE::Point3D> &unique_points, std::vector<int> &unique_points_id, double threshold)
{
    std::vector<MUSE::Point3D> sorted_points = points;

    unique_points.push_back(sorted_points.at(0));
    for (uint i=1; i < sorted_points.size(); i++)
    {
        MUSE::Point2D p0, p1;
        p0.x = sorted_points.at(i-1).x;
        p0.y = sorted_points.at(i-1).y;
        p1.x = sorted_points.at(i).x;
        p1.y = sorted_points.at(i).y;

        if (dist(p0, p1) > threshold)
        {
            unique_points.push_back(sorted_points.at(i));
            unique_points_id.push_back(i);
        }
    }

    std::cout << "Removing duplicated points ... COMPLETED. " << unique_points.size() << " left" << std::endl;

}


void remove_duplicates (const std::vector<double> &points_x, const std::vector<double> &points_y, std::vector<int> &unique_points_id, std::vector<double> &unique_points_x, std::vector<double> &unique_points_y, double threshold)
{
    unique_points_x.push_back(points_x.at(0));
    unique_points_y.push_back(points_y.at(0));
    unique_points_id.push_back(0);

    for (uint i=1; i < points_x.size(); i++)
    {
        MUSE::Point2D p0, p1;
        p0.x = points_x.at(i-1);
        p0.y = points_y.at(i-1);
        p1.x = points_x.at(i);
        p1.y = points_y.at(i);

        if (dist(p0, p1) > threshold)
        {
            unique_points_x.push_back(points_x.at(i));
            unique_points_y.push_back(points_y.at(i));
            unique_points_id.push_back(i);
        }
    }

    std::cout << "Removing duplicated points ... COMPLETED. " << unique_points_x.size() << " left" << std::endl;
}


MUSE::Point2D linear_interpolation (MUSE::Point2D p0, MUSE::Point2D p1, const double &t)
{
    MUSE::Point2D p;
    p.x = ((1-t)*p0.x) + (t*p1.x);
    p.y = ((1-t)*p0.y) + (t*p1.y);

    return p;
}

//// convert the pixel coordinate to lat/lon coordinates
//MUSE::Point2D pixel2world ( const int& x, const int& y, const double &raster_width, const double &raster_height)
//{
//    // compute the ratio of the pixel location to its dimension
//    double rx = (double)x / raster_width;
//    double ry = (double)y / raster_height;

//    // compute LERP of each coordinate
//    MUSE::Point2D rightSide = linear_interpolation(tr, br, ry);
//    MUSE::Point2D leftSide  = linear_interpolation(tl, bl, ry);

//    // compute the actual Lat/Lon coordinate of the interpolated coordinate
//    return linear_interpolation( leftSide, rightSide, rx );
//}

