add_library(core)

target_sources(core
PUBLIC
	core.h
	logging.h
	typedefs.h
	uassert.h
PRIVATE
	core.cpp
)

target_include_directories(core
PUBLIC
	${CMAKE_SOURCE_DIR}
)

# DISABLED
#target_precompile_headers(core PUBLIC pch.h)
