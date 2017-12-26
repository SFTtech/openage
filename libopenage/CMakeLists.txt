# source file configuration
# for the resulting binary


# add new sources here, dependencies for linking and including
# are specified below the source file list.

declare_binary(libopenage)


add_sources(libopenage
	assetmanager.cpp
	engine.cpp
	game_control.cpp
	game_renderer.cpp
	handlers.cpp
	main.cpp
	options.cpp
	screenshot.cpp
	texture.cpp
	config.cpp
)

pxdgen(
	main.h
	engine.h
)


# add subsystem folders
add_subdirectory("audio")
add_subdirectory("console")
add_subdirectory("coord")
add_subdirectory("cvar")
add_subdirectory("datastructure")
add_subdirectory("gui")
add_subdirectory("error")
add_subdirectory("gamestate")
add_subdirectory("input")
add_subdirectory("log")
add_subdirectory("job")
add_subdirectory("pathfinding")
add_subdirectory("pyinterface")
add_subdirectory("renderer")
add_subdirectory("rng")
add_subdirectory("simulation")
add_subdirectory("shader")
add_subdirectory("terrain")
add_subdirectory("testing")
add_subdirectory("unit")
add_subdirectory("util")


# run codegen, add files to executable
codegen_run()
add_sources(libopenage GENERATED ${CODEGEN_TARGET_TUS})

# after this point, no further sources can be added to the executable
finalize_binary(libopenage openage library allow_no_undefined)
set_target_properties(libopenage PROPERTIES
	VERSION 0
	AUTOMOC ON
	AUTOGEN_TARGET_DEPENDS "codegen"
)

# library dependency specification

# freetype includedir hint for ubuntu...
find_path(FREETYPE_INCLUDE_DIRS freetype/freetype.h HINTS /usr/include/freetype2)
include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

# provide apple qt5 location
if(APPLE)
	list(APPEND CMAKE_PREFIX_PATH /usr/local/opt/qt5)
endif()

# windows does not have libm
if(NOT WIN32)
	find_library(MATH_LIB m)
	find_library(UTIL_LIB util)
endif()
if(MSVC)
	find_library(OGG_LIB ogg)
endif()
if(NOT APPLE AND NOT MSVC)
	find_library(RT_LIB rt)
	if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
		find_library(EXECINFO_LIB execinfo)
	endif()
endif()

find_library(FONTCONFIG_LIB fontconfig)

find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2Image REQUIRED)
find_package(Opusfile REQUIRED)
find_package(Epoxy REQUIRED)
find_package(HarfBuzz 1.0.0 REQUIRED)

set(QT_VERSION_REQ "5.5")
find_package(Qt5Core ${QT_VERSION_REQ} REQUIRED)
find_package(Qt5Quick ${QT_VERSION_REQ} REQUIRED)

if(WANT_BACKTRACE)
	find_package(GCCBacktrace)
endif()

#######################################
# nyan integration

# first, try to locate nyan directly
# this discovers the system package or the user-registry package
find_package(nyan CONFIG)

# if this didn't work, we can download nyan like a git submodule.
# this is the treeish to be checked out.
set(nyan_clone_version origin/master)

option(
	DOWNLOAD_NYAN
	"whether to clone the nyan project in case it is not found"
	OFF
)

option(
	FORCE_DOWNLOAD_NYAN
	"Force the download and usage of the nyan project"
	OFF
)

option(
	DISABLE_SUBPROJECT_UPDATES
	"Disable the automatic update of subprojects over the internet"
	OFF
)

