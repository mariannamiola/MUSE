// https://proj.org/development/reference/datatypes.html#c.PJ_DIRECTION
// https://github.com/OSGeo/PROJ/blob/master/examples/pj_obs_api_mini_demo.c

#include "coordinate_systems.h"

#include <iostream>
#include <fstream>
#include <string>

#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <ogrsf_frmts.h>
#include <cpl_error.h>

void analyzeSRS(const char* wkt, const std::string& file_path, std::string &epsg_code)
{
    // Crea oggetto SpatialReference
    OGRSpatialReference srs;
    if (srs.importFromWkt(wkt) != OGRERR_NONE) {
        std::cout << "=== ERROR to elaborate projection for the file: " << file_path << std::endl;
        return;
    }

    // Codice EPSG
    //const char*
    const char* code = srs.GetAuthorityCode(nullptr);
    epsg_code = code ? std::string(code) : "unknown";
    std::cout << "=== Setting Project authority ..." << std::endl;

    std::cout << std::endl;
    std::cout << "==================================== " << std::endl;
    if (code)
    {
        std::cout << "=== EPSG: " << epsg_code << std::endl;
    }

    // Tipo di sistema
    if (srs.IsGeographic())
    {
        std::cout << "=== Type: Geographic Coordinate System (lat/lon)" << std::endl;
        std::cout << "=== WARNING: Convert data to projected coordinate system (meters) for suitable geoemtric processing!" << std::endl;

        // Unità angolari
        char* angular_units = nullptr;
        double angular_factor = srs.GetAngularUnits(&angular_units);
        std::cout << "=== Units: " << (angular_units ? angular_units : "unknown")
                  << " (factor: " << angular_factor << ")" << std::endl;

    } else if (srs.IsProjected()) {
        std::cout << "=== Type: Projected Coordinate System" << std::endl;

        // Nome proiezione
        const char* projection = srs.GetAttrValue("PROJECTION");
        if (projection) {
            std::cout << "=== Projection: " << projection << std::endl;
        }

        // Unità lineari
        char* linear_units = nullptr;
        double linear_factor = srs.GetLinearUnits(&linear_units);
        std::cout << "=== Units: " << (linear_units ? linear_units : "unknown")
                  << " (factor: " << linear_factor << ")" << std::endl;

    } else {
        std::cout << "Type: Unknown Coordinate System" << std::endl;
    }

    // Nome del sistema
    const char* crs_name = nullptr;
    if (srs.IsProjected()) {
        crs_name = srs.GetAttrValue("PROJCS");
    } else if (srs.IsGeographic()) {
        crs_name = srs.GetAttrValue("GEOGCS");
    }

    if (crs_name) {
        std::cout << "=== Name: " << crs_name << std::endl;
    }
    std::cout << "==================================== " << std::endl;
    std::cout << std::endl;
}

void analyzeRaster(GDALDataset* dataset, const std::string& file_path, std::string &epsg_code)
{
    const char* projection_wkt = dataset->GetProjectionRef();
    if (!projection_wkt || strlen(projection_wkt) == 0) {
        std::cout << "=== ERROR: No information found for the file: " << file_path << std::endl;
        return;
    }

    analyzeSRS(projection_wkt, file_path, epsg_code);
}

void analyzeVector(const std::string& file_path, std::string &epsg_code)
{
    GDALDataset* dataset = static_cast<GDALDataset*>(
        GDALOpenEx(file_path.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr));

    if (!dataset) {
        std::cout << "=== ERROR to open vector file " << file_path << std::endl;
        std::cout << "=== GDAL ERROR: " << CPLGetLastErrorMsg() << std::endl;
        return;
    }

    // Ottieni il primo layer
    OGRLayer* layer = dataset->GetLayer(0);
    if (!layer) {
        std::cout << "=== No layer found in the file: " << file_path << std::endl;
        GDALClose(dataset);
        return;
    }

    // Ottieni la proiezione dal layer
    const OGRSpatialReference* srs = layer->GetSpatialRef();
    if (!srs) {
        std::cout << "=== No projection information found in the file: " << file_path << std::endl;
        GDALClose(dataset);
        return;
    }

    // Converte in WKT
    char* wkt = nullptr;
    srs->exportToWkt(&wkt);

    if (wkt) {
        analyzeSRS(wkt, file_path, epsg_code);
        CPLFree(wkt);
    }

    GDALClose(dataset);
}

void printSpatialReferenceInfo(const std::string& file_path, std::string &epsg_code)
{
    // Verifica se il file esiste
    GDALAllRegister();
    OGRRegisterAll();

    std::ifstream file(file_path);
    if (!file.is_open())
    {
        std::cerr << "=== ERROR while reading file " << file_path << std::endl;
        return;
    }
    //file.close();

    std::cout << "=== Reading geospatial file information ..." << std::endl;
    // Prova prima con GDAL (per raster)
    GDALDataset* dataset = static_cast<GDALDataset*>(
        GDALOpen(file_path.c_str(), GA_ReadOnly));

    if (dataset) {
        // È un raster, usa GDAL
        analyzeRaster(dataset, file_path, epsg_code);
        GDALClose(dataset);
    } else {
        // Potrebbe essere vettoriale, prova con OGR
        analyzeVector(file_path, epsg_code);
    }
}


///
/// \brief listAvailableDrivers
///
void listAvailableDrivers() {
    std::cout << "\n=== Available drivers" << std::endl;

    std::cout << "Driver GDAL (raster): " << GDALGetDriverCount() << std::endl;
    for (int i = 0; i < std::min(10, GDALGetDriverCount()); i++) {
        GDALDriverH driver = GDALGetDriver(i);
        std::cout << "  - " << GDALGetDriverShortName(driver)
                  << " (" << GDALGetDriverLongName(driver) << ")" << std::endl;
    }

    std::cout << "Driver OGR (vector): " << OGRGetDriverCount() << std::endl;
    for (int i = 0; i < std::min(10, OGRGetDriverCount()); i++) {
        OGRSFDriverH driver = OGRGetDriver(i);
        std::cout << "  - " << OGR_Dr_GetName(driver) << std::endl;
    }
}

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
