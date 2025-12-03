#!/bin/bash

(
set -e	#exit if an error occours

# apps
project=muse_project
realtime=muse_realtime
data=muse_data
plot=muse_plot
utility=muse_utility
geometry=muse_geometry
vario=muse_vario
compute=muse_compute
manipulate=muse_manipulate
export=muse_export


####################################################################### da modificare ...
#MAC
#export BUILD=/Users/mariannamiola/muse

#### Directiory (full path) where this scripts lies
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

#UBUNTU
export MUSE=${SCRIPT_DIR}
####################################################################### da modificare


export APPS=${MUSE}/apps
cd ${APPS}
mkdir -p build

export BUILD=${APPS}/build
cd ${BUILD}
#cmake ..
cmake -DCMAKE_BUILD_TYPE=Release ..
#cmake -DCMAKE_BUILD_TYPE=Debug ..
#make
cmake --build . #--parallel 8

# Bin creation

#cd ${BUILD}
BIN_FOLDER=${SCRIPT_DIR}/bin
mkdir -p ${BIN_FOLDER}

ln -sf ${BUILD}/muse_project/${project} ${BIN_FOLDER}
ln -sf ${BUILD}/muse_realtime/${realtime} ${BIN_FOLDER}
ln -sf ${BUILD}/muse_data/${data} ${BIN_FOLDER}
ln -sf ${BUILD}/muse_plot/${plot} ${BIN_FOLDER}
ln -sf ${BUILD}/muse_utility/${utility} ${BIN_FOLDER}
ln -sf ${BUILD}/muse_geometry/${geometry} ${BIN_FOLDER}
ln -sf ${BUILD}/muse_vario/${vario} ${BIN_FOLDER}
ln -sf ${BUILD}/muse_compute/${compute} ${BIN_FOLDER}
ln -sf ${BUILD}/muse_manipulate/${manipulate} ${BIN_FOLDER}
ln -sf ${BUILD}/muse_export/${export} ${BIN_FOLDER}
ln -sf ${BUILD}/muse_export/${export}_raster ${BIN_FOLDER}



# Export paths and variables

#export PATH=${BIN_FOLDER}:$PATH
)
