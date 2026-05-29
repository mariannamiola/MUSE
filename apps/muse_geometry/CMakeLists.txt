project(muse_geometry LANGUAGES CXX)

# #####`
# # VTK
# find_package(VTK COMPONENTS
#     CommonCore
#     CommonDataModel
#     IOXML
#     IOLegacy)
# if(VTK_FOUND)
#     include_directories(SYSTEM ${VTK_INCLUDE_DIRS})
# endif()

# message("VTK found @ " VTK\n)

#####
# gdal
# add_definitions(-DMUSE_USES_GDAL)
# #find_package(GDAL REQUIRED)
# find_package(GDAL CONFIG REQUIRED PATHS ${CMAKE_SOURCE_DIR}/../external/gdal/installed/lib/cmake/gdal NO_DEFAULT_PATH)
# set (GDAL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/gdal/installed/include/)
# set (GDAL_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/gdal/installed/lib/)

# if (UNIX AND NOT APPLE)
#     set (GDAL_LIBRARY ${GDAL_LIBRARY_DIR}/libgdal.so)
# elseif (MSVC)
#     set (GDAL_LIBRARY ${GDAL_LIBRARY_DIR}/gdal.lib)
# else() ###macos
#     set (GDAL_LIBRARY ${GDAL_LIBRARY_DIR}/libgdal.dylib)
# endif()

#include_directories(${GDAL_INCLUDE_DIR})

#message ("${GDAL_INCLUDE_DIR}")
#message ("${GDAL_LIBRARY}")

#####
# cinolib
#set(CINOLIB_USES_VTK    ON )

add_executable(${PROJECT_NAME}
  main.cpp
  #${PROJ_LIB}
)

target_link_libraries(${PROJECT_NAME} PUBLIC cinolib PROJ::proj GDAL::GDAL)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	VOLUME_MESHER_EXECUTABLE="${VOLUME_MESHER_EXECUTABLE_PATH}"
)

#####

set(TETGEN_GENERATOR ${PROJECT_NAME}_tetmesh-generator)

add_executable(${TETGEN_GENERATOR}
  main-tetmesh-generator.cpp
)

target_link_libraries(${TETGEN_GENERATOR} PUBLIC cinolib)

