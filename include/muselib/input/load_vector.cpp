#include "load_vector.h"

#include <ogrsf_frmts.h>
#include <iostream>
#include <fstream>
#include <string>

#include "muselib/colors.h"
#include "muselib/utils.h"

#define IOSUCCESS 0
#define IOERROR 1


int load_vectorfile(const std::string filename, std::vector<std::vector<Point3D>> &boundary, std::vector<std::vector<Point3D>> &points, std::string &geometryType)
{
    const std::string ext = filename.substr(filename.find_last_of("."));

    if (ext.compare(".shp") == 0)
        return load_shapefile_shp(filename, boundary, points, geometryType);

    if (ext.compare(".gpkg") == 0)
        return load_gpkg(filename, boundary, points, geometryType);

    std::cerr << "ERROR: Unsupported Vector File format." << std::endl;
    return IOERROR;
}


// Read data from a file
// GDAL - Vector API Tutorial: https://gdal.org/tutorials/vector_api_tut.html
int load_shapefile_shp (const std::string filename, std::vector<std::vector<Point3D>> &boundary, std::vector<std::vector<Point3D>> &points, std::string &geometry_type)
{
    std::string shape_filename;
    shape_filename = filename.substr(filename.find_last_of("/")+1, filename.length());
    std::string basename = get_basename(shape_filename);

#ifdef MUSE_USES_GDAL

    points.clear();
    boundary.clear();

    std::vector<Point3D> set_points;

    // 1. Register all the format drivers that are desired
    GDALAllRegister();

    // 2. Open shape file
    GDALDataset *poDS;
    poDS = static_cast<GDALDataset*> (GDALOpenEx(filename.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL ));
    if( poDS == NULL )
    {
        std::cerr << "Error while loading shapefile " << filename << std::endl;
        exit(1);
    }

    // 3. GDALDataset can potentially have many layers associated with it.

    // prendi i layer dal dataset
    int numLayers = GDALDatasetGetLayerCount(poDS);
    std::cout << "GDAL - Number of Layers: " << numLayers << std::endl;

    OGRLayer  *poLayer = poDS->GetLayerByName(basename.c_str());
    if(poLayer == NULL)
    {
        std::cerr << "ERROR: Failed to get layer " << basename.c_str() << std::endl;
        exit(1);
    }

    OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();

    // 3. Reading features from the layer
    poLayer->ResetReading(); //to ensure we are starting at the beginning of the layer

    OGRFeature *poFeature;

    //We iterate through all the features in the layer using OGRLayer::GetNextFeature()
    std::cout << "GDAL - Number of Features: " << poLayer->GetFeatureCount() << std::endl;

    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
        for( int iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
        {
            OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );

            switch( poFieldDefn->GetType() )
            {
                case OFTInteger:
                    break;
                case OFTInteger64:
                    break;
                case OFTReal:
                    break;
                case OFTString:
                    break;
                default:
                    break;
            }
        }

        // Extract geometry from the feature
        OGRGeometry *poGeometry = poFeature->GetGeometryRef();

        //std::cout << poGeometry->getGeometryType() << "; name = " << poGeometry->getGeometryName() << std::endl;

        if( poGeometry == NULL)
        {
            OGRFeature::DestroyFeature( poFeature );
            //continue;
        }

        if( poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
        {
            geometry_type = poGeometry->getGeometryName();

            OGRPoint *poPoint = (OGRPoint *) poGeometry;
            //printf( "%.3f,%3.f\n", poPoint->getX(), poPoint->getY() );

            // This function can transform a larget set of points.....
            Point3D point;
            point.x = poPoint->getX();
            point.y = poPoint->getY();
            point.z = poPoint->getZ();

            set_points.push_back(point);
        }

        else if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
        {
            geometry_type = poGeometry->getGeometryName();

            //points.push_back(std::vector<Point3D> ());

            OGRLineString* poLine = (OGRLineString*) poGeometry;

            //std::cout << "Geometry Type: LINESTRING" << std::endl;
            //std::cout << "Number of points: " << ls->getNumPoints() << std::endl;

            for(int i = 0; i < poLine->getNumPoints(); i++ )
            {
                OGRPoint p;
                poLine->getPoint(i,&p);

                // This function can transform a larget set of points.....
                Point3D point;
                point.x = p.getX();
                point.y = p.getY();
                point.z = p.getZ();

                //points.push_back(point);
                set_points.push_back(point);
                //points.at(points.size()-1).push_back(point);
            }
        }

        else if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiLineString)
        {
            geometry_type = poGeometry->getGeometryName();

            boundary.push_back(std::vector<Point3D> ());

            OGRMultiLineString* poMultiLine = (OGRMultiLineString*) poGeometry;


            //std::cout << "Geometry Type: MULTILINESTRING" << std::endl;
            //std::cout << "Number of points: " << ls->getNumPoints() << std::endl;

            for(int j=0; j< poMultiLine->getNumGeometries(); j++)
            {
                OGRLineString* poLine = (OGRLineString*)poMultiLine->getGeometryRef(j);

                for(int i = 0; i < poLine->getNumPoints(); i++ )
                {
                    OGRPoint p;
                    poLine->getPoint(i,&p);

                    // This function can transform a larget set of points.....
                    Point3D point;
                    point.x = p.getX();
                    point.y = p.getY();
                    point.z = p.getZ();

                    //boundary.push_back(point);
                    boundary.at(boundary.size()-1).push_back(point);
                }
            }
        }

        else if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
        {
            geometry_type = poGeometry->getGeometryName();
            boundary.push_back(std::vector<Point3D> ());

            OGRPolygon *poly = (OGRPolygon*) poGeometry;
            std::cout << "### Extraction of exterior ring ... " << std::endl;

            //OGRLineString* ls= (OGRLineString*)poGeometry->Boundary();

            OGRLinearRing* ls= (OGRLinearRing*)poly->getExteriorRing();

            if(ls->getNumPoints() != 0)
            {
                std::cout << "Number of (exterior) polygon points: " << ls->getNumPoints() << std::endl;
                std::cout << std::endl;

                for(int i = 0; i < ls->getNumPoints(); i++ )
                {
                    OGRPoint p;
                    ls->getPoint(i,&p);

                    // This function can transform a larget set of points.....
                    Point3D point;
                    point.x = p.getX();
                    point.y = p.getY();
                    point.z = p.getZ();
                    point.index = i;

                    if(i == ls->getNumPoints()-1)
                    {
                        if(equalPoint(point, boundary.at(boundary.size()-1).at(0)))
                            break;
                    }
                    //boundary.push_back(point);
                    boundary.at(boundary.size()-1).push_back(point);
                }
            }

            std::cout << "### Extraction of interior ring ... " << std::endl;
            std::cout << "Number of interior rings: " << poly->getNumInteriorRings() << std::endl;
            std::cout << std::endl;

            if(poly->getNumInteriorRings() > 0)
            {
                for (int pl=0; pl < poly->getNumInteriorRings(); pl++) //loop on internal polygons
                {
                    OGRLinearRing* ls= (OGRLinearRing*)poly->getInteriorRing(pl);

                    if(ls->getNumPoints() != 0)
                    {
                        std::cout << "Counter (interior) feature: " << pl << std::endl;
                        std::cout << "Number of (interior) polygon points: " << ls->getNumPoints() << std::endl;

                        for(int i = 0; i < ls->getNumPoints(); i++ )
                        {
                            OGRPoint p;
                            ls->getPoint(i,&p);

                            // This function can transform a larget set of points.....
                            Point3D point;
                            point.x = p.getX();
                            point.y = p.getY();
                            point.z = p.getZ();
                            point.index = i;

                            if(i == ls->getNumPoints()-1)
                            {
                                if(equalPoint(point, boundary.at(boundary.size()-1).at(0)))
                                    break;
                            }
                            //boundary.push_back(point);
                            boundary.at(boundary.size()-1).push_back(point);
                        }
                    }
                }
            }

//             geometry_type = poGeometry->getGeometryName();

//             boundary.push_back(std::vector<Point3D> ());

//             OGRLineString* ls= (OGRLineString*)poGeometry->Boundary();

// //            std::cout << "Geometry Type: POLYGON" << std::endl;
// //            std::cout << "Number of points: " << ls->getNumPoints() << std::endl;

//             for(int i = 0; i < ls->getNumPoints(); i++ )
//             {
//                 OGRPoint p;
//                 ls->getPoint(i,&p);

//                 // This function can transform a larget set of points.....
//                 Point3D point;
//                 point.x = p.getX();
//                 point.y = p.getY();
//                 point.z = p.getZ();
//                 point.index = i;

//                 //boundary.push_back(point);
//                 boundary.at(boundary.size()-1).push_back(point);
//             }
        }

        else
        {
            std::cout << "GDAL - Geometry Name: " << poGeometry->getGeometryName() << std::endl;
            std::cerr << "\033[0;31mERROR setting ShapeGeometryType\033[0m" << std::endl;
        }

        OGRFeature::DestroyFeature( poFeature );
    }

    if(set_points.size() > 0)
        points.push_back(set_points);

    GDALClose( poDS );
    return IOSUCCESS;

