#ifndef VARIO_MULTIFRAME_H
#define VARIO_MULTIFRAME_H

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include "muselib/geostatistics/fitvario.h"
#include "muselib/metadata/fitvario_methods.h"

namespace MUSE
{
    class VarioFrame;
    class VarioMultiFrame;    
}

class MUSE::VarioFrame
{
public:

    struct StatisticsInfo
    {
        double mean = 0.0;
        double var = 1.0;

        double mean_zscore = 0.0;
        double var_zscore = 1.0;


#ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(mean));
            ar (CEREAL_NVP(var));
            ar (CEREAL_NVP(mean_zscore));
            ar (CEREAL_NVP(var_zscore));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(mean));
            ar (CEREAL_NVP(var));
            ar (CEREAL_NVP(mean_zscore));
            ar (CEREAL_NVP(var_zscore));
        }
#endif
    };

    std::string frame_name;
    StatisticsInfo stats;
    MUSE::EllipseParameter ellipse_par;
    MUSE::variogram_methods vario;

    // Get Methods
    const std::string &getFrameName             () const    { return frame_name; }
    const StatisticsInfo &getStatisticsInfo     () const    { return stats; }
    const MUSE::EllipseParameter &getSummary    () const    { return ellipse_par; }
    const MUSE::variogram_methods &getVario     () const    { return vario; }

    // Set Methods
    void setFrameName   (const std::string &d)              { frame_name = d; }
    void setStatisticsInfo (const StatisticsInfo &d)        { stats = d; }
    void setSummary     (const MUSE::EllipseParameter &d)   { ellipse_par = d; }
    void setVario       (const MUSE::variogram_methods &d)  { vario = d; }



#ifdef MUSE_USES_CEREAL
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar (CEREAL_NVP(frame_name));
        ar (CEREAL_NVP(stats));
        ar (CEREAL_NVP(ellipse_par));
        ar (CEREAL_NVP(vario));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(frame_name));
        ar (CEREAL_NVP(stats));
        ar (CEREAL_NVP(ellipse_par));
        ar (CEREAL_NVP(vario));
    }
#endif

};


class MUSE::VarioMultiFrame
{
public:
    std::vector<VarioFrame> multiframe;

    void setMultiFrame (const std::vector<MUSE::VarioFrame> &d) { multiframe = d; }

    // Additional Methods
    bool read  (const std::string filename);
    bool write (const std::string filename);

#ifdef MUSE_USES_CEREAL
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar (CEREAL_NVP(multiframe));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(multiframe));
    }
#endif

private:

    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename, const int &precision = 2);

};

#ifndef STATIC_MUSELIB
#include "vario_multiframe.cpp"
#endif

#endif // VARIO_MULTIFRAME_H
