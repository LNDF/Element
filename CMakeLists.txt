cmake_minimum_required(VERSION 3.0.0)
project(element VERSION 0.1.0)

#include(CTest)
#enable_testing()

# source files
file(GLOB_RECURSE SRC_FILES ./src/*.*)
add_executable(element ${SRC_FILES})
target_include_directories(element PUBLIC ./src/)

# asset files
file(GLOB_RECURSE RESOURCE_FILES ./resources/*.*)
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR})

# compile options
set_property(TARGET element PROPERTY CXX_STANDARD 20)
target_compile_options(element PRIVATE -Wall)

#LIBS
# glfw
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(element PRIVATE glfw)

# glew
add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(element PRIVATE libglew_static)

# glm
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(element PRIVATE glm)

# imgui
set(IMGUI_DIR lib/imgui)
file(GLOB IMGUI_FONTS ${IMGUI_DIR}/misc/fonts/*.ttf)
set(IMGUI_FILES
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imconfig.h
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/imstb_rectpack.h
    ${IMGUI_DIR}/imstb_textedit.h
    ${IMGUI_DIR}/imstb_truetype.h
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
    ${IMGUI_FONTS}
)
add_library(imgui ${IMGUI_FILES})
target_include_directories(imgui PUBLIC ${IMGUI_DIR})
target_link_libraries(element PRIVATE imgui)

# openssl crypto
add_subdirectory(lib/openssl EXCLUDE_FROM_ALL)
target_link_libraries(element PRIVATE crypto)

# spdlog
add_subdirectory(lib/spdlog EXCLUDE_FROM_ALL)
target_link_libraries(element PRIVATE spdlog::spdlog)

#end libs

# CMake options
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
