# Copyright 2020-2020 the openage authors. See copying.md for legal info.

# - Find libpng library
# Find the native libpng headers and library.
# This module defines
#  LIBPNG_INCLUDE_DIRS   - where to find ogg/ogg.h etc.
#  LIBPNG_LIBRARIES      - List of libraries when using libogg
#  LIBPNG_FOUND          - True if ogg is found.

find_path(LIBPNG_INCLUDE_DIR
	NAMES libpng/png.h
	DOC "libpng include directory"
)

find_library(LIBPNG_LIBRARY
	NAMES png
	DOC "Path to libpng library"
)

# handle the QUIETLY and REQUIRED arguments and set LIBPNG_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libpng DEFAULT_MSG LIBPNG_INCLUDE_DIR LIBPNG_LIBRARY)

mark_as_advanced(LIBPNG_INCLUDE_DIR LIBPNG_LIBRARY)

# export the variables
set(LIBPNG_INCLUDE_DIRS "${LIBPNG_INCLUDE_DIR}")
set(LIBPNG_LIBRARIES "${LIBPNG_LIBRARY}")
