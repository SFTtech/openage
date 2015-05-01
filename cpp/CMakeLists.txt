# source file configuration
# for the resulting binary


# add new sources here, dependencies for linking and including
# are specified below the source file list.

declare_executable(${PROJECT_NAME})

add_sources(${PROJECT_NAME}
	main.cpp
	args.cpp
	game_main.cpp
	game_save.cpp
	assetmanager.cpp
	callbacks.cpp
	datamanager.cpp
	engine.cpp
	font.cpp
	handlers.cpp
	player.cpp
	python.cpp
	screenshot.cpp
	texture.cpp

	config.cpp
)


# add subsystem folders
add_subdirectory("audio")
add_subdirectory("console")
add_subdirectory("coord")
add_subdirectory("crossplatform")
add_subdirectory("datastructure")
add_subdirectory("log")
add_subdirectory("mapgen")
add_subdirectory("job")
add_subdirectory("keybinds")
add_subdirectory("pathfinding")
add_subdirectory("shader")
add_subdirectory("terrain")
add_subdirectory("testing")
add_subdirectory("unit")
add_subdirectory("util")
add_subdirectory("rng")

# finalize testing
finalize_tests(cpp)

# run codegen, add files to executable
codegen_run()
add_sources(${PROJECT_NAME} GENERATED ${CODEGEN_TARGET_TUS})

# after this point, no further sources can be added to the executable
finalize_executable(${PROJECT_NAME})

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

if(WANT_GPERFTOOLS_PROFILER OR WANT_GPERFTOOLS_TCMALLOC)
	find_package(GPerfTools)
endif()

if(WANT_GPERFTOOLS_PROFILER AND GPERFTOOLS_FOUND)
	have_config_option(gperftools-profiler GPERFTOOLS_PROFILER true)
	include_directories(${GPERFTOOLS_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${GPERFTOOLS_PROFILER})
else()
	have_config_option(gperftools-profiler GPERFTOOLS_PROFILER false)
endif()

if(WITH_GPERFTOOLS_TCMALLOC AND GPERFTOOLS_FOUND)
	have_config_option(gperftools-tcmalloc GPERFTOOLS_TCMALLOC true)
	include_directories(${GPERFTOOLS_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${GPERFTOOLS_TCMALLOC})
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

configure_file(config.h.in ${CMAKE_SOURCE_DIR}/cpp/config.h)
configure_file(config.cpp.in ${CMAKE_SOURCE_DIR}/cpp/config.cpp)

# directories for header inclusion
include_directories(
	${OPENGL_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${FTGL_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${OPUS_INCLUDE_DIRS}
	${PYTHON_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${SDL2IMAGE_INCLUDE_DIRS}
)

# link the executable to those libraries
target_link_libraries(${PROJECT_NAME}
	${FONTCONFIG_LIB}
	${FREETYPE_LIBRARIES}
	${FTGL_LIBRARIES}
	${EPOXY_LIBRARIES}
	${MATH_LIB}
	${OPENGL_LIBRARY}
	${OPUS_LIBRARIES}
	${PYTHON_LIBRARY}
	${SDL2IMAGE_LIBRARIES}
	${SDL2_LIBRARY}
	${UTIL_LIB}
)

# main binary installation
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
