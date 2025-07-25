cmake_minimum_required(VERSION 3.14)

project(apps LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (USE_ARMADILLO ON)

ENABLE_LANGUAGE(C)

include_directories(BEFORE "../include")

#####
# tclap
include_directories(BEFORE "../external/tclap-1.4.0-rc1/include")

#####
# nlohmann-json
include_directories(BEFORE "../external/nlohmann")

#####
# cereal
add_definitions(-DMUSE_USES_CEREAL)
include_directories(BEFORE "../external/cereal/include")

#####
# concaveman
include_directories(BEFORE "../external/concaveman")

#####
# Matplot++
include_directories(BEFORE "../external/matplotplusplus")
# MESSAGE(STATUS "Building MATPLOTPLUSPLUS ...")
# set (MATPLOT_DIR ${CMAKE_SOURCE_DIR}/../external/matplotplusplus)
# add_custom_command(
#   OUTPUT build_complete
#   COMMAND mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O2" && cmake --build . --parallel 20 --config Release && sudo cmake --install .
#   WORKING_DIRECTORY ${MATPLOT_DIR}
# )
find_package(Matplot++ REQUIRED)


#####
# PROJ
add_definitions(-DMUSE_USES_PROJ)
find_package(PROJ CONFIG REQUIRED PATHS ${CMAKE_SOURCE_DIR}/../external/PROJ/installed/lib/cmake/proj NO_DEFAULT_PATH)
if (PROJ_FOUND)
    MESSAGE(STATUS "PROJ found.")
endif()
#set (PROJ_DIR ${CMAKE_SOURCE_DIR}/../external/PROJ)
#set (PROJ_LIB ${CMAKE_SOURCE_DIR}/../external/PROJ/build)
#set (proj_lib_file "${PROJ_DIR}/build/lib/libproj.so")
#find_package(PROJ CONFIG REQUIRED)


#####
# GDAL
find_package(GDAL REQUIRED)
add_definitions(-DMUSE_USES_GDAL)
INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})
 # find_package(GDAL CONFIG REQUIRED PATHS ${CMAKE_SOURCE_DIR}/../external/gdal/installed/lib/cmake/gdal NO_DEFAULT_PATH)
 # if(GDAL_FOUND)
 # set (GDAL_DIR ${CMAKE_SOURCE_DIR}/external/gdal)
 #     set(GDAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../external/gdal/installed/include)
 #        set(GDAL_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/../external/gdal/installed/lib)
 #        if (UNIX AND NOT APPLE)
 #            set (GDAL_LIB ${GDAL_LIBRARY_DIR}/libgdal.so)
 #        elseif (MSVC)
 #            set (GDAL_LIB ${GDAL_LIBRARY_DIR}/gdal.lib)
 #        else() ###macos
 #            set (GDAL_LIB ${GDAL_LIBRARY_DIR}/libgdal.dylib)
 #            set (GDAL_LIBRARY ${GDAL_LIB})
 #        endif()
 #     INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})
 #     MESSAGE(STATUS "GDAL found: ${GDAL_INCLUDE_DIRS}")
 #     # Extract GDAL version from gdal_version.h
 #     SET(GDAL_VERSION_FILE ${GDAL_INCLUDE_DIRS}/gdal_version.h)
 #     IF (NOT EXISTS ${GDAL_VERSION_FILE})
 #       message(
 #           "Could not find file: ${GDAL_VERSION_FILE} "
 #           "containing version information in GDAL install located at: "
 #           "${GDAL_INCLUDE_DIRS}.")
 #     ELSE (NOT EXISTS ${GDAL_VERSION_FILE})
 #     FILE(READ ${GDAL_VERSION_FILE} GDAL_VERSION_FILE_CONTENTS)
 #        STRING(REGEX MATCH "define GDAL_VERSION_MAJOR +[0-9]+"
 #           GDAL_VERSION_MAJOR "${GDAL_VERSION_FILE_CONTENTS}")
 #         STRING(REGEX REPLACE "define GDAL_VERSION_MAJOR +([0-9]+)" "\\1"
 #           GDAL_VERSION_MAJOR "${GDAL_VERSION_MAJOR}")
 #         STRING(REGEX MATCH "define GDAL_VERSION_MINOR +[0-9]+"
 #           GDAL_VERSION_MINOR "${GDAL_VERSION_FILE_CONTENTS}")
 #         STRING(REGEX REPLACE "define GDAL_VERSION_MINOR +([0-9]+)" "\\1"
 #           GDAL_VERSION_MINOR "${GDAL_VERSION_MINOR}")
 #         STRING(REGEX MATCH "define GDAL_VERSION_REV +[0-9]+"
 #           GDAL_VERSION_REV "${GDAL_VERSION_FILE_CONTENTS}")
 #         STRING(REGEX REPLACE "define GDAL_VERSION_REV +([0-9]+)" "\\1"
 #           GDAL_VERSION_REV "${GDAL_VERSION_REV}")
 #         SET(GDAL_VERSION "${GDAL_VERSION_MAJOR}.${GDAL_VERSION_MINOR}.${GDAL_VERSION_REV}")
 #     ENDIF(NOT EXISTS ${GDAL_VERSION_FILE})
 #     MESSAGE(STATUS "GDAL version: ${GDAL_VERSION}")
 #     ADD_DEFINITIONS( -DGDALVER=${GDAL_VERSION_MAJOR} )
 # ENDIF()


