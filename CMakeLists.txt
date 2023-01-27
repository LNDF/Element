cmake_minimum_required(VERSION 3.0.0)
project(element VERSION 0.0.1)

#include(CTest)
#enable_testing()

# source files
file(GLOB_RECURSE SRC_FILES ./src/*.*)
add_library(element ${SRC_FILES})
target_include_directories(element PUBLIC ./src/)

# asset files
file(GLOB_RECURSE RESOURCE_FILES ./resources/*.*)
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR})

# user options
option(ELEMENT_ENABLE_LOG "Enable logging in Element engine" ON) 

# compile options
set_property(TARGET element PROPERTY CXX_STANDARD 20)
target_compile_options(element PRIVATE -Wall)

# define options
target_compile_definitions(element PUBLIC ELM_VERSION="${PROJECT_VERSION}")
if(ELEMENT_ENABLE_LOG)
    target_compile_definitions(element PUBLIC ELM_ENABLE_LOGGING=1)    
endif(ELEMENT_ENABLE_LOG)


#LIBS
# glfw
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(element PUBLIC glfw)

# glew
add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(element PUBLIC libglew_static)

# glm
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(element PUBLIC glm)

# spdlog
if(ELEMENT_ENABLE_LOG)
    add_subdirectory(lib/spdlog EXCLUDE_FROM_ALL)
    target_link_libraries(element PUBLIC spdlog::spdlog)
endif(ELEMENT_ENABLE_LOG)

#end libs

# CMake options
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
