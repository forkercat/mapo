add_executable(main)

target_sources(main
PUBLIC
	hello_app.h
PRIVATE
	hello_app.cpp
	main.cpp
)

target_link_libraries(main
PRIVATE
	engine
)

target_include_directories(main
PRIVATE
	${PROJECT_SOURCE_DIR}
)
