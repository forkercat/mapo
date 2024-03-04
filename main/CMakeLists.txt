add_executable(main)

target_sources(main
PUBLIC

PRIVATE
	main.cpp
)

target_link_libraries(main
PRIVATE
	core
)
