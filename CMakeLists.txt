cmake_minimum_required(VERSION 3.25)
project(Map)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")


if(BSD)


    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

    include_directories(/usr/local/include)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")


endif()


add_executable(Map main.cpp Map.h)