# if nyan was not found, consider downloading it as subproject
# only use the subproject mode if it was requested
# or if it was used before.
if(NOT nyan_FOUND OR FORCE_DOWNLOAD_NYAN OR NYAN_WAS_DOWNLOADED)
	if(DOWNLOAD_NYAN OR FORCE_DOWNLOAD_NYAN OR NYAN_WAS_DOWNLOADED)

		message(STATUS "Processing nyan as submodule project...")

		fetch_project(
			NAME nyan
			DISABLE_UPDATES ${DISABLE_SUBPROJECT_UPDATES}
			GIT_REPOSITORY https://github.com/SFTtech/nyan
			GIT_TAG ${nyan_clone_version}
		)

		# don't register nyan to the userpackage-repo!
		set(REGISTER_USERPACKAGE OFF)
		# don't generate the `doc` target again (name conflict!)
		set(DOXYGEN_ENABLE OFF)

		# register the targets
		add_subdirectory(${nyan_SOURCE_DIR} ${nyan_BINARY_DIR})

		message(STATUS "nyan processed successfully!")
		set(NYAN_WAS_DOWNLOADED TRUE CACHE INTERNAL
		    "nyan subproject was used")

	else()
		message(FATAL_ERROR "
  Could not find the cmake package configuration file \"nyanConfig.cmake\".
  To find it, you have several options:
  * If your distribution provides it, install \"nyan\" through the package manager.
  * If you want openage to automatically download \"nyan\", append `-DDOWNLOAD_NYAN=YES` to the cmake invocation or use `./configure --download-nyan`.
  * If you want to build nyan manually, follow the build instructions:
      [[  doc/building.md#nyan-installation  ]]
  * If you already built nyan but it still can't be found (cmake package repo fails):
    * Try to set \"nyan_DIR\" to the nyan build directory (it contains nyanConfig.cmake)
      either through:  \"./configure $youroptions -- -Dnyan_DIR=/home/dev/nyan/build\"
      or:              \"cmake $yourotheroptions -Dnyan_DIR=/home/dev/nyan/build ..\"

  In case of other problems, please try to figure them out (and tell us what you did).
  Contact information is in README.md.
")
	endif()
endif()


if(GCCBacktrace_FOUND)
	include_directories(${GCCBacktrace_INCLUDE_DIRS})
	target_link_libraries(libopenage PUBLIC "${GCCBacktrace_LIBRARIES}")
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
	target_link_libraries(libopenage PUBLIC ${GPERFTOOLS_PROFILER})
else()
	have_config_option(gperftools-profiler GPERFTOOLS_PROFILER false)
endif()

if(WITH_GPERFTOOLS_TCMALLOC AND GPERFTOOLS_FOUND)
	have_config_option(gperftools-tcmalloc GPERFTOOLS_TCMALLOC true)
	include_directories(${GPERFTOOLS_INCLUDE_DIR})
	target_link_libraries(libopenage PUBLIC ${GPERFTOOLS_TCMALLOC})
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

if(APPLE)
	find_path(QTPLATFORM_INCLUDE_DIRS QtPlatformHeaders "/usr/local/opt/qt5/include")
endif()

# directories for header inclusion
include_directories(
	${OPENGL_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${OPUS_INCLUDE_DIRS}
	${PNG_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIR}
	${SDL2IMAGE_INCLUDE_DIRS}
	${HarfBuzz_INCLUDE_DIRS}
	${QTPLATFORM_INCLUDE_DIRS}
)

# link the executable to those libraries
target_link_libraries(libopenage
	PUBLIC
		${CMAKE_DL_LIBS}
		${FONTCONFIG_LIB}
		${FREETYPE_LIBRARIES}
		${EPOXY_LIBRARIES}
		${MATH_LIB}
		${OPENGL_LIBRARY}
		${OPUS_LIBRARIES}
		${PNG_LIBRARIES}
		${SDL2IMAGE_LIBRARIES}
		${SDL2_LIBRARY}
		${UTIL_LIB}
		${HarfBuzz_LIBRARIES}
		${RT_LIB}
		${OGG_LIB}
		${EXECINFO_LIB}
		nyan::nyan

	# TODO: change to PUBLIC (or, alternatively, remove all keywords
	# of this type) when qt cmake scripts change declarations of the
	# IMPORTED libraries to GLOBAL.
	PRIVATE
		Qt5::Core
		Qt5::Quick
)

install(TARGETS libopenage
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
