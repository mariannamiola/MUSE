#!/usr/bin/env bash
set -euo pipefail

# ==========================
# CONFIG
# ==========================
SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
EXTERNAL_DIR="${SCRIPT_DIR}/external"
BUILD_TYPE=Release
NPROC=$(getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu)

# Detect OS
OS="$(uname)"
if [[ "$OS" == "Darwin" ]]; then
    LIB_EXT="dylib"
else
    LIB_EXT="so"
fi

# ==========================
# GENERIC BUILD FUNCTION
# ==========================
build_lib() {
    local NAME=$1
    shift

    echo "========================="
    echo "Building $NAME"
    echo "========================="

    cd "${EXTERNAL_DIR}/${NAME}"
    mkdir -p build
    cd build

    cmake .. "$@"
    cmake --build . --parallel ${NPROC}
    cmake --install .
}

# ==========================
# SPECIFIC BUILD FUNCTION (MATPLOT)
# ==========================
build_mplib() {
    local NAME=$1
    shift

    echo "========================="
    echo "Building $NAME"
    echo "========================="

    cd "${EXTERNAL_DIR}/${NAME}"
    mkdir -p build
    cd build

    cmake .. "$@" -DCMAKE_CXX_FLAGS="-O2" -DCMAKE_INSTALL_PREFIX="${EXTERNAL_DIR}/${NAME}/installed"
    cmake --build . --parallel ${NPROC} --config Release
    cmake --install .
}

# ==========================
# LIBRARIES
# ==========================

build_matplot() {
    build_mplib "matplotplusplus" \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
}

build_proj() {
    build_lib "PROJ" \
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}/PROJ/installed \
        -DBUILD_APPS=OFF \
        -DENABLE_CURL=OFF \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
}

build_gdal() {
    build_lib "gdal" \
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}/gdal/installed \
        -DGDAL_BUILD_OPTIONAL_DRIVERS=OFF \
        -DOGR_BUILD_OPTIONAL_DRIVERS=OFF \
        -DOGR_ENABLE_DRIVER_GPKG=ON \
        -DGDAL_ENABLE_DRIVER_AAIGRID=ON \
        -DGDAL_ENABLE_DRIVER_GRASSASCII=ON \
        -DGDAL_ENABLE_DRIVER_AIG=ON \
        -DCMAKE_PREFIX_PATH=${EXTERNAL_DIR}/PROJ/installed \
        -DBUILD_PYTHON_BINDINGS=OFF \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
}

build_vtk() {
    build_lib "VTK" \
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}/VTK/installed \
        -DVTK_BUILD_TESTING=OFF \
        -DVTK_GROUP_ENABLE_Qt=NO \
        -DBUILD_SHARED_LIBS=ON \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
}

build_flann() {
    build_lib "GeoStatsLib/external/flann-lib" \
        -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}/GeoStatsLib/external/flann-lib/installed \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
        -DBUILD_PYTHON_BINDINGS=OFF \
        -DBUILD_MATLAB_BINDINGS=OFF 
}

# ==========================
# ARGUMENT PARSER
# ==========================

if [[ $# -eq 0 ]]; then
    echo "Usage: ./00_buildDeps.sh [all|proj|gdal|vtk|matplot|flann]"
    exit 1
fi

for arg in "$@"; do
    case $arg in
        all)
            build_matplot
            build_proj
            build_gdal
            build_vtk
            build_flann
            ;;
        matplot)
            build_matplot
            ;;
        proj)
            build_proj
            ;;
        gdal)
            build_gdal
            ;;
        vtk)
            build_vtk
            ;;
        flann)
            build_flann
            ;;
        *)
            echo "Unknown option: $arg"
            exit 1
            ;;
    esac
done

