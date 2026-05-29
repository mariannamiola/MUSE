#ifndef SAVE_VTK_H
#define SAVE_VTK_H

#include <string>
#include <vector>

#include <cinolib/geometry/vec_mat.h>

int save_vtk(const std::string filename,
             const std::vector<cinolib::vec3d> &verts,
             const std::vector<std::vector<uint>> &polys);

#ifndef STATIC_MUSELIB
#include "save_vtk.cpp"
#endif

#endif // SAVE_VTK_H
