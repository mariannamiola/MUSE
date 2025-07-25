#ifndef COORDINATE_SYSTEMS_H
#define COORDINATE_SYSTEMS_H

#ifdef MUSE_USES_PROJ
#include <proj/crs.hpp>
#endif

#include <string>

void coordinate_transformation (const double &x, const double &y, const double &z, const std::string source, const std::string target, double &tx, double &ty, double &tz);



#ifndef STATIC_MUSELIB
#include "coordinate_systems.cpp"
#endif

#endif // COORDINATE_SYSTEMS_H
