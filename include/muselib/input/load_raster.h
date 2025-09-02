#ifndef LOAD_RASTER_H
#define LOAD_RASTER_H

#include <string>
#include <vector>
#include "muselib/data_structures/point.h"


int load_rasterfile (const std::string filename, std::vector<std::vector<float>> &points, float &XOrigin, float &YOrigin, int &nXSize, int &nYSize, float &XSizePixel, float &YSizePixel);
int load_gridfile   (const std::string filename, std::vector<std::vector<float>> &points, float &XOrigin, float &YOrigin, int &XPixel, int &YPixel, float &XSizePixel, float &YSizePixel);


#ifndef STATIC_MUSELIB
#include "load_raster.cpp"
#endif

#endif // LOAD_RASTER_H
