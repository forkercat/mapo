# glfw
find_package(GLFW3 REQUIRED)

# glad
add_library(glad STATIC
#	"glad/include/KHR/khrplatform.h"
#	"glad/include/glad/glad.h"
	"glad/src/glad.c")

target_include_directories(glad PRIVATE "glad/include")