#####
# geostatlib
add_definitions(-DMUSE_USES_GEOSTATSLIB)
add_definitions(-DGEOSTATSLIB_USES_CINOLIB)
add_definitions(-DGEOSTATLIB_USES_FLANN)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LZ4 REQUIRED liblz4)
find_package(LZ4)
include_directories(BEFORE "../external/geostatslib/include")


#####
# cinolib
#find_package (glfw3 REQUIRED)
add_definitions(-DMUSE_USES_CINOLIB)

#add_definitions(-DCINOLIB_USES_OPENGL_GLFW_IMGUI)
#add_definitions(-DCINOLIB_USES_QT)
#add_definitions(-DCINOLIB_USES_SHEWCHUK_PREDICATES)
#include_directories (BEFORE "../external/cinolib/include")
#include_directories (BEFORE "../external/cinolib/external/eigen")
#add_library(shewchuk_predicates ../external/cinolib/external/shewchuk_predicates/shewchuk.c)

set(CINOLIB_USES_OPENGL_GLFW_IMGUI    OFF )
set(CINOLIB_USES_TETGEN               ON )
set(CINOLIB_USES_TRIANGLE             ON )
set(CINOLIB_USES_SHEWCHUK_PREDICATES  ON )
set(CINOLIB_USES_INDIRECT_PREDICATES  OFF )
set(CINOLIB_USES_GRAPH_CUT            OFF)
set(CINOLIB_USES_BOOST                ON )
set(CINOLIB_USES_VTK                  ON )
set(cinolib_DIR "${PROJECT_SOURCE_DIR}/../external/cinolib")
message(${cinolib_DIR})
find_package(cinolib REQUIRED)
link_libraries(cinolib)



#####
# OpenGL
#if (OPENGL_FOUND)
#    message("Found OpenGL for Mac OS")
#    message("OpenGL for Mac OS, include dir: ${OPENGL_INCLUDE_DIR}")
#    message("OpenGL for Mac OS, link libraries: ${OPENGL_gl_LIBRARY}")
#else (OPENGL_FOUND)
#    message(FATAL_ERROR "OpenGL for Mac OS not found")
#endif()


#####
# triangle
add_definitions(-DMUSE_USES_TRIANGLE)
#add_definitions(-DCINOLIB_USES_TRIANGLE)
add_definitions(-DTRILIBRARY)
include_directories (BEFORE "../external/triangle")
set (TRIANGLE_DIR ${CMAKE_SOURCE_DIR}/../external/triangle)
set (TRIANGLE_LIB ${CMAKE_SOURCE_DIR}/../external/triangle/build)
set (triangle_lib_file "${TRIANGLE_DIR}/build/libtriangle.a")


#####
# tetgen
add_definitions(-DMUSE_USES_TETGEN)
#add_definitions(-DCINOLIB_USES_TETGEN)
add_definitions(-DTETLIBRARY)
include_directories (BEFORE "../external/tetgen")
set (TETGEN_DIR ${CMAKE_SOURCE_DIR}/../external/tetgen)
set (TETGEN_LIB ${CMAKE_SOURCE_DIR}/../external/tetgen/build)
set (tetgen_lib_file "${TETGEN_DIR}/build/libtetgen.a")



#####
# OpenMP
#find_package(OpenMP)
find_package(OpenMP REQUIRED)
if(APPLE)
    message("\nConfiguring OPENMP compiler variables\n")
    if (OPENMP_FOUND)

       #set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
       set (OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp /usr/local/lib/libomp.dylib -I /usr/local/include/")
       set (OpenMP_CXX_LIB_NAMES "libomp")
       #set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
    message("${CMAKE_CXX_FLAGS}\n")
    message("${CMAKE_EXE_LINKER_FLAGS}\n")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()


#####
# Flann
#include_directories ("/Users/mariannamiola/Downloads/flann/1.9.1/interfaces/flann")
#include_directories ("/Users/mariannamiola/Downloads/flann/1.9.1/interfaces/flann")
#link_directories ("/Users/mariannamiola/Downloads/flann/1.9.1")
#set (FLANN_DIR ${CMAKE_SOURCE_DIR}/../flann)

#####
# Armadillo
if(DEFINED USE_ARMADILLO)
    add_definitions(-DGEOSTATSLIB_USES_ARMADILLO)
    find_package(Armadillo REQUIRED)
    include_directories(${ARMADILLO_INCLUDE_DIRS})
    link_directories(${ARMADILLO_LIBRARIES} ${LAPACK_LIBRARIES} )
endif(DEFINED USE_ARMADILLO)
if(EXISTS ${ARMADILLO_INCLUDE_DIRS})
    message("Armadillo found @ " ${ARMADILLO_INCLUDE_DIRS}\n)
    message("Armadillo lib found @ " ${ARMADILLO_LIBRARIES}\n)
endif()


#####
# libigl
add_definitions(-DMUSE_USES_LIBIGL)
include_directories(BEFORE "../external/libigl/include")


#####
# ellipse-fitting
include_directories(BEFORE "../external/ellipse-fitting")


#####
# database
include_directories(BEFORE "../external/muse_database/include")




add_subdirectory(muse_project)
add_subdirectory(muse_data)
add_subdirectory(muse_geometry)
add_subdirectory(muse_manipulate)
add_subdirectory(muse_vario)
add_subdirectory(muse_compute)
add_subdirectory(muse_plot)
add_subdirectory(muse_utility)
add_subdirectory(muse_export)
