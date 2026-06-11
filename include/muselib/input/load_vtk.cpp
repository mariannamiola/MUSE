#include "load_vtk.h"

#include <fstream>
#include <iostream>
#include <locale>

#define IOSUCCESS 0
#define IOERROR 1

int load_vtk(const std::string filename,
             std::vector<cinolib::vec3d> &verts,
             std::vector<std::vector<uint>> &polys)
{
    verts.clear();
    polys.clear();

    std::ifstream file_in(filename, std::fstream::in);
    if(!file_in.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        return IOERROR;
    }

    file_in.imbue(std::locale::classic());

    std::string token;
    std::vector<std::vector<uint>> raw_cells;
    std::vector<int> cell_types;

    while(file_in >> token)
    {
        if(token == "POINTS")
        {
            size_t n_points = 0;
            std::string data_type;
            file_in >> n_points >> data_type;

            verts.reserve(n_points);
            for(size_t i = 0; i < n_points; ++i)
            {
                double x = 0.0, y = 0.0, z = 0.0;
                if(!(file_in >> x >> y >> z))
                {
                    std::cerr << "ERROR: invalid POINTS section in " << filename << std::endl;
                    return IOERROR;
                }
                verts.emplace_back(x, y, z);
            }
        }
        else if(token == "CELLS")
        {
            size_t n_cells = 0;
            size_t total_size = 0;
            file_in >> n_cells >> total_size;

            raw_cells.clear();
            raw_cells.reserve(n_cells);

            for(size_t cid = 0; cid < n_cells; ++cid)
            {
                size_t n_verts = 0;
                if(!(file_in >> n_verts))
                {
                    std::cerr << "ERROR: invalid CELLS section in " << filename << std::endl;
                    return IOERROR;
                }

                std::vector<uint> cell;
                cell.reserve(n_verts);
                for(size_t j = 0; j < n_verts; ++j)
                {
                    uint vid = 0;
                    if(!(file_in >> vid))
                    {
                        std::cerr << "ERROR: invalid CELLS connectivity in " << filename << std::endl;
                        return IOERROR;
                    }
                    cell.push_back(vid);
                }
                raw_cells.push_back(cell);
            }
        }
        else if(token == "CELL_TYPES")
        {
            size_t n_types = 0;
            file_in >> n_types;

            cell_types.clear();
            cell_types.reserve(n_types);

            for(size_t i = 0; i < n_types; ++i)
            {
                int ctype = 0;
                if(!(file_in >> ctype))
                {
                    std::cerr << "ERROR: invalid CELL_TYPES section in " << filename << std::endl;
                    return IOERROR;
                }
                cell_types.push_back(ctype);
            }
        }
    }

    if(verts.empty() || raw_cells.empty())
    {
        std::cerr << "ERROR: missing points/cells in vtk file " << filename << std::endl;
        return IOERROR;
    }

    if(!cell_types.empty())
    {
        const size_t n = std::min(raw_cells.size(), cell_types.size());
        polys.reserve(n);

        for(size_t i = 0; i < n; ++i)
        {
            if(cell_types.at(i) == 10 && raw_cells.at(i).size() == 4) // VTK_TETRA
            {
                polys.push_back(raw_cells.at(i));
            }
            else if(cell_types.at(i) == 12 && raw_cells.at(i).size() == 8) // VTK_HEXAHEDRON
            {
                polys.push_back(raw_cells.at(i));
            }
        }
    }
    else
    {
        for(const auto &cell : raw_cells)
        {
            if(cell.size() == 4 || cell.size() == 8)
                polys.push_back(cell);
        }
    }

    if(polys.empty())
    {
        std::cerr << "ERROR: no tetra/hexa cells found in vtk file " << filename << std::endl;
        return IOERROR;
    }

    std::cout << "Loading vtk file: " << filename << " ... COMPLETED." << std::endl;
    return IOSUCCESS;
}
