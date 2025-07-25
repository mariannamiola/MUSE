#ifndef PLOTS_H
#define PLOTS_H

#include <string>
#include <vector>

#include <geostatslib/statistics/variogram.h>

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
void biv_plot_leg   (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label, bool set_legend, std::string legend);

void variogram_plot (const MUSE::PlotStruct &dataplot, const variogram model, const std::string &title, const std::string &x_label, const std::string &y_label, const double &eps_y=0.05);
void ellipse_plot   (const MUSE::EllipseParameter &ellipse_par, const double &eps);

void x_err_plot     (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label);
void y_err_plot     (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label);

void tri_plot       (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label, const std::string &z_label);



#ifndef STATIC_MUSELIB
#include "plots.cpp"
#endif

#endif // PLOTS_H
