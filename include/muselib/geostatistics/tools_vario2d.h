#ifndef TOOLS_VARIO2D_H
#define TOOLS_VARIO2D_H

#include "muselib/metadata/vario_meta.h"
#include "muselib/geostatistics/fitvario.h"

#include <cinolib/geometry/vec_mat.h>

MUSE::BackprojectedEllipse backproject_ellipse_to_original_plane(const MUSE::EllipseParameter &ellipse_xy, const std::vector<double> &range_pts_x, const std::vector<double> &range_pts_y, const MUSE::Rotation &rotation);

#ifndef STATIC_MUSELIB
#include "tools_vario2d.cpp"
#endif

#endif // TOOLS_VARIO2D_H