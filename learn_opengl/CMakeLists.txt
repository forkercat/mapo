add_executable(LearnOpenGL)

find_package(OpenGL REQUIRED)

target_sources(LearnOpenGL
PUBLIC

PRIVATE
	learn_opengl.cpp
)

target_link_libraries(LearnOpenGL
PRIVATE
	OpenGL::GL
	${GLFW3_LIBRARY}
	glad
	core
)

target_include_directories(LearnOpenGL
PRIVATE
	${GLFW3_INCLUDE_DIR}
)
