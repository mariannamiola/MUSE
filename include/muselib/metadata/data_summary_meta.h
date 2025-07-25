#ifndef DATA_SUMMARY_META_H
#define DATA_SUMMARY_META_H

#include <string>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include "muselib/data_structures/data.h"
#include "muselib/metadata/data_summary.h"
#include "muselib/metadata/data_statistics.h"

namespace MUSE
{
    class DataSummaryMeta;
}

class MUSE::DataSummaryMeta
{
public:


    // Get Methods
    const MUSE::Data          &getData        () const    { return  data; }
    const MUSE::DataSummary   &getSummary     () const    { return  summary; }
    const MUSE::Statistics    &getStatistics  () const    { return  statistics; }


    // Set Methods
    void setData        (const MUSE::Data &d)               { data = d; }
    void setSummary     (const MUSE::DataSummary &d)        { summary = d; }
    void setStatistics  (const MUSE::Statistics &d)         { statistics = d; }


    // Additional Methods
    bool read  (const std::string filename);
    bool write (const std::string filename);




#ifdef MUSE_USES_CEREAL
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar (CEREAL_NVP(data));
        ar (CEREAL_NVP(summary));
        ar (CEREAL_NVP(statistics));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(data));
        ar (CEREAL_NVP(summary));
        ar (CEREAL_NVP(statistics));
    }
#endif

private:

    MUSE::Data data;
    MUSE::DataSummary summary;
    MUSE::Statistics statistics;



    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename, const int &precision = 3) ;
};



#ifndef STATIC_MUSELIB
#include "data_summary_meta.cpp"
#endif

#endif // DATA_SUMMARY_META_H