#else

    std::cerr << "ERROR : GDAL library missing. Install GDAL and recompile defining symbol MUSE_USES_GDAL" << std::endl;
    return IOERROR;

#endif
}


int getnPoints_shapefile_shp (const std::string filename)
{
    unsigned int points_number = 0;

    std::string shape_filename;
    shape_filename = filename.substr(filename.find_last_of("/")+1, filename.length());
    std::string basename = get_basename(shape_filename);

#ifdef MUSE_USES_GDAL

    // 1. Register all the format drivers that are desired
    GDALAllRegister();

    // 2. Open shape file
    GDALDataset *poDS;
    poDS = static_cast<GDALDataset*> (GDALOpenEx(filename.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL ));
    if( poDS == NULL )
    {
        std::cerr << "Error while loading shapefile " << filename << std::endl;
        exit(1);
    }

    // 3. GDALDataset can potentially have many layers associated with it.
    OGRLayer  *poLayer = poDS->GetLayerByName(basename.c_str());

    OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();

    // 3. Reading features from the layer
    OGRFeature *poFeature;
    poLayer->ResetReading(); //to ensure we are starting at the beginning of the layer

    //We iterate through all the features in the layer using OGRLayer::GetNextFeature()
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
        for( int iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
        {
            OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );

            switch( poFieldDefn->GetType() )
            {
                case OFTInteger:
                    break;
                case OFTInteger64:
                    break;
                case OFTReal:
                    break;
                case OFTString:
                    break;
                default:
                    break;
            }
        }

        // Extract geometry from the feature
        OGRGeometry* poGeometry = poFeature->GetGeometryRef();

        if( poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
        {
            OGRPoint *poPoint = (OGRPoint *) poGeometry;
            //printf( "%.3f,%3.f,%3.f\n", poPoint->getX(), poPoint->getY(), poPoint->getZ() );
            points_number++;
        }
    }

    GDALClose( poDS );
    return points_number;

#else

    std::cerr << "ERROR : GDAL library missing. Install GDAL and recompile defining symbol MUSE_USES_GDAL" << std::endl;
    return IOERROR;

#endif
}


