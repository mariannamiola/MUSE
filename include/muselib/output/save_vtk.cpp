#include "save_vtk.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>

#define IOSUCCESS 0
#define IOERROR 1

int save_vtk(const std::string filename,
             const std::vector<cinolib::vec3d> &verts,
             const std::vector<std::vector<uint>> &polys)
{
    std::ofstream file_out(filename, std::fstream::out);
    if(!file_out.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        return IOERROR;
    }

    file_out.imbue(std::locale::classic());
    file_out << "# vtk DataFile Version 3.0\n";
    file_out << "MUSE volume mesh\n";
    file_out << "ASCII\n";
    file_out << "DATASET UNSTRUCTURED_GRID\n";

    file_out << "POINTS " << verts.size() << " double\n";
    file_out << std::setprecision(17);
    for(const auto &v : verts)
    {
        file_out << v.x() << " " << v.y() << " " << v.z() << "\n";
    }

    std::vector<std::vector<uint>> supported_cells;
    supported_cells.reserve(polys.size());
    for(const auto &p : polys)
    {
        if(p.size() == 4 || p.size() == 8)
            supported_cells.push_back(p);
    }

    size_t cells_size = 0;
    for(const auto &p : supported_cells)
        cells_size += (1 + p.size());

    file_out << "CELLS " << supported_cells.size() << " " << cells_size << "\n";
    for(const auto &p : supported_cells)
    {
        file_out << p.size();
        for(uint vid : p)
            file_out << " " << vid;
        file_out << "\n";
    }

    file_out << "CELL_TYPES " << supported_cells.size() << "\n";
    for(const auto &p : supported_cells)
    {
        if(p.size() == 4) file_out << 10 << "\n";  // VTK_TETRA
        else              file_out << 12 << "\n";  // VTK_HEXAHEDRON
    }

    file_out.close();
    std::cout << "Saving vtk file: " << filename << " ... COMPLETED." << std::endl;
    return IOSUCCESS;
}
