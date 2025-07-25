cmake_minimum_required(VERSION 3.21)
project(MUSE_DB LANGUAGES CXX)

find_package(SQLite3 REQUIRED)

include_directories(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/external/tclap-1.4.0-rc1/include")

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC SQLite::SQLite3)
