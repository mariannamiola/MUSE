#ifndef MERGE_MESHES_H
#define MERGE_MESHES_H

#include "muselib/geometry/surface_mesh.h"

template<class M, class V, class E, class P>
void merge_meshes(const MUSE::SurfaceMesh<M,V,E,P> &mesh0, const MUSE::SurfaceMesh<M,V,E,P> &mesh1, MUSE::SurfaceMesh<M,V,E,P> &merge_mesh);

// template<class M, class V, class E, class P>
// void merge_meshes_at_coincident_vertices(const MUSE::SurfaceMesh<M,V,E,P> &mesh0, const MUSE::SurfaceMesh<M,V,E,P> &mesh1, MUSE::SurfaceMesh<M,V,E,P> &mesh_merge);

#ifndef STATIC_MUSELIB
#include "merge_meshes.cpp"
#endif

#endif // MERGE_MESHES_H
