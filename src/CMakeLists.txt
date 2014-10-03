# source file configuration
# for the resulting binary


# add new sources here, dependencies for linking and including
# are specified below the source file list.


create_binary(${PROJECT_NAME})

add_sources(${PROJECT_NAME}
	main.cpp
	args.cpp

	callbacks.cpp
	engine.cpp
	font.cpp
	handlers.cpp
	input.cpp
	log.cpp
	python.cpp
	terrain.cpp
	terrain_chunk.cpp
	terrain_object.cpp
	texture.cpp
)


# add subsystem folders
add_subdirectory("gamedata")
add_subdirectory("audio")
add_subdirectory("console")
add_subdirectory("coord")
add_subdirectory("job")
add_subdirectory("pathfinding")
add_subdirectory("shader")
add_subdirectory("util")
add_subdirectory("testing")

# define the resulting executable by all sources gathered
define_executable(${PROJECT_NAME})

# library dependency specification

# freetype includedir hint for ubuntu...
find_path(FREETYPE_INCLUDE_DIRS freetype/freetype.h HINTS /usr/include/freetype2)
find_path(OPUSFILE_INCLUDE_DIR opusfile.h HINTS /usr/include/opus)

include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

# windows does not have libm
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
