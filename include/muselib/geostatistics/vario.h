#ifndef VARIO_H
#define VARIO_H

#include <iostream>
#include <geostatslib/statistics/data_structures.h>
#include <geostatslib/statistics/variogram.h>
#include <geostatslib/statistics/normal_score.h>


double get_degrees(const double &rad);
double get_radians(const double &deg);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double get_rangex (const double &h, const double &azimuth_deg); //degree (direction of variogram) = rotation angle starting from y axis
double get_rangey (const double &h, const double &azimuth_deg); //degree (direction of variogram) = rotation angle starting from y axis

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

normalscore normal_score2(const std::vector<double> &variable);
normalscore normal_score3(const std::vector<double> &input);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//Load experimental variogram (omnidirectional)
void load_exp_variogram (const std::string filename, exp_variog variogram);

//Compute experimental variogram (omnidirectional, costant lag)
exp_variog exp_variogram (const vector<double> &values, const vector<double> &coord_z, const int &n_points, const double &limit_dist, const double &tol_factor);
//exp_variog exp_variogram2D (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const int n_points, double max_dist, const double tol_factor);

//exp_variog exp_variogram (const vector<double> & values, const vector<double> & coord_x, const vector<double> & coord_y, const vector<double> & coord_z, const int n_points, double max_dist, const double tol_factor);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//Compute experimental variogram (omnidirectional, constant lag)
exp_variog exp_variogram_constlag (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const vector<double> &coord_z,
                                    const int &n_points,
                                    const double &limit_dist,
                                    const double &tol_factor);

//Compute experimental variogram (omnidirectional, variable lag)
exp_variog exp_variogram_varlag (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const vector<double> &coord_z,
                                    const double &step,
                                    const int &n_points,
                                    const double &limit_dist,
                                    const double &tol_factor);

exp_variog exp_variogram (const vector<double> & values, const vector<double> & coord_x, const vector<double> & coord_y, const vector<double> & coord_z,
                            const std::string &lagspac_type,
                            const double &step,
                            const int &n_points,
                            const double &limit_dist,
                            const double &tol_factor);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//Compute experimental variogram (directional, costant lag)
//std::vector<exp_variog> dir_exp_variogram_lagconst (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y,
                                                        // const double &degree_step,
                                                        // const double &degree_tolerance,
                                                        // const int n_points,
                                                        // const double &limit_dist,
                                                        // const double &tol_factor,
                                                        // bool compute_max = true);

std::vector<exp_variog> dir_exp_variogram_lagconst (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y,
                                                    const std::vector<double> &seq,
                                                    const double &degree_tolerance,
                                                    const double &bandwidth,
                                                    const int &n_points,
                                                    const double &limit_dist,
                                                    const double &tol_factor);

// std::vector<exp_variog> dir_exp_variogram_lagconst (const vector<double> &values, const vector<double> &coord_z,
//                                                     const std::vector<double> &seq,
//                                                     const double &degree_tolerance,
//                                                     const double &bandwidth,
//                                                     const int &n_points,
//                                                     const double &limit_dist,
//                                                     const double &tol_factor);

//Compute experimental variogram (directional, variable lag)
//std::vector<exp_variog> dir_exp_variogram_lagvar (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const double &degree_step, const double &degree_tolerance,
                                                    // const int n_points,
                                                    // const double &limit_dist,
                                                    // const double &tol_factor,
                                                    // const double &percent = 10,
                                                    // bool compute_max = true);

std::vector<exp_variog> dir3DH_exp_variogram (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const vector<double> &coord_z,
                                                    const std::string &lagspac_type,
                                                    const std::vector<double> &seq,
                                                    const double &hor_degree_tolerance,
                                                    const double &vert_degree_tolerance,
                                                    const double &hor_bandwidth,
                                                    const double &vert_bandwidth,
                                                    const double &step,
                                                    const int &n_points,
                                                    const double &limit_dist,
                                                    const double &tol_factor);

std::vector<exp_variog> dir3DV_exp_variogram (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const vector<double> &coord_z,
                                                    const std::string &lagspac_type, const double &spacing_samples,
                                                    const std::vector<double> &seq,
                                                    const double &vert_degree_tolerance,
                                                    const double &vert_bandwidth,
                                                    const double &step,
                                                    const int &n_points,
                                                    const double &limit_dist,
                                                    const double &tol_factor);

//Orthonormal basis (u = local "east", v = local "north", n = normal) of an arbitrary plane
//defined by dip azimuth (degree, clockwise from North) and dip (degree from horizontal)
void plane_basis (const double &dip_azimuth_deg, const double &dip_deg,
                  std::vector<double> &u, std::vector<double> &v, std::vector<double> &n);

//World unit vector of the in-plane direction theta (degree, clockwise from the local "north" v)
std::vector<double> plane_direction_vector (const double &dip_azimuth_deg, const double &dip_deg, const double &theta_deg);

//Compute directional experimental variograms on an arbitrary plane: samples are projected on the
//local plane frame, then the standard 3D-horizontal machinery (dir3DH_exp_variogram) is reused,
//where vertical tolerance/bandwidth act as off-plane tolerance/bandwidth
std::vector<exp_variog> dirPlane_exp_variogram (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const vector<double> &coord_z,
                                                    const double &dip_azimuth_deg,
                                                    const double &dip_deg,
                                                    const std::string &lagspac_type,
                                                    const std::vector<double> &seq,
                                                    const double &inplane_degree_tolerance,
                                                    const double &offplane_degree_tolerance,
                                                    const double &inplane_bandwidth,
                                                    const double &offplane_bandwidth,
                                                    const double &step,
                                                    const int &n_points,
                                                    const double &limit_dist,
                                                    const double &tol_factor);

std::vector<double> lagvar (const double &coverage, const double &dist_min, const double &factor, const int &n_points, const int &n_points_start, const double &tol_factor, std::vector<double> &tol);
std::vector<double> lagvar1 (const double &coverage, const double &n_points, const double &percent, std::vector<double> &tol);
std::vector<double> lagvar2 (const double &coverage, const double &dist_min, const double &step_factor, const int &n_points);

std::vector<double> lagvar_new (const double &coverage, const double &dist_min, const int &n_points, const double &delta_factor, const double &tol_factor, std::vector<double> &tol);


std::vector<exp_variog> dir_exp_variogram_lagvar (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y,
                                                    const std::vector<double> &seq,
                                                    const double &degree_tolerance,
                                                    const double &bandwidth,
                                                    const double &step,
                                                    const int &n_points,                                                    
                                                    const double &limit_dist,
                                                    const double &tol_factor);

std::vector<exp_variog> dir_exp_variogram (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y,
                                                    const std::string &lagspac_type,
                                                    const std::vector<double> &seq,
                                                    const double &degree_tolerance,
                                                    const double &bandwidth,
                                                    const double &step,
                                                    const int &n_points,
                                                    const double &limit_dist,
                                                    const double &tol_factor);


//Compute experimental variogram (directional, costant lag, fixed range from omni)
std::vector<exp_variog> dir_exp_variogram_range (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const double &degree_step, const double &degree_tolerance, const int n_points, double range_omni);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//Clean experimental variogram
exp_variog clean_exp_variogram (const exp_variog &ev, const uint min_npoints);
exp_variog clean_exp_variogram (const exp_variog &ev, const uint min_npoints, int &nzeros);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//Diagnose experimental variogram - check for anomalies
void diagnose_exp_variogram (const exp_variog &ev, const std::vector<double> &values);





#ifndef STATIC_MUSELIB
#include "vario.cpp"
#endif

#endif // VARIO_H
