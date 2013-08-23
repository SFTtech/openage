include_directories(${CMAKEDEMO_SOURCE_DIR}/src)
link_directories(${CMAKEDEMO_BINARY_DIR}/src)

find_package(OpenGL)
#findpackage sdl2

add_executable(game
	game.cpp
	engine/engine.cpp
	engine/texture.cpp
)

target_link_libraries(game
	m
	SDL2
	${OPENGL_LIBRARY}
)
