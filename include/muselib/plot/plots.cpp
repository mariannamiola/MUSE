#include "plots.h"

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


matplot::figure_handle biv_plot_leg (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label, bool set_legend, std::string legend)
{
    auto fig = matplot::figure(true);
    fig->backend()->run_command("unset warnings");
    fig->size(800, 600);   // golden ratio

    auto p = matplot::scatter(dataplot.x, dataplot.y);
    p->marker_style(matplot::line_spec::marker_style::point);
    p->marker_size(10);
    p->marker_face(true); 

    matplot::title(title);
    matplot::xlabel(x_label);
    matplot::ylabel(y_label);

    if(set_legend == true)
        p->display_name(legend);
    return fig;
}

///
/// \brief ellipsoid_plot draws the fitted 3D anisotropy ellipsoid as a red wireframe tracing its
/// parallels/meridians, together with the directional range points (blue) used for the fitting:
/// it is the summary picture of the 3D fit. The wireframe is generated parametrically from the
/// principal axes stored in the structure, so the drawn orientation reflects exactly the fitted
/// azimuth/roll/pitch.
///
matplot::figure_handle ellipsoid_plot (const MUSE::EllipsoidParameter &ellipsoid_par, const MUSE::PlotStruct &range_points, const std::string &title)
{
    auto fig = matplot::figure(true);
    fig->backend()->run_command("unset warnings");
    fig->size(900, 800);

    // Shortcuts for the semi-axes and the principal directions of the fitted ellipsoid
    const double a = ellipsoid_par.max_semiaxis;
    const double b = ellipsoid_par.min_semiaxis;
    const double c = ellipsoid_par.z_semiaxis;
    const std::vector<double> &ea = ellipsoid_par.max_axis_dir;
    const std::vector<double> &eb = ellipsoid_par.min_axis_dir;
    const std::vector<double> &ec = ellipsoid_par.z_axis_dir;

    // Parametric point on the ellipsoid surface, expressed in world coordinates:
    // p(u,v) = a*cos(u)cos(v)*ea + b*cos(u)sin(v)*eb + c*sin(u)*ec
    auto surf_point = [&](const double u, const double v, double &x, double &y, double &z)
    {
        double c0 = a*std::cos(u)*std::cos(v);
        double c1 = b*std::cos(u)*std::sin(v);
        double c2 = c*std::sin(u);

        x = c0*ea.at(0) + c1*eb.at(0) + c2*ec.at(0);
        y = c0*ea.at(1) + c1*eb.at(1) + c2*ec.at(1);
        z = c0*ea.at(2) + c1*eb.at(2) + c2*ec.at(2);
    };

    matplot::hold(matplot::on);

    // NB: matplot++'s surf() (used for a shaded ellipsoid body) always draws its quad grid with
    // a hard-coded black hairline in gnuplot's hidden3d/pm3d renderer -- edge_color() has no
    // effect on it, and it does not get less visible at higher grid resolutions (more quads just
    // means more total black hairline, confirmed experimentally). There is no public matplot++
    // knob to remove or recolor it, so the shaded body is dropped entirely here in favor of a
    // clean red wireframe only -- no unwanted black lines this way.

    // Red wireframe tracing a handful of parallels and meridians, each resampled at
    // n_wire_samples points (independently of the fill grid) so they read as smooth ellipses --
    // explicit plot3() lines, since surf()'s own edge coloring can't be trusted (see note above).
    const matplot::color_array wire_color{0.f, 0.82f, 0.10f, 0.10f};
    const int n_wire_lat = 6;  //parallels drawn
    const int n_wire_lon = 10; //meridians drawn
    const int n_wire_samples = 60;
    for(int k=0; k<=n_wire_lat; k++) //parallels (latitude rings)
    {
        double u = -M_PI/2.0 + k*(M_PI/n_wire_lat);
        std::vector<double> px(n_wire_samples+1), py(n_wire_samples+1), pz(n_wire_samples+1);
        for(int s=0; s<=n_wire_samples; s++)
        {
            double v = s*(2.0*M_PI/n_wire_samples);
            surf_point(u, v, px[s], py[s], pz[s]);
        }
        auto wl = matplot::plot3(px, py, pz);
        wl->color(wire_color);
        wl->line_width(1.f);
    }
    for(int k=0; k<=n_wire_lon; k++) //meridians (longitude arcs)
    {
        double v = k*(2.0*M_PI/n_wire_lon);
        std::vector<double> mx(n_wire_samples+1), my(n_wire_samples+1), mz(n_wire_samples+1);
        for(int s=0; s<=n_wire_samples; s++)
        {
            double u = -M_PI/2.0 + s*(M_PI/n_wire_samples);
            surf_point(u, v, mx[s], my[s], mz[s]);
        }
        auto wl = matplot::plot3(mx, my, mz);
        wl->color(wire_color);
        wl->line_width(1.f);
    }

    // Principal axes of the ellipsoid, drawn as thicker segments through the origin
    const std::vector<const std::vector<double>*> axes_dir = {&ea, &eb, &ec};
    const std::vector<double> axes_len = {a, b, c};
    for(size_t k=0; k<axes_dir.size(); k++)
    {
        std::vector<double> x = {-axes_len.at(k)*axes_dir.at(k)->at(0), axes_len.at(k)*axes_dir.at(k)->at(0)};
        std::vector<double> y = {-axes_len.at(k)*axes_dir.at(k)->at(1), axes_len.at(k)*axes_dir.at(k)->at(1)};
        std::vector<double> z = {-axes_len.at(k)*axes_dir.at(k)->at(2), axes_len.at(k)*axes_dir.at(k)->at(2)};

        auto l = matplot::plot3(x, y, z);
        l->line_width(2.5);
        // NB: color() must be given a 4-element {alpha,R,G,B} array -- a 3-element one leaves
        // the line's "user_color" flag unset, so matplot silently overrides it with the next
        // auto-cycled colororder color instead of keeping this explicit gray.
        l->color({0.f, 0.267f, 0.267f, 0.267f}); //dark gray principal axes
    }

    // Directional range points used for the ellipsoid fitting (fit quality at a glance)
    auto pts = matplot::plot3(range_points.x, range_points.y, range_points.z, "o");
    pts->marker_size(5);
    pts->marker_face(true);
    pts->color({0.f, 0.f, 0.447f, 0.741f}); //blue markers, same convention as ellipse_plot()

    matplot::xlabel("X (East)");
    matplot::ylabel("Y (North)");
    matplot::zlabel("Z");

    // Light, unobtrusive grid behind everything else
    auto gl = matplot::gca()->grid_line_style();
    gl.color({0.6f, 0.8f, 0.8f, 0.8f}); //color_array is {alpha, R, G, B} -- pale, mostly-transparent gray
    gl.line_width(0.5f);
    matplot::gca()->grid_line_style(gl);
    matplot::grid(matplot::on);

    // Equal scale on all axes: the largest semi-axis defines the cube half-side.
    // Without this the ellipsoid appears distorted and anisotropy ratios are wrong.
    double r_max = std::max({a, b, c});
    for(const auto &v : {range_points.x, range_points.y, range_points.z})
        for(double val : v)
            r_max = std::max(r_max, std::abs(val));
    r_max *= 1.05; // 5 % margin
    matplot::xlim({-r_max, r_max});
    matplot::ylim({-r_max, r_max});
    matplot::zlim({-r_max, r_max});

    // Title placed below the plot instead of matplot's default top position: a borderless helper
    // axes spanning a thin strip at the bottom of the figure, with its own (bold, black, enlarged)
    // title as the only thing drawn on it. A raw "set label" command would be simpler, but it does
    // not survive: matplot issues a "reset" at the start of every redraw, which wipes any gnuplot
    // command not re-emitted by matplot itself on every draw -- title() on a tracked axes is.
    auto caption_axes = fig->add_axes({0.05f, 0.f, 0.9f, 0.001f});
    caption_axes->axis(false);            //hide box/ticks, keep the title only
    caption_axes->title(title);
    caption_axes->title_font_size_multiplier(1.35f);

    return fig;
}


