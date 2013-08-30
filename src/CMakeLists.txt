#include_directories(${CMAKEDEMO_SOURCE_DIR}/src)
#link_directories(${CMAKEDEMO_BINARY_DIR}/src)

find_package(OpenGL)
find_package(GLEW)
#findpackage sdl2

add_executable(${PROJECT_NAME}
	openage.cpp
	engine/engine.cpp
	engine/texture.cpp
	log/log.cpp
	util/fps.cpp
	util/filetools.cpp
	engine/shader.cpp
)

target_link_libraries(${PROJECT_NAME}
	m
	SDL2
	SDL2_image
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
)
