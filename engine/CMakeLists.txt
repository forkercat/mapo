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
	# Render
	render/vulkan_device.h
	render/vulkan_pipeline.h
	render/vulkan_swapchain.h
	render/vulkan_buffer.h
	render/vulkan_renderer.h
	render/vulkan_descriptors.h
	render/vulkan_frame_info.h
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
	model.cpp
	game_object.cpp
	component.cpp
	camera.cpp
	keyboard_controller.cpp
	# Render
	render/vulkan_device.cpp
	render/vulkan_pipeline.cpp
	render/vulkan_swapchain.cpp
	render/vulkan_buffer.cpp
	render/vulkan_renderer.cpp
	render/vulkan_descriptors.cpp
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
