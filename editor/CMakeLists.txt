add_executable(editor-app)

target_sources(editor-app
PUBLIC
	editor_layer.h
PRIVATE
	editor_layer.cpp
	editor_app.cpp
)

target_link_libraries(editor-app
PRIVATE
	common
	engine
)
