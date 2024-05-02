add_library(platform)

target_sources(platform
PUBLIC
PRIVATE
	macos/macos_input.cpp
)

target_include_directories(platform
PRIVATE # Do not populate the root dir to main or other modules.
	${PROJECT_SOURCE_DIR}
)

target_link_libraries(core
PUBLIC
	glm::glm
)
