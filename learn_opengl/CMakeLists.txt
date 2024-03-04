add_executable(LearnOpenGL)

target_sources(LearnOpenGL
PUBLIC

PRIVATE
	learn_opengl.cpp
)

target_link_libraries(LearnOpenGL
PRIVATE
	OpenGL::GL
	glfw
	glad::glad
	glm::glm
	core
)

#target_include_directories(LearnOpenGL
#PRIVATE
#	${GLFW3_INCLUDE_DIR}
#)
