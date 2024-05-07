add_executable(main-editor-app)

target_sources(main-editor-app
PUBLIC

PRIVATE
	editor_app.cpp
)

target_link_libraries(main-editor-app
PRIVATE
	common
	engine
)
