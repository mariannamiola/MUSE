#include "plots.h"

#include <matplot/matplot.h>

#include <geostatslib/statistics/data_structures.h>
#include <geostatslib/statistics/stats.h>

#include "muselib/geostatistics/utils.h"

#include <ellipse_fit.h>

// https://alandefreitas.github.io/matplotplusplus/plot-types/data-distribution/histogram/
void hist_plot (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label, const size_t n_bins, bool set_bin = false)
{
    double min = *min_element(dataplot.x.begin(), dataplot.x.end());
    double max = *max_element(dataplot.x.begin(), dataplot.x.end());

    // Plots:
    // 1. Histogram and normal distribution
    if(set_bin) //if set_bin is true, it is possible to set the number of bins as input parameter (n_bins)
    {
        auto h = matplot::hist(dataplot.x, n_bins);
        h->normalization(matplot::histogram::normalization::pdf);
        matplot::hold(matplot::on);
        h.reset();
    }
    else //automatic generation of number of bins
    {
        auto h = matplot::hist(dataplot.x);
        h->normalization(matplot::histogram::normalization::pdf);
        matplot::hold(matplot::on);
        h.reset();
    }


    double mu = mean(dataplot.x);
    double sigma = stdev(dataplot.x); //dev std
    auto f = [&](double y)
    {
        return exp(-pow((y - mu), 2.) / (2. * pow(sigma, 2.))) /
               (sigma * sqrt(2. * matplot::pi));
    };

    matplot::fplot(f, std::array<double, 2>{min, max})->line_width(1.5);

    matplot::title(title);
    matplot::xlabel(x_label);
    matplot::ylabel(y_label);
    matplot::grid(matplot::on);

    //matplot::show();

    //std::cout << "Histogram with " << h->num_bins() << " bins" << std::endl;
}


void biv_plot (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label)
{
    auto p = matplot::scatter(dataplot.x, dataplot.y);

    matplot::title(title);
    matplot::xlabel(x_label);
    matplot::ylabel(y_label);
    matplot::grid(matplot::on);

    p->marker_style(matplot::line_spec::marker_style::asterisk);



    //matplot::show();
}


void biv_plot_leg (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label, bool set_legend, std::string legend)
{
    auto p = matplot::scatter(dataplot.x, dataplot.y);

    matplot::title(title);
    matplot::xlabel(x_label);
    matplot::ylabel(y_label);
    matplot::grid(matplot::on);

    //p->marker_style(matplot::line_spec::marker_style::circle);

    if(set_legend == true)
        p->display_name(legend);



    //matplot::xlim({0, 4000});

    //matplot::show();
}

void color_map (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label)
{
    matplot::binscatter(dataplot.x, dataplot.y, matplot::bin_scatter_style::automatic);
    matplot::colormap(matplot::gca(), matplot::palette::parula());

    //matplot::show();
}


void variogram_plot (const MUSE::PlotStruct &dataplot, const variogram model, const std::string &title, const std::string &x_label, const std::string &y_label, const double &eps_y)
{
    auto p1 = matplot::scatter(dataplot.x, dataplot.y);

    matplot::title(title);
    matplot::xlabel(x_label);
    matplot::ylabel(y_label);
    matplot::grid(matplot::on);

    p1->marker_style(matplot::line_spec::marker_style::asterisk);
    p1->display_name("Experimental variogram");

    matplot::hold(matplot::on);

    //punto in corrispondenza di h=0 -> gamma è pari al nugget
    std::vector<double> model_gamma, model_h;
    model_h.push_back(0.0);
    model_gamma.push_back(model.nugget);

    variogram_type type;
    convert_from_str(model.type, type);

//    for(size_t i=0; i< dataplot.x.size(); i++)
//    {
//        model_h.push_back(dataplot.x.at(i));

//        //double g = get_gamma (dataplot.x.at(i), model.range, model.nugget, 1-model.nugget, type);
//        double g = get_gamma (dataplot.x.at(i), model.range, model.nugget, model.sill - model.nugget, type);
//        model_gamma.push_back(g);
//    }

    double delta = dataplot.x.at(dataplot.x.size()-1)/(100-1);
    for(size_t i=1; i<= 100; i++)
    {
        double h = model_h.at(i-1) + delta;
        model_h.push_back(h);

        //double g = get_gamma (dataplot.x.at(i), model.range, model.nugget, 1-model.nugget, type);
        double g = get_gamma (h, model.range, model.nugget, model.sill - model.nugget, type);
        model_gamma.push_back(g);
    }


    auto p2 = matplot::plot(model_h, model_gamma);
    p2->display_name("Model variogram - " + model.type);
    matplot::hold(matplot::on);

    //matplot::legend();

    matplot::ylim({0, model.sill + eps_y}); //0.05
}

void x_err_plot (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label)
{
    matplot::errorbar(dataplot.x, dataplot.y, dataplot.err, matplot::error_bar::type::horizontal, "o");

    matplot::title(title);
    matplot::xlabel(x_label);
    matplot::ylabel(y_label);
    matplot::grid(matplot::on);

    //matplot::show();
}

void y_err_plot (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label)
{
    matplot::errorbar(dataplot.x, dataplot.y, dataplot.err, matplot::error_bar::type::vertical, "o");

    matplot::title(title);
    matplot::xlabel(x_label);
    matplot::ylabel(y_label);
    matplot::grid(matplot::on);

    //matplot::show();
}


