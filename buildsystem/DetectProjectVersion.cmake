# Copyright 2015-2026 the openage authors. See copying.md for legal info.

# Determines the version of the project
# stores a full version string with commit hash and number in the variable `PROJECT_VERSION`

# If the project version was already defined or previously retrieved, don't bother.
if(PROJECT_VERSION)
	return()
endif()

# if .git exists, try running git describe
if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/.git")
	message(STATUS "Set PROJECT_VERSION from git.")

	# Tell CMake to reconfigure when the commit hash changes. Without this,
	# PROJECT_VERSION is baked into the generated config files at the first
	# configure and goes stale on every subsequent commit until the user
	# explicitly reruns cmake. Watch .git/HEAD (so branch switches and
	# detached-HEAD moves trigger), the ref file HEAD currently points at
	# (so commits on the current branch trigger), and packed-refs (so the
	# git gc / repack path is covered too). New tags will still need a
	# manual reconfigure, since watching .git/refs/tags/ as a glob would
	# scale poorly on repos with many tags.
	set(_OPENAGE_GIT_HEAD "${CMAKE_SOURCE_DIR}/.git/HEAD")
	if(EXISTS "${_OPENAGE_GIT_HEAD}")
		set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_OPENAGE_GIT_HEAD}")

		file(READ "${_OPENAGE_GIT_HEAD}" _OPENAGE_GIT_HEAD_CONTENT)
		string(STRIP "${_OPENAGE_GIT_HEAD_CONTENT}" _OPENAGE_GIT_HEAD_CONTENT)
		if(_OPENAGE_GIT_HEAD_CONTENT MATCHES "^ref: (.+)$")
			set(_OPENAGE_GIT_REF_FILE "${CMAKE_SOURCE_DIR}/.git/${CMAKE_MATCH_1}")
			if(EXISTS "${_OPENAGE_GIT_REF_FILE}")
				set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_OPENAGE_GIT_REF_FILE}")
			endif()
		endif()

		set(_OPENAGE_GIT_PACKED_REFS "${CMAKE_SOURCE_DIR}/.git/packed-refs")
		if(EXISTS "${_OPENAGE_GIT_PACKED_REFS}")
			set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_OPENAGE_GIT_PACKED_REFS}")
		endif()
	endif()

	execute_process(
		COMMAND git describe --tags --long
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
	file(STRINGS ${PROJECT_VERSION_FILE} FILE_DESCRIBE_VERSION)

	STRING(REGEX REPLACE "([0-9]+\\.[0-9]+\\.[0-9]+)"
		   "\\1" PROJECT_VERSION "${FILE_DESCRIBE_VERSION}")
endif()

# Still could not detect the version. Don't worry, raise a warning (shout a curse word?) and move on.
if(NOT PROJECT_VERSION)
	message(WARNING "Could not determine project version.")
	set(PROJECT_VERSION "0.0")
endif()
