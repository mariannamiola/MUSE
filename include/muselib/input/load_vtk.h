#ifndef LOAD_VTK_H
#define LOAD_VTK_H

#include <string>
#include <vector>

#include <cinolib/geometry/vec_mat.h>

int load_vtk(const std::string filename,
             std::vector<cinolib::vec3d> &verts,
             std::vector<std::vector<uint>> &polys);

#ifndef STATIC_MUSELIB
#include "load_vtk.cpp"
#endif

#endif // LOAD_VTK_H
