cmake_minimum_required(VERSION 3.0.0)
project(map VERSION 0.1.0)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

add_executable(map main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
