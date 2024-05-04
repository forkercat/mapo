add_executable(main-editor-app)

target_sources(main-editor-app
PUBLIC

PRIVATE
	editor_app.cpp
)

target_link_libraries(main-editor-app
PRIVATE
	engine
)

target_include_directories(main-editor-app
PRIVATE
	${PROJECT_SOURCE_DIR}
)
