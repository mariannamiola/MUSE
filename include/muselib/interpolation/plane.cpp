#include "plane.h"

#include <geostatslib/statistics/data_structures.h>
#include <geostatslib/statistics/normal_score.h>
//#include <geostatslib/statistics/ordinary_kriging.h>

#include "muselib/geostatistics/vario.h"
#include "muselib/geostatistics/fitvario.h"
//#include "muselib/geostatistics/utils.h"

using namespace MUSE;

fittedPlane fitPlane(const std::vector<Point3D> &points)
{
    fittedPlane parameters;

    std::cout << "Fitting points by a Plane in 3 Dimensions ..." << std::endl;

    //Compute the mean of the points
    std::cout << "N. points = " << points.size() << std::endl;

    Point3D mean;
    mean.x = 0.0;
    mean.y = 0.0;
    mean.z = 0.0;

    for(unsigned int i=0; i<points.size(); i++)
    {
        mean.x+=points.at(i).x;
        mean.y+=points.at(i).y;
        mean.z+=points.at(i).z;
    }

    mean.x=mean.x/points.size();
    mean.y=mean.y/points.size();
    mean.z=mean.z/points.size();
    std::cout << "Mean-point = " << mean.x << "; " << mean.y << "; " << mean.z << std::endl;

    //Compute the linear system matrix and vector elements
    double sum_xx=0.0, sum_xy=0.0, sum_xz=0.0, sum_yy=0.0, sum_yz=0.0;

    for(unsigned int i=0; i< points.size(); i++)
    {
        Point3D d;
        d.x = points.at(i).x-mean.x;
        d.y = points.at(i).y-mean.y;
        d.z = points.at(i).z-mean.z;

        sum_xx += d.x*d.x;
        sum_xy += d.x*d.y;
        sum_xz += d.x*d.z;
        sum_yy += d.y*d.y;
        sum_yz += d.y*d.z;
    }

    //Solve the linear system
    double det = sum_xx*sum_yy - sum_xy*sum_xy;
    if(det!=0)
    {
        //Compute the fitted plane h(x,y) = meanZ + meanA0*(x-meanX) + meanA1*(y-meanY)
        parameters.meanX = mean.x;
        parameters.meanY = mean.y;
        parameters.meanZ = mean.z;

        parameters.meanA0 = (sum_yy*sum_xz - sum_xy*sum_yz)/det;
        parameters.meanA1 = (sum_xx*sum_yz - sum_xy*sum_xz)/det;
        parameters.b = parameters.meanZ - parameters.meanA0 * parameters.meanX - parameters.meanA1 * parameters.meanY;
    }
    else
    {
        parameters.meanX = 0;
        parameters.meanY = 0;
        parameters.meanZ = 0;

        parameters.meanA0 = 0;
        parameters.meanA1 = 0;
        parameters.b = 0;
    }

    std::cout << "Plane coefficients: meanA0 = " << parameters.meanA0 << "; meanA1 = " << parameters.meanA1 <<  "; b = " << parameters.b << std::endl;

    std::cout << "Fitting simplificated points by a Plane in 3 Dimensions ... COMPLETED." << std::endl;

    return parameters;
}


// void OK_interpolation (const std::vector<double> &residual, const std::vector<double> &coord_x, const std::vector<double> &coord_y, const std::vector<double> &coord_z, std::vector<point3d> &points_to_est)
// {
//     //DATA PREPARATION
//     normalscore normal_residual = normal_score(residual);
//     std::vector<point3d> res_input;
//     for(uint i=0; i< normal_residual.values.size(); i++)
//         res_input.push_back(point3d({coord_x.at(i), coord_y.at(i), coord_z.at(i)}, {normal_residual.values.at(i)}));

//     //EXPERIMENTAL VARIOGRAM COMPUTATION
//     exp_variog exp_vario = exp_variogram (normal_residual.values, coord_x, coord_y, coord_z, "VARIABLE", 1.0, 15, -DBL_MAX, 2.0);
//     exp_vario = clean_experimental_variogram(exp_vario, 10);

//     //FITTING VARIOGRAM
//     std::cout << "Automatic fitting of experimental variogram ... " << std::endl;
//     variogram fitted_exp_vario = fit_variogram(exp_vario, 100.0, 100.0);

//     Variogram Vario;
//     Vario.set_range(fitted_exp_vario.range);
//     Vario.nugget = fitted_exp_vario.nugget;
//     Vario.sill = fitted_exp_vario.sill - fitted_exp_vario.nugget;
//     //MODIFICATO COME 1 - NUGGET!!!!!!!!!!!!!!!!!
//     Vario.set_azimuth(fitted_exp_vario.get_azimuth());

//     variogram_type type;
//     convert_from_str(fitted_exp_vario.type, type);
//     Vario.type = type;

//     covariance covar = covariance(Vario);

//     //ordinary_kriging(points_to_est.size(), points_to_est, res_input, covar, fitted_exp_vario.range, 10, false);
// }
