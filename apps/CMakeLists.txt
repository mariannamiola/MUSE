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
#####
# GDAL (prefer external installed copy to avoid system conflicts)
# Set the external install prefix (adjust path if your install uses a different folder name)
add_definitions(-DMUSE_USES_GDAL)
set(GDAL_EXTERNAL_PREFIX "${CMAKE_SOURCE_DIR}/../external/gdal/installed" CACHE PATH "External GDAL install prefix")
if(EXISTS "${GDAL_EXTERNAL_PREFIX}")
    list(APPEND CMAKE_PREFIX_PATH "${GDAL_EXTERNAL_PREFIX}")
endif()

# Try to find GDAL from the external install first (CONFIG mode), otherwise fall back to the system package
find_package(GDAL CONFIG QUIET PATHS "${GDAL_EXTERNAL_PREFIX}/lib/cmake/gdal" NO_DEFAULT_PATH)
message(STATUS "GDAL_FOUND = ${GDAL_FOUND}")
if(GDAL_FOUND)
    message("Using GDAL version at path: ${GDAL_EXTERNAL_PREFIX}/lib/cmake/gdal")
    message("Using GDAL imported target: GDAL::GDAL")
endif()

 


#####
# geostatlib
add_definitions(-DMUSE_USES_GEOSTATSLIB)
add_definitions(-DGEOSTATSLIB_USES_CINOLIB)
add_definitions(-DGEOSTATLIB_USES_FLANN)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LZ4 REQUIRED liblz4)
find_package(LZ4)
#include_directories(BEFORE "../external/geostatslib/include")
include_directories(BEFORE "../external/GeoStatsLib/include")



#####
# cinolib
#find_package (glfw3 REQUIRED)
add_definitions(-DMUSE_USES_CINOLIB)

#add_definitions(-DCINOLIB_USES_OPENGL_GLFW_IMGUI)
#add_definitions(-DCINOLIB_USES_QT)
#add_definitions(-DCINOLIB_USES_SHEWCHUK_PREDICATES)
include_directories (BEFORE "../external/cinolib/include")
include_directories (BEFORE "../external/cinolib/external/eigen")
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
# VTK
set(VTK_EXTERNAL_PREFIX "${CMAKE_SOURCE_DIR}/../external/VTK/installed/lib/cmake/vtk-9.6")
message("${VTK_EXTERNAL_PREFIX}")
if(EXISTS "${VTK_EXTERNAL_PREFIX}")
  list(APPEND CMAKE_PREFIX_PATH "${VTK_EXTERNAL_PREFIX}")
endif()
find_package(VTK CONFIG REQUIRED
    COMPONENTS
        CommonCore
        CommonDataModel
        IOXML
        IOLegacy)
message(STATUS "VTK found: ${VTK_FOUND}")



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



# #####
# # OpenMP
# #find_package(OpenMP)
# find_package(OpenMP REQUIRED)
# if(APPLE)
#     message("\nConfiguring OPENMP compiler variables\n")
#     if (OPENMP_FOUND)

#        #set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#        #set (OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp /usr/local/lib/libomp.dylib -I /usr/local/include/")
#        #set (OpenMP_CXX_LIB_NAMES "libomp")
#        #set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#         set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#     endif()
#     message("${CMAKE_CXX_FLAGS}\n")
#     message("${CMAKE_EXE_LINKER_FLAGS}\n")
# else()
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
# endif()


#####
# OpenMP (macOS_MM fix)
if(APPLE)
    message(STATUS "Configuring OpenMP for macOS")

    # Percorsi libomp
    set(LIBOMP_INCLUDE_DIR "/opt/homebrew/opt/libomp/include")
    set(LIBOMP_LIB_DIR "/opt/homebrew/opt/libomp/lib")

    find_library(OMP_LIBRARY omp
        PATHS ${LIBOMP_LIB_DIR} /usr/local/opt/libomp/lib
        REQUIRED
    )

    # Flag per Apple Clang
    set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I${LIBOMP_INCLUDE_DIR}")
    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I${LIBOMP_INCLUDE_DIR}")
    set(OpenMP_C_LIB_NAMES "omp")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY "${OMP_LIBRARY}")

endif()

# Cerca OpenMP con CMake
find_package(OpenMP REQUIRED)


#####
# Flann
#find_package(FLANN REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../external/GeoStatsLib/external/flann-lib/installed NO_DEFAULT_PATH)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/GeoStatsLib/external/flann-lib/installed/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/GeoStatsLib/external/flann-lib/installed/lib)

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
#include_directories(BEFORE "../external/muse_database/include")


#####
# CLI Documentation Generation
find_package(Python3 COMPONENTS Interpreter)
if(Python3_FOUND)
    set(CLI_DOCS_SCRIPT "${CMAKE_SOURCE_DIR}/../scripts/generate_cli_docs.py")
    set(HTML_DOCS_SCRIPT "${CMAKE_SOURCE_DIR}/../scripts/generate_html_docs.py")
    set(MD_DOCS_OUTPUT "${CMAKE_SOURCE_DIR}/../docs/md")
    set(HTML_DOCS_OUTPUT "${CMAKE_SOURCE_DIR}/../docs/html")
    
    # Generate Markdown documentation
    add_custom_target(cli-docs-md ALL
        COMMAND ${Python3_EXECUTABLE} ${CLI_DOCS_SCRIPT} ${CMAKE_SOURCE_DIR} ${MD_DOCS_OUTPUT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
        COMMENT "Generating CLI markdown documentation from source code..."
        VERBATIM
    )
    
    # Generate HTML documentation from Markdown
    add_custom_target(cli-docs-html ALL
        COMMAND ${Python3_EXECUTABLE} ${HTML_DOCS_SCRIPT} ${MD_DOCS_OUTPUT} ${HTML_DOCS_OUTPUT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
        DEPENDS cli-docs-md
        COMMENT "Generating HTML documentation from markdown..."
        VERBATIM
    )
    
    # Combined target for both
    add_custom_target(cli-docs ALL
        DEPENDS cli-docs-md cli-docs-html
        COMMENT "Generating complete CLI documentation (Markdown + HTML)"
    )
    
    message(STATUS "CLI documentation generation enabled")
    message(STATUS "Markdown output: ${MD_DOCS_OUTPUT}")
    message(STATUS "HTML output: ${HTML_DOCS_OUTPUT}")
else()
    message(WARNING "Python3 not found - CLI documentation generation disabled")
endif()


add_subdirectory(muse_project)
add_subdirectory(muse_data)
add_subdirectory(muse_geometry)
add_subdirectory(muse_manipulate)
add_subdirectory(muse_vario)
add_subdirectory(muse_compute)
add_subdirectory(muse_plot)
add_subdirectory(muse_utility)
add_subdirectory(muse_export)
