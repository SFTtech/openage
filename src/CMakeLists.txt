include_directories(${CMAKEDEMO_SOURCE_DIR}/src)
link_directories(${CMAKEDEMO_BINARY_DIR}/src)

find_package(OpenGL)
#findpackage sdl2

add_executable(${PROJECT_NAME}
	main.cpp
	texture.cpp
)

target_link_libraries(${PROJECT_NAME}
	m
	SDL2
	${OPENGL_LIBRARY}
)
