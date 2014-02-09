#source file configuration
#
#add new sources here, dependencies for linking and including
#are specified beliw the source file list.

add_executable(${PROJECT_NAME}
	main.cpp

	engine/callbacks.cpp
	engine/engine.cpp
	engine/font.cpp
	engine/init.cpp
	engine/input.cpp
	engine/log.cpp
	engine/python.cpp
	engine/console/callbacks.cpp
	engine/console/console.cpp
	engine/console/init.cpp
	engine/coord/camgame.cpp
	engine/coord/camhud.cpp
	engine/coord/chunk.cpp
	engine/coord/phys2.cpp
	engine/coord/phys3.cpp
	engine/coord/tile.cpp
	engine/coord/tile3.cpp
	engine/coord/vec2.cpp
	engine/coord/vec2f.cpp
	engine/coord/vec3.cpp
	engine/coord/vec3f.cpp
	engine/coord/window.cpp
	engine/shader/program.cpp
	engine/shader/shader.cpp
	engine/terrain.cpp
	engine/terrain_chunk.cpp
	engine/terrain_object.cpp
	engine/texture.cpp
	engine/util/color.cpp
	engine/util/error.cpp
	engine/util/file.cpp
	engine/util/fps.cpp
	engine/util/misc.cpp
	engine/util/strings.cpp
	engine/util/timer.cpp
	engine/util/unicode.cpp

	openage/callbacks.cpp
	openage/gamestate.cpp
	openage/init.cpp
	openage/main.cpp
	openage/objects/building.cpp
	openage/objects/terrain.cpp

	test/metatest.cpp
	test/testing.cpp
)


#library dependency specification

set(FREETYPE_INCLUDE_DIRS "/usr/include/freetype2")

include(FindPkgConfig)
find_library(FONTCONFIG_LIB fontconfig)
find_library(MATH_LIB m)
find_package(Freetype REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PythonLibs 3.3 REQUIRED)
pkg_search_module(FTGL REQUIRED ftgl)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_IMAGE REQUIRED sdl2_image SDL2_image)

include_directories(
	${FREETYPE_INCLUDE_DIRS}
	${FTGL_INCLUDE_DIRS}
	${PYTHON_INCLUDE_DIR}
	${SDL2_INCLUDE_DIRS}
)
#link_directories(rofl/lib/lol)

target_link_libraries(${PROJECT_NAME}
	${FONTCONFIG_LIB}
	${FREETYPE_LIBRARIES}
	${FTGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${MATH_LIB}
	${OPENGL_LIBRARY}
	${PYTHON_LIBRARY}
	${SDL2_IMAGE_LIBRARIES}
	${SDL2_LIBRARIES}
)

#installation procedure
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
