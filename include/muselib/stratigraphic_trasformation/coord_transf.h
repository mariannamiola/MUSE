#ifndef COORD_TRANSF_H
#define COORD_TRANSF_H

#include <iostream>

namespace MUSE {

    enum stratigraphicCondition
    {
        PROPORTIONAL,
        TRUNCATION,
        ONLAP,
        COMBINATION
    };
}

void forward_transformation     (MUSE::stratigraphicCondition type, const double &start_value, const double &top_value, const double &bot_value, double &end_value, const double &avg_thick);
void back_transformation        (MUSE::stratigraphicCondition type, const double &start_value, const double &top_value, const double &bot_value, double &end_value, const double &avg_thick);

void convert_from_str           (const std::string &str, MUSE::stratigraphicCondition &type);

#ifndef STATIC_MUSELIB
#include "coord_transf.cpp"
#endif

#endif // COORD_TRANSF_H
