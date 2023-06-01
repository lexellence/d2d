# version 3.11 or later of CMake needed later for installing GoogleTest
# so let's require it now.
cmake_minimum_required(VERSION 3.11)
cmake_policy(SET CMP0076 NEW)

project(d2d)

add_library(${PROJECT_NAME} STATIC)
add_subdirectory(${PROJECT_SOURCE_DIR}/Source)
add_subdirectory(${PROJECT_SOURCE_DIR}/Include)

# Precompiled Header
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
    message(STATUS "Compiling using pre-compiled header support")
    target_precompile_headers(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/Include/d2pch.h)
else()
    message(STATUS "Disabled pre-compiled header support")
endif()

# PUBLIC needed to make library available elsewhere in project
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Include)

# Installed libraries
find_package(Boost COMPONENTS program_options log log_setup REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::log Boost::log_setup)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2 SDL2_image SDL2_net box2d GL GLU freetype drawtext)

# Libraries included with project
# target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/lib/libdrawtext-0.6)
# target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/lib/libdrawtext-0.6/libdrawtext.a)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/lib/hjson-cpp-2.3/)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/lib/hjson-cpp-2.3/libhjson.a)

target_compile_features(d2d PUBLIC cxx_std_20)
