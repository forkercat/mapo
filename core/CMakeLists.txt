add_library(core)

target_sources(core
PUBLIC
	core.h
	logging.h
	typedefs.h
	uassert.h
	timer.h
	timestep.h
	# string
	string/string.h
	string/string_name.h
	# memory
	memory/memory.h
	memory/allocator.h
	# templates
	templates/hash_map.h
	templates/hash_set.h
	# input
	input/input.h
	input/key_codes.h
	input/mouse_codes.h
PRIVATE
	core.cpp
	# string
	string/string.cpp
	string/string_name.cpp
	# memory
	memory/allocator.cpp
)

target_link_libraries(core
PUBLIC
	glm::glm
PRIVATE
	common
)
