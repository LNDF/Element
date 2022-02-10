cmake_minimum_required(VERSION 3.0.0)
project(engine-core VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(engine-core engine-core.cpp)

set_property(TARGET engine-core PROPERTY CXX_STANDARD 11)
target_compile_options(engine-core PRIVATE -Wall)

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

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
