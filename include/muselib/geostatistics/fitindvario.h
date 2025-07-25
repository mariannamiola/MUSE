#ifndef FITINDVARIO_H
#define FITINDVARIO_H

#include <iostream>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include <geostatslib/statistics/data_structures.h>
#include <geostatslib/statistics/variogram.h>

#include "fitvario.h"


//INDICATOR FITTING - OMNIDIRECTIONAL
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

variogram        fit_ind_variogram          (const exp_variog &ev,  const double &range_step,   const double &nugget_step,  const double &variance);
variogram        fit_ind_variogram_1par     (const exp_variog &ev,  const double &range_step,   const double &nugget,       const double &variance);
variogram        fit_ind_variogram_1par     (const exp_variog &ev,  const double &range_step,   const double &nugget_step,  const double &variance,     variogram_type &model_type);
variogram        fit_ind_variogram_2par     (const exp_variog &ev,  const double &range_step,   const double &nugget,       const double &variance,     variogram_type &model_type,     bool is_print = false);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

MUSE::VarioError fit_ind_variogram_1par_mse (const exp_variog &ev,  const double &range_step,   const double &nugget_step,  const double &variance,     variogram_type &model_type,     bool is_print = false);
MUSE::VarioError fit_ind_variogram_2par_mse (const exp_variog &ev,  const double &range_step,   const double &nugget,       const double &variance,     variogram_type &model_type,     bool is_print = false);



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//INDICATOR FITTING - DIRECTIONAL
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

vector<variogram>        fit_ind_dir_variogram          (const std::vector<exp_variog> &dev,    const std::vector<double> &directions,  const double & degree_tolerance,    const double &range_step,   const double &nugget_step,  const double &variance,                                     const std::vector<double> &weigth,  bool is_print = false);
vector<variogram>        fit_ind_dir_variogram_1par     (const std::vector<exp_variog> &dev,    const std::vector<double> &directions,  const double & degree_tolerance,    const double &range_step,   const double &nugget,       const double &variance,                                                                         bool is_print = false);
vector<variogram>        fit_ind_dir_variogram_1par     (const std::vector<exp_variog> &dev,    const std::vector<double> &directions,  const double & degree_tolerance,    const double &range_step,   const double &nugget_step,  const double &variance,     variogram_type &model_type,     const std::vector<double> &weigth,  bool is_print = false);
vector<variogram>        fit_ind_dir_variogram_2par     (const std::vector<exp_variog> &dev,    const std::vector<double> &directions,                                      const double &range_step,   const double &nugget,       const double &variance,     variogram_type &model_type,                                         bool is_print = false);


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

vector<MUSE::VarioError> fit_ind_dir_variogram_1par_mse (const std::vector<exp_variog> &dev,    const std::vector<double> &directions,                                      const double &range_step,   const double &nugget_step,  const double &variance,     variogram_type &model_type);
vector<MUSE::VarioError> fit_ind_dir_variogram_2par_mse (const std::vector<exp_variog> &dev,    const std::vector<double> &directions,                                      const double &range_step,   const double &nugget,       const double &variance,     variogram_type &model_type,                                         bool is_print = false);


#ifndef STATIC_MUSELIB
#include "fitindvario.cpp"
#endif

#endif // FITINDVARIO_H
