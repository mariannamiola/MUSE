#include "load_xyz.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>

#include "muselib/colors.h"

#define IOSUCCESS 0
#define IOERROR 1



// Function to extract the numeric part of the filename
int extractNumber(const std::string& filename)
{
    size_t pos1 = filename.find_last_of('_') + 1;
    size_t pos2 = filename.find_last_of('.');
    std::string num_str = filename.substr(pos1, pos2 - pos1);
    return std::stoi(num_str);  // Convert the number from string to int
}

int load1d_xyzfile (const std::string filename, std::vector<double> &points)
{
    points.clear();
    std::ifstream file_in;
    file_in.open(filename, std::fstream::out);
    if(!file_in.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        return IOERROR;
    }

    double x;
    while(file_in >> x)
        points.push_back(x);

    file_in.close();
    std::cout << FGRN("Loading xyz file: ") << filename << FGRN(" ... COMPLETED.") << std::endl;

    return IOSUCCESS;
}


int load_xyzfile (const std::string filename, std::vector<Point3D> &points)
{
    points.clear();
    std::ifstream file_in;
    file_in.open(filename, std::fstream::out);
    if(!file_in.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        return IOERROR;
    }

    double x,y,z;
    while(file_in >> x >> y >> z)
    {
        Point3D point;
        point.x = x;
        point.y = y;
        point.z = z;

        points.push_back(point);
    }
    file_in.close();
    std::cout << "Loading xyz file: " << filename << " ... COMPLETED." << std::endl;

    return IOSUCCESS;
}


int load_xyzfile (const std::string filename, std::vector<double> &vecx, std::vector<double> &vecy, std::vector<double> &vecz)
{
    // vecx.clear();
    // vecy.clear();
    // vecz.clear();

    std::ifstream file_in;
    file_in.open(filename, std::fstream::out);
    if(!file_in.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        return IOERROR;
    }

    double x,y,z;
    while(file_in >> x >> y >> z)
    {
//        Point3D point;
//        point.x = x;
//        point.y = y;
//        point.z = z;

        //std::cout << x << " " << y << " " << z << std::endl;

        vecx.push_back(x);
        vecy.push_back(y);
        vecz.push_back(z);

    }
    file_in.close();
    std::cout << "Loading xyz file: " << filename << " ... COMPLETED." << std::endl;

    return IOSUCCESS;
}


void export1d_xyz (const std::string filename, std::vector<double> &points, const int &precision)
{
    std::ofstream file_out;
    file_out.open(filename, std::fstream::out);
    if(!file_out.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        exit(1);
    }
    else
    {
        for(size_t i=0; i<points.size(); i++)
            file_out << std::fixed << std::setprecision(precision) << points.at(i) << std::endl;
        file_out.close();
    }
}

void export2d_xyz (const std::string filename, std::vector<Point2D> &points, const int &precision)
{
    std::ofstream file_out;
    file_out.open(filename, std::fstream::out);
    if(!file_out.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        exit(1);
    }
    else
    {
        for(size_t i=0; i<points.size(); i++)
            file_out << std::fixed << std::setprecision(precision) << points.at(i).x << " " << points.at(i).y << std::endl;
        file_out.close();
    }
}

void export3d_xyz (const std::string filename, std::vector<Point3D> &points, const int &precision)
{
    std::ofstream file_out;
    file_out.open(filename, std::fstream::out);
    if(!file_out.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        exit(1);
    }
    else
    {
        for(size_t i=0; i<points.size(); i++)
            file_out << std::fixed << std::setprecision(precision) << points.at(i).x << " " << points.at(i).y << " " << points.at(i).z << std::endl;
        file_out.close();
    }
}

void export3d_xyz (const std::string filename, const std::vector<double> &vecx, const std::vector<double> &vecy, const std::vector<double> &vecz, const int &precision)
{
    std::ofstream file_out;
    file_out.open(filename, std::fstream::out);
    if(!file_out.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        exit(1);
    }
    else
    {
        for(size_t i=0; i<vecx.size(); i++)
            file_out << std::fixed << std::setprecision(precision) << vecx.at(i) << " " << vecy.at(i) << " " << vecz.at(i) << std::endl;
        file_out.close();
    }
}

void export_idxyz (const std::string filename, std::vector<Point3D> &points, const int &precision)
{
    std::ofstream file_out;
    file_out.open(filename, std::fstream::out);
    if(!file_out.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        exit(1);
    }
    else
    {
        for(size_t i=0; i<points.size(); i++)
            file_out << std::fixed << std::setprecision(precision) << points.at(i).index << " " << points.at(i).x << " " << points.at(i).y << " " << points.at(i).z << std::endl;
        file_out.close();
    }
}

void export_idxyzv (const std::string filename, std::vector<double> &x, std::vector<double> &y, std::vector<double> &z, std::vector<double> &v, const int &precision)
{
    std::ofstream file_out;
    file_out.open(filename, std::fstream::out);
    if(!file_out.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        exit(1);
    }
    else
    {
        for(size_t i=0; i<x.size(); i++)
            file_out << std::fixed << std::setprecision(precision) << x.at(i) << " " << y.at(i) << " " << z.at(i) << " " << v.at(i) << std::endl;
        file_out.close();
    }
}

void export_idxyzv (const std::string filename, std::vector<std::string> &id, std::vector<double> &x, std::vector<double> &y, std::vector<double> &z, std::vector<double> &v, const int &precision)
{
    std::ofstream file_out;
    file_out.open(filename, std::fstream::out);
    if(!file_out.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        exit(1);
    }
    else
    {
        for(size_t i=0; i<id.size(); i++)
            file_out << std::fixed << std::setprecision(precision) << id.at(i) << " " << x.at(i) << " " << y.at(i) << " " << z.at(i) << " " << v.at(i) << std::endl;
        file_out.close();
    }
}