void color_map (const MUSE::PlotStruct &dataplot, const std::string &title, const std::string &x_label, const std::string &y_label)
{
    matplot::binscatter(dataplot.x, dataplot.y, matplot::bin_scatter_style::automatic);
    matplot::colormap(matplot::gca(), matplot::palette::parula());

    //matplot::show();
}


void variogram_plot (const MUSE::PlotStruct &dataplot, const variogram model, const std::string &title, const std::string &x_label, const std::string &y_label, const size_t &N)
{
    auto fig = matplot::figure(true);
    fig->backend()->run_command("unset warnings");
    fig->size(800, 600);   // golden ratio

    auto p1 = matplot::scatter(dataplot.x, dataplot.y);

    matplot::title(title);
    matplot::xlabel(x_label);
    matplot::ylabel(y_label);
    //matplot::grid(matplot::on);

    //p1->display_name("Experimental variogram");

    // --- STILE DATI SPERIMENTALI ---
    p1->marker_style(matplot::line_spec::marker_style::point);
    p1->marker_size(10);
    p1->marker_face(true); 
    //p1->color("blue");
    p1->display_name("Experimental");

    // --- ASSI E LABEL ---
    matplot::xlabel(x_label);
    matplot::ylabel(y_label);
    matplot::title(""); // meglio lasciare vuoto per paper

    // --- GRIGLIA LEGGERA ---
    matplot::grid(matplot::on);
    //matplot::gca()->grid_line_style(matplot::line_spec::line_style::dashed_line);
    //matplot::gca()->grid_alpha(0.25);

    // --- HOLD ---
    matplot::hold(matplot::on);

    //punto in corrispondenza di h=0 -> gamma è pari al nugget
    std::vector<double> model_gamma, model_h;
    
    model_h.push_back(0.0);
    model_gamma.push_back(model.nugget);

    variogram_type type;
    convert_from_str(model.type, type);

    double delta = dataplot.x.at(dataplot.x.size()-1)/(N-1);
    for(size_t i=1; i<= N; i++)
    {
        double h = model_h.at(i-1) + delta;
        model_h.push_back(h);

        //double g = get_gamma (dataplot.x.at(i), model.range, model.nugget, 1-model.nugget, type);
        double g = get_gamma (h, model.range, model.nugget, model.sill - model.nugget, type);
        model_gamma.push_back(g);
    }

    // --- MODELLO ---
    auto p2 = matplot::plot(model_h, model_gamma);
    p2->line_style("-");
    //p2->color("red");   // rosso elegante
    p2->line_width(4);
    p2->display_name("Model");

    // --- LIMITI ASSI ---
    double x_max = dataplot.x.back() * 1.08;
    double y_max = model.sill * 1.20;
    matplot::xlim({0, x_max});
    matplot::ylim({0, y_max});

    //matplot::ylim({0, model.sill + eps_y}); //0.05

    // --- LEGENDA ---
    //auto leg = matplot::legend();
    //leg->location(matplot::legend::general_alignment::bottomright);
    //leg->box(true);
    //matplot::gca()->legend()->font_size(3);
    //leg->font_size(6);
    
    // // --- BOX ---
    // std::string param_text =
    // "Model: " + model.type + "\n" +
    // "sill = "   + std::to_string(model.sill)   + "\n" +
    // "nugget = " + std::to_string(model.nugget) + "\n" +
    // "range = "  + std::to_string(model.range)  + "\n";

    // auto t = matplot::text(0.02, 0.95, param_text);
    // t->font_size(9);
} 

