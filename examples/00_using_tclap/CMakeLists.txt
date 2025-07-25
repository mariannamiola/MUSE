cmake_minimum_required(VERSION 3.14)

project(using_tclap LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(BEFORE "../../external/tclap-1.4.0-rc1/include")

add_executable(using_tclap
  main.cpp
)
