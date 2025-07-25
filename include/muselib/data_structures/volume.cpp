#include "volume.h"

template<class M, class V, class E, class F, class P>
void MUSE::Volume::setSummary(const cinolib::AbstractPolyhedralMesh<M,V,E,F,P> &mesh)
{
    this->summary.nverts = mesh.num_verts();
    this->summary.nedges = mesh.num_edges();
    this->summary.nfaces = mesh.num_faces();
    this->summary.npolys = mesh.num_polys();

    this->summary.min_edge = mesh.edge_min_length();
    this->summary.max_edge = mesh.edge_max_length();
    this->summary.avg_edge = mesh.edge_avg_length();

    this->summary.avg_poly = mesh.mesh_volume()/mesh.num_polys();

    this->summary.bbox_dx = mesh.bbox().delta_x();
    this->summary.bbox_dy = mesh.bbox().delta_y();
    this->summary.bbox_dz = mesh.bbox().delta_z();

    this->summary.volume = mesh.mesh_volume();
}