void tri_plot (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label, const std::string &z_label)
{
    std::vector<double> x_grid = {0, 0.5, 1, 0};
    std::vector<double> y_grid = {0, 0.866, 0, 0};
    auto p = matplot::plot(x_grid, y_grid);
    p->color("gray");
    matplot::hold(matplot::on);
    x_grid.clear();
    y_grid.clear();

//    x_grid = {0.05, 0.1, 0.55};
//    y_grid = {0.0866, 0, 0.7794};
//    p = matplot::plot(x_grid, y_grid);
//    p->color("gray");
//    matplot::hold(matplot::on);
//    x_grid.clear();
//    y_grid.clear();

    x_grid = {0.1, 0.2, 0.6};
    y_grid = {0.1732, 0, 0.6928};
    p = matplot::plot(x_grid, y_grid);
    p->color("gray");
    matplot::hold(matplot::on);
    x_grid.clear();
    y_grid.clear();

//    x_grid = {0.15, 0.3, 0.65};
//    y_grid = {0.2598, 0, 0.6062};
//    p = matplot::plot(x_grid, y_grid);
//    p->color("gray");
//    matplot::hold(matplot::on);
//    x_grid.clear();
//    y_grid.clear();

    x_grid = {0.20, 0.4, 0.7};
    y_grid = {0.3464, 0, 0.5196};
    p = matplot::plot(x_grid, y_grid);
    p->color("gray");
    matplot::hold(matplot::on);
    x_grid.clear();
    y_grid.clear();

//    x_grid = {0.25, 0.5, 0.75};
//    y_grid = {0.433, 0, 0.433};
//    p = matplot::plot(x_grid, y_grid);
//    p->color("gray");
//    matplot::hold(matplot::on);
//    x_grid.clear();
//    y_grid.clear();

    x_grid = {0.3, 0.6, 0.80};
    y_grid = {0.5196, 0, 0.3464};
    p = matplot::plot(x_grid, y_grid);
    p->color("gray");
    matplot::hold(matplot::on);
    x_grid.clear();
    y_grid.clear();

//    x_grid = {0.35, 0.7, 0.85};
//    y_grid = {0.6062, 0, 0.2598};
//    p = matplot::plot(x_grid, y_grid);
//    p->color("gray");
//    matplot::hold(matplot::on);
//    x_grid.clear();
//    y_grid.clear();
//    matplot::show();

    x_grid = {0.4, 0.8, 0.90};
    y_grid = {0.6928, 0, 0.1732};
    p = matplot::plot(x_grid, y_grid);
    p->color("gray");
    matplot::hold(matplot::on);
    x_grid.clear();
    y_grid.clear();

//    x_grid = {0.45, 0.9, 0.95};
//    y_grid = {0.7794, 0, 0.0866};
//    p = matplot::plot(x_grid, y_grid);
//    p->color("gray");
//    matplot::hold(matplot::on);
//    x_grid.clear();
//    y_grid.clear();


    // Trasformazione coordinate
    std::vector<double> xx;
    std::vector<double> yy;

    size_t n = dataplot.x.size();
    for(size_t i=0; i<n; i++)
    {
        double sum = dataplot.x.at(i)+dataplot.y.at(i)+dataplot.z.at(i);
        double valx = 0.5*(dataplot.x.at(i)/sum) + dataplot.z.at(i)/sum;
        double valy = (pow(3,0.5)*0.5)* dataplot.x.at(i)/sum;

        xx.push_back(valx);
        yy.push_back(valy);
    }

    matplot::scatter(xx, yy);

    matplot::title(title);

    matplot::text(0.2, 0.5, x_label);
    matplot::text(0.8, 0.5, y_label);
    matplot::text(0.5, 0, z_label);

    //matplot::xlabel(x_label);
    matplot::ylabel(y_label);
    matplot::grid(matplot::on);
    //matplot::show();
}



void ellipse_plot (const MUSE::EllipseParameter &ellipse_par, const double &eps)
{
    //Estrazione punti sull'ellisse
    std::vector<double> x_points, y_points;

    double n_points = 100;
    double tmin = 0;
    double tmax = 2*M_PI;
    double t = (tmax-tmin)/(n_points-1);

    std::vector<double> step (n_points);
    step.at(0) = 0;
    for(size_t i=1; i< step.size(); i++)
        step[i] = step[i-1] + t;

    for(size_t i=0; i<step.size(); i++)
    {
        double x = ellipse_par.center_x + ellipse_par.max_semiaxis * cos(step.at(i)) * cos(ellipse_par.phi_rad) - ellipse_par.min_semiaxis * sin(step.at(i)) * sin(ellipse_par.phi_rad);
        double y = ellipse_par.center_y + ellipse_par.max_semiaxis * cos(step.at(i)) * sin(ellipse_par.phi_rad) + ellipse_par.min_semiaxis * sin(step.at(i)) * cos(ellipse_par.phi_rad);

        x_points.push_back(x);
        y_points.push_back(y);
    }

    matplot::plot(x_points, y_points);

    //Setting limits for axis
    double max_x = *max_element(x_points.begin(), x_points.end()) + eps;
    double max_y = *max_element(y_points.begin(), y_points.end()) + eps;
    if(max_x >= max_y)
        matplot::axis({-max_x, max_x, -max_x, max_x});
    else
        matplot::axis({-max_y, max_y, -max_y, max_y});

//    std::string str = "a = " + to_string(width);
//    auto [text, arrow] = matplot::textarrow(center_x, center_y, width*cos(M_PI-phi), width*sin(M_PI-phi), str);
//    //text ->color("red").font_size(100);
    //arrow ->color("blue");
}








