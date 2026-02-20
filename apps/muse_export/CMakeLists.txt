cmake_minimum_required(VERSION 3.5)

project(muse_export LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#####
# find GDAL library
add_definitions(-DMUSE_USES_GDAL)
#include_directories(${GDAL_INCLUDE_DIR})

include_directories(BEFORE "../muse/include")
include_directories(BEFORE "../muse/external/geostatslib/include")
include_directories(BEFORE "../muse/external/cinolib/include")


add_executable(muse_export main.cpp)
add_executable(muse_export_raster main_raster.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC GDAL::GDAL VTK::CommonCore
    VTK::CommonDataModel
    VTK::IOXML
    VTK::IOLegacy
)

find_package(OpenMP)
target_link_libraries(muse_export_raster PUBLIC OpenMP::OpenMP_CXX)

include(GNUInstallDirs)
install(TARGETS muse_export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
