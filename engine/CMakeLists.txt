set(PLATFORM_SRC_DIR ${PROJECT_SOURCE_DIR}/platform)

add_library(engine)

target_sources(engine
PUBLIC
	entry_point.h
	application.h
	window.h
	layer.h
	layer_stack.h
	model.h
	game_object.h
	component.h
	camera.h
	keyboard_controller.h
	utils.h
	# Input
	input/input.h
	input/key_codes.h
	input/mouse_codes.h
	# Renderer
	renderer/vk_common.h
	renderer/render_context.h
	renderer/renderer.h
	renderer/device.h
	renderer/pipeline.h
	renderer/swapchain.h
	renderer/buffer.h
	renderer/descriptors.h
	renderer/frame_info.h
	# UI
	ui/imgui_layer.h
	# System
	system/simple_render_system.h
	system/point_light_system.h
	system/imgui_system.h
	system/rainbow_system.h
	scene.h
	# Platform
	${PLATFORM_SRC_DIR}/macos/macos_window.h
PRIVATE
	application.cpp
	window.cpp
	layer.cpp
	layer_stack.cpp
	model.cpp
	game_object.cpp
	component.cpp
	camera.cpp
	keyboard_controller.cpp
	# Renderer
	renderer/render_context.cpp
	renderer/renderer.cpp
	renderer/device.cpp
	renderer/pipeline.cpp
	renderer/swapchain.cpp
	renderer/buffer.cpp
	renderer/descriptors.cpp
	# UI
	ui/imgui_layer.cpp
	# System
	system/simple_render_system.cpp
	system/point_light_system.cpp
	system/imgui_system.cpp
	scene.cpp
	# Platform
	${PLATFORM_SRC_DIR}/macos/macos_input.cpp
	${PLATFORM_SRC_DIR}/macos/macos_window.cpp
)

target_link_libraries(engine
PUBLIC
	core
	ImGui
	Vulkan::Vulkan # Should be in private. But Editor still directly uses Vulkan resources (macros).
PRIVATE
	common
)

add_custom_target(
	compile-shaders ALL
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/assets/shaders
	COMMAND ${CMAKE_COMMAND} -E echo "Running pre-built script to compile shaders..."
	COMMAND ${PROJECT_SOURCE_DIR}/assets/shaders/compile.sh
	COMMENT "Executing pre-built script..."
)

add_dependencies(engine compile-shaders)
