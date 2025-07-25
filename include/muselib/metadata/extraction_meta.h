#ifndef EXTRACTION_META_H
#define EXTRACTION_META_H

#include <string>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include "muselib/data_structures/project.h"
#include "muselib/data_structures/rotation.h"


namespace MUSE
{
    class ExtractionMeta;
}

class MUSE::ExtractionMeta
{
public:

    struct DataExtraction
    {
        std::string geometry;

        int n_points = 0.0;
        std::vector<uint> id_points;


        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(geometry));
            ar (CEREAL_NVP(n_points));
            ar (CEREAL_NVP(id_points));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(geometry));
            ar (CEREAL_NVP(n_points));
            ar (CEREAL_NVP(id_points));
        }
        #endif
    };



    // Get Methods
    const MUSE::Project     &getProject         () const    { return  project; }

    const std::vector<std::string> &getCommands () const    { return commands;}
    const std::string &getCommand (const unsigned int i) const { return commands.at(i); }

    const std::vector<std::string> &getDeps () const { return  dependencies; }
    const std::string &getDep (const unsigned int i) const { return  dependencies.at(i); }

    const MUSE::Rotation    &getRotation        () const    { return  rotation; }

    const DataExtraction    &getDataExtraction  () const    { return  subdataset; }



    // Set Methods
    void setProject         (const MUSE::Project &d)        { project = d; }
    void setCommands        (const std::vector<std::string> &d) { commands = d; }

    void setDependencies    (const std::vector<std::string> &d) { dependencies = d; }

    void setRotation        (const MUSE::Rotation &d)       { rotation = d; }

    void setDataExtraction  (const DataExtraction &d)       { subdataset = d; }


    // Additional Methods
    bool read  (const std::string filename);
    bool write (const std::string filename);




#ifdef MUSE_USES_CEREAL
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar (CEREAL_NVP(project));
        ar (CEREAL_NVP(commands));
        ar (CEREAL_NVP(dependencies));
        ar (CEREAL_NVP(rotation));
        ar (CEREAL_NVP(subdataset));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(project));
        ar (CEREAL_NVP(commands));
        ar (CEREAL_NVP(dependencies));
        ar (CEREAL_NVP(rotation));
        ar (CEREAL_NVP(subdataset));
    }
#endif

private:

    MUSE::Project project;
    std::vector<std::string> commands;
    std::vector<std::string> dependencies;
    MUSE::Rotation rotation;

    DataExtraction subdataset;


    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename) ;
};



#ifndef STATIC_MUSELIB
#include "extraction_meta.cpp"
#endif



#endif // EXTRACTION_META_H
