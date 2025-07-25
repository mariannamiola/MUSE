cmake_minimum_required(VERSION 3.5)

project(muse_export LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#####
# find GDAL library
add_definitions(-DMUSE_USES_GDAL)
# FIND_PACKAGE(GDAL COMPONENTS REQUIRED)
# IF(GDAL_FOUND)
#   INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIRS})
#   MESSAGE(STATUS "GDAL found.")
#   # Extract GDAL version from gdal_version.h
#   SET(GDAL_VERSION_FILE ${GDAL_INCLUDE_DIRS}/gdal_version.h)
#   IF (NOT EXISTS ${GDAL_VERSION_FILE})
#       GDAL_REPORT_NOT_FOUND(
#           "Could not find file: ${GDAL_VERSION_FILE} "
#           "containing version information in GDAL install located at: "
#           "${GDAL_INCLUDE_DIRS}.")
#   ELSE (NOT EXISTS ${GDAL_VERSION_FILE})
#     FILE(READ ${GDAL_VERSION_FILE} GDAL_VERSION_FILE_CONTENTS)
#         STRING(REGEX MATCH "define GDAL_VERSION_MAJOR +[0-9]+"
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
#   ENDIF(NOT EXISTS ${GDAL_VERSION_FILE})
#   MESSAGE(STATUS "GDAL version: ${GDAL_VERSION}")
#   ADD_DEFINITIONS( -DGDALVER=${GDAL_VERSION_MAJOR} )
# ENDIF()



include_directories(BEFORE "../muse/include")
include_directories(BEFORE "../muse/external/geostatslib/include")
include_directories(BEFORE "../muse/external/cinolib/include")

add_executable(muse_export main.cpp)
add_executable(muse_export_raster main_raster.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC ${GDAL_LIBRARY})

find_package(OpenMP)
target_link_libraries(muse_export_raster PUBLIC OpenMP::OpenMP_CXX)

include(GNUInstallDirs)
install(TARGETS muse_export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