/* void variogram_plot(const MUSE::PlotStruct &dataplot, const variogram model,
                    const std::string &title, const std::string &x_label,
                    const std::string &y_label, const double &eps_y)
{
    auto fig = matplot::figure(true);
    fig->backend()->run_command("unset warnings");
    fig->size(900, 556);   // golden ratio

    // ── 1. CURVA MODELLO ─────────────────────────────────────────────────
    std::vector<double> model_h, model_gamma;
    model_h.push_back(0.0);
    model_gamma.push_back(model.nugget);

    variogram_type type;
    convert_from_str(model.type, type);

    const size_t N = 300;
    double delta = (dataplot.x.back() * 1.08) / static_cast<double>(N);
    for (size_t i = 1; i <= N; i++) {
        double h = i * delta;
        model_h.push_back(h);
        model_gamma.push_back(
            get_gamma(h, model.range, model.nugget, model.sill - model.nugget, type));
    }

    auto p2 = matplot::plot(model_h, model_gamma);
    p2->color({0.12, 0.47, 0.71});   // rosso Tableau {0.84, 0.15, 0.16}
    p2->line_width(3.0);             // ← più spesso
    p2->line_style("-");
    p2->display_name("Model (" + model.type + ")");

    matplot::hold(matplot::on);

    // ── 2. PUNTI SPERIMENTALI ────────────────────────────────────────────
    auto p1 = matplot::scatter(dataplot.x, dataplot.y);
    p1->marker_style(matplot::line_spec::marker_style::cross);
    p1->marker_size(14);             // ← più grandi
    p1->color({0.84, 0.15, 0.16});   // blu Tableau — niente più giallo
    p1->display_name("Experimental");

    // ── 3. ASSI ───────────────────────────────────────────────────────────
    matplot::xlabel("Lag distance");
    matplot::ylabel("Semivariogram");

    double x_max = dataplot.x.back() * 1.08;
    double y_max = model.sill * 1.20;
    matplot::xlim({0.0, x_max});
    matplot::ylim({0.0, y_max});

    // ── 4. GRIGLIA LEGGERISSIMA ───────────────────────────────────────────
    //matplot::grid(matplot::on);
    //matplot::gca()->grid_alpha(0.08);   // ← quasi impercettibile

    // ── 5. BORDO PULITO ───────────────────────────────────────────────────
    matplot::gca()->box(false);
    matplot::gca()->font_size(13);

    // ── 6. LEGENDA CON PARAMETRI ─────────────────────────────────────────
    // Formattiamo i parametri del modello con precisione ragionevole
    auto fmt = [](double v, int dec) -> std::string {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(dec) << v;
        return ss.str();
    };

    std::string model_label = "Model (" + model.type + ")"
                            + "  |  sill = "   + fmt(model.sill,   3)
                            + "  nugget = "     + fmt(model.nugget, 3)
                            + "  range = "      + fmt(model.range,  1);

    p2->display_name("Model");
    p1->display_name("Experimental");

    auto leg = matplot::legend();
    leg->location(matplot::legend::general_alignment::bottomright);
    leg->box(false);
    leg->font_size(11);

    // ── 6. BOX ─────────────────────────────────────────
    std::string param_text =
    "Model: " + model.type + "\n" +
    "sill = "   + fmt(model.sill, 3)   + "\n" +
    "nugget = " + fmt(model.nugget, 3) + "\n" +
    "range = "  + fmt(model.range, 1);

    double x_text = x_max * 0.05;
    double y_text = y_max * 0.95;

    auto t = matplot::text(x_text, y_text, param_text);
    t->font_size(11);

    //t->background_color(1,1,1);  // bianco
    //t->edge_color({0.7, 0.7, 0.7});   // grigio chiaro
    //t->margin(5);

    // ── 7. SALVATAGGIO ────────────────────────────────────────────────────
    //matplot::save("variogram_" + model.type + ".svg");
    //matplot::save("variogram_" + model.type + ".pdf");
} */

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



void ellipse_plot (matplot::figure_handle fig, const MUSE::EllipseParameter &ellipse_par, const double &eps)
{
    fig->size(600, 600);   // golden ratio

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

    auto p1 = matplot::plot(x_points, y_points);
    //p1->marker_style(matplot::line_spec::marker_style::point);
    //p1->marker_size(10);
    p1->marker_face(true); 
    p1->line_width(4);
    p1->line_style("-");

    //Setting limits for axis
    double max_x = *max_element(x_points.begin(), x_points.end()) + eps;
    double max_y = *max_element(y_points.begin(), y_points.end()) + eps;
    if(max_x >= max_y)
        matplot::axis({-max_x, max_x, -max_x, max_x});
    else
        matplot::axis({-max_y, max_y, -max_y, max_y});

    // --- GRIGLIA LEGGERA ---
    matplot::grid(matplot::on);

}








