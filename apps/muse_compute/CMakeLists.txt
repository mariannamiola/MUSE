project(muse_compute LANGUAGES CXX)

find_package(SQLite3 REQUIRED)

#include_directories(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(${PROJECT_NAME}
  main.cpp
)
target_link_libraries(${PROJECT_NAME} PUBLIC Matplot++::matplot ${ARMADILLO_LIBRARIES} OpenMP::OpenMP_CXX SQLite::SQLite3 ${LZ4_LINK_LIBRARIES}
)
