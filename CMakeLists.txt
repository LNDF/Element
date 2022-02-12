cmake_minimum_required(VERSION 3.0.0)
project(engine-core VERSION 0.1.0)

#include(CTest)
#enable_testing()

# source files
file(GLOB_RECURSE SRC_FILES ./src/*.*)
add_library(engine-core ${SRC_FILES})

# asset files
file(GLOB_RECURSE RESOURCE_FILES ./resources/*.*)
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR})

# compile options
set_property(TARGET engine-core PROPERTY CXX_STANDARD 11)
target_compile_options(engine-core PRIVATE -Wall)

# LIBS
# glfw
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(engine-core PRIVATE glfw)

# glew
add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(engine-core PRIVATE libglew_static)

# glm
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(engine-core PRIVATE glm)

# CMake options
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
