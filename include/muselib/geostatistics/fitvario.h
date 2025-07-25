#ifndef FITVARIO_H
#define FITVARIO_H

#include <iostream>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

//for include INT_MAX
#ifdef __linux__
    #include <bits/stdc++.h>
#endif

#include <ellipse_fit.h>

#include <geostatslib/statistics/data_structures.h>
#include <geostatslib/statistics/variogram.h>

enum add_variogram_type{
    NUGGET,
    LINEAR_WITH_SILL,
    CIRCULAR,
    PENTASHPERICAL,
    BESSEL,
    LOGARITHMIC,
    POWER,
    PERIODIC,
    HOLE_EFFECT
};

namespace MUSE {

    struct VarioError
    {
        variogram vario;
        double mse;
    };

    struct EllipseParameter
    {
        double center_x;
        double center_y;

        double phi_rad;

        double max_direction;
        double min_direction;

        double max_semiaxis;
        double min_semiaxis;


        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(center_x));
            ar (CEREAL_NVP(center_y));

            ar (CEREAL_NVP(phi_rad));

            ar (CEREAL_NVP(max_direction));
            ar (CEREAL_NVP(min_direction));

            ar (CEREAL_NVP(max_semiaxis));
            ar (CEREAL_NVP(min_semiaxis));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(center_x));
            ar (CEREAL_NVP(center_y));

            ar (CEREAL_NVP(phi_rad));

            ar (CEREAL_NVP(max_direction));
            ar (CEREAL_NVP(min_direction));

            ar (CEREAL_NVP(max_semiaxis));
            ar (CEREAL_NVP(min_semiaxis));
        }
        #endif
    };
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double      get_gamma                           (const double &h, const double &r, const double &c0, const double &c, variogram_type model_type);
double      get_gamma                           (const double &h, const double &a, const double &c0, const double &c, add_variogram_type model_type);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//FITTING - OMNIDIRECTIONAL
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

variogram   fit_variogram                       (const exp_variog &ev, const double &range_precision, const double &nugget_precision);
variogram   fit_variogram                       (const exp_variog &ev, const double &range_precision, const double &nugget_precision, variogram_type model_type);

variogram   fit_variogram_1par                  (const exp_variog &ev, const double &range_precision, const double &nugget);
variogram   fit_variogram                       (const exp_variog &ev, const double &range_precision, variogram_type model_type, const double &nugget, bool is_print = false);

MUSE::VarioError            fit_variogram_mse   (const exp_variog &ev, const double &range_precision, const double &nugget_precision, variogram_type model_type, bool is_print = false);
MUSE::VarioError fit_variogram_mse_1par (const exp_variog &ev, const double &range_precision, const double &nugget, variogram_type model_type, bool is_print);



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//FITTING - DIRECTIONAL
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

vector<MUSE::VarioError>    fit_dir_variogram   (const std::vector<exp_variog> &dev, const double & degree_step, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, variogram_type &model_type);
vector<MUSE::VarioError>    fit_dir_variogram   (const std::vector<exp_variog> &dev, const std::vector<double> &seq, const double &range_precision, const double &nugget_precision, variogram_type &model_type);
vector<MUSE::VarioError> fit_dir_variogram_1par (const std::vector<exp_variog> &dev, const std::vector<double> &seq, const double &range_precision, const double &nugget, variogram_type &model_type, bool is_print = false);

vector<variogram>           fit_dir_variogram   (const std::vector<exp_variog> &dev, const double & degree_step, const double & degree_tolerance, const double &range_precision, variogram_type &model_type, double &nugget);
vector<variogram>           fit_dir_variogram   (const std::vector<exp_variog> &dev, const std::vector<double> &seq, const double &range_precision, variogram_type &model_type, const double &nugget, bool is_print = false);


vector<variogram>           fit_dir_variogram   (const std::vector<exp_variog> &dev, const double & degree_step, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, const std::vector<double> &weigth, bool is_print = false);
vector<variogram>           fit_dir_variogram   (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, const std::vector<double> &weigth, bool is_print);
vector<variogram>           fit_dir_variogram   (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, variogram_type &type, const std::vector<double> &weigth, bool is_print);

vector<variogram>           fit_dir_variogram   (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget, bool is_print);

vector<variogram>           fit_dir_variogram (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, const std::string &type, const std::vector<double> &weight, bool is_print);


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void fit_anisotropy_ellipse                     (const std::vector<double> &x, const std::vector<double> &y, MUSE::EllipseParameter &ellipse_par);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::




#ifndef STATIC_MUSELIB
#include "fitvario.cpp"
#endif

#endif // FITVARIO_H
