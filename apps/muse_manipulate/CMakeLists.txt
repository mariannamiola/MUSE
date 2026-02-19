project(muse_manipulate LANGUAGES CXX)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}
  main.cpp
)

target_link_libraries(${PROJECT_NAME} Threads::Threads
  VTK::CommonCore
  VTK::CommonDataModel
  VTK::IOXML
  VTK::IOLegacy
)
