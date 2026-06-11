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
        double center_x = 0.0;
        double center_y = 0.0;

        double phi_rad = 0.0;

        double max_direction = 0.0;
        double min_direction = 0.0;

        double max_semiaxis = 0.0;
        double min_semiaxis = 0.0;


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

    // Parameters of the 3D anisotropy ellipsoid fitted on directional variogram ranges.
    // The ellipsoid is centered at the origin (variogram ranges are symmetric by construction)
    // and is described by 3 semi-axes plus 3 rotation angles (azimuth, roll, pitch).
    // The angle convention matches GSLIB-like setrot() in geostatslib/statistics/data_structures.h:
    // - azimuth: direction (degree, clockwise from North/Y axis) of the major axis projection on XY;
    // - roll:    inclination (degree) of the major axis above the horizontal plane (ang2 of setrot);
    // - pitch:   rotation (degree) of the two secondary axes around the major axis (ang3 of setrot).
    // Semi-axes are mapped on the same slots used by variogram_methods/Variogram:
    // - max_semiaxis -> range_max (major axis);
    // - min_semiaxis -> range_min (secondary axis, the most horizontal one);
    // - z_semiaxis   -> range_z   (secondary axis, the most vertical one).
    struct EllipsoidParameter
    {
        // Semi-axes length (same measure units of the input coordinates)
        double max_semiaxis = 0.0;
        double min_semiaxis = 0.0;
        double z_semiaxis   = 0.0;

        // Rotation angles (degree) following the setrot() convention described above
        double azimuth = 0.0;
        double roll    = 0.0;
        double pitch   = 0.0;

        // Unit vectors (world coordinates) of the three principal axes
        std::vector<double> max_axis_dir = {0.0, 0.0, 0.0};
        std::vector<double> min_axis_dir = {0.0, 0.0, 0.0};
        std::vector<double> z_axis_dir   = {0.0, 0.0, 0.0};

        // Mean absolute residual of the quadric fit (0 = perfect fit) and validity flag
        double fit_residual = 0.0;
        bool   is_valid = false;

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(max_semiaxis));
            ar (CEREAL_NVP(min_semiaxis));
            ar (CEREAL_NVP(z_semiaxis));

            ar (CEREAL_NVP(azimuth));
            ar (CEREAL_NVP(roll));
            ar (CEREAL_NVP(pitch));

            ar (CEREAL_NVP(max_axis_dir));
            ar (CEREAL_NVP(min_axis_dir));
            ar (CEREAL_NVP(z_axis_dir));

            ar (CEREAL_NVP(fit_residual));
            ar (CEREAL_NVP(is_valid));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(max_semiaxis));
            ar (CEREAL_NVP(min_semiaxis));
            ar (CEREAL_NVP(z_semiaxis));

            ar (CEREAL_NVP(azimuth));
            ar (CEREAL_NVP(roll));
            ar (CEREAL_NVP(pitch));

            ar (CEREAL_NVP(max_axis_dir));
            ar (CEREAL_NVP(min_axis_dir));
            ar (CEREAL_NVP(z_axis_dir));

            ar (CEREAL_NVP(fit_residual));
            ar (CEREAL_NVP(is_valid));
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

/// @brief weighted mean squared error of a fitted variogram model against an experimental
/// variogram, with the same point-validity rule and weights used inside the fitting loops.
/// Normalized by the sum of the weights, so that the score is comparable across
/// experimental variograms with different numbers of lags and of point pairs.
double variogram_fit_wmse                       (const exp_variog &ev, const variogram &v, weightsType w_type);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void fit_anisotropy_ellipse                     (const std::vector<double> &x, const std::vector<double> &y, MUSE::EllipseParameter &ellipse_par);

/// @brief fit a 3D anisotropy ellipsoid (centered at the origin) on directional variogram range points
void fit_anisotropy_ellipsoid                   (const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z, MUSE::EllipsoidParameter &ellipsoid_par);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::




#ifndef STATIC_MUSELIB
#include "fitvario.cpp"
#endif

#endif // FITVARIO_H
