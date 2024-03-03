cmake_minimum_required(VERSION 3.27)
project(mapo)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(OpenGL REQUIRED)

# 3rdparty
add_subdirectory(3rdparty)

set(3RDPARTY_PRIVATE_INCLUDE_DIRS
	${GLFW3_INCLUDE_DIR}
	"3rdparty/glad/include")

set(3RDPARTY_LIBRARIES
	OpenGL::GL ${GLFW3_LIBRARY} glad)

# Targets
add_executable(mapo
	"main/main.cpp"
	"core/typedefs.h")

add_executable(learn
	"learn/main.cpp")

# Include directories and link libraries.
target_include_directories(mapo PRIVATE ${3RDPARTY_PRIVATE_INCLUDE_DIRS})
target_link_libraries(mapo PRIVATE ${3RDPARTY_LIBRARIES})

target_include_directories(learn PRIVATE ${3RDPARTY_PRIVATE_INCLUDE_DIRS})
target_link_libraries(learn PRIVATE ${3RDPARTY_LIBRARIES})

# Precompiled headers
target_precompile_headers(mapo PRIVATE "core/pch.h")
target_precompile_headers(learn PRIVATE "core/pch.h")