int load_shapefile_xyz(const std::string filename, std::vector<std::vector<Point3D>> &boundaries)
{
    boundaries.clear();
    boundaries.push_back(std::vector<Point3D>());

    std::ifstream in;
    in.open(filename.c_str());

    if (!in.is_open())
    {
        std::cerr << "Error while loading shapefile " << filename << std::endl;
        return IOERROR;
    }

    double x,y,z;
    while (in >> x >> y >> z)
    {
        Point3D p;
        p.x = x;
        p.y = y;
        p.z = z;

        boundaries.at(0).push_back(p);
    }
    in.close();
    std::cout << "Loading shapefile: " << filename << " ... COMPLETED." << std::endl;

    return IOSUCCESS;
}


int load_gpkg (const std::string filename, std::vector<std::vector<Point3D>> &boundary, std::vector<std::vector<Point3D>> &points, std::string &geometry_type)
{
    std::string gpkg_filename = filename.substr(filename.find_last_of("/")+1, filename.length());
    std::string basename = get_basename(gpkg_filename);

//#ifdef MUSE_USES_GDAL

    points.clear();
    boundary.clear();

    std::vector<Point3D> set_points;

    // 1. Register all the format drivers that are desired
    GDALAllRegister();

    std::cout << "driver# " << GetGDALDriverManager()->GetDriverCount() << std::endl;
    for(int i=0; i< GetGDALDriverManager()->GetDriverCount(); i++)
    {
       auto driver = GetGDALDriverManager()->GetDriver(i);
       auto info = driver->GetDescription();
       std::cout << "driver " << i << ": " << info << std::endl;
    }

    auto driver = GetGDALDriverManager()->GetDriverByName("GPKG");

    // 2. Open shape file
    GDALDataset *poDS;
    try {
        std::cout << FYEL("=== WARNING - Insert reading function in try-catch ... ") << std::endl;
        std::cout << FYEL("=== ... to investigate a segmentation fault with reading geopackage.") << std::endl;
        std::cout << FYEL("=== INTERNAL CRUSHED if the reading not continue!") << std::endl;
        poDS = static_cast<GDALDataset*> (GDALOpenEx(filename.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL ));
    } catch (std::exception gdal_e) {
       std::cerr << "error: " << gdal_e.what() << std::endl;
    }

    if( poDS == NULL )
    {
        std::cerr << "Error while loading GeoPackage file " << filename << std::endl;
        exit(1);
    }

    // 3. GDALDataset can potentially have many layers associated with it.


    int numLayers = GDALDatasetGetLayerCount(poDS);
    std::cout << "Number of layers: " << numLayers << std::endl;



    // prendi i layer dal dataset
    //OGRLayer  *poLayer = poDS->GetLayerByName(basename.c_str());
    OGRLayer  *poLayer = poDS->GetLayer(0);
    if(poLayer == NULL)
    {
        std::cerr << "ERROR: Failed to get layer " << basename.c_str() << std::endl;
        exit(1);
    }

    OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();

    int iFieldCount = poFDefn->GetFieldCount();
    std::vector<std::map<std::string,std::string>> featureAttributes;

    // 3. Reading features from the layer

    OGRFeature *poFeature;
    poLayer->ResetReading(); //to ensure we are starting at the beginning of the layer

    int numFeature = poLayer->GetFeatureCount();

    std::cout << "GDAL - Number of features: " << numFeature << std::endl;
    uint count_features = 0;

    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
        for( int iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
        {
            OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );

            switch( poFieldDefn->GetType() )
            {
                case OFTInteger:
                    break;
                case OFTInteger64:
                    break;
                case OFTReal:
                    break;
                case OFTString:
                    break;
                default:
                    break;
            }
        }

        // Extract geometry from the feature
        OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        count_features++;

        std::cout << "--------------------------------" << std::endl;
        std::cout << "### GDAL Geometry type: " << poGeometry->getGeometryType() << "; name = " << poGeometry->getGeometryName() << std::endl;
        std::cout << "### Counter feature: " << count_features << std::endl;

        if( poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
        {
            geometry_type = poGeometry->getGeometryName();

            //points.push_back(std::vector<Point3D> ());

            OGRPoint *poPoint = (OGRPoint *) poGeometry;
            //printf( "%.3f,%3.f\n", poPoint->getX(), poPoint->getY() );

            // This function can transform a larget set of points.....
            Point3D point;
            point.x = poPoint->getX();
            point.y = poPoint->getY();
            point.z = poPoint->getZ();
            //point.index = ;

            //TO DO: INDICIZZARE GLI INDEX DEL PUNTO PER LA FUNZIONE RIMOZIONE DUPLICATI!!!!!!!

            set_points.push_back(point);
            //points.at(points.size()-1).push_back(point);
        }

        else if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
        {
            geometry_type = poGeometry->getGeometryName();
            //geometry_type = "LINESTRING";

            //points.push_back(std::vector<Point3D> ());

            OGRLineString* poLine = (OGRLineString*) poGeometry;

            //std::cout << "Geometry Type: LINESTRING" << std::endl;
            //std::cout << "Number of points: " << ls->getNumPoints() << std::endl;

            for(int i = 0; i < poLine->getNumPoints(); i++ )
            {
                OGRPoint p;
                poLine->getPoint(i,&p);

                // This function can transform a larget set of points.....
                Point3D point;
                point.x = p.getX();
                point.y = p.getY();
                point.z = p.getZ();
                point.index = i;

                set_points.push_back(point);
                //points.at(points.size()-1).push_back(point);
            }
        }

        else if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiLineString)
        {
            geometry_type = poGeometry->getGeometryName();
            //geometry_type = "MULTILINESTRING";

            boundary.push_back(std::vector<Point3D> ());

            OGRMultiLineString* poMultiLine = (OGRMultiLineString*) poGeometry;

            //std::cout << "Geometry Type: MULTILINESTRING" << std::endl;
            //std::cout << "Number of points: " << ls->getNumPoints() << std::endl;

            for(int j=0; j< poMultiLine->getNumGeometries(); j++)
            {
                OGRLineString* poLine = (OGRLineString*)poMultiLine->getGeometryRef(j);

                for(int i = 0; i < poLine->getNumPoints(); i++ )
                {
                    OGRPoint p;
                    poLine->getPoint(i,&p);

                    // This function can transform a larget set of points.....
                    Point3D point;
                    point.x = p.getX();
                    point.y = p.getY();
                    point.z = p.getZ();
                    point.index = i;

                    //boundary.push_back(point);
                    boundary.at(boundary.size()-1).push_back(point);
                }
            }
        }

        else if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
        {
            geometry_type = poGeometry->getGeometryName();
            boundary.push_back(std::vector<Point3D> ());

            OGRPolygon *poly = (OGRPolygon*) poGeometry;
            std::cout << "### Extraction of exterior ring ... " << std::endl;

            //OGRLineString* ls= (OGRLineString*)poGeometry->Boundary();

            OGRLinearRing* ls= (OGRLinearRing*)poly->getExteriorRing();

            if(ls->getNumPoints() != 0)
            {
                std::cout << "Number of (exterior) polygon points: " << ls->getNumPoints() << std::endl;
                std::cout << std::endl;

                for(int i = 0; i < ls->getNumPoints(); i++ )
                {
                    OGRPoint p;
                    ls->getPoint(i,&p);

                    // This function can transform a larget set of points.....
                    Point3D point;
                    point.x = p.getX();
                    point.y = p.getY();
                    point.z = p.getZ();
                    point.index = i;

                    //boundary.push_back(point);
                    boundary.at(boundary.size()-1).push_back(point);
                }
            }

            std::cout << "### Extraction of interior ring ... " << std::endl;
            std::cout << "Number of interior rings: " << poly->getNumInteriorRings() << std::endl;
            std::cout << std::endl;

            if(poly->getNumInteriorRings() > 0)
            {
                for (int pl=0; pl < poly->getNumInteriorRings(); pl++) //loop on internal polygons
                {
                    OGRLinearRing* ls= (OGRLinearRing*)poly->getInteriorRing(pl);

                    if(ls->getNumPoints() != 0)
                    {
                        std::cout << "Counter (interior) feature: " << pl << std::endl;
                        std::cout << "Number of (interior) polygon points: " << ls->getNumPoints() << std::endl;

                        for(int i = 0; i < ls->getNumPoints(); i++ )
                        {
                            OGRPoint p;
                            ls->getPoint(i,&p);

                            // This function can transform a larget set of points.....
                            Point3D point;
                            point.x = p.getX();
                            point.y = p.getY();
                            point.z = p.getZ();
                            point.index = i;

                            //boundary.push_back(point);
                            boundary.at(boundary.size()-1).push_back(point);
                        }
                    }
                }
            }
        }

        else if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPolygon)
        {
            geometry_type = poGeometry->getGeometryName();
            //geometry_type = "MULTIPOLYGON";

            boundary.push_back(std::vector<Point3D> ());

            //OGRMultiLineString* poMultiLine = (OGRMultiLineString*) poGeometry;
            OGRMultiPolygon *poMultiPolygon = (OGRMultiPolygon *) poGeometry;

            //std::cout << "Geometry Type: MULTILINESTRING" << std::endl;
            //std::cout << "Number of points: " << ls->getNumPoints() << std::endl;

            for (int currGeometry = 0; currGeometry < poMultiPolygon->getNumGeometries(); currGeometry++)
            {
                OGRPolygon *poPolygon = ( OGRPolygon* )poMultiPolygon->getGeometryRef( currGeometry );

                OGRLinearRing *poLinearRing = poPolygon->getExteriorRing();

                for ( int currentPoint = 0; currentPoint < poLinearRing->getNumPoints(); currentPoint++ )
                {
                    Point3D point;
                    point.x = poLinearRing->getX(currentPoint);
                    point.y = poLinearRing->getY(currentPoint);
                    point.z = poLinearRing->getZ(currentPoint);
                    point.index = currentPoint;

                    boundary.at(boundary.size()-1).push_back(point);
                }
                std::cout << "DIM: " << boundary.at(0).size() <<std::endl;
            }

//            for(int j=0; j< poMultiLine->getNumGeometries(); j++)
//            {
//                OGRLineString* poLine = (OGRLineString*)poMultiLine->getGeometryRef(j);

//                for(int i = 0; i < poLine->getNumPoints(); i++ )
//                {
//                    OGRPoint p;
//                    poLine->getPoint(i,&p);

//                    // This function can transform a larget set of points.....
//                    Point3D point;
//                    point.x = p.getX();
//                    point.y = p.getY();
//                    point.z = p.getZ();

//                    //boundary.push_back(point);
//                    boundary.at(boundary.size()-1).push_back(point);
//                }
//            }
        }

        else
        {
            std::cout << poGeometry->getGeometryType() << "; name = " << poGeometry->getGeometryName() << std::endl;
            std::cerr << "\033[0;31mERROR setting Geopackage_GeometryType\033[0m" << std::endl;
        }

        OGRFeature::DestroyFeature( poFeature );
    }

    if(set_points.size() > 0)
        points.push_back(set_points);

    //std::cout << "DIM: " << boundary.at(0).size() <<std::endl;

    GDALClose( poDS );
    return IOSUCCESS;

