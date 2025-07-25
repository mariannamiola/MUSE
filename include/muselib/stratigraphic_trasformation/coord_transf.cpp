#include "coord_transf.h"
#include <cfloat>


void forward_transformation (MUSE::stratigraphicCondition type, const double &start_value, const double &top_value, const double &bot_value, double &end_value, const double &avg_thick)
{
    switch (type)
    {
    case MUSE::stratigraphicCondition::PROPORTIONAL:
    {
        //double avg_thick = 1.0;
        if(start_value != DBL_MAX || start_value != -DBL_MAX)
            end_value = ((start_value - bot_value)/(top_value-bot_value)) * avg_thick;
        else
            end_value = start_value;
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
        std::cout << "No correlation surface can be explicity defined. The coordinate transformation is not possible." << std::endl;
        break;
    }
    }
}

void back_transformation (MUSE::stratigraphicCondition type, const double &start_value, const double &top_value, const double &bot_value, double &end_value, const double &avg_thick)
{
    switch (type)
    {
    case MUSE::stratigraphicCondition::PROPORTIONAL:
    {
        //double avg_thick = 1.0;
        if(start_value != DBL_MAX || start_value != -DBL_MAX)
            end_value = bot_value + start_value/avg_thick * (top_value-bot_value);
        else
            end_value = start_value;
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
        std::cout << "No correlation surface can be explicity defined. The coordinate transformation is not possible." << std::endl;
        break;
    }
    }
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
