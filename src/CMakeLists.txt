include_directories(/usr/include/freetype2)
#link_directories(${CMAKEDEMO_BINARY_DIR}/src)

find_package(OpenGL)
find_package(GLEW)
#findpackage sdl2

add_executable(${PROJECT_NAME}
	log/log.cpp
	util/color.cpp
	util/error.cpp
	util/filetools.cpp
	util/fps.cpp
	util/misc.cpp
	util/strings.cpp
	util/timer.cpp
	openage.cpp
	engine/engine.cpp
	engine/font.cpp
	engine/console.cpp
	engine/terrain.cpp
	engine/texture.cpp
	engine/coordinates.cpp
	engine/shader/shader.cpp
	engine/shader/program.cpp
)

target_link_libraries(${PROJECT_NAME}
	m
	SDL2
	SDL2_image
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	ftgl
	fontconfig
)
