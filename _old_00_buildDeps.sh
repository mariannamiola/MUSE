#!/bin/bash

(
set -e	#exit if an error occours

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

### MATPLOTPLUSPLUS
export MATPLOT=${SCRIPT_DIR}/external/matplotplusplus
cd ${MATPLOT}
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O2"
cmake --build . --parallel 20 --config Release
sudo cmake --install .


### PROJ
export PROJ=${SCRIPT_DIR}/external/PROJ
cd ${PROJ}
mkdir -p build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=${PROJ}/installed -DBUILD_APPS=OFF -DENABLE_CURL=OFF -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel 16
cmake --install .


### GDAL
export GDAL=${SCRIPT_DIR}/external/gdal
cd ${GDAL}
mkdir -p build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=${GDAL}/installed -DGDAL_BUILD_OPTIONAL_DRIVERS=OFF -DOGR_BUILD_OPTIONAL_DRIVERS=OFF -DOGR_ENABLE_DRIVER_GPKG=ON -DPROJ_INCLUDE_DIR=${PROJ}/installed/include -DPROJ_LIBRARY_RELEASE=${PROJ}/installed/lib/libproj.so -DBUILD_PYTHON_BINDINGS=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel 16
cmake --install .


### VTK
export VTK=${SCRIPT_DIR}/external/VTK
cd ${VTK}
mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_INSTALL_PREFIX=${VTK}/installed \
         -DVTK_BUILD_TESTING=OFF \
         -DVTK_GROUP_ENABLE_Qt=NO \
         -DBUILD_SHARED_LIBS=ON

cmake --build . --parallel 16
cmake --install .
)
