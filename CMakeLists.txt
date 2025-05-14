cmake_minimum_required(VERSION 3.15)

project(Compiler VERSION 0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR src)
set(INCLUDE_DIR inc)

include_directories(${INCLUDE_DIR})


add_executable(out ${SOURCE_DIR})
