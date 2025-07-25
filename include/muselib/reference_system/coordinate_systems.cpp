// https://proj.org/development/reference/datatypes.html#c.PJ_DIRECTION
// https://github.com/OSGeo/PROJ/blob/master/examples/pj_obs_api_mini_demo.c

#include "coordinate_systems.h"

#include <iostream>


void coordinate_transformation (const double &x, const double &y, const double &z,
                                const std::string source, const std::string target,
                                //const unsigned int source, const unsigned int target,
                                double &tx, double &ty, double &tz)
{
    #ifdef MUSE_USES_PROJ

    PJ_CONTEXT *C;
    PJ *P; //oggetto trasformazione
    PJ *norm;
    PJ_COORD coords, transf_coords;

    C = proj_context_create(); //Create a new threading-context

    // Create a transformation object that is a pipeline between two known coordinate reference systems
//    P = proj_create_crs_to_crs (C,
//                                std::string("EPSG:" + std::to_string(source)).c_str(), // source crs (from)
//                                std::string("EPSG:" + std::to_string(target)).c_str(), // target crs (to)
//                                nullptr);

    P = proj_create_crs_to_crs (C, source.c_str(), target.c_str(), nullptr);

    if (0 == P)
    {
        std::cerr << "ERROR PROJ: Failed to create transformation" << std::endl;
        exit(1);
    }

    //Coordinate di partenza
    coords.xyz.x = x;
    coords.xyz.y = y;
    coords.xyz.z = z;

    transf_coords = proj_trans(P, PJ_DIRECTION::PJ_FWD, coords); //input: oggetto trasformazione, direzione di trasformazione, coordinate che saranno trasformate
    //PJ_FWD: trasformazione in avanti (altrimenti: identità (IDENT) o inversa (INV))

    //Coordinate trasformate
    tx = transf_coords.xyz.x;
    ty = transf_coords.xyz.y;
    tz = transf_coords.xyz.z;

    proj_context_destroy(C);

#else

    std::cerr << "ERROR : PROJ library missing. Install GDAL and recompile defining symbol MUSE_USES_PROJ" << std::endl;

#endif
}



//#endif
