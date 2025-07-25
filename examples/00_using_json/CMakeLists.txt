cmake_minimum_required(VERSION 3.14)

project(using_json LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(BEFORE "../../external/nlohmann")

add_executable(using_json
  main.cpp
)

