#ifndef MANIPULATE_META_H
#define MANIPULATE_META_H

#include <string>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>


#include "muselib/data_structures/project.h"
#include "muselib/data_structures/info_data.h"
#include "muselib/metadata/extraction_meta.h"
#include "muselib/data_structures/rotation.h"


namespace MUSE
{
    class ManipulateMeta;
}

class MUSE::ManipulateMeta
{
public:

    struct  DataProjection
    {
        bool proj_is_set = false;
        std::string proj_direction;
        std::string mesh;

        std::string data_type;
        bool subdataset_is_set = false;

        std::string top_name;
        std::string bottom_name;


        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(proj_is_set));
            ar (CEREAL_NVP(proj_direction));
            ar (CEREAL_NVP(mesh));
            ar (CEREAL_NVP(data_type));
            ar (CEREAL_NVP(subdataset_is_set));
            ar (CEREAL_NVP(top_name));
            ar (CEREAL_NVP(bottom_name));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(proj_is_set));
            ar (CEREAL_NVP(proj_direction));
            ar (CEREAL_NVP(mesh));
            ar (CEREAL_NVP(data_type));
            ar (CEREAL_NVP(subdataset_is_set));
            ar (CEREAL_NVP(top_name));
            ar (CEREAL_NVP(bottom_name));
        }
        #endif
    };



    struct StratigraphicTransf
    {
        bool strat_transf_is_set = false;
        std::string data_type;
        std::string top_name;
        std::string bottom_name;

        std::string transformation_type;
        double avg_thick = 0.0;


        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(strat_transf_is_set));
            ar (CEREAL_NVP(data_type));
            ar (CEREAL_NVP(top_name));
            ar (CEREAL_NVP(bottom_name));

            ar (CEREAL_NVP(transformation_type));
            ar (CEREAL_NVP(avg_thick));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(strat_transf_is_set));
            ar (CEREAL_NVP(data_type));
            ar (CEREAL_NVP(top_name));
            ar (CEREAL_NVP(bottom_name));

            ar (CEREAL_NVP(transformation_type));
            ar (CEREAL_NVP(avg_thick));
        }
        #endif
    };




    // Get Methods
    const MUSE::Project         &getProject             () const    { return  project; }

    const std::vector<std::string> &getCommands () const    { return commands;}
    const std::string &getCommand (const unsigned int i) const { return commands.at(i); }

    const std::vector<std::string> &getDeps () const { return  dependencies; }
    const std::string &getDep (const unsigned int i) const { return  dependencies.at(i); }

    const MUSE::Rotation        &getRotation            () const    { return  rotation; }
    const DataProjection        &getDataProjection      () const    { return  projection; }
    const StratigraphicTransf   &getStratigraphicTransf () const    { return  stratigraphic_transf; }



    // Set Methods
    void setProject             (const MUSE::Project &d)        { project = d; }
    void setCommands        (const std::vector<std::string> &d) { commands = d; }

    void setDependencies    (const std::vector<std::string> &d) { dependencies = d; }

    void setRotation            (const MUSE::Rotation &d)       { rotation = d; }
    void setDataProjection      (const DataProjection &d)       { projection = d; }
    void setStratigraphicTransf (const StratigraphicTransf &d)  { stratigraphic_transf = d; }


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
        ar (CEREAL_NVP(projection));
        ar (CEREAL_NVP(stratigraphic_transf));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(project));
        ar (CEREAL_NVP(commands));
        ar (CEREAL_NVP(dependencies));

        ar (CEREAL_NVP(rotation));
        ar (CEREAL_NVP(projection));
        ar (CEREAL_NVP(stratigraphic_transf));
    }
#endif

private:

    MUSE::Project project;
    std::vector<std::string> commands;
    std::vector<std::string> dependencies;

    MUSE::Rotation rotation;
    DataProjection projection;
    StratigraphicTransf stratigraphic_transf;


    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename, const int &precision = 3);
};



#ifndef STATIC_MUSELIB
#include "manipulate_meta.cpp"
#endif

#endif // MANIPULATE_META_H
