cmake_minimum_required(VERSION 3.20)

project(aoc-2023)

set(BOOST_ROOT /usr/include/boost169)
find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to Debug as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)

add_compile_options(-std=c++2a -Wall)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")

add_executable(aoc-2023 main.cpp runner.cpp week1.cpp)
