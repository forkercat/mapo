cmake_minimum_required(VERSION 3.27)
project(mapo-engine)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# 3rdparty
add_subdirectory(3rdparty)

# Subdirectories
add_subdirectory(core)
add_subdirectory(main)
add_subdirectory(learn_opengl)
