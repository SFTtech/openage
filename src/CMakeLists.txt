#source file configuration
#
#


#add new sources here, dependencies for linking and including
#are specified beliw the source file list.

add_executable(${PROJECT_NAME}
	main.cpp
	args.cpp

	engine/callbacks.cpp
	engine/engine.cpp
	engine/font.cpp
	engine/init.cpp
	engine/input.cpp
	engine/log.cpp
	engine/python.cpp
	engine/audio/audio_manager.cpp
	engine/audio/category.cpp
	engine/audio/format.cpp
	engine/audio/loader_policy.cpp
	engine/audio/resource.cpp
	engine/audio/resource_loader.cpp
	engine/audio/sound.cpp
	engine/console/console.cpp
	engine/console/draw.cpp
	engine/coord/camgame.cpp
	engine/coord/camhud.cpp
	engine/coord/chunk.cpp
	engine/coord/phys2.cpp
	engine/coord/phys3.cpp
	engine/coord/term.cpp
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
	engine/util/fds.cpp
	engine/util/fps.cpp
	engine/util/misc.cpp
	engine/util/strings.cpp
	engine/util/timer.cpp
	engine/util/unicode.cpp
	engine/console/buf.cpp

	openage/callbacks.cpp
	openage/gamestate.cpp
	openage/init.cpp
	openage/main.cpp

	testing/testing.cpp
	testing/term.cpp
	testing/register.cpp
)


#add our generated files directory
add_subdirectory("gamedata")


#library dependency specification

#freetype includedir hint for ubuntu...
find_path(FREETYPE_INCLUDE_DIRS freetype/freetype.h HINTS /usr/include/freetype2)
find_path(OPUSFILE_INCLUDE_DIR opusfile.h HINTS /usr/include/opus)

include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

#windows does not have libm
if(NOT WIN32)
	find_library(MATH_LIB m)
endif()

find_library(FONTCONFIG_LIB fontconfig)
find_library(OPUSFILE_LIB opusfile)
find_library(UTIL_LIB util)

find_package(Freetype REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PythonLibs 3.3 REQUIRED)
pkg_search_module(FTGL REQUIRED ftgl)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_IMAGE REQUIRED sdl2_image SDL2_image)

##inotify support
#find_path(INOTIFY_INCLUDE_DIR sys/inotify.h  HINTS /usr/include/${CMAKE_LIBRARY_ARCHITECTURE})
#find_package_handle_standard_args(INOTIFY DEFAULT_MSG INOTIFY_INCLUDE_DIR)
##if(NOT INOTIFY_FOUND) ...

include_directories(
	${OPENGL_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${FTGL_INCLUDE_DIRS}
	${OPUSFILE_INCLUDE_DIR}
	${PYTHON_INCLUDE_DIR}
	${SDL2_INCLUDE_DIRS}
)
#link_directories(rofl/lib/lol)

target_link_libraries(${PROJECT_NAME}
	gamedata
	${FONTCONFIG_LIB}
	${FREETYPE_LIBRARIES}
	${FTGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${MATH_LIB}
	${OPENGL_LIBRARY}
	${OPUSFILE_LIB}
	${PYTHON_LIBRARY}
	${SDL2_IMAGE_LIBRARIES}
	${SDL2_LIBRARIES}
	${UTIL_LIB}
)


#TODO: install shaders, data search paths

#installation definitions
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
