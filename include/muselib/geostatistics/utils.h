#ifndef GEOSTATS_UTILS_H
#define GEOSTATS_UTILS_H

#include <iostream>
#include <geostatslib/statistics/data_structures.h>
#include <geostatslib/statistics/variogram.h>

#include "muselib/stratigraphic_trasformation/coord_transf.h"

namespace MUSE {

    enum VarioType
    {
        EXPERIMENTAL,
        MODEL
    };

    enum VarioDirection
    {
        OMNI,
        DIR
    };

    enum Mode
    {
        AUTO,
        MANUAL
    };
}

void convert_from_str   (const std::string &str, MUSE::VarioType &type);
void convert_from_str   (const std::string &str, MUSE::VarioDirection &type);
void convert_from_str   (const std::string &str, MUSE::Mode &type);
void convert_from_str   (const std::string &str, variogram_type &type);

void convert_to_str     (std::string &str, variogram_type &type);



#ifndef STATIC_MUSELIB
#include "utils.cpp"
#endif

#endif // GEOSTATS_UTILS_H