//#else

    std::cerr << "ERROR : GDAL library missing. Install GDAL and recompile defining symbol MUSE_USES_GDAL" << std::endl;
    return IOERROR;

//#endif

}



// int export_gpkg_attributes_to_csv(const std::string filename, const std::string &csv_filename)
// {
//     std::string gpkg_filename = filename.substr(filename.find_last_of("/") + 1, filename.length());
//     std::string basename = get_basename(gpkg_filename);


//     // 1. Register all the format drivers that are desired
//     GDALAllRegister();

//     // 2. Open GeoPackage file
//     GDALDataset *poDS;
//     poDS = static_cast<GDALDataset*>(GDALOpenEx(filename.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL));
//     if (poDS == NULL) {
//         std::cerr << "Error while loading GeoPackage file " << filename << std::endl;
//         return IOERROR;
//     }

//     // 3. Get the layer
//     OGRLayer *poLayer = poDS->GetLayer(0);
//     if (poLayer == NULL) {
//         std::cerr << "ERROR: Failed to get layer " << basename.c_str() << std::endl;
//         GDALClose(poDS);
//         return IOERROR;
//     }

//     OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
//     int iFieldCount = poFDefn->GetFieldCount();
//     std::vector<std::map<std::string, std::string>> featureAttributes;

//     // 4. Reading features from the layer
//     OGRFeature *poFeature;
//     poLayer->ResetReading(); // to ensure we are starting at the beginning of the layer

