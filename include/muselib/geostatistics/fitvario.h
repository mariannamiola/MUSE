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

enum weightsType
{
    // original Cressie weights: N(h) / [gamma_exp(h)]^2, reduces influence of
    // highly variable experimental lags (Cressie 1985, p. 90)
    CRESSIE,
    
    // variant used by Pebesma/gstat: N(h) / h^2 (number of pairs over squared lag);
    // emphasizes small lags by distance rather than semivariance
    CRESSIE_WEIGHTED,
    
    // modified version: N(h) / h  (less aggressive distance weighting)
    CRESSIE_WEIGHTED_MODIFIED
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


// helper for fitting weights
static double compute_weight(const exp_variog &ev, size_t idx, weightsType w_type);


//FITTING - OMNIDIRECTIONAL
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/// @brief fitting experimental variogram: solution with minimum error varying on range, nugget and model type
variogram   fit_variogram (const exp_variog &ev, const double &range_precision, const double &nugget_precision, weightsType w_type); //ok

/// @brief fitting experimental variogram: solution with minimum error varying on range and nugget - model type fixed
variogram   fit_variogram (const exp_variog &ev, const double &range_precision, const double &nugget_precision, variogram_type model_type, weightsType w_type); //ok

/// @brief fitting experimental variogram: solution with minimum error varying on range and model type - nugget fixed
variogram   fit_variogram_1par (const exp_variog &ev, const double &range_precision, const double &nugget, weightsType w_type); //OK

/// @brief fitting experimental variogram: solution with minimum error varying on range - nugget and model type fixed
variogram   fit_variogram  (const exp_variog &ev, const double &range_precision, variogram_type model_type, const double &nugget, bool is_print, weightsType w_type); //OK

/// @brief fitting experimental variogram: solution with minimum error varying on range and nugget - model type fixed - output with error (mse)
MUSE::VarioError fit_variogram_mse (const exp_variog &ev, const double &range_precision, const double &nugget_precision, variogram_type model_type, bool is_print, weightsType w_type); //ok

/// @brief fitting experimental variogram: solution with minimum error varying on range - nugget and model type fixed - output with error (mse)
MUSE::VarioError fit_variogram_mse_1par (const exp_variog &ev, const double &range_precision, const double &nugget, variogram_type model_type, bool is_print, weightsType w_type); //OK

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//FITTING - DIRECTIONAL
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/// @brief loop on directions for fitting variograms with minimum error varying on range and nugget - model type fixed - output with error (mse) for each direction
vector<MUSE::VarioError>    fit_dir_variogram   (const std::vector<exp_variog> &dev, const double & degree_step, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, variogram_type &model_type, weightsType w_type);

/// @brief loop on directions for fitting variograms with minimum error varying on range and nugget - model type fixed - output with error (mse) for each direction
vector<MUSE::VarioError>    fit_dir_variogram   (const std::vector<exp_variog> &dev, const std::vector<double> &seq, const double &range_precision, const double &nugget_precision, variogram_type &model_type, weightsType w_type);

/// @brief loop on directions for fitting variograms with minimum error varying on range - nugget and model type fixed - output with error (mse) for each direction
vector<MUSE::VarioError> fit_dir_variogram_1par (const std::vector<exp_variog> &dev, const std::vector<double> &seq, const double &range_precision, const double &nugget, variogram_type &model_type, bool is_print, weightsType w_type);

/// @brief loop on directions for fitting variograms with minimum error varying on range - nugget and model type fixed
vector<variogram>           fit_dir_variogram   (const std::vector<exp_variog> &dev, const double & degree_step, const double & degree_tolerance, const double &range_precision, variogram_type &model_type, double &nugget, weightsType w_type);

/// \brief loop on directions for fitting variograms with minimum error varying on range - nugget and model type fixed - with discrete directions
vector<variogram>           fit_dir_variogram   (const std::vector<exp_variog> &dev, const std::vector<double> &seq, const double &range_precision, variogram_type &model_type, const double &nugget, bool is_print, weightsType w_type);

/// @brief fitting variogram for each direction with minimum error varying on range, nugget and model type - with automatic selection of directions - with selection of best model based on MSE on all directions and weighted average of nugget values for best model
vector<variogram>           fit_dir_variogram   (const std::vector<exp_variog> &dev, const double & degree_step, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, const std::vector<double> &weigth, bool is_print, weightsType w_type);

/// \brief fit_dir_variogram ranging on models and nugget - with directions and final average nugget
vector<variogram>           fit_dir_variogram   (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, const std::vector<double> &weigth, bool is_print, weightsType w_type);

/// @brief Fit directional variogram with ranging on models and fixed nugget
vector<variogram>           fit_dir_variogram   (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, variogram_type &type, const std::vector<double> &weigth, bool is_print, weightsType w_type); //ok

/// \brief fit_dir_variogram computes directional variogram fitting, with model type fixed
vector<variogram>           fit_dir_variogram   (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget, bool is_print, weightsType w_type); //ok

/// @brief fitting variogram for each direction with minimum error varying on range, nugget and model type - with selection of best model based on MSE on all directions and weighted average of nugget values for best model - with discrete directions
vector<variogram>           fit_dir_variogram (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, const std::string &type, const std::vector<double> &weight, bool is_print, weightsType w_type); //ok


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void fit_anisotropy_ellipse                     (const std::vector<double> &x, const std::vector<double> &y, MUSE::EllipseParameter &ellipse_par);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::




#ifndef STATIC_MUSELIB
#include "fitvario.cpp"
#endif

#endif // FITVARIO_H
