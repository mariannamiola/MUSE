project(muse_vario LANGUAGES CXX)

#####
# Eigen (for ellipse-fitting)
#find_package(Eigen3 REQUIRED)
#include_directories(${EIGEN3_INCLUDE_DIR})
#message(${EIGEN3_INCLUDE_DIR})

add_executable(${PROJECT_NAME}
  main.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC Matplot++::matplot) # Eigen3::Eigen)
