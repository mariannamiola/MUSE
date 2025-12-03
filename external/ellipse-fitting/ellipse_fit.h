#ifndef ELLIPSE_FIT_H
#define ELLIPSE_FIT_H

//https://github.com/mericdurukan/ellipse-fitting
//modified with python implementation: https://scipython.com/blog/direct-linear-least-squares-fitting-of-an-ellipse/
//added inline in ellipse_fit.cpp

#include <Eigen/Dense>
#include <Eigen/Core>
#include <iostream>
#include <vector>
using namespace std;
using namespace Eigen;


class ellipse_fit{


public:

    void set(vector<vector<double> > input);
    void fit(double& result_center_x, double& result_center_y, double& result_phi, double& result_width, double& result_hight);

private:

    vector<vector<double> >input_matrix;
    void take_input(vector<vector<double> > input);

};

#ifndef STATIC_MUSELIB
#include "ellipse_fit.cpp"
#endif

#endif // ELLIPSE_FIT_H


