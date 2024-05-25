# ImGui
add_library(ImGui STATIC)

target_sources(ImGui
PUBLIC
	imgui/imgui.h
	imgui/imconfig.h
	imgui/imgui_internal.h
	imgui/imstb_rectpack.h
	imgui/imstb_textedit.h
	imgui/imstb_truetype.h
	imgui/backends/imgui_impl_glfw.h
	imgui/backends/imgui_impl_vulkan.h
PRIVATE
	imgui/imgui.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/imgui_demo.cpp
	imgui/backends/imgui_impl_glfw.cpp
	imgui/backends/imgui_impl_vulkan.cpp
)

target_include_directories(ImGui
PUBLIC
	.
	imgui
)

target_link_libraries(ImGui
PRIVATE
	glfw
	Vulkan::Vulkan
)
