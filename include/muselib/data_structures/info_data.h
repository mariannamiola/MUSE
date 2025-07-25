#ifndef INFO_DATA_H
#define INFO_DATA_H

#include <string>

#include <cereal/archives/json.hpp>

namespace MUSE
{
    class InfoData;
}

///
/// \brief The MUSE::InfoData class: store general information about CSV input file and related conversion in MUSE DataFormat
///
class MUSE::InfoData
{
    public:

    //std::string v_name = "Unknown";
    std::string x_name = "Unknown";
    std::string y_name = "Unknown";
    std::string z_name = "Unknown";
    std::string id_name = "Unknown";

    // Add any other additional descriptive info

    #ifdef MUSE_USES_CEREAL
    template <class Archive>
    void serialize( Archive & ar )
    {
        //ar (CEREAL_NVP(v_name));
        ar (CEREAL_NVP(x_name));
        ar (CEREAL_NVP(y_name));
        ar (CEREAL_NVP(z_name));
        ar (CEREAL_NVP(id_name));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(x_name));
        ar (CEREAL_NVP(y_name));
        ar (CEREAL_NVP(z_name));
        //ar (CEREAL_NVP(v_name));
        ar (CEREAL_NVP(id_name));
    }
    #endif

};

#endif // INFO_DATA_H
