#include "data_meta.h"

#include <fstream>
#include <iostream>
#include <math.h>

namespace MUSE
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


bool DataMeta::read(const std::string filename)
{
    return readConfFileJSON(filename);
}

bool DataMeta::write(const std::string filename)
{
    return writeConfFileJSON(filename);
}

bool DataMeta::readConfFileJSON(const std::string filename)
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


bool DataMeta::writeConfFileJSON (const std::string filename)
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
        cereal::JSONOutputArchive archive (ss);
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

