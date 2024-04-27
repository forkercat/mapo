add_executable(main-cpp)

target_sources(main-cpp
PUBLIC
PRIVATE
	main.cpp
)

target_link_libraries(main-cpp
PRIVATE
	core
)

target_include_directories(main-cpp
PRIVATE
	${PROJECT_SOURCE_DIR}
)
