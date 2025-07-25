#ifndef METADATA_H
#define METADATA_H

#include <string>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include "muselib/data_structures/project.h"
#include "muselib/data_structures/data.h"

namespace MUSE
{
    class Metadata;
}

class MUSE::Metadata
{
public:

    // Get Methods
    const MUSE::Project     &getProject         ()          const    { return  project; }

    const std::vector<std::string> &getCommands ()          const    { return commands;}
    const std::string &getCommand (const unsigned int i)    const { return commands.at(i); }

    const std::vector<std::string> &getDeps     ()          const { return  dependencies; }
    const std::string &getDep (const unsigned int i)        const { return  dependencies.at(i); }

    const std::vector<MUSE::Data>  &getMultiData()          const    { return  data; }
    const MUSE::Data &getData (const unsigned int i)        const    { return  data.at(i); }


    // Set Methods
    void setProject     (const MUSE::Project &d)            { project = d; }
    void setCommands    (const std::vector<std::string> &d) { commands = d; }
    void setDependencies(const std::vector<std::string> &d) { dependencies = d; }
    void setMultiData   (const std::vector<MUSE::Data> &d)  { data = d; }


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
        ar (CEREAL_NVP(data));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(project));
        ar (CEREAL_NVP(commands));
        ar (CEREAL_NVP(dependencies));
        ar (CEREAL_NVP(data));
    }
#endif


private:

    MUSE::Project project;
    std::vector<std::string> commands;
    std::vector<std::string> dependencies;

    std::vector<MUSE::Data> data;


    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename);

};


#ifndef STATIC_MUSELIB
#include "metadata.cpp"
#endif

#endif // METADATA_H
