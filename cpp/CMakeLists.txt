# source file configuration
# for the resulting binary


# add new sources here, dependencies for linking and including
# are specified below the source file list.

declare_executable(${PROJECT_NAME})

add_sources(${PROJECT_NAME}
	main.cpp
	args.cpp

	game_main.cpp

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
add_subdirectory("audio")
add_subdirectory("console")
add_subdirectory("coord")
add_subdirectory("job")
add_subdirectory("pathfinding")
add_subdirectory("shader")
add_subdirectory("util")
add_subdirectory("datastructure")
add_subdirectory("testing")

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
find_package(Opusfile REQUIRED)

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
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

find_package(SDL2 REQUIRED)
find_package(FTGL REQUIRED)
find_package(SDL2Image REQUIRED)


##inotify support
#find_path(INOTIFY_INCLUDE_DIR sys/inotify.h  HINTS /usr/include/${CMAKE_LIBRARY_ARCHITECTURE})
#find_package_handle_standard_args(INOTIFY DEFAULT_MSG INOTIFY_INCLUDE_DIR)
##if(NOT INOTIFY_FOUND) ...

# directories for header inclusion
include_directories(
	${OPENGL_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${FTGL_INCLUDE_DIRS}
	${OPUSFILE_INCLUDE_DIR}
	${PYTHON_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${SDL2IMAGE_INCLUDE_DIRS}
)

# link the executable to those libraries
target_link_libraries(${PROJECT_NAME}
	${FONTCONFIG_LIB}
	${FREETYPE_LIBRARIES}
	${FTGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${MATH_LIB}
	${OPENGL_LIBRARY}
	${OPUSFILE_LIBRARIES}
	${PYTHON_LIBRARY}
	${SDL2IMAGE_LIBRARIES}
	${SDL2_LIBRARY}
	${UTIL_LIB}
)

# main binary installation
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
