# Copyright 2015-2016 the openage authors. See copying.md for legal info.

# FindHarfBuzz
# ---------
#
# Locate HarfBuzz, the awesome text shaping library.
#
# The module defines the following variables:
#
# ::
#
#    HarfBuzz_FOUND - Found HarfBuzz library
#    HarfBuzz_INCLUDE_DIRS - HarfBuzz include directories
#    HarfBuzz_LIBRARIES - The libraries needed to use HarfBuzz
#    HarfBuzz_VERSION_STRING - the version of HarfBuzz found
#
# Example Usage:
#
# ::
#
#     find_package(HarfBuzz REQUIRED)
#     include_directories(${HarfBuzz_INCLUDE_DIRS})
#
# ::
#
#     add_executable(foo foo.cc)
#     target_link_libraries(foo ${HarfBuzz_LIBRARIES})

find_path(HarfBuzz_INCLUDE_DIR harfbuzz/hb.h
	HINTS $ENV{HARFBUZZ_DIR}
	PATHS
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		/usr/freeware/include
)

find_library(HarfBuzz_LIBRARY
	NAMES harfbuzz libharfbuzz
	HINTS $ENV{HARFBUZZ_DIR}
	PATH_SUFFIXES lib64 lib
	PATHS
		/usr/lib
		/usr/local/lib
		/sw
		/usr/freeware
)

if(HarfBuzz_INCLUDE_DIR)
	set(HarfBuzz_VERSION_FILE "${HarfBuzz_INCLUDE_DIR}/harfbuzz/hb-version.h")
	if(EXISTS "${HarfBuzz_VERSION_FILE}")
		file(STRINGS "${HarfBuzz_VERSION_FILE}" hb_version_str
		     REGEX "^#define[\t ]+HB_VERSION_STRING[\t ]+\".*\"")

		string(REGEX REPLACE "^#define[\t ]+HB_VERSION_STRING[\t ]+\"([^\"]*)\".*" "\\1"
		       HarfBuzz_VERSION_STRING "${hb_version_str}")
		unset(hb_version_str)
	endif()
	unset(HarfBuzz_VERSION_FILE)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HarfBuzz
	FOUND_VAR HarfBuzz_FOUND
	REQUIRED_VARS HarfBuzz_LIBRARY HarfBuzz_INCLUDE_DIR
	VERSION_VAR HarfBuzz_VERSION_STRING
)

if(HarfBuzz_FOUND)
	set(HarfBuzz_INCLUDE_DIRS "${HarfBuzz_INCLUDE_DIR}")
	set(HarfBuzz_LIBRARIES "${HarfBuzz_LIBRARY}")
endif()

mark_as_advanced(HarfBuzz_INCLUDE_DIR HarfBuzz_LIBRARY)
