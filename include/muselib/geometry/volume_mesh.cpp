#include "volume_mesh.h"

#include <cinolib/meshes/tetmesh.h>
#include <cinolib/meshes/hexmesh.h>

#include <cinolib/meshes/abstract_mesh.h>

#include <vtkSmartPointer.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkIntArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridWriter.h>

namespace MUSE
{
template<class M, class V, class E, class F, class P>
VolumeMesh<M,V,E,F,P>::VolumeMesh(const char * filename, const MeshType type)
{
    if (type == MeshType::TETMESH)
    {
        std::cout << "Loading tetmesh ... " << filename << std::endl;

        cinolib::Tetmesh<> *m = new cinolib::Tetmesh<> ;
        m->load (filename);

        std::cout << m->num_verts() << " / " << m->num_polys() << std::endl;

        this->init(m->vector_verts(), m->vector_polys());
         _mesh_type = type;

        delete  m;
    }
    else if (type == MeshType::HEXMESH)
    {
        std::cout << "Loading hexmesh ... " << filename << std::endl;

        cinolib::Hexmesh<> *m = new cinolib::Hexmesh<> ;
        m->load (filename);

        this->init(m->vector_verts(), m->vector_polys());
        _mesh_type = type;

        delete  m;
    }
    else
    {
        std::cout << "ERROR. Only tetmesh/hexmesh are supported as VolumeMesh." << std::endl;
        exit(1);
    }
}


template<class M, class V, class E, class F, class P>
void VolumeMesh<M,V,E,F,P>::save(const char * filename, const MeshType type) const
{
    const std::string fname = filename;
    const std::string ext = fname.substr(fname.find_last_of("."));

    std::vector<std::vector<uint>> poly;
    for(uint pid=0; pid < this->num_polys(); pid++)
        poly.push_back(this->poly_verts_id(pid));

    if (ext.compare(".vtk") == 0 || ext.compare(".mesh") == 0)
    {
        if (type == MeshType::HEXMESH)
        {
            //cinolib::Hexmesh<> *m = new cinolib::Hexmesh<>(this->vector_verts(), this->vector_polys());
            cinolib::Hexmesh<> *m = new cinolib::Hexmesh<>(this->vector_verts(), poly);
            m->save(filename);
            delete m;
        }
        else if (type == MeshType::TETMESH)
        {
            //cinolib::Tetmesh<> *m = new cinolib::Tetmesh<>(this->vector_verts(), this->vector_polys());
            cinolib::Tetmesh<> *m = new cinolib::Tetmesh<>(this->vector_verts(), poly);
            m->save(filename);
            delete m;
        }
    }
    else
    {
        std::cout << "ERROR. Only tetmesh/hexmesh are supported as VolumeMesh." << std::endl;
        exit(1);
    }
}

template<class M, class V, class E, class F, class P>
MeshType VolumeMesh<M,V,E,F,P>::set_meshtype() const
{
    MeshType type;
    for (uint pid=0; pid < this->num_polys(); pid++)
    {
        if(this->poly_is_tetrahedron(pid))
            type = MeshType::TETMESH;
        else if(this->poly_is_hexahedron(pid))
            type = MeshType::HEXMESH;
        else
            type = MeshType::POLYHEDRALMESH;
    }
    return type;
}


template<class M, class V, class E, class F, class P>
void VolumeMesh<M,V,E,F,P>::write_poly_VTK(const char * filename)
{
    setlocale(LC_NUMERIC, "en_US.UTF-8"); // makes sure "." is the decimal separator

    vtkSmartPointer<vtkUnstructuredGridWriter> writer = vtkSmartPointer<vtkUnstructuredGridWriter>::New();
    vtkSmartPointer<vtkUnstructuredGrid>       grid   = vtkSmartPointer<vtkUnstructuredGrid>::New();
    vtkSmartPointer<vtkPoints>                 points = vtkSmartPointer<vtkPoints>::New();

    points->SetDataTypeToDouble();
    // write the vertex coordinates
    //
    for(size_t i=0; i<this->num_verts(); i++)
    {
        points->InsertNextPoint(this->vert(i).x(), this->vert(i).y(), this->vert(i).z());
    }

    // write the tetrahedra
    //
    for(size_t pid=0; pid<this->num_polys(); pid++)
    {
        const std::vector<uint> &verts = this->adj_p2v(pid);

        if (this->poly_is_tetrahedron(pid))
        {
            vtkIdType poly[] = { verts.at(0), verts.at(1), verts.at(2), verts.at(3) };
            grid->InsertNextCell(VTK_TETRA, 4, poly);
        }
        else
            if (this->poly_is_hexahedron(pid))
            {
                vtkIdType poly[] = { verts.at(0), verts.at(1), verts.at(2), verts.at(3), verts.at(4), verts.at(5), verts.at(6), verts.at(7) };
                grid->InsertNextCell(VTK_HEXAHEDRON, 8, poly);
            }
    }

    // create the output mesh
    //
    grid->SetPoints(points);

#if VTK_MAJOR_VERSION < 6
    writer->SetInput(grid);
#else
    writer->SetInputData(grid);
#endif
    writer->SetFileName(filename);
    writer->Write();
}

}
