add_library(engine)

target_sources(engine
PUBLIC
	window.h
	model.h
	game_object.h
	camera.h
	keyboard_controller.h
	utils.h
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
	system/rainbow_system.h
PRIVATE
	window.cpp
	model.cpp
	game_object.cpp
	camera.cpp
	keyboard_controller.cpp
	vulkan_device.cpp
	vulkan_pipeline.cpp
	vulkan_swapchain.cpp
	vulkan_buffer.cpp
	vulkan_renderer.cpp
	vulkan_descriptors.cpp
	# System
	system/simple_render_system.cpp
	system/point_light_system.cpp
)

target_link_libraries(engine
PUBLIC
	glfw
	Vulkan::Vulkan
	core
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
