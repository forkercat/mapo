cmake_minimum_required(VERSION 3.27)
project(mapo-engine)

set(CMAKE_CXX_STANDARD 17)

#find_package(OpenGL REQUIRED)
#find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(Stb REQUIRED)
find_package(tinyobjloader REQUIRED)

# Subdirectories
add_subdirectory(core)
add_subdirectory(engine)
add_subdirectory(main)
add_subdirectory(editor)
add_subdirectory(3rdparty)

add_subdirectory(main-cpp)
