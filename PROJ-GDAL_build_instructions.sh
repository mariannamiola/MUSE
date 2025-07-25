#!/bin/bash

###BUILD INSTRUCTION FOR LINUX

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

### PROJ
export PROJ=${SCRIPT_DIR}/external/PROJ
cd ${PROJ}
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=${PROJ}/installed -DBUILD_APPS=OFF -DENABLE_CURL=OFF -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel 16
cmake --install .


### GDAL
export GDAL=${SCRIPT_DIR}/external/gdal
cd ${GDAL}
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=${GDAL}/installed -DGDAL_BUILD_OPTIONAL_DRIVERS=OFF -DOGR_BUILD_OPTIONAL_DRIVERS=OFF -DOGR_ENABLE_DRIVER_GPKG=ON -DPROJ_INCLUDE_DIR=${PROJ}/installed/include -DPROJ_LIBRARY_RELEASE=${PROJ}/installed/lib/libproj.so -DBUILD_PYTHON_BINDINGS=OFF -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel 16
cmake --install .



###### cmake .. -DCMAKE_INSTALL_PREFIX=/home/marianna/muse/external/gdal/installed -DGDAL_BUILD_OPTIONAL_DRIVERS=OFF -DOGR_BUILD_OPTIONAL_DRIVERS=OFF -DOGR_ENABLE_DRIVER_GPKG=ON -DPROJ_INCLUDE=/home/marianna/muse/external/PROJ/installed/include -DPROJ_LIBRARY=/home/marianna/muse/external/PROJ/installed/lib/libproj.so -DBUILD_PYTHON_BINDINGS=OFF -DCMAKE_BUILD_TYPE=Debug
