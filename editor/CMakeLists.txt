add_executable(editor)

target_sources(editor
PUBLIC
	editor_layer.h
	scene_panel.h
PRIVATE
	editor_layer.cpp
	editor_app.cpp
	scene_panel.cpp
)

target_link_libraries(editor
PRIVATE
	common
	engine
)
