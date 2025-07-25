project(muse_data LANGUAGES CXX)

add_executable(${PROJECT_NAME}
  main.cpp
  matrix.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC Matplot++::matplot) #PROJ::proj)
    #PRIVATE PROJ::proj)
