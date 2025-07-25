#include "utils.h"


void convert_from_str (const std::string &str, MUSE::VarioType &type)
{
    if(str.compare("EXPERIMENTAL") == 0)
        type = MUSE::VarioType::EXPERIMENTAL;

    else if(str.compare("MODEL") == 0)
        type = MUSE::VarioType::MODEL;
}

void convert_from_str (const std::string &str, MUSE::VarioDirection &type)
{
    if(str.compare("OMNI") == 0)
        type = MUSE::VarioDirection::OMNI;

    else if(str.compare("DIR") == 0)
        type = MUSE::VarioDirection::DIR;
}

void convert_from_str (const std::string &str, MUSE::Mode &type)
{
    if(str.compare("AUTO") == 0)
        type = MUSE::Mode::AUTO;

    else if(str.compare("MANUAL") == 0)
        type = MUSE::Mode::MANUAL;
}

void convert_from_str (const std::string &str, variogram_type &type)
{
    if(str.compare("GAUSSIAN") == 0 || str.compare("Gau") == 0)
        type = variogram_type::GAUSSIAN;

    else if(str.compare("LINEAR") == 0 || str.compare("Lin") == 0)
        type = variogram_type::LINEAR;

    else if(str.compare("EXPONENTIAL") == 0 || str.compare("Exp") == 0)
        type = variogram_type::EXPONENTIAL;

    else if(str.compare("SPHERICAL") == 0 || str.compare("Sph") == 0)
        type = variogram_type::SPHERIC;

    else if(str.compare("DEFAULT") == 0 || str.compare("Def") == 0)
        type = variogram_type::DEFAULT;

    else
    {
        std::cout << "ERROR: Unknown variogram model." << std::endl;
        exit(1);
    }
}


void convert_to_str (std::string &str, variogram_type &type)
{
    if(type == variogram_type::GAUSSIAN)
        str = "Gau";
    else if(type == variogram_type::SPHERIC)
        str = "Sph";
    else if(type == variogram_type::EXPONENTIAL)
        str = "Exp";
    else if(type == variogram_type::LINEAR)
        str = "LINEAR";
    else if(type == variogram_type::DEFAULT)
        str = "DEFAULT";
}










