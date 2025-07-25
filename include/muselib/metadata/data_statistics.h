#ifndef DATA_STATISTICS_H
#define DATA_STATISTICS_H

#include <string>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include "muselib/data_structures/data.h"

namespace MUSE
{
    class Statistics;
}

class MUSE::Statistics
{
public:

    uint     n_val;
    double   mean_val;
    double   var_val;
    double   stdev_val;
    double   median_val;
    double   min_val;
    double   max_val;

    // Additional Methods
    bool read  (const std::string filename);
    bool write (const std::string filename);

    void setStatistics     (const std::vector<double> &values);


#ifdef MUSE_USES_CEREAL
template <class Archive>
void serialize( Archive & ar )
{
    ar (CEREAL_NVP(mean_val));
    ar (CEREAL_NVP(var_val));
    ar (CEREAL_NVP(stdev_val));
    ar (CEREAL_NVP(median_val));
    ar (CEREAL_NVP(min_val));
    ar (CEREAL_NVP(max_val));
}

template <class Archive>
void deserialize( Archive & ar )
{
    ar (CEREAL_NVP(mean_val));
    ar (CEREAL_NVP(var_val));
    ar (CEREAL_NVP(stdev_val));
    ar (CEREAL_NVP(median_val));
    ar (CEREAL_NVP(min_val));
    ar (CEREAL_NVP(max_val));
}
#endif


private:


    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename, const int &precision = 3) ;
};



#ifndef STATIC_MUSELIB
#include "data_statistics.cpp"
#endif

#endif // DATA_STATISTICS_H
