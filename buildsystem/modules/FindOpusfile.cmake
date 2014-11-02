# Copyright 2014-2014 the openage authors. See copying.md for legal info.

# Try to find Opusfile
#
# Once done this will define:
#  OPUSFILE_FOUND        - System has Opusfile
#  OPUSFILE_INCLUDE_DIRS - The Opusfile include directories
#  OPUSFILE_LIBRARIES    - The libraries needed to use Opusfile

find_package(PkgConfig)

find_path(OPUSFILE_INCLUDE_DIR opusfile.h
	HINTS
	/usr/include/opus
	/usr/local/include/opus
)

find_library(OPUSFILE_LIBRARY NAMES opusfile)

set(OPUSFILE_LIBRARIES "${OPUSFILE_LIBRARY}")
set(OPUSFILE_INCLUDE_DIRS "${OPUSFILE_INCLUDE_DIR}")

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set OPUSFILE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Opusfile DEFAULT_MSG
                                  OPUSFILE_LIBRARY OPUSFILE_INCLUDE_DIR)

mark_as_advanced(OPUSFILE_INCLUDE_DIR OPUSFILE_LIBRARY)
