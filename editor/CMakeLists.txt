add_executable(editor)

target_sources(editor
PUBLIC
	editor_layer.h
PRIVATE
	editor_layer.cpp
	editor_app.cpp
)

target_link_libraries(editor
PRIVATE
	common
	engine
)