//     while ((poFeature = poLayer->GetNextFeature()) != NULL)
//     {
//         std::map<std::string, std::string> attributes;
//         for (int iField = 0; iField < iFieldCount; iField++)
//         {
//             OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(iField);
//             std::string fieldName = poFieldDefn->GetNameRef();
//             std::string fieldValue;

//             switch (poFieldDefn->GetType()) {
//             case OFTInteger:
//                 fieldValue = std::to_string(poFeature->GetFieldAsInteger(iField));
//                 break;
//             case OFTInteger64:
//                 fieldValue = std::to_string(poFeature->GetFieldAsInteger64(iField));
//                 break;
//             case OFTReal:
//             {
//                 //std::cout << Value << std::endl;
//                 char charValue[50];
//                 sprintf(charValue, "%.15g", poFeature->GetFieldAsDouble(iField));
//                 std::string fieldValue_tmp(charValue);
//                 fieldValue = fieldValue_tmp;

//                 //std::cout << fieldValue << std::endl;
//                 //fieldValue = std::to_string(poFeature->GetFieldAsDouble(iField));
//                 //sprintf(poFeature->GetFieldAsDouble(iField), "%.17g");
//                 //fieldValue = std::to_string(poFeature->GetFieldAsDouble(iField));
//                 break;
//             }
//             case OFTString:
//                 fieldValue = poFeature->GetFieldAsString(iField);
//                 break;
//             default:
//                 fieldValue = "";
//                 break;
//             }
//             attributes[fieldName] = fieldValue;
//         }
//         featureAttributes.push_back(attributes);

