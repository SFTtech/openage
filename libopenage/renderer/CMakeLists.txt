add_sources(libopenage
	animation.cpp
	color.cpp
    definitions.cpp
	geometry.cpp
	render_factory.cpp
	renderer.cpp
	shader_program.cpp
	text.cpp
	texture.cpp
	texture_array.cpp
	types.cpp
    uniform_buffer.cpp
	uniform_input.cpp
	util.cpp
	window.cpp
	window_event_handler.cpp
)

add_subdirectory(camera/)
add_subdirectory(demo/)
add_subdirectory(font/)
add_subdirectory(gui/)
add_subdirectory(resources/)
add_subdirectory(stages/)

if(OPENGL_FOUND)
    add_subdirectory(opengl/)
endif()

if(VULKAN_FOUND)
    add_subdirectory(vulkan/)
endif()
