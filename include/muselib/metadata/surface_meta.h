#ifndef SURFACE_META_H
#define SURFACE_META_H

#include <string>

#include "muselib/data_structures/point.h"
#include "muselib/data_structures/project.h"
#include "muselib/data_structures/geometry.h"
#include "muselib/data_structures/surface.h"
#include "muselib/data_structures/rotation.h"

namespace MUSE
{
    class SurfaceMeta;
}

class MUSE::SurfaceMeta
{
public:

    struct DataSummary
    {
        double  x_min = DBL_MAX;
        double  x_max = -DBL_MAX;
        double  y_min = DBL_MAX;
        double  y_max = -DBL_MAX;
        double  z_min = DBL_MAX;
        double  z_max = -DBL_MAX;


        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(x_min));
            ar (CEREAL_NVP(x_max));
            ar (CEREAL_NVP(y_min));
            ar (CEREAL_NVP(y_max));
            ar (CEREAL_NVP(z_min));
            ar (CEREAL_NVP(z_max));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(x_min));
            ar (CEREAL_NVP(x_max));
            ar (CEREAL_NVP(y_min));
            ar (CEREAL_NVP(y_max));
            ar (CEREAL_NVP(z_min));
            ar (CEREAL_NVP(z_max));
        }
        #endif

        void setDataSummary     (const std::vector<Point3D> &d);

    };

    struct Extrusion
    {
        std::string type;
        std::string direction;
        double      value = 0.0;

        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(type));
            ar (CEREAL_NVP(direction));
            ar (CEREAL_NVP(value));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(type));
            ar (CEREAL_NVP(direction));
            ar (CEREAL_NVP(value));
        }
        #endif
    };


    // Get Methods
    const MUSE::Project         &getProject         () const    { return  project; }

    const std::vector<std::string> &getCommands () const    { return commands;}
    const std::string &getCommand (const unsigned int i) const { return commands.at(i); }

    const std::vector<std::string> &getDeps () const { return  dependencies; }
    const std::string &getDep (const unsigned int i) const { return  dependencies.at(i); }

    const MUSE::GeospatialData  &getGeospatialData  () const    { return geospatialdata; }
    const DataSummary           &getDataSummary     () const    { return data_summary; }

    const MUSE::Rotation        &getDataRotation    () const    { return data_rotation; }
    const Extrusion             &getExtrusion      () const     { return extrusion; }
    const MUSE::Surface         &getMeshSummary     () const    { return mesh; }



    // Set Methods
    void setProject         (const MUSE::Project &d)        { project = d; }

    void setCommands        (const std::vector<std::string> &d) { commands = d; }
    void setDependencies    (const std::vector<std::string> &d) { dependencies = d; }

    void setGeospatialData  (const MUSE::GeospatialData &d) { geospatialdata = d; }


    void setDataSummary     (const DataSummary &d)          { data_summary = d; }

    void setDataRotation    (const MUSE::Rotation &d)       { data_rotation = d; }
    void setExtrusion       (const Extrusion &d)            { extrusion = d; }
    void setMeshSummary     (const MUSE::Surface &d)        { mesh = d; }



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
        ar (CEREAL_NVP(geospatialdata));
        ar (CEREAL_NVP(data_summary));
        ar (CEREAL_NVP(data_rotation));
        ar (CEREAL_NVP(extrusion));
        ar (CEREAL_NVP(mesh));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(project));
        ar (CEREAL_NVP(commands));
        ar (CEREAL_NVP(dependencies));
        ar (CEREAL_NVP(geospatialdata));
        ar (CEREAL_NVP(data_summary));
        ar (CEREAL_NVP(data_rotation));
        ar (CEREAL_NVP(extrusion));
        ar (CEREAL_NVP(mesh));
    }
#endif

private:

    MUSE::Project project;
    std::vector<std::string> commands;
    std::vector<std::string> dependencies;

    MUSE::GeospatialData geospatialdata;
    DataSummary data_summary;
    MUSE::Rotation data_rotation;
    Extrusion extrusion;
    MUSE::Surface mesh;



    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename, const int &precision = 3) ;
};



#ifndef STATIC_MUSELIB
#include "surface_meta.cpp"
#endif

#endif // SURFACE_META_H
