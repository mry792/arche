cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)
project(arche VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME})

add_subdirectory(include)
add_subdirectory(src)
