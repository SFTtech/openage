# Copyright 2015-2015 the openage authors. See copying.md for legal info.

# Determines the version of the project and stores it in the variable PROJECT_VERSION.

# If the project version was already defined or previously retrieved, don't bother.
if(PROJECT_VERSION)
	return()
endif()

if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/.git")
	# if .git exists, try running git describe
	execute_process(
		COMMAND git describe
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		RESULT_VARIABLE _RES
		OUTPUT_VARIABLE PROJECT_VERSION
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

	if(_RES EQUAL 0)
		return()
	endif()
endif()

# Fallback for downloaded zip's or git unavailability
set(PROJECT_VERSION_FILE "${CMAKE_SOURCE_DIR}/openage_version")
if(EXISTS ${PROJECT_VERSION_FILE})
	file(STRINGS ${PROJECT_VERSION_FILE} PROJECT_VERSION)
endif()

# Still could not detect the version. Don't worry, raise a warning (shout a curse word?) and move on.
if(NOT PROJECT_VERSION)
	message(WARNING "Could not determine project version.")
	set(PROJECT_VERSION "0.0")
endif()
