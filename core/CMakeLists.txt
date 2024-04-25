add_library(core)

target_sources(core
PUBLIC
	core.h
	logging.h
	typedefs.h
	uassert.h
	timer.h
PRIVATE
	core.cpp
)

target_include_directories(core
PRIVATE # Do not populate the root dir to main or other modules.
	${PROJECT_SOURCE_DIR}
)

target_link_libraries(core
PUBLIC
	glm::glm
)
