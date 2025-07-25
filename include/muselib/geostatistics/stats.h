#ifndef MUSESTATS_H
#define MUSESTATS_H

#include <vector>
#include <cmath>

double quartile (const std::vector<double> &values, const double &percentile);

#ifndef STATIC_MUSELIB
#include "stats.cpp"
#endif

#endif // MUSESTATS_H
