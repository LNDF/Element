cmake_minimum_required(VERSION 3.0.0)
project(glad VERSION 0.1.36)

add_library(glad ./src/glad.c)

set_property(TARGET glad PROPERTY CXX_STANDARD 20)
target_compile_options(glad PRIVATE -Wall)

target_include_directories(glad PUBLIC ./include)