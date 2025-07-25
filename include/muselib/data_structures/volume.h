#ifndef VOLUME_H
#define VOLUME_H

#include <string>

#include <cereal/archives/json.hpp>

#include <cinolib/meshes/meshes.h>

namespace MUSE
{
    class Volume;
}

class MUSE::Volume
{
public:

    struct Parameters
    {
        std::string type;
        //std::string boundary;

        std::string opt;

        double resx = 0.0;
        double resy = 0.0;
        double resz = 0.0;

    #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(opt));

            ar (CEREAL_NVP(resx));
            ar (CEREAL_NVP(resy));
            ar (CEREAL_NVP(resz));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(opt));

            ar (CEREAL_NVP(resx));
            ar (CEREAL_NVP(resy));
            ar (CEREAL_NVP(resz));
        }
    #endif

    };


    struct Summary
    {
        uint nverts;
        uint nedges;
        uint nfaces;
        uint npolys;

        double min_edge;
        double max_edge;
        double avg_edge;

        double avg_poly;

        double bbox_dx;
        double bbox_dy;
        double bbox_dz;

        double volume;


        // Add any other additional descriptive info

        #ifdef MUSE_USES_CEREAL
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar (CEREAL_NVP(nverts));
            ar (CEREAL_NVP(nedges));
            ar (CEREAL_NVP(nfaces));
            ar (CEREAL_NVP(npolys));

            ar (CEREAL_NVP(min_edge));
            ar (CEREAL_NVP(max_edge));
            ar (CEREAL_NVP(avg_edge));

            ar (CEREAL_NVP(avg_poly));

            ar (CEREAL_NVP(bbox_dx));
            ar (CEREAL_NVP(bbox_dy));
            ar (CEREAL_NVP(bbox_dz));

            ar (CEREAL_NVP(volume));
        }

        template <class Archive>
        void deserialize( Archive & ar )
        {
            ar (CEREAL_NVP(nverts));
            ar (CEREAL_NVP(nedges));
            ar (CEREAL_NVP(nfaces));
            ar (CEREAL_NVP(npolys));

            ar (CEREAL_NVP(min_edge));
            ar (CEREAL_NVP(max_edge));
            ar (CEREAL_NVP(avg_edge));

            ar (CEREAL_NVP(avg_poly));

            ar (CEREAL_NVP(bbox_dx));
            ar (CEREAL_NVP(bbox_dy));
            ar (CEREAL_NVP(bbox_dz));

            ar (CEREAL_NVP(volume));
        }
        #endif

    };

    // Get Methods
    const Parameters    &getParameters  () const    { return parameters; }
    const Summary       &getSummary     () const    { return summary; }


    // Set Methods
    void setParameters  (const Parameters &d)    { parameters = d; }

    template<class M, class V, class E, class F, class P>
    void setSummary     (const cinolib::AbstractPolyhedralMesh<M,V,E,F,P> &mesh);


#ifdef MUSE_USES_CEREAL
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar (CEREAL_NVP(parameters));
        ar (CEREAL_NVP(summary));

    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP(parameters));
        ar (CEREAL_NVP(summary));

    }
#endif

private:

    Parameters parameters;
    Summary summary;

};


#ifndef STATIC_MUSELIB
#include "volume.cpp"
#endif

#endif // VOLUME_H
