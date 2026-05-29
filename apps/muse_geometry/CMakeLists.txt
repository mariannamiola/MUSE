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

#####
# triangle/tetgen
set (TRIANGLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/triangle)
set (TETGEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/tetgen)

add_custom_command(
  OUTPUT ${TRIANGLE_DIR}/build/libtriangle.a
  COMMAND mkdir -p build && cd build && cmake .. && make
  #DEPENDS ${SOURCE_FILES} /tmp/bin/create_foo_hh main.cpp
  WORKING_DIRECTORY ${TRIANGLE_DIR}
)

add_custom_command(
  OUTPUT ${TETGEN_DIR}/build/libtetgen.a
  COMMAND mkdir -p build && cd build && cmake .. && make
  #DEPENDS ${SOURCE_FILES} /tmp/bin/create_foo_hh main.cpp
  WORKING_DIRECTORY ${TETGEN_DIR}
)

add_executable(${PROJECT_NAME}
  main.cpp
  ${TRIANGLE_DIR}/build/libtriangle.a
  ${TETGEN_DIR}/build/libtetgen.a
  #${PROJ_LIB}
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${triangle_lib_file} ${tetgen_lib_file} PROJ::proj GDAL::GDAL)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	VOLUME_MESHER_EXECUTABLE="${VOLUME_MESHER_EXECUTABLE_PATH}"
)

#####

set(TETGEN_GENERATOR ${PROJECT_NAME}_tetmesh-generator)

add_executable(${TETGEN_GENERATOR}
  main-tetmesh-generator.cpp
  ${TRIANGLE_DIR}/build/libtriangle.a
  ${TETGEN_DIR}/build/libtetgen.a
)

target_link_libraries(${TETGEN_GENERATOR} PUBLIC ${triangle_lib_file} ${tetgen_lib_file})

