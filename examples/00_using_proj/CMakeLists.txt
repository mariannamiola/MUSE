cmake_minimum_required(VERSION 3.14)

project(using_proj LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# PROJ
find_package(PROJ CONFIG REQUIRED)

add_executable(using_proj
  main.cpp
)
target_link_libraries(using_proj PRIVATE PROJ::proj)
