# Copyright 2015-2015 the openage authors. See copying.md for legal info.

# This module defines:
#
#  BACKTRACE_FOUND         - whether libbacktrace was found
#  BACKTRACE_LIBRARY       - library linker directives

set(BACKTRACE_LIBRARY "-lbacktrace")

try_compile(BACKTRACE_TEST_RESULT
	"${CMAKE_BINARY_DIR}"
	"${CMAKE_CURRENT_LIST_DIR}/FindBacktrace_test.cpp"
	LINK_LIBRARIES "${BACKTRACE_LIBRARY}"
	OUTPUT_VARIABLE dbg)

mark_as_advanced(BACKTRACE_TEST_RESULT)

if(NOT BACKTRACE_TEST_RESULT)
	set(BACKTRACE_LIBRARY "")
endif()

# handle the QUIETLY and REQUIRED arguments and set BACKTRACE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Backtrace DEFAULT_MSG BACKTRACE_LIBRARY)
