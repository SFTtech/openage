# Copyright 2014-2019 the openage authors. See copying.md for legal info.

# main C++ library definitions.
# dependency and source file setup for the resulting library.
declare_binary(libopenage openage library allow_no_undefined)


#################################################################
# source files and folders are added at the bottom of this file #
#################################################################

# set basic library settings
set_target_properties(libopenage PROPERTIES
	VERSION 0
	AUTOMOC ON
	AUTOGEN_TARGET_DEPENDS "codegen"
)

##################################################
# library dependency specification

# freetype includedir hint for ubuntu...
find_path(FREETYPE_INCLUDE_DIRS freetype/freetype.h HINTS /usr/include/freetype2)

# provide apple qt5 location
if(APPLE)
	list(APPEND CMAKE_PREFIX_PATH /usr/local/opt/qt5)
endif()

# windows does not have libm
if(NOT WIN32)
	find_library(MATH_LIB m)
	find_library(UTIL_LIB util)
endif()
if(WIN32)
	find_library(OGG_LIB ogg)
	target_link_libraries(libopenage PRIVATE DbgHelp)
endif()
if(NOT APPLE AND NOT WIN32)
	find_library(RT_LIB rt)
	if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
		find_library(EXECINFO_LIB execinfo)
	endif()
endif()

find_library(FONTCONFIG_LIB fontconfig)

