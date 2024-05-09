set(PLATFORM_SRC_DIR ${PROJECT_SOURCE_DIR}/platform)

add_library(engine)

target_sources(engine
PUBLIC
	entry_point.h
	application.h
	window.h
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
	# Render
	render/render_context.h
	render/render_command.h
	render/renderer.h
	render/renderer_api.h
	# System
	system/simple_render_system.h
	system/point_light_system.h
	system/imgui_system.h
	system/rainbow_system.h
	scene.h
	# Platform
	${PLATFORM_SRC_DIR}/macos/macos_window.h
	${PLATFORM_SRC_DIR}/vulkan/vulkan_render_context.h
	${PLATFORM_SRC_DIR}/vulkan/vulkan_renderer_api.h
	${PLATFORM_SRC_DIR}/vulkan/vulkan_device.h
	${PLATFORM_SRC_DIR}/vulkan/vulkan_pipeline.h
	${PLATFORM_SRC_DIR}/vulkan/vulkan_swapchain.h
	${PLATFORM_SRC_DIR}/vulkan/vulkan_buffer.h
	${PLATFORM_SRC_DIR}/vulkan/vulkan_renderer.h
	${PLATFORM_SRC_DIR}/vulkan/vulkan_descriptors.h
	${PLATFORM_SRC_DIR}/vulkan/vulkan_frame_info.h
PRIVATE
	application.cpp
	window.cpp
	model.cpp
	game_object.cpp
	component.cpp
	camera.cpp
	keyboard_controller.cpp
	# Render
	render/render_context.cpp
	render/render_command.cpp
	render/renderer.cpp
	render/renderer_api.cpp
	# System
	system/simple_render_system.cpp
	system/point_light_system.cpp
	system/imgui_system.cpp
	scene.cpp
	# Platform
	${PLATFORM_SRC_DIR}/macos/macos_input.cpp
	${PLATFORM_SRC_DIR}/macos/macos_window.cpp
	${PLATFORM_SRC_DIR}/vulkan/vulkan_render_context.cpp
	${PLATFORM_SRC_DIR}/vulkan/vulkan_renderer_api.cpp
	${PLATFORM_SRC_DIR}/vulkan/vulkan_device.cpp
	${PLATFORM_SRC_DIR}/vulkan/vulkan_pipeline.cpp
	${PLATFORM_SRC_DIR}/vulkan/vulkan_swapchain.cpp
	${PLATFORM_SRC_DIR}/vulkan/vulkan_buffer.cpp
	${PLATFORM_SRC_DIR}/vulkan/vulkan_renderer.cpp
	${PLATFORM_SRC_DIR}/vulkan/vulkan_descriptors.cpp
)

target_link_libraries(engine
PUBLIC
	core
	ImGui
PRIVATE
	Vulkan::Vulkan
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
