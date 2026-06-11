#ifndef VARIO_PLANE_METHODS_H
#define VARIO_PLANE_METHODS_H

#include <string>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

#include "muselib/geostatistics/fitvario.h"

#include "muselib/metadata/vario_methods.h"
#include "muselib/metadata/fitvario_methods.h"


namespace MUSE
{

// Variographic analysis on a single arbitrary plane.
// The plane is defined by its dip azimuth (degree, clockwise from North) and dip (degree from
// the horizontal): directional variograms are computed inside the plane and expressed in the
// local plane frame (direction 0 = local "north" = dip vector; direction 90 = local "east" = strike).
// The in-plane anisotropy is summarized by the local ellipse; a collection of planes feeds the
// fitting of the 3D anisotropy ellipsoid (see MUSE::EllipsoidParameter).
class plane_vario_methods
{
    public:

    // Set Methods
    void setPlaneId      (const int i)                                       { plane_id = i; }
    void setDipAzimuth   (const double d)                                    { dip_azimuth = d; }
    void setDip          (const double d)                                    { dip = d; }
    void setNormal       (const double nx, const double ny, const double nz) { normal = {nx, ny, nz}; }
    void setDirExpVariog (const std::vector<MUSE::exp_variog_methods> &d)    { experimental_vario = d; }
    void setFitExpVariog (const std::vector<MUSE::variogram_methods> &d)     { fit_experimental_vario = d; }
    void setEllipse      (const MUSE::EllipseParameter &d)                   { ellipse = d; }


    // Get Methods
    int                        getPlaneId      () const { return plane_id; }
    double                     getDipAzimuth   () const { return dip_azimuth; }
    double                     getDip          () const { return dip; }
    const std::vector<double> &getNormal       () const { return normal; }

    const std::vector<MUSE::exp_variog_methods> &getDirExpVariog () const { return experimental_vario; }
    const std::vector<MUSE::variogram_methods>  &getFitExpVariog () const { return fit_experimental_vario; }

    const MUSE::EllipseParameter &getEllipse () const { return ellipse; }



#ifdef MUSE_USES_CEREAL
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar (CEREAL_NVP (plane_id));
        ar (CEREAL_NVP (dip_azimuth));
        ar (CEREAL_NVP (dip));
        ar (CEREAL_NVP (normal));

        ar (CEREAL_NVP (experimental_vario));
        ar (CEREAL_NVP (fit_experimental_vario));

        ar (CEREAL_NVP (ellipse));
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
        ar (CEREAL_NVP (plane_id));
        ar (CEREAL_NVP (dip_azimuth));
        ar (CEREAL_NVP (dip));
        ar (CEREAL_NVP (normal));

        ar (CEREAL_NVP (experimental_vario));
        ar (CEREAL_NVP (fit_experimental_vario));

        ar (CEREAL_NVP (ellipse));
    }
#endif

private:

    // Plane identifier inside the multi-plane workflow (0 = reference plane)
    int plane_id = 0;

    // Plane orientation: dip direction azimuth and dip, both in degree
    double dip_azimuth = 0.0;
    double dip = 0.0;

    // Plane normal unit vector in world coordinates (redundant but convenient downstream)
    std::vector<double> normal = {0.0, 0.0, 1.0};

    // Directional experimental and fitted variograms expressed in the local plane frame
    std::vector<MUSE::exp_variog_methods> experimental_vario;
    std::vector<MUSE::variogram_methods> fit_experimental_vario;

    // Anisotropy ellipse fitted on the in-plane directional ranges (local plane coordinates)
    MUSE::EllipseParameter ellipse;
};

}


#endif // VARIO_PLANE_METHODS_H
