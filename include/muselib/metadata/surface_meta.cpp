#include "surface_meta.h"

#include <fstream>
#include <iostream>
#include <math.h>

namespace MUSE
{

void SurfaceMeta::DataSummary::setDataSummary (const std::vector<Point3D> &data)
{
    for(size_t i=0; i<data.size(); i++)
    {
        if(data.at(i).x < this->x_min)
            this->x_min = data.at(i).x;

        if(data.at(i).x > this->x_max)
            this->x_max = data.at(i).x;

        if(data.at(i).y < this->y_min)
            this->y_min = data.at(i).y;

        if(data.at(i).y > this->y_max)
            this->y_max = data.at(i).y;

        if(data.at(i).z < this->z_min)
            this->z_min = data.at(i).z;

        if(data.at(i).z > this->z_max)
            this->z_max = data.at(i).z;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


bool SurfaceMeta::read(const std::string filename)
{
    return readConfFileJSON(filename);
}

bool SurfaceMeta::write(const std::string filename)
{
    return writeConfFileJSON(filename);
}

bool SurfaceMeta::readConfFileJSON(const std::string filename)
{
#ifdef MUSE_USES_CEREAL
    std::ifstream ss (filename.c_str(), std::ifstream::in);

    if (!ss.is_open())
    {
        std::cerr << "[ERROR] Error in opening " << filename << "." << std::endl;
        std::cerr << "[ERROR] Configuration file cannot be read from disk." << std::endl;
        return false;
    }

    cereal::JSONInputArchive archive (ss);
    deserialize(archive);
    ss.close();

    return true;
#else
    std::cerr << "CEREAL Library is required to load an instrument from file." << std::endl;
    return false;
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


bool SurfaceMeta::writeConfFileJSON (const std::string filename, const int &precision)
{
#ifdef MUSE_USES_CEREAL
    std::ofstream ss (filename.c_str(), std::ofstream::out);

    if (!ss.is_open())
    {
        std::cerr << "[ERROR] Error in opening " << filename << "." << std::endl;
        std::cerr << "[ERROR] Configuration file cannot be written on disk." << std::endl;
        return false;
    }

    {
        cereal::JSONOutputArchive::Options options (precision); //set precision for double
        cereal::JSONOutputArchive archive (ss, options);
        serialize(archive);
    }
    ss.close();

    return true;
#else
    std::cerr << "CEREAL Library is required to save an instrument on file." << std::endl;
    return false;
#endif
}

}
