//Example for plotting histogram with mathplotplusplus library
// https://alandefreitas.github.io/matplotplusplus/plot-types/data-distribution/histogram/

#include <cmath>
#include <matplot/matplot.h>
#include <random>

int main()
{
    using namespace matplot;

    std::vector<double> x = randn(5000, 5, 2);

    auto h = hist(x);
    std::cout << "Histogram with " << h->num_bins() << " bins" << std::endl;
    h->normalization(histogram::normalization::pdf);
    hold(on);

    double mu = 5;
    double sigma = 2;
    auto f = [&](double y) {
        return exp(-pow((y - mu), 2.) / (2. * pow(sigma, 2.))) /
               (sigma * sqrt(2. * pi));
    };
    fplot(f, std::array<double, 2>{-5, 15})->line_width(1.5); //plot della funzione f

    xlabel("X values [-]");
    ylabel("Y values [-]");
    grid(on);

    //save("/Users/mariannamiola/Documents/Dottorato/4_Output/examples/01_histogram_plot/histogram.jpeg", "jpeg");

    show();







    return 0;
}
