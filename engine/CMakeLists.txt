add_library(engine)

target_sources(engine
PUBLIC
	model.h
	game_object.h
	component.h
	camera.h
	keyboard_controller.h
	utils.h
	vulkan_window.h
	vulkan_device.h
	vulkan_pipeline.h
	vulkan_swapchain.h
	vulkan_buffer.h
	vulkan_renderer.h
	vulkan_descriptors.h
	vulkan_frame_info.h
	# System
	system/simple_render_system.h
	system/point_light_system.h
	system/imgui_system.h
	system/rainbow_system.h
	scene.h
PRIVATE
	model.cpp
	game_object.cpp
	component.cpp
	camera.cpp
	keyboard_controller.cpp
	vulkan_window.cpp
	vulkan_device.cpp
	vulkan_pipeline.cpp
	vulkan_swapchain.cpp
	vulkan_buffer.cpp
	vulkan_renderer.cpp
	vulkan_descriptors.cpp
	# System
	system/simple_render_system.cpp
	system/point_light_system.cpp
	system/imgui_system.cpp
	scene.cpp
)

target_link_libraries(engine
PUBLIC
	glfw
	Vulkan::Vulkan
	core
	ImGui
)

target_include_directories(engine
PRIVATE
	${PROJECT_SOURCE_DIR}
)

add_custom_target(
	compile-shaders ALL
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/assets/shaders
	COMMAND ${CMAKE_COMMAND} -E echo "Running pre-built script to compile shaders..."
	COMMAND ${PROJECT_SOURCE_DIR}/assets/shaders/compile.sh
	COMMENT "Executing pre-built script..."
)

add_dependencies(engine compile-shaders)
