add_sources(libopenage
	buffer.cpp
	color.cpp
	context.cpp
	pipeline.cpp
	program.cpp
	renderer.cpp
	shader.cpp
	task.cpp
	tests.cpp
	text.cpp
	texture.cpp
	vertex_buffer.cpp
	window.cpp
)

pxdgen(
	tests.h
)

add_subdirectory(font/)
add_subdirectory(opengl/)
add_subdirectory(shaders/)
