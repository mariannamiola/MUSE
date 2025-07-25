project(muse_project LANGUAGES CXX)

add_executable(${PROJECT_NAME}
  main.cpp
)
target_link_libraries(${PROJECT_NAME} PUBLIC)
#target_link_libraries(${PROJECT_NAME} PUBLIC "stdc++fs")
