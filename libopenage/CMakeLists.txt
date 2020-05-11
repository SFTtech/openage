# Copyright 2014-2019 the openage authors. See copying.md for legal info.



#################################################################
# source files and folders are added at the bottom of this file #
#################################################################

# set basic library settings
set_target_properties(libopenage
		PROPERTIES
		VERSION 0
		AUTOMOC ON
		AUTOGEN_TARGET_DEPENDS "codegen"
)


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
# directories for header inclusion
target_include_directories(libopenage
		PUBLIC
			${CMAKE_CURRENT_BINARY_DIR}
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}
			${PLATFORM_FONT_INCLUDE_DIRS}
			${PLATFORM_SDL2_INCLUDE_DIRS}
			${PLATFORM_CALC_INCLUDE_DIRS}
			${PLATFORM_QT_INCLUDE_DIRS}
			${PLATFORM_AUDIO_INCLUDE_DIRS}
			${PLATFORM_GRAPHIC_INCLUDE_DIRS}
)

##################################################
# dependency linking

# all the libraries are not exposed to the
# public api of libopenage
target_link_libraries(libopenage
		PRIVATE
			Threads::Threads
			nyan::nyan
			PkgConfig::PLATFORM_SDL2
			PkgConfig::PLATFORM_CALC
			PkgConfig::PLATFORM_AUDIO
			PkgConfig::PLATFORM_GRAPHIC
			PkgConfig::PLATFORM_QT
			PkgConfig::PLATFORM_FONT
			${UTIL_LIB}
			${MATH_LIB}
			${RT_LIB}
			${CMAKE_DL_LIBS}
			${EXECINFO_LIB}
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
add_subdirectory("event")
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
add_subdirectory("versions")
