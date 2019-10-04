# Copyright 2015-2019 the openage authors. See copying.md for legal info.

# Determines the version of the project
# stores a full version string with commit hash and number in the variable `PROJECT_VERSION`

# If the project version was already defined or previously retrieved, don't bother.
if(PROJECT_VERSION)
	message(STATUS "Project version already set")
	return()
endif()

# if .git exists, try running git describe
if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/.git")
	message(STATUS "Set PROJECT_VERSION from git.")
	execute_process(
		COMMAND git describe
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		RESULT_VARIABLE _RES
		OUTPUT_VARIABLE PROJECT_VERSION
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if(_RES EQUAL 0)
		set(USED_GIT_VERSION 1)
		return()
	endif()
endif()

# Fallback for downloaded zip's or git unavailability
set(PROJECT_VERSION_FILE "${CMAKE_SOURCE_DIR}/openage_version")
if(EXISTS ${PROJECT_VERSION_FILE})
	message(STATUS "Set PROJECT_VERSION from file.")
	file(STRINGS ${PROJECT_VERSION_FILE} FILE_DESCRIBE_VERSION)

	STRING(REGEX REPLACE "v([0-9]+\\.[0-9]+\\.[0-9]+)"
			"\\1" PROJECT_VERSION "${FILE_DESCRIBE_VERSION}")
endif()

# Still could not detect the version. Don't worry, raise a warning (shout a curse word?) and move on.
if(NOT PROJECT_VERSION)
	message(WARNING "Could not determine project version.")
	set(PROJECT_VERSION "0.0")
endif()
