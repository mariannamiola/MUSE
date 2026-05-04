#ifndef FITVARIO_METHODS_H
#define FITVARIO_METHODS_H

#include <string>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include <geostatslib/statistics/data_structures.h>
#include <geostatslib/statistics/variogram.h>

namespace MUSE
{

struct nested_variogram_structure_methods
{
    std::string type;
    double sill = 0.0;
    double range = 0.0;
    double range_min = 0.0;
    double range_max = 0.0;
    double anisotropy_ratio = 1.0;
    double azimuth = 0.0;

#ifdef MUSE_USES_CEREAL
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar (CEREAL_NVP (type));
        ar (CEREAL_NVP (sill));
        ar (CEREAL_NVP (range));
        ar (CEREAL_NVP (range_min));
        ar (CEREAL_NVP (range_max));
        ar (CEREAL_NVP (anisotropy_ratio));
        ar (CEREAL_NVP (azimuth));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP (type));
        ar (CEREAL_NVP (sill));
        ar (CEREAL_NVP (range));
        ar (CEREAL_NVP (range_min));
        ar (CEREAL_NVP (range_max));
        ar (CEREAL_NVP (anisotropy_ratio));
        ar (CEREAL_NVP (azimuth));
    }
#endif
};

class variogram_methods : public variogram
{
    public:

    // Set Methods
    void setDirection       (const double d)                {degree_direction = d; }
    void setNugget          (const double n)                {nugget = n; }
    void setSill            (const double s)                {sill = s; }
    void setType            (const std::string t)           {type = t; }
    void setRangeZ          (const double rz)               {range_z = rz; }
    void clearNestedStructures()                            {nested_structures.clear();}
    void setNestedStructures (const std::vector<variogram_structure> &components)
    {
        nested_structures.clear();
        nested_structures.reserve(components.size());

        for (const variogram_structure &component : components)
        {
            nested_variogram_structure_methods serialized_component;
            serialized_component.type = component.type;
            serialized_component.sill = component.sill;
            serialized_component.range = component.range;
            serialized_component.range_min = component.range_min;
            serialized_component.range_max = component.range_max;
            serialized_component.anisotropy_ratio = component.anisotropy_ratio;
            serialized_component.azimuth = component.azimuth;

            nested_structures.push_back(serialized_component);
        }
    }


    // Get Methods
    double              getDirection        ()  const       {return degree_direction;}
    double              getNugget           ()  const       {return nugget; }
    double              getSill             ()  const       {return sill; }
    std::string         getType             ()  const       {return type; }
    double              getRangeZ           ()  const       {return range_z; }
    bool                hasNestedStructures ()  const       {return !nested_structures.empty();}
    const std::vector<nested_variogram_structure_methods> &getNestedStructures () const {return nested_structures;}
    std::vector<variogram_structure> getNestedStructuresAsVariogram() const
    {
        std::vector<variogram_structure> components;
        components.reserve(nested_structures.size());

        for (const nested_variogram_structure_methods &serialized_component : nested_structures)
        {
            variogram_structure component;
            component.type = serialized_component.type;
            component.sill = serialized_component.sill;
            component.range = serialized_component.range;
            component.range_min = serialized_component.range_min;
            component.range_max = serialized_component.range_max;
            component.anisotropy_ratio = serialized_component.anisotropy_ratio;
            component.azimuth = serialized_component.azimuth;

            components.push_back(component);
        }

        return components;
    }



#ifdef MUSE_USES_CEREAL
    template <class Archive>
    void save( Archive & ar ) const
    {
        ar (CEREAL_NVP (degree_direction));
        ar (CEREAL_NVP (type));
        ar (CEREAL_NVP (nugget));
        ar (CEREAL_NVP (sill));
        ar (CEREAL_NVP (range));
        ar (CEREAL_NVP (range_max));
        ar (CEREAL_NVP (range_min));
        ar (CEREAL_NVP (range_z));
        ar (CEREAL_NVP (nested_structures));
    }

    template <class Archive>
    void load( Archive & ar )
    {
        ar (CEREAL_NVP (degree_direction));
        ar (CEREAL_NVP (type));
        ar (CEREAL_NVP (nugget));
        ar (CEREAL_NVP (sill));
        ar (CEREAL_NVP (range));
        ar (CEREAL_NVP (range_max));
        ar (CEREAL_NVP (range_min));
        ar (CEREAL_NVP (range_z));

        nested_structures.clear();
        try
        {
            ar (CEREAL_NVP (nested_structures));
        }
        catch (...)
        {
            nested_structures.clear();
        }
    }
#endif

private:

    double degree_direction = 0.0;
    double range_z = 0.0;
    std::vector<nested_variogram_structure_methods> nested_structures;
};

}


#endif // FITVARIO_METHODS_H
