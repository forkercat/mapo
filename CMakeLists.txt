cmake_minimum_required(VERSION 3.27)
project(mapo-engine)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Subdirectories
add_subdirectory(core)
add_subdirectory(main)
add_subdirectory(editor)
add_subdirectory(learn_opengl)
