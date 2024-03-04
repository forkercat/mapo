# glfw
find_package(GLFW3 REQUIRED)

# glad
add_library(glad STATIC)

target_sources(glad
PUBLIC
	glad/include/KHR/khrplatform.h
	glad/include/glad/glad.h
PRIVATE
	glad/src/glad.c
)

target_include_directories(glad PUBLIC "${CMAKE_CURRENT_LIST_DIR}/glad/include")
