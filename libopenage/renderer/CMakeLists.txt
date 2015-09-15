add_sources(libopenage
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
	window.cpp
)

pxdgen(
	tests.h
)

add_subdirectory(font/)
add_subdirectory(opengl/)
add_subdirectory(shaders/)
