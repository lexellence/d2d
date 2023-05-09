# version 3.11 or later of CMake needed later for installing GoogleTest
# so let's require it now.
cmake_minimum_required(VERSION 3.11)
cmake_policy(SET CMP0076 NEW)

project(d2d)

# d2d headers
#include_directories(../Include)

# library headers
#include_directories(
#    ../../../../lib/libdrawtext/include
#    ../../../../lib/hjson
#    )
add_library(${PROJECT_NAME} STATIC)
#find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)


add_subdirectory(${PROJECT_SOURCE_DIR}/Source)

# Precompiled Header
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
    message(STATUS "Compiling using pre-compiled header support")
    target_precompile_headers(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/Include/d2pch.h)
else()
    message(STATUS "Disabled pre-compiled header support")
endif()

#target_include_directories(d2d PUBLIC
 #   ../Include
 #   ../../../../lib/libdrawtext/include
#    ../../../../lib/hjson
#)

# PUBLIC needed to make library available elsewhere in project
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}/Include
)

#target_link_directories(d2d PUBLIC
#    ../../../lib/libdrawtext/lib/x64/Release
#    ../../../lib/hjson/lib/x64/Release
#)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2 SDL2_image SDL2_net box2d hjson drawtext GL GLU)
#target_link_libraries(${PROJECT_NAME} PUBLIC SDL2 SDL2_image SDL2_net box2d hjson drawtext OpenGL::GL OpengL::GLU GLU)

# Tell compiler to use C++20 features. The code doesn't actually use any of them.
target_compile_features(d2d PUBLIC cxx_std_20)
