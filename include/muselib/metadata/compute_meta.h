#ifndef COMPUTE_META_H
#define COMPUTE_META_H

#include <string>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include "muselib/metadata/fitvario_methods.h"

#include "muselib/data_structures/project.h"
//#include "muselib/data_structures/dependency.h"
#include "muselib/data_structures/info_data.h"
#include "muselib/data_structures/rotation.h"


namespace MUSE
{
    class ComputeMeta;
}

class MUSE::ComputeMeta
{
public:

    struct Manipulate
    {
        std::string sub_dataset = "NO";
        std::string domain;

        std::string stratigraphic_transf = "NO";
        std::string filename;

        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(sub_dataset));
            ar (CEREAL_NVP(domain));

            ar (CEREAL_NVP(stratigraphic_transf));
            ar (CEREAL_NVP(filename));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(sub_dataset));
            ar (CEREAL_NVP(domain));

            ar (CEREAL_NVP(stratigraphic_transf));
            ar (CEREAL_NVP(filename));
        }
        #endif
    };

    struct Processing
    {
        std::string v_name;

        std::string declustering;
        std::string normal_score;

        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(v_name));
            ar (CEREAL_NVP(declustering));
            ar (CEREAL_NVP(normal_score));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(v_name));
            ar (CEREAL_NVP(declustering));
            ar (CEREAL_NVP(normal_score));
        }
        #endif
    };

    struct InfoVariogram
    {
        std::string dimension;
        std::string direction;

        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(dimension));
            ar (CEREAL_NVP(direction));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(dimension));
            ar (CEREAL_NVP(direction));
        }
        #endif
    };

    struct Simulation
    {
        std::string geometry;
        uint n_elements;

        std::string sim_criterion;
        int n_iterations;

        bool back_normal_score = false;
        std::string extrapolation_type;
        double min_extrapolation_value;
        double max_extrapolation_value;

        // double est_mean_zscore;
        // double est_var_zscore;
        // double est_mean;
        // double est_var;


        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(geometry));
            ar (CEREAL_NVP(n_elements));

            ar (CEREAL_NVP(sim_criterion));
            ar (CEREAL_NVP(n_iterations));

            ar (CEREAL_NVP(back_normal_score));
            ar (CEREAL_NVP(extrapolation_type));
            ar (CEREAL_NVP(min_extrapolation_value));
            ar (CEREAL_NVP(max_extrapolation_value));

            // ar (CEREAL_NVP(est_mean));
            // ar (CEREAL_NVP(est_var));
            // ar (CEREAL_NVP(est_mean_zscore));
            // ar (CEREAL_NVP(est_var_zscore));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(geometry));
            ar (CEREAL_NVP(n_elements));

            ar (CEREAL_NVP(sim_criterion));
            ar (CEREAL_NVP(n_iterations));

            ar (CEREAL_NVP(back_normal_score));
            ar (CEREAL_NVP(extrapolation_type));
            ar (CEREAL_NVP(min_extrapolation_value));
            ar (CEREAL_NVP(max_extrapolation_value));

            // ar (CEREAL_NVP(est_mean));
            // ar (CEREAL_NVP(est_var));
            // ar (CEREAL_NVP(est_mean_zscore));
            // ar (CEREAL_NVP(est_var_zscore));
        }
        #endif
    };





    // Get Methods
    const MUSE::Project     &getProject         () const    { return  project; }

    const std::vector<std::string> &getCommands () const    { return commands;}
    const std::string &getCommand (const unsigned int i) const { return commands.at(i); }

    const std::vector<std::string> &getDeps     () const { return  dependencies; }
    const std::string &getDep (const unsigned int i) const { return  dependencies.at(i); }

    const MUSE::InfoData    &getInfoData        () const    { return  data; }
    const MUSE::Rotation    &getRotation        () const    { return  rotation; }

    const Manipulate        &getManipulate      () const    { return  manipulate; }
    const Processing        &getProcessing      () const    { return  variable; }
    const InfoVariogram     &getInfoVariogram   () const    { return  info_vario; }

    const MUSE::variogram_methods &getFitExpVariog () const { return  fit_experimental_vario; }

    const Simulation        &getSimulation      () const    { return  simulation; }



    // Set Methods
    void setProject         (const MUSE::Project &d)    { project = d; }

    void setCommands        (const std::vector<std::string> &d) { commands = d; }

    void setDependencies    (const std::vector<std::string> &d) { dependencies = d; }
    //void setDependency      (const std::string &d)              { dependencies.at(i) = d; }
    //void setDependencies    (const std::vector<MUSE::Dependency> &d) { dependencies = d; }

    void setInfoData        (const MUSE::InfoData &d)   { data = d; }
    void setRotation        (const MUSE::Rotation &d)   { rotation = d; }

    void setManipulate      (const Manipulate &d)       { manipulate = d; }
    void setProcessing      (const Processing &d)       { variable = d; }
    void setInfoVariogram   (const InfoVariogram &d)    { info_vario = d; }

    void setFitExpVariog    (const MUSE::variogram_methods &d)    { fit_experimental_vario = d; }

    void setSimulation      (const Simulation &d)       { simulation = d; }


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
        ar (CEREAL_NVP(rotation));
        ar (CEREAL_NVP(manipulate));
        ar (CEREAL_NVP(variable));

        ar (CEREAL_NVP(info_vario));
        ar (CEREAL_NVP(fit_experimental_vario));

        ar (CEREAL_NVP(simulation));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(project));
        ar (CEREAL_NVP(commands));
        ar (CEREAL_NVP(dependencies));
        ar (CEREAL_NVP(data));
        ar (CEREAL_NVP(rotation));
        ar (CEREAL_NVP(manipulate));
        ar (CEREAL_NVP(variable));

        ar (CEREAL_NVP(info_vario));
        ar (CEREAL_NVP(fit_experimental_vario));

        ar (CEREAL_NVP(simulation));
    }
#endif

private:

    MUSE::Project project;

    std::vector<std::string> commands;
    std::vector<std::string> dependencies;


    MUSE::InfoData data;
    MUSE::Rotation rotation;

    Manipulate manipulate;
    Processing variable;
    InfoVariogram info_vario;

    //std::vector<MUSE::variogram_methods> fit_experimental_vario;
    MUSE::variogram_methods fit_experimental_vario;

    Simulation simulation;


    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename, const int &precision = 4) ;
};



#ifndef STATIC_MUSELIB
#include "compute_meta.cpp"
#endif


#endif // COMPUTE_META_H
