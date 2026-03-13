#include "load_raster.h"

#include <iostream>
#include <fstream>

#include "gdal_priv.h"
#include "cpl_conv.h" // for CPLMalloc()
#include "gdal_utils.h"

#include "muselib/utils.h"

#define IOSUCCESS 0
#define IOERROR 1

//https://gdal.org/tutorials/raster_api_tut.html

int load_rasterfile(const std::string filename, std::vector<std::vector<float>> &points, float &XOrigin, float &YOrigin, int &nXSize, int &nYSize, float &XSizePixel, float &YSizePixel)
{
    const std::string ext = filename.substr(filename.find_last_of("."));

    if (ext.compare(".asc") == 0 || ext.compare(".gpkg") == 0 || ext.compare(".tif") == 0 || ext.compare(".tiff") == 0)
        return load_gridfile(filename, points, XOrigin, YOrigin, nXSize, nYSize, XSizePixel, YSizePixel);

    std::cerr << "ERROR: Unsupported Raster File format." << std::endl;
    return IOERROR;
}


int load_gridfile (const std::string filename, std::vector<std::vector<float>> &points, float &XOrigin, float &YOrigin, int &XPixel, int &YPixel, float &XSizePixel, float &YSizePixel)
{
    points.clear();

#ifdef MUSE_USES_GDAL

    // 1. Register all the format drivers that are desired
    GDALAllRegister();

    // 2. Open shape file
    GDALDatasetUniquePtr poDataset;
    const GDALAccess eAccess = GA_ReadOnly;
    poDataset = GDALDatasetUniquePtr(GDALDataset::FromHandle(GDALOpen( filename.c_str(), eAccess )));

    //GDALDataset *poDataset;
    //poDataset = (GDALDataset*) (GDALOpen(filename.c_str(), GA_ReadOnly));
    if( !poDataset )
    {
        std::cerr << "Error while loading ASCIIgrid file " << filename << std::endl;
        return IOERROR;
    }


    // raster dimensions
    const int xSize = poDataset->GetRasterXSize();
    const int ySize = poDataset->GetRasterYSize();


    double adfGeoTransform[6];
    printf( "Driver: %s/%s\n", poDataset->GetDriver()->GetDescription(), poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
    printf( "Size is %dx%dx%d\n", poDataset->GetRasterXSize(), poDataset->GetRasterYSize(), poDataset->GetRasterCount() );

    if( poDataset->GetProjectionRef()  != NULL )
        printf( "Projection is `%s'\n", poDataset->GetProjectionRef() );
    double geoXOrigin = 0.0;
    double geoYOrigin = 0.0;
    double pixelXSize = 0.0;
    double pixelYSize = 0.0;

    if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
    {
        geoXOrigin  = adfGeoTransform[0];
        geoYOrigin  = adfGeoTransform[3];
        pixelXSize  = adfGeoTransform[1];
        pixelYSize  = adfGeoTransform[5];

        printf( "Origin (corner) = (%.6f,%.6f)\n", geoXOrigin, geoYOrigin );
        printf( "Pixel Size = (%.6f,%.6f)\n", pixelXSize, pixelYSize );

        // GDAL origin refers to the top-left corner of the top-left pixel.
        // Most raster values are cell-centered, so shift the origin to the center of the first cell
        // to make the coordinates consistent with the returned elevation grid.
        XOrigin = static_cast<float>(geoXOrigin + pixelXSize * 0.5);
        YOrigin = static_cast<float>(geoYOrigin + pixelYSize * 0.5);
        XSizePixel = static_cast<float>(pixelXSize);
        YSizePixel = static_cast<float>(pixelYSize);

        printf( "Origin (cell center) = (%.6f,%.6f)\n", XOrigin, YOrigin );
    }
    else
    {
        // Fallback (should not happen with valid georeferenced rasters)
        XOrigin = 0.0f;
        YOrigin = 0.0f;
        XSizePixel = 1.0f;
        YSizePixel = -1.0f;
    }

    GDALRasterBand  *poBand;
    int             nBlockXSize, nBlockYSize;
    int             bGotMin, bGotMax;
    double          adfMinMax[2];

    poBand = poDataset->GetRasterBand( 1 );
    poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
    printf( "Block = %dx%d Type = %s, ColorInterp = %s\n", nBlockXSize, nBlockYSize, GDALGetDataTypeName(poBand->GetRasterDataType()),
           GDALGetColorInterpretationName(poBand->GetColorInterpretation()) );

    adfMinMax[0] = poBand->GetMinimum( &bGotMin );
    adfMinMax[1] = poBand->GetMaximum( &bGotMax );

    if( ! (bGotMin && bGotMax) )
        GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
    printf( "Min = %.3fd, Max = %.3f\n", adfMinMax[0], adfMinMax[1] );
    if( poBand->GetOverviewCount() > 0 )
        printf( "Band has %d overviews.\n", poBand->GetOverviewCount() );
    if( poBand->GetColorTable() != NULL )
        printf( "Band has a color table with %d entries.\n", poBand->GetColorTable()->GetColorEntryCount() );

    int nXSize = poBand->GetXSize(); //width
    int nYSize = poBand->GetYSize(); //height
    int   bands = poDataset->GetRasterCount();
    XPixel = nXSize;
    YPixel = nYSize;

    float *pafScanline;
    pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize*nYSize);

    std::vector<std::vector<float>> out_vec = std::vector<std::vector<float>> (nYSize, std::vector<float> (nXSize,0));

    poBand->RasterIO( GF_Read, 0, 0, nXSize, nYSize, pafScanline, nXSize, nYSize, GDT_Float32, 0, 0 );

    //std::cout << "After allocation" << std::endl;
    for(int j = 0; j < nYSize; j++)
    {
        for(int i = 0; i < nXSize; i++)
        {
            out_vec[j][i] = pafScanline[j*nXSize + i];
            //std::cout << "row = " << j  << "; col = " << i << "; val = " << out_vec [j][i] << std::endl;
        }
    }
    CPLFree(pafScanline);

    // For formats like ESRI ASCII Grid (ArcGrid), the file is written top-to-bottom,
    // but the geotransform origin is the lower-left corner (positive pixel y).
    // In that case we need to flip rows so that row 0 corresponds to the lower-left
    // cell center, matching quadmesh coordinate conventions.
    if (YSizePixel > 0)
    {
        std::reverse(out_vec.begin(), out_vec.end());
    }

    points = out_vec;

    std::cout << std::endl;
    std::cout << "Reading Raster ... COMPLETED." << std::endl;


    //GDALClose( poDataset );
    return IOSUCCESS;

#else

    std::cerr << "ERROR : GDAL library missing. Install GDAL and recompile defining symbol MUSE_USES_GDAL" << std::endl;
    return IOERROR;

#endif
}
