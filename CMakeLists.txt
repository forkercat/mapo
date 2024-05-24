cmake_minimum_required(VERSION 3.27)
project(mapo-engine)

set(CMAKE_CXX_STANDARD 17)

#find_package(OpenGL REQUIRED)
#find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(Stb REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(ImGuizmo CONFIG REQUIRED)

# Common interface target
add_library(common INTERFACE)
target_include_directories(common INTERFACE ${PROJECT_SOURCE_DIR} "3rdparty/")

if(APPLE AND CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	target_compile_definitions(common INTERFACE MP_MACOS_BUILD)
endif()

# Subdirectories
add_subdirectory(core)
add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(3rdparty)

add_subdirectory(main-cpp)
