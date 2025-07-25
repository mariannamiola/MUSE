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

int load_rasterfile(const std::string filename, std::vector<std::vector<float>> &points, float &XOrigin, float &YOrigin, int &nXSize, int &nYSize)
{
    const std::string ext = filename.substr(filename.find_last_of("."));

    if (ext.compare(".asc") == 0 || ext.compare(".gpkg") == 0)
        return load_gridfile(filename, points, XOrigin, YOrigin, nXSize, nYSize);

    std::cerr << "ERROR: Unsupported Raster File format." << std::endl;
    return IOERROR;
}


int load_gridfile (const std::string filename, std::vector<std::vector<float>> &points, float &XOrigin, float &YOrigin, int &XPixel, int &YPixel)
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
        exit(1);
    }


    // raster dimensions
    const int xSize = poDataset->GetRasterXSize();
    const int ySize = poDataset->GetRasterYSize();


    double adfGeoTransform[6];
    printf( "Driver: %s/%s\n", poDataset->GetDriver()->GetDescription(), poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
    printf( "Size is %dx%dx%d\n", poDataset->GetRasterXSize(), poDataset->GetRasterYSize(), poDataset->GetRasterCount() );

    if( poDataset->GetProjectionRef()  != NULL )
        printf( "Projection is `%s'\n", poDataset->GetProjectionRef() );
    if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
    {
        printf( "Origin = (%.6f,%.6f)\n", adfGeoTransform[0], adfGeoTransform[3] );
        printf( "Pixel Size = (%.6f,%.6f)\n", adfGeoTransform[1], adfGeoTransform[5] );
    }

    XOrigin = adfGeoTransform[0];
    YOrigin = adfGeoTransform[3];
    XPixel = adfGeoTransform[1];
    YPixel = adfGeoTransform[5];

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

    float *pafScanline;
    pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize*nYSize);

    std::vector<std::vector<float>> out_vec = std::vector<std::vector<float>> (nYSize, std::vector<float> (nXSize,0));

    poBand->RasterIO( GF_Read, 0, 0, nXSize, nYSize, pafScanline, nXSize, nYSize, GDT_Float32, 0, 0 );

    //std::cout << "After allocation" << std::endl;
    for(int j = 0; j < nYSize; j++)
    {
        for(int i = 0; i < nXSize; i++)
        {
            out_vec[i][j] = pafScanline[j*nXSize+i];
            std::cout << "row = " << i  << "; col = " << j << "; val = " << out_vec [i][j] << std::endl;
        }
    }
    CPLFree(pafScanline);
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
