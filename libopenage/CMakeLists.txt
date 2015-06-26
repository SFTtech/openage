# source file configuration
# for the resulting binary


# add new sources here, dependencies for linking and including
# are specified below the source file list.

declare_binary(libopenage)

add_sources(libopenage
	assetmanager.cpp
	callbacks.cpp
	engine.cpp
	font.cpp
	game_control.cpp
	game_main.cpp
	game_renderer.cpp
	game_save.cpp
	game_spec.cpp
	handlers.cpp
	main.cpp
	player.cpp
	screenshot.cpp
	texture.cpp

	config.cpp
)

pxdgen(
	main.h
)


# add subsystem folders
add_subdirectory("audio")
add_subdirectory("console")
add_subdirectory("coord")
add_subdirectory("crossplatform")
add_subdirectory("datastructure")
add_subdirectory("error")
add_subdirectory("log")
add_subdirectory("job")
add_subdirectory("keybinds")
add_subdirectory("pathfinding")
add_subdirectory("pyinterface")
add_subdirectory("shader")
add_subdirectory("terrain")
add_subdirectory("testing")
add_subdirectory("unit")
add_subdirectory("util")
add_subdirectory("rng")

# run codegen, add files to executable
codegen_run()
add_sources(libopenage GENERATED ${CODEGEN_TARGET_TUS})

# after this point, no further sources can be added to the executable
finalize_binary(libopenage openage library allow_no_undefined)

# library dependency specification

# freetype includedir hint for ubuntu...
find_path(FREETYPE_INCLUDE_DIRS freetype/freetype.h HINTS /usr/include/freetype2)
include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

# windows does not have libm
if(NOT WIN32)
	find_library(MATH_LIB m)
endif()

find_library(FONTCONFIG_LIB fontconfig)
if(NOT WIN32)
	find_library(UTIL_LIB util)
endif()

find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(FTGL REQUIRED)
find_package(SDL2Image REQUIRED)
find_package(Opusfile REQUIRED)
pkg_search_module(EPOXY REQUIRED epoxy)

if(WANT_BACKTRACE)
	find_package(Backtrace)
endif()

if(BACKTRACE_FOUND)
	target_link_libraries(libopenage "${BACKTRACE_LIBRARY}")
	have_config_option(backtrace BACKTRACE true)
else()
	have_config_option(backtrace BACKTRACE false)
endif()

if(WANT_GPERFTOOLS_PROFILER OR WANT_GPERFTOOLS_TCMALLOC)
	find_package(GPerfTools)
endif()

if(WANT_GPERFTOOLS_PROFILER AND GPERFTOOLS_FOUND)
	have_config_option(gperftools-profiler GPERFTOOLS_PROFILER true)
	include_directories(${GPERFTOOLS_INCLUDE_DIR})
	target_link_libraries(libopenage ${GPERFTOOLS_PROFILER})
else()
	have_config_option(gperftools-profiler GPERFTOOLS_PROFILER false)
endif()

if(WITH_GPERFTOOLS_TCMALLOC AND GPERFTOOLS_FOUND)
	have_config_option(gperftools-tcmalloc GPERFTOOLS_TCMALLOC true)
	include_directories(${GPERFTOOLS_INCLUDE_DIR})
	target_link_libraries(libopenage ${GPERFTOOLS_TCMALLOC})
else()
	have_config_option(gperftools-tcmalloc GPERFTOOLS_TCMALLOC false)
endif()

# inotify support
if(WANT_INOTIFY)
	find_package(Inotify)
endif()

if(WANT_INOTIFY AND INOTIFY_FOUND)
	have_config_option(inotify INOTIFY true)
	include_directories(${INOTIFY_INCLUDE_DIR})
else()
	have_config_option(inotify INOTIFY false)
endif()

get_config_option_string()

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
configure_file(config.cpp.in ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp)

# directories for header inclusion
include_directories(
	${OPENGL_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${FTGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${OPUS_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIR}
	${SDL2IMAGE_INCLUDE_DIRS}
)

# link the executable to those libraries
target_link_libraries(libopenage
	dl
	rt
	${FONTCONFIG_LIB}
	${FREETYPE_LIBRARIES}
	${FTGL_LIBRARIES}
	${EPOXY_LIBRARIES}
	${MATH_LIB}
	${OPENGL_LIBRARY}
	${OPUS_LIBRARIES}
	${SDL2IMAGE_LIBRARIES}
	${SDL2_LIBRARY}
	${UTIL_LIB}
)

install(TARGETS libopenage LIBRARY DESTINATION lib)
