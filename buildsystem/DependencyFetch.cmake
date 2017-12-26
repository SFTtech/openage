# Copyright 2017-2017 the openage authors. See copying.md for legal info.

# Fetch project with given name from the Internet.
# Basically wraps ExternalProject and does a nested cmake invocation.
#
# Usage: fetch_project(NAME ${projectname} ${locationspecification...})
#
# sets in caller scope:
#   ${projectname}_SOURCE_DIR
#   ${projectname}_BINARY_DIR
#
# You need to specify the location with options of
# ExternalProject_Add, for example:
#
# fetch_project(
#     NAME yourmom
#     GIT_REPOSITORY https://github.com/nevergonna/giveyouup
#     GIT_TAG origin/master
# )
#
function(fetch_project)
	cmake_parse_arguments(PROJ "" "NAME;DISABLE_UPDATES" "" ${ARGN})

	if(PROJ_NAME STREQUAL "")
		message(FATAL_ERROR "no project name given")
	endif()

	set(PROJ_DIR "${CMAKE_BINARY_DIR}/${PROJ_NAME}-external")
	set(PROJ_DL_DIR "${PROJ_DIR}/dl")
	set(PROJ_SRC_DIR "${PROJ_DIR}/source")
	set(PROJ_BIN_DIR "${PROJ_DIR}/bin")
	set(PROJ_STAMP_DIR "${PROJ_DIR}/stamp")

	# CLion creates this file for multiple projects
	file(REMOVE "${PROJ_DL_DIR}/CMakeCache.txt")

	# create the ExternalProject configuration file
	# for the nested cmake call.
	configure_file(
		"${BUILDSYSTEM_DIR}/templates/ExternalFetch.cmake.in"
		"${PROJ_DL_DIR}/CMakeLists.txt"
		@ONLY
	)

	# run cmake to "configure" the external project.
	# this prepares the download that will be done in the next step.
	execute_process(COMMAND
		${CMAKE_COMMAND}
		"-G${CMAKE_GENERATOR}"
		"-DCMAKE_MAKE_PROGRAM:FILE=${CMAKE_MAKE_PROGRAM}"
		.
		OUTPUT_QUIET
		RESULT_VARIABLE config_ok
		WORKING_DIRECTORY "${PROJ_DL_DIR}"
	)

	if(config_ok)
		message(FATAL_ERROR "Failed to set up external project ${PROJ_NAME}")
	endif()

	# Now actually download the project.
	# This does not "build" it! The "build" is the download execution.
	execute_process(COMMAND
		${CMAKE_COMMAND} --build .
		OUTPUT_QUIET
		RESULT_VARIABLE download_ok
		WORKING_DIRECTORY "${PROJ_DL_DIR}"
	)

	if(download_ok)
		message(FATAL_ERROR "Failed to download external project ${PROJ_NAME}.")
	endif()

	set(${PROJ_NAME}_SOURCE_DIR "${PROJ_SRC_DIR}" PARENT_SCOPE)
	set(${PROJ_NAME}_BINARY_DIR "${PROJ_BIN_DIR}" PARENT_SCOPE)
endfunction()
