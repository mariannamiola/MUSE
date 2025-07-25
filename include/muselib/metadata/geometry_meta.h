#ifndef GEOMETRY_META_H
#define GEOMETRY_META_H

#include <string>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include "muselib/data_structures/point.h"
#include "muselib/data_structures/project.h"
#include "muselib/data_structures/geometry.h"
#include "muselib/data_structures/surface.h"
#include "muselib/data_structures/volume.h"
#include "muselib/data_structures/rotation.h"

//DA RIMUOVERE!! PRIMA CONTROLLARE

namespace MUSE
{
    class GeometryMeta;
}

class MUSE::GeometryMeta
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


    struct ObjectInfo
    {
        std::string method;
        std::string surface;
        std::string extrusion_type;
        std::string extrusion_direction;
        double      extrusion_value = 0.0;

        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(method));
            ar (CEREAL_NVP(surface));
            ar (CEREAL_NVP(extrusion_type));
            ar (CEREAL_NVP(extrusion_direction));
            ar (CEREAL_NVP(extrusion_value));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(method));
            ar (CEREAL_NVP(surface));
            ar (CEREAL_NVP(extrusion_type));
            ar (CEREAL_NVP(extrusion_direction));
            ar (CEREAL_NVP(extrusion_value));
        }
        #endif
    };


    // Get Methods
    const MUSE::Project         &getProject         () const    { return  project; }

    const std::vector<std::string> &getCommands     () const    { return commands;}
    const std::string &getCommand (const unsigned int i) const { return commands.at(i); }

    const MUSE::GeospatialData  &getGeospatialData  () const    { return geospatialdata; }
    const DataSummary           &getDataSummary     () const    { return data_summary; }

    const MUSE::Rotation        &getDataRotation    () const    { return data_rotation; }
    const ObjectInfo            &getObjectInfo      () const    { return object_info; }
    const MUSE::Surface         &getMeshSummary     () const    { return mesh; }
    const MUSE::Volume          &getVolumeSummary   () const    { return volume; }




    // Set Methods
    void setProject         (const MUSE::Project &d)        { project = d; }

    void setCommands        (const std::vector<std::string> &d) { commands = d; }

    void setGeospatialData  (const MUSE::GeospatialData &d) { geospatialdata = d; }

    void setDataSummary     (const DataSummary &d)          { data_summary = d; }

    void setDataRotation    (const MUSE::Rotation &d)       { data_rotation = d; }
    void setObjectInfo      (const ObjectInfo &d)           { object_info = d; }
    void setMeshSummary     (const MUSE::Surface &d)        { mesh = d; }

    void setVolumeSummary   (const MUSE::Volume &d)         { volume = d; }



    // Additional Methods
    bool read  (const std::string filename);
    bool write (const std::string filename);




#ifdef MUSE_USES_CEREAL
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar (CEREAL_NVP(project));
        ar (CEREAL_NVP(commands));
        ar (CEREAL_NVP(geospatialdata));
        ar (CEREAL_NVP(data_summary));
        ar (CEREAL_NVP(data_rotation));
        ar (CEREAL_NVP(object_info));
        ar (CEREAL_NVP(mesh));
        ar (CEREAL_NVP(volume));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(project));
        ar (CEREAL_NVP(commands));
        ar (CEREAL_NVP(geospatialdata));
        ar (CEREAL_NVP(data_summary));
        ar (CEREAL_NVP(data_rotation));
        ar (CEREAL_NVP(object_info));
        ar (CEREAL_NVP(mesh));
        ar (CEREAL_NVP(volume));
    }
#endif

private:

    MUSE::Project project;
    std::vector<std::string> commands;
    MUSE::GeospatialData geospatialdata;
    DataSummary data_summary;
    MUSE::Rotation data_rotation;
    ObjectInfo object_info;
    MUSE::Surface mesh;
    MUSE::Volume volume;



    bool readConfFileJSON   (const std::string filename);
    bool writeConfFileJSON  (const std::string filename, const int &precision = 3) ;
};



#ifndef STATIC_MUSELIB
#include "geometry_meta.cpp"
#endif

#endif // GEOMETRY_META_H
