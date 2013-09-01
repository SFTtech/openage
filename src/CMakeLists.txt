#include_directories(${CMAKEDEMO_SOURCE_DIR}/src)
#link_directories(${CMAKEDEMO_BINARY_DIR}/src)

find_package(OpenGL)
find_package(GLEW)
#findpackage sdl2

add_executable(${PROJECT_NAME}
	log/log.cpp
	util/format.cpp
	util/error.cpp
	util/fps.cpp
	util/filetools.cpp
	openage.cpp
	engine/engine.cpp
	engine/texture.cpp
	engine/shader/shader.cpp
	engine/shader/program.cpp
)

target_link_libraries(${PROJECT_NAME}
	m
	SDL2
	SDL2_image
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
)
