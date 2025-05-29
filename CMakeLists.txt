cmake_minimum_required(VERSION 3.15)

project(Compiler VERSION 0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(src)

add_executable(out
    src/main.cpp
    src/compiler.cpp
    src/lexer.cpp
)

