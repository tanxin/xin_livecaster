project(livecaster)
message(STATUS "*** We are trying to build project ${PROJECT_NAME} ***")
add_subdirectory(src bin)

install(FILES COPYRIGHT README DESTINATION share/doc/cmake/livecascter)

set(CMAKE_C_FLAGS_DEBUG "-g")
cmake_minimum_required(VERSION 2.6)
