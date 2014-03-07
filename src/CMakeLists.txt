#source file configuration
#
#


#add new sources here, dependencies for linking and including
#are specified beliw the source file list.

add_executable(${PROJECT_NAME}
	main.cpp
	openage/main.cpp
	openage/init.cpp
	openage/callbacks.cpp
	openage/gamestate.cpp
	openage/objects/building.cpp
	openage/objects/terrain.cpp
	engine/engine.cpp
	engine/callbacks.cpp
	engine/init.cpp
	engine/font.cpp
	engine/terrain.cpp
	engine/terrain_chunk.cpp
	engine/terrain_object.cpp
	engine/texture.cpp
	engine/input.cpp
	engine/shader/shader.cpp
	engine/shader/program.cpp
	engine/coord/chunk.cpp
	engine/coord/tile.cpp
	engine/coord/tile3.cpp
	engine/coord/phys2.cpp
	engine/coord/phys3.cpp
	engine/coord/camgame.cpp
	engine/coord/camhud.cpp
	engine/coord/window.cpp
	engine/coord/vec2.cpp
	engine/coord/vec2f.cpp
	engine/coord/vec3.cpp
	engine/coord/vec3f.cpp
	engine/log.cpp
	engine/util/color.cpp
	engine/util/error.cpp
	engine/util/file.cpp
	engine/util/fps.cpp
	engine/util/misc.cpp
	engine/util/strings.cpp
	engine/util/timer.cpp
	engine/util/unicode.cpp
	engine/console/callbacks.cpp
	engine/console/console.cpp
	engine/console/init.cpp
)


#add our generated files directory
add_subdirectory("gamedata")


#library dependency specification

set(FREETYPE_INCLUDE_DIRS "/usr/include/freetype2")

include(FindPkgConfig)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_library(FONTCONFIG_LIB fontconfig)
find_library(MATH_LIB m)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_IMAGE REQUIRED sdl2_image SDL2_image)
pkg_search_module(FTGL REQUIRED ftgl)

include_directories(
	${SDL2_INCLUDE_DIRS}
	${FTGL_INCLUDE_DIRS}
	${FREETYPE_INCLUDE_DIRS}
)
#link_directories(rofl/lib/lol)

target_link_libraries(${PROJECT_NAME}
	gamedata
	${MATH_LIB}
	${SDL2_LIBRARIES}
	${SDL2_IMAGE_LIBRARIES}
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${FTGL_LIBRARIES}
	${FONTCONFIG_LIB}
	${FREETYPE_LIBRARIES}
)


#TODO: install shaders, data search paths

#installation definitions
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
