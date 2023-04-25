cmake_minimum_required(VERSION 3.0.0)
project(element VERSION 0.0.1)

#include(CTest)
#enable_testing()

macro(OVERRIDE_OPTION option type value)
  set(${option} ${value} CACHE ${type} "" FORCE)
endmacro()

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
target_compile_definitions(element PUBLIC ELM_VERSION_ID=0)
if(ELEMENT_ENABLE_LOG)
    target_compile_definitions(element PUBLIC ELM_ENABLE_LOGGING=1)    
endif(ELEMENT_ENABLE_LOG)


#LIBS

# vulkan
find_package(Vulkan COMPONENTS shaderc_combined REQUIRED)
target_link_libraries(element PUBLIC Vulkan::Vulkan)
target_link_libraries(element PUBLIC Vulkan::shaderc_combined)

# glm
add_subdirectory(lib/glm)
target_link_libraries(element PUBLIC glm)

# cereal
OVERRIDE_OPTION(BUILD_DOC BOOL OFF)
OVERRIDE_OPTION(BUILD_SANDBOX BOOL OFF)
OVERRIDE_OPTION(SKIP_PERFORMANCE_COMPARISON BOOL ON)
OVERRIDE_OPTION(BUILD_TESTS BOOL OFF)
add_subdirectory(lib/cereal)
target_link_libraries(element PUBLIC cereal)

# spdlog
if(ELEMENT_ENABLE_LOG)
    #OVERRIDE_OPTION(SPDLOG_USE_STD_FORMAT BOOL ON)
    add_subdirectory(lib/spdlog)
    target_link_libraries(element PUBLIC spdlog::spdlog)
endif(ELEMENT_ENABLE_LOG)

#end libs

# CMake options
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
