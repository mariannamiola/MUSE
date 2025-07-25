#ifndef VOLUME_META_H
#define VOLUME_META_H

#include <string>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include "muselib/data_structures/project.h"
#include "muselib/data_structures/volume.h"

namespace MUSE
{
    class VolumeMeta;
}

class MUSE::VolumeMeta
{
public:

    // Get Methods
    const MUSE::Project         &getProject         () const    { return  project; }

    const std::vector<std::string> &getCommands () const    { return commands;}
    const std::string &getCommand (const unsigned int i) const { return commands.at(i); }

    const std::vector<std::string> &getDeps () const { return  dependencies; }
    const std::string &getDep (const unsigned int i) const { return  dependencies.at(i); }

    const MUSE::Volume          &getMeshSummary   () const    { return mesh; }




    // Set Methods
    void setProject         (const MUSE::Project &d)        { project = d; }
    void setCommands        (const std::vector<std::string> &d) { commands = d; }

    void setDependencies    (const std::vector<std::string> &d) { dependencies = d; }

    void setMeshSummary   (const MUSE::Volume &d)         { mesh = d; }



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
        ar (CEREAL_NVP(mesh));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(project));
        ar (CEREAL_NVP(commands));
        ar (CEREAL_NVP(dependencies));
        ar (CEREAL_NVP(mesh));
    }
#endif

private:

    MUSE::Project project;
    std::vector<std::string> commands;
    std::vector<std::string> dependencies;
    MUSE::Volume mesh;



    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename, const int &precision = 3) ;
};



#ifndef STATIC_MUSELIB
#include "volume_meta.cpp"
#endif

#endif // VOLUME_META_H
