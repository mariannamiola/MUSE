#ifndef VARIO_META_H
#define VARIO_META_H

#include <string>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include <geostatslib/statistics/variogram.h>

#include "muselib/geostatistics/fitvario.h"

#include "muselib/metadata/vario_methods.h"
#include "muselib/metadata/fitvario_methods.h"

#include "muselib/data_structures/project.h"
//#include "muselib/data_structures/dependency.h"
#include "muselib/data_structures/info_data.h"
#include "muselib/data_structures/rotation.h"


namespace MUSE
{
    class VarioMeta;
}

class MUSE::VarioMeta
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
        //double mean = 0.0;
        //double var = 1.0;

        std::string declustering = "NO";

        std::string normal_score;
        //double mean_zscore = 0.0;
        //double var_zscore = 1.0;

        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(v_name));
            // ar (CEREAL_NVP(mean));
            // ar (CEREAL_NVP(var));
            ar (CEREAL_NVP(declustering));
            ar (CEREAL_NVP(normal_score));
            // ar (CEREAL_NVP(mean_zscore));
            // ar (CEREAL_NVP(var_zscore));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(v_name));
            // ar (CEREAL_NVP(mean));
            // ar (CEREAL_NVP(var));
            ar (CEREAL_NVP(declustering));
            ar (CEREAL_NVP(normal_score));
            // ar (CEREAL_NVP(mean_zscore));
            // ar (CEREAL_NVP(var_zscore));
        }
        #endif
    };

    struct InfoVariogram
    {
        std::string dimension;
        std::string direction;

        double degree_step = 0.0;
        double degree_tolerance = 0.0;
        std::string set_directions;
        int n_directions = 0.0;

        bool clean_is_set = false;
        int n_min_points_for_clean = 0.0;
        bool penalty_function = false;

        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(dimension));
            ar (CEREAL_NVP(direction));

            ar (CEREAL_NVP(degree_step));
            ar (CEREAL_NVP(degree_tolerance));
            ar (CEREAL_NVP(set_directions));
            ar (CEREAL_NVP(n_directions));

            ar (CEREAL_NVP(clean_is_set));
            ar (CEREAL_NVP(n_min_points_for_clean));
            ar (CEREAL_NVP(penalty_function));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(dimension));
            ar (CEREAL_NVP(direction));

            ar (CEREAL_NVP(degree_step));
            ar (CEREAL_NVP(degree_tolerance));
            ar (CEREAL_NVP(set_directions));
            ar (CEREAL_NVP(n_directions));

            ar (CEREAL_NVP(clean_is_set));
            ar (CEREAL_NVP(n_min_points_for_clean));
            ar (CEREAL_NVP(penalty_function));
        }
        #endif
    };


    // Get Methods
    const MUSE::Project     &getProject     () const    { return  project; }

    const std::vector<std::string> &getCommands () const    { return commands;}
    const std::string &getCommand (const unsigned int i) const { return commands.at(i); }

    const std::vector<std::string> &getDeps () const { return  dependencies; }
    const std::string &getDep (const unsigned int i) const { return  dependencies.at(i); }

    const MUSE::InfoData    &getInfoData    () const    { return  infodata; }
    const MUSE::Rotation    &getRotation    () const    { return  rotation; }

    const Manipulate        &getManipulate  () const    { return  manipulate; }
    const Processing        &getProcessing  () const    { return  variable; }

    const InfoVariogram     &getInfoVariogram  () const  { return info_vario; }

    const std::vector<MUSE::exp_variog_methods> &getDirExpVariog () const       { return  experimental_vario; }
    const MUSE::exp_variog_methods &getExpVariog (const unsigned int i) const   { return  experimental_vario.at(i); }

    const std::vector<MUSE::variogram_methods> &getFitExpVariog () const        { return  fit_experimental_vario; }
    const MUSE::variogram_methods &getFitExpVariog (const unsigned int i) const { return  fit_experimental_vario.at(i); }

    const MUSE::EllipseParameter           &getSummary  () const    { return  summary; }



    // Set Methods
    void setProject         (const MUSE::Project &d)    { project = d; }

    void setCommands        (const std::vector<std::string> &d) { commands = d; }

    void setDependencies    (const std::vector<std::string> &d) { dependencies = d; }

    void setInfoData        (const MUSE::InfoData &d)   { infodata = d; }
    void setRotation        (const MUSE::Rotation &d)   { rotation = d; }

    void setManipulate      (const Manipulate &d)       { manipulate = d; }
    void setProcessing      (const Processing &d)       { variable = d; }
    void setInfoVariogram   (const InfoVariogram &d)    { info_vario = d; }

    void setDirExpVariog    (const std::vector<MUSE::exp_variog_methods> &d)   { experimental_vario = d; }
    void setFitExpVariog    (const std::vector<MUSE::variogram_methods> &d)    { fit_experimental_vario = d; }

    void setSummary         (const MUSE::EllipseParameter &d)   { summary = d; }


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

        ar (CEREAL_NVP(infodata));
        ar (CEREAL_NVP(rotation));
        ar (CEREAL_NVP(manipulate));

        ar (CEREAL_NVP(variable));

        ar (CEREAL_NVP(info_vario));
        ar (CEREAL_NVP(experimental_vario));
        ar (CEREAL_NVP(fit_experimental_vario));

        ar (CEREAL_NVP(summary));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(project));
        ar (CEREAL_NVP(commands));
        ar (CEREAL_NVP(dependencies));

        ar (CEREAL_NVP(infodata));
        ar (CEREAL_NVP(rotation));
        ar (CEREAL_NVP(manipulate));

        ar (CEREAL_NVP(variable));

        ar (CEREAL_NVP(info_vario));
        ar (CEREAL_NVP(experimental_vario));
        ar (CEREAL_NVP(fit_experimental_vario));

        ar (CEREAL_NVP(summary));
    }
#endif

private:

    MUSE::Project project;

    std::vector<std::string> commands;
    std::vector<std::string> dependencies;

    MUSE::InfoData infodata;
    MUSE::Rotation rotation;

    Manipulate manipulate;
    Processing variable;
    InfoVariogram info_vario;

    std::vector<MUSE::exp_variog_methods> experimental_vario;
    std::vector<MUSE::variogram_methods> fit_experimental_vario;

    MUSE::EllipseParameter summary;


    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename, const int &precision = 4);
};



#ifndef STATIC_MUSELIB
#include "vario_meta.cpp"
#endif

#endif // VARIO_META_H
