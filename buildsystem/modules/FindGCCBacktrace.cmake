# Copyright 2015-2017 the openage authors. See copying.md for legal info.

# This module defines:
#
#  GCCBacktrace_FOUND           - whether libbacktrace was found
#  GCCBacktrace_INCLUDE_DIRS    - The include dirs for libbacktrace
#  GCCBacktrace_LIBRARIES       - The list of libraries

find_path(GCCBacktrace_INCLUDE_DIR "backtrace.h")
find_library(GCCBacktrace_LIBRARY
	NAMES
		"backtrace"
	PATHS
		/usr/lib
		/usr/local/lib
		/sw
		/usr/freeware
)

include(CheckCXXSourceCompiles)
list(APPEND CMAKE_REQUIRED_INCLUDES ${GCCBacktrace_INCLUDE_DIR})
list(APPEND CMAKE_REQUIRED_LIBRARIES ${GCCBacktrace_LIBRARY})
set(CMAKE_REQUIRED_QUIET TRUE)
CHECK_CXX_SOURCE_COMPILES("
#include <iostream>
#include <backtrace.h>

int callback(void * /* unused */, uintptr_t pc) {
	std::cout << (void *) pc << std::endl;
	return 0;
}

int main() {
	struct backtrace_state *state = nullptr;
	state = backtrace_create_state(nullptr, false, nullptr, nullptr);
	backtrace_simple(state, 0, callback, nullptr, nullptr);
	return 0;
}
" HAVE_GCC_BACKTRACE)

if(NOT HAVE_GCC_BACKTRACE)
	unset(GCCBacktrace_INCLUDE_DIR)
	unset(GCCBacktrace_LIBRARY)
else()
	set(GCCBacktrace_LIBRARIES ${GCCBacktrace_LIBRARY})
	set(GCCBacktrace_INCLUDE_DIRS ${GCCBacktrace_INCLUDE_DIR})
endif()

# handle the QUIETLY and REQUIRED arguments and set BACKTRACE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
	GCCBacktrace
	FOUND_VAR GCCBacktrace_FOUND
	REQUIRED_VARS GCCBacktrace_LIBRARIES GCCBacktrace_INCLUDE_DIRS)

mark_as_advanced(GCCBacktrace_LIBRARIES GCCBacktrace_INCLUDE_DIRS)
