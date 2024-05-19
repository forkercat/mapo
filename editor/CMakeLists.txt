add_executable(editor)

target_sources(editor
PUBLIC
	editor_layer.h
	# Panels
	panel/panel.h
	panel/scene_panel.h
	panel/info_panel.h
PRIVATE
	editor_layer.cpp
	editor_app.cpp
	# Panels
	panel/panel.cpp
	panel/scene_panel.cpp
	panel/info_panel.cpp
)

target_link_libraries(editor
PRIVATE
	common
	engine
)
