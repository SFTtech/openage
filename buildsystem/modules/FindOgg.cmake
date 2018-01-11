# Copyright 2018-2018 the openage authors. See copying.md for legal info.

# - Find ogg library
# Find the native Ogg headers and library.
# This module defines
#  OGG_INCLUDE_DIRS   - where to find ogg/ogg.h etc.
#  OGG_LIBRARIES      - List of libraries when using libogg
#  OGG_FOUND          - True if ogg is found.

find_path(OGG_INCLUDE_DIR
	NAMES ogg/ogg.h
	DOC "Ogg include directory"
)

find_library(OGG_LIBRARY
	NAMES ogg
	DOC "Path to ogg library"
)

# handle the QUIETLY and REQUIRED arguments and set OGG_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Ogg DEFAULT_MSG OGG_LIBRARY OGG_INCLUDE_DIR)

mark_as_advanced(OGG_INCLUDE_DIR OGG_LIBRARY)

# export the variables
set(OGG_LIBRARIES "${OGG_LIBRARY}")
set(OGG_INCLUDE_DIRS "${OGG_INCLUDE_DIR}")
