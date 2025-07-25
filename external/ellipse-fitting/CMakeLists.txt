cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)

set(SOURCES 
ellipse_fit.h
)

link_libraries(stdc++fs)

add_executable (ellipse_main ellipse_main.cpp ellipse_fit.cpp)

