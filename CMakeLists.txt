project(rtspcaster)
message(STATUS "*** We are trying to build project ${PROJECT_NAME} ***")
add_subdirectory(src bin)

install(FILES COPYRIGHT README DESTINATION share/doc/cmake/rtspcascter)

set(CMAKE_C_FLAGS_DEBUG "-g")
cmake_minimum_required(VERSION 2.6)