find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2Image REQUIRED)
find_package(Opusfile REQUIRED)
find_package(Epoxy REQUIRED)
find_package(HarfBuzz 1.0.0 REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

set(QT_VERSION_REQ "5.9")
find_package(Qt5Core ${QT_VERSION_REQ} REQUIRED)
find_package(Qt5Quick ${QT_VERSION_REQ} REQUIRED)

if(WANT_BACKTRACE)
	find_package(GCCBacktrace)
endif()

##################################################
# nyan integration

# first, try to locate nyan directly
# this discovers the system package or the user-registry package
find_package(nyan CONFIG)

# if this didn't work, we can download nyan like a git submodule.
# this is the treeish to be checked out.
if(NOT DEFINED NYAN_CLONE_VERSION)
	set(NYAN_CLONE_VERSION origin/master)
endif()

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
if((NOT nyan_FOUND AND DOWNLOAD_NYAN) OR FORCE_DOWNLOAD_NYAN)
	message(STATUS "Downloading nyan as submodule project...")

	if(DISABLE_SUBPROJECT_UPDATES)
		set(DISABLE_NYAN_UPDATES "DISABLE_UPDATES")
	endif()

	fetch_project(
		NAME nyan
		${DISABLE_NYAN_UPDATES}
		GIT_REPOSITORY https://github.com/SFTtech/nyan
		GIT_TAG ${NYAN_CLONE_VERSION}
	)

	# don't register nyan to the userpackage-repo!
	set(REGISTER_USERPACKAGE OFF)
	# don't generate the `doc` target again (name conflict!)
	set(DOXYGEN_ENABLE OFF)

	# register the targets
	add_subdirectory(${nyan_SOURCE_DIR} ${nyan_BINARY_DIR})

	message(STATUS "nyan processed successfully!")

elseif(NOT nyan_FOUND)
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


##################################################
# optional dependencies

# advanced stacktraces with libbacktrace from gcc
if(GCCBacktrace_FOUND)
	target_include_directories(libopenage PRIVATE ${GCCBacktrace_INCLUDE_DIRS})
	target_link_libraries(libopenage PRIVATE "${GCCBacktrace_LIBRARIES}")
	have_config_option(backtrace BACKTRACE true)
else()
	have_config_option(backtrace BACKTRACE false)
endif()

# google performance tools
if(WANT_GPERFTOOLS_PROFILER OR WANT_GPERFTOOLS_TCMALLOC)
	find_package(GPerfTools)
endif()

if(WANT_GPERFTOOLS_PROFILER AND GPERFTOOLS_FOUND)
	have_config_option(gperftools-profiler GPERFTOOLS_PROFILER true)
	target_include_directories(libopenage PRIVATE ${GPERFTOOLS_INCLUDE_DIR})
	target_link_libraries(libopenage PRIVATE ${GPERFTOOLS_PROFILER})
else()
	have_config_option(gperftools-profiler GPERFTOOLS_PROFILER false)
endif()

if(WITH_GPERFTOOLS_TCMALLOC AND GPERFTOOLS_FOUND)
	have_config_option(gperftools-tcmalloc GPERFTOOLS_TCMALLOC true)
	target_include_directories(libopenage PRIVATE ${GPERFTOOLS_INCLUDE_DIR})
	target_link_libraries(libopenage PRIVATE ${GPERFTOOLS_TCMALLOC})
else()
	have_config_option(gperftools-tcmalloc GPERFTOOLS_TCMALLOC false)
endif()

# inotify support
if(WANT_INOTIFY)
	find_package(Inotify)
endif()

if(WANT_INOTIFY AND INOTIFY_FOUND)
	have_config_option(inotify INOTIFY true)
	target_include_directories(libopenage PRIVATE ${INOTIFY_INCLUDE_DIRS})
else()
	have_config_option(inotify INOTIFY false)
endif()

# ncurses support
if(WANT_NCURSES)
	set(CURSES_NEED_NCURSES TRUE)
	find_package(Curses)
endif()

if(WANT_NCURSES AND CURSES_FOUND)
	have_config_option(ncurses NCURSES true)
	target_include_directories(libopenage PRIVATE ${Curses_INCLUDE_DIRS})
	target_link_libraries(libopenage PRIVATE ${CURSES_LIBRARIES})
else()
	have_config_option(ncurses NCURSES false)
endif()

# opengl support
if(WANT_OPENGL)
	find_package(OpenGL)
endif()

# vulkan support
if(WANT_VULKAN)
	find_package(Vulkan)
endif()

if(WANT_OPENGL AND OPENGL_FOUND)
	have_config_option(opengl OPENGL true)
	target_link_libraries(libopenage PRIVATE OpenGL::GL)
else()
	have_config_option(opengl OPENGL false)
endif()

if(WANT_VULKAN AND VULKAN_FOUND)
	have_config_option(vulkan VULKAN true)
	target_link_libraries(libopenage PRIVATE Vulkan::Vulkan)
else()
	have_config_option(vulkan VULKAN false)
endif()

if(NOT (OPENGL_FOUND OR VULKAN_FOUND))
	message(FATAL_ERROR "One of OpenGL or Vulkan is required!")
endif()

##################################################
# build configuration generation
get_config_option_string()

configure_file(config.h.in config.h)
configure_file(config.cpp.in config.cpp)

configure_file(
	"${CMAKE_SOURCE_DIR}/openage/config.py.in"
	"${CMAKE_BINARY_DIR}/openage/config.py"
)


##################################################
# platform specifics
if(APPLE)
	find_path(QTPLATFORM_INCLUDE_DIRS QtPlatformHeaders "/usr/local/opt/qt5/include")
endif()


##################################################
# directories for header inclusion
target_include_directories(libopenage
	PUBLIC
	${CMAKE_CURRENT_BINARY_DIR}
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${EPOXY_INCLUDE_DIRS}
	${OPUS_INCLUDE_DIRS}
	${PNG_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIR}
	${SDL2IMAGE_INCLUDE_DIRS}
	${HarfBuzz_INCLUDE_DIRS}
	${QTPLATFORM_INCLUDE_DIRS}
)

##################################################
# dependency linking

# all the libraries are not exposed
# to the public api of libopenage
target_link_libraries(libopenage
	PRIVATE
		Threads::Threads
		nyan::nyan
		Eigen3::Eigen
		${PNG_LIBRARIES}
		${OPUS_LIBRARIES}
		${OGG_LIB}

		${CMAKE_DL_LIBS}
		${FONTCONFIG_LIB}
		${FREETYPE_LIBRARIES}
		${EPOXY_LIBRARIES}
		${MATH_LIB}
		${SDL2IMAGE_LIBRARIES}
		${SDL2_LIBRARY}
		${UTIL_LIB}
		${HarfBuzz_LIBRARIES}
		${RT_LIB}
		${EXECINFO_LIB}
		Qt5::Core
		Qt5::Quick
)

##################################################
# installation of the library
install(TARGETS libopenage
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(WIN32)
	install(FILES $<TARGET_FILE:nyan::nyan>
		DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()


##################################################
# source file definitions
get_codegen_scu_file()

# add new sources here, dependencies for linking and including
# are specified above the source file list.

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
	${CMAKE_CURRENT_BINARY_DIR}/config.cpp
	${CODEGEN_SCU_FILE}
)

pxdgen(
	main.h
	engine.h
)

# add subsystem folders
add_subdirectory("audio")
add_subdirectory("console")
add_subdirectory("coord")
add_subdirectory("curve")
add_subdirectory("cvar")
add_subdirectory("datastructure")
add_subdirectory("error")
add_subdirectory("event")
add_subdirectory("gamestate")
add_subdirectory("gui")
add_subdirectory("input")
add_subdirectory("job")
add_subdirectory("log")
add_subdirectory("main")
add_subdirectory("pathfinding")
add_subdirectory("presenter")
add_subdirectory("pyinterface")
add_subdirectory("renderer")
add_subdirectory("rng")
add_subdirectory("shader")
add_subdirectory("simulation")
add_subdirectory("terrain")
add_subdirectory("testing")
add_subdirectory("unit")
add_subdirectory("util")
