#include "vario_multiframe.h"

namespace MUSE
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool VarioMultiFrame::read(const std::string filename)
{
    return readConfFileJSON(filename);
}

bool VarioMultiFrame::write(const std::string filename)
{
    return writeConfFileJSON(filename);
}


bool VarioMultiFrame::readConfFileJSON(const std::string filename)
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


bool VarioMultiFrame::writeConfFileJSON (const std::string filename, const int &precision)
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
