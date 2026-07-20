#ifndef PLOTS_H
#define PLOTS_H

#include <limits>
#include <string>
#include <vector>

#include <matplot/matplot.h>
#include <geostatslib/statistics/variogram.h>
#include <geostatslib/statistics/normal_score.h>

#include "muselib/geostatistics/fitvario.h"


namespace MUSE
{
    class PlotStruct;
}

class MUSE::PlotStruct
{
    public:

        std::vector<std::string> ID; //da implementare ancora! (rif: portarsi dietro l'id dei punti)

        std::vector<double> x;
        std::vector<double> y;
        std::vector<double> z;

        std::vector<double> err; //parallelo asse y
};


void hist_plot      (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label, const size_t n_bins, bool set_bin);

void biv_plot       (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label);
matplot::figure_handle biv_plot_leg   (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label, bool set_legend, std::string legend);

void variogram_plot (const MUSE::PlotStruct &dataplot, const variogram model, const std::string &title, const std::string &x_label, const std::string &y_label, const size_t &N = 100);
void ellipse_plot   (matplot::figure_handle fig, const MUSE::EllipseParameter &ellipse_par, const double &eps, const std::string &caption = "");

//Summary picture of the FULL 3D fit: wireframe of the fitted anisotropy ellipsoid (drawn from
//its principal axes) together with the directional range points used for the fitting
matplot::figure_handle ellipsoid_plot (const MUSE::EllipsoidParameter &ellipsoid_par, const MUSE::PlotStruct &range_points, const std::string &title);

void x_err_plot     (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label);
void y_err_plot     (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label);

void tri_plot       (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label, const std::string &z_label);


// --- Cumulative probability distribution functions (cpdf) of simulation results ---------------
//
// A mesh cell holds a set of EQUIPROBABLE simulated values (one per realization). In normal-score
// space these are draws from a local Gaussian N(m*, sigma*); the local cpdf is reconstructed by
// fitting m*, sigma* to the cell scores and mapping a Gaussian quantile grid back to variable
// space through the SAME MUSE normal-score transform (back_normal_score). This reuses exactly the
// anamorphosis (ns.x / ns.nsco) built by MUSE, so the curve is consistent with the simulation.

// LOCAL uncertainty (Fig. 8 of Zuccolini et al., 2025): cpdf of the target cell (bold) together with
// the cpdf of its neighbouring cells (thin), around a location. Each curve is reconstructed from the
// cell's equiprobable NORMAL-SCORE values (local Gaussian) back-transformed through the MUSE normal
// score. `target_nscores` are the target cell's equiprobable scores; `neighbor_nscores[j]` those of
// neighbour j (empty = no neighbours). `thresholds` draws red dotted vertical reference lines.
matplot::figure_handle cpdf_plot_local(const std::vector<double> &target_nscores,
                                       const std::vector<std::vector<double>> &neighbor_nscores,
                                       const normalscore &ns,
                                       const std::string &title, const std::string &x_label, const std::string &y_label,
                                       const std::vector<double> &thresholds = std::vector<double>(),
                                       const std::string &type_extrapolation = "none",
                                       const double &min_value = std::numeric_limits<double>::max(),
                                       const double &max_value = std::numeric_limits<double>::max(),
                                       const size_t &n_levels = 200);

// One cpdf per SIMULATION overlaid in a single plot (Fig. 4 of Zuccolini et al., 2025): each curve
// is the cumulative distribution of one realization over the whole domain, in variable space.
// `sims[r]` = the values of realization r over all cells (already back-transformed). Overlapping
// curves => low global uncertainty; spread curves => high global uncertainty. Each realization cdf
// is evaluated on a common probability grid (n_levels) for a smooth, light curve.
matplot::figure_handle cpdf_plot_sims (const std::vector<std::vector<double>> &sims,
                                       const std::string &title, const std::string &x_label, const std::string &y_label,
                                       const size_t &n_levels = 200);



#ifndef STATIC_MUSELIB
#include "plots.cpp"
#endif

#endif // PLOTS_H
