#include "coord_transf.h"
#include <cfloat>
#include <cmath>
#include <stdexcept>

namespace {
void validate_transformation(double value, double top, double bottom, double thickness)
{
    if(!std::isfinite(value) || std::abs(value)==DBL_MAX ||
       !std::isfinite(top) || std::abs(top)==DBL_MAX ||
       !std::isfinite(bottom) || std::abs(bottom)==DBL_MAX || top<=bottom)
        throw std::runtime_error("Invalid stratigraphic coordinate or layer bounds");
    if(!std::isfinite(thickness) || thickness<=0)
        throw std::runtime_error("Reference thickness must be finite and positive");
}
}



void forward_transformation (MUSE::stratigraphicCondition type, const double &start_value, const double &top_value, const double &bot_value, double &end_value, const double &avg_thick)
{
    validate_transformation(start_value, top_value, bot_value, avg_thick);
    switch (type)
    {
    case MUSE::stratigraphicCondition::PROPORTIONAL:
    {
        end_value = ((start_value - bot_value)/(top_value-bot_value)) * avg_thick;
        break;
    }
    case MUSE::stratigraphicCondition::TRUNCATION:
    {
        end_value = start_value - bot_value;
        break;
    }
    case MUSE::stratigraphicCondition::ONLAP:
    {
        end_value = start_value - top_value;
        break;
    }
    case MUSE::stratigraphicCondition::COMBINATION:
    {
        throw std::runtime_error("COMBINATION has no defined correlation surface");
    }
    default: throw std::runtime_error("Unknown stratigraphic transformation");
    }
    if(!std::isfinite(end_value)) throw std::runtime_error("Nonfinite stratigraphic transformation result");
}

void back_transformation (MUSE::stratigraphicCondition type, const double &start_value, const double &top_value, const double &bot_value, double &end_value, const double &avg_thick)
{
    validate_transformation(start_value, top_value, bot_value, avg_thick);
    switch (type)
    {
    case MUSE::stratigraphicCondition::PROPORTIONAL:
    {
        end_value = bot_value + start_value/avg_thick * (top_value-bot_value);
        break;
    }
    case MUSE::stratigraphicCondition::TRUNCATION:
    {
        end_value = start_value + bot_value;
        break;
    }
    case MUSE::stratigraphicCondition::ONLAP:
    {
        end_value = start_value + top_value;
        break;
    }
    case MUSE::stratigraphicCondition::COMBINATION:
    {
        throw std::runtime_error("COMBINATION has no defined correlation surface");
    }
    default: throw std::runtime_error("Unknown stratigraphic transformation");
    }
    if(!std::isfinite(end_value)) throw std::runtime_error("Nonfinite inverse transformation result");
}



void convert_from_str (const std::string &str, MUSE::stratigraphicCondition &type)
{
    if(str.compare("PROPORTIONAL") == 0)
        type = MUSE::stratigraphicCondition::PROPORTIONAL;

    else if(str.compare("TRUNCATION") == 0)
        type = MUSE::stratigraphicCondition::TRUNCATION;

    else if(str.compare("ONLAP") == 0)
        type = MUSE::stratigraphicCondition::ONLAP;

    else if(str.compare("COMBINATION") == 0)
        type = MUSE::stratigraphicCondition::COMBINATION;
}
