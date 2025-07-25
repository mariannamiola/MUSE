#ifndef PLANE_H
#define PLANE_H

#include <cmath>
#include <vector>
#include <ctime>

#include <ellipse_fit.h>

void fit_ellipse (std::vector<double> &vec_x, std::vector<double> &vec_y);



#ifndef STATIC_MUSELIB
#include "ellipse.cpp"
#endif

#endif // PLANE_H