//         OGRFeature::DestroyFeature(poFeature);
//     }

//     // 5. Export attributes to CSV
//     std::ofstream csv_file;
//     csv_file.open(csv_filename);
//     if (!csv_file.is_open()) {
//         std::cerr << "Error while opening CSV file " << csv_filename << std::endl;
//         return IOERROR;
//     }

//     // Write header
//     for (int iField = 0; iField < iFieldCount; iField++)
//     {
//         OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(iField);
//         std::string fieldName = poFieldDefn->GetNameRef();

//         csv_file << fieldName;
//         if (iField < iFieldCount - 1) {
//             csv_file << ";";
//         }
//     }
//     csv_file << "\n";

//     // Write attributes
//     for (const auto &attributes : featureAttributes) {
//         for (int iField = 0; iField < iFieldCount; iField++) {
//             OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(iField);
//             std::string fieldName = poFieldDefn->GetNameRef();
//             csv_file << attributes.at(fieldName);
//             if (iField < iFieldCount - 1) {
//                 csv_file << ";";
//             }
//         }
//         csv_file << "\n";
//     }

//     csv_file.close();



//     GDALClose(poDS);

//     return IOSUCCESS;
// }


int export_attributes_to_csv(const std::string filename, const std::string &csv_filename)
{
    std::string gpkg_filename = filename.substr(filename.find_last_of("/") + 1, filename.length());
    std::string basename = get_basename(gpkg_filename);


    // 1. Register all the format drivers that are desired
    GDALAllRegister();

    // 2. Open GeoPackage file
    GDALDataset *poDS;
    poDS = static_cast<GDALDataset*>(GDALOpenEx(filename.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL));
    if (poDS == NULL) {
        std::cerr << "Error while loading GeoPackage file " << filename << std::endl;
        return IOERROR;
    }

    // 3. Get the layer
    OGRLayer *poLayer = poDS->GetLayer(0);
    if (poLayer == NULL) {
        std::cerr << "ERROR: Failed to get layer " << basename.c_str() << std::endl;
        GDALClose(poDS);
        return IOERROR;
    }

    OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
    int iFieldCount = poFDefn->GetFieldCount();
    std::vector<std::map<std::string, std::string>> featureAttributes;

    // 4. Reading features from the layer
    OGRFeature *poFeature;
    poLayer->ResetReading(); // to ensure we are starting at the beginning of the layer

    while ((poFeature = poLayer->GetNextFeature()) != NULL)
    {
        std::map<std::string, std::string> attributes;
        for (int iField = 0; iField < iFieldCount; iField++)
        {
            OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(iField);
            std::string fieldName = poFieldDefn->GetNameRef();
            std::string fieldValue;

            switch (poFieldDefn->GetType()) {
            case OFTInteger:
                fieldValue = std::to_string(poFeature->GetFieldAsInteger(iField));
                break;
            case OFTInteger64:
                fieldValue = std::to_string(poFeature->GetFieldAsInteger64(iField));
                break;
            case OFTReal:
            {
                //std::cout << Value << std::endl;
                char charValue[50];
                sprintf(charValue, "%.15g", poFeature->GetFieldAsDouble(iField));
                std::string fieldValue_tmp(charValue);
                fieldValue = fieldValue_tmp;

                //std::cout << fieldValue << std::endl;
                //fieldValue = std::to_string(poFeature->GetFieldAsDouble(iField));
                //sprintf(poFeature->GetFieldAsDouble(iField), "%.17g");
                //fieldValue = std::to_string(poFeature->GetFieldAsDouble(iField));
                break;
            }
            case OFTString:
                fieldValue = poFeature->GetFieldAsString(iField);
                break;
            default:
                fieldValue = "";
                break;
            }
            attributes[fieldName] = fieldValue;
        }
        featureAttributes.push_back(attributes);

        OGRFeature::DestroyFeature(poFeature);
    }

    // 5. Export attributes to CSV
    std::ofstream csv_file;
    csv_file.open(csv_filename);
    if (!csv_file.is_open()) {
        std::cerr << "Error while opening CSV file " << csv_filename << std::endl;
        return IOERROR;
    }

    // Write header
    for (int iField = 0; iField < iFieldCount; iField++)
    {
        OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(iField);
        std::string fieldName = poFieldDefn->GetNameRef();

        csv_file << fieldName;
        if (iField < iFieldCount - 1) {
            csv_file << ";";
        }
    }
    csv_file << "\n";

    // Write attributes
    for (const auto &attributes : featureAttributes) {
        for (int iField = 0; iField < iFieldCount; iField++) {
            OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(iField);
            std::string fieldName = poFieldDefn->GetNameRef();
            csv_file << attributes.at(fieldName);
            if (iField < iFieldCount - 1) {
                csv_file << ";";
            }
        }
        csv_file << "\n";
    }

    csv_file.close();



    GDALClose(poDS);

    return IOSUCCESS;
}
