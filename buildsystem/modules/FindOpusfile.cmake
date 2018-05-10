# This file was taken from Unvanquished,
# Copyright 2000-2009 Kitware, Inc., Insight Software Consortium
# It's licensed under the terms of the 3-clause OpenBSD license.
# Modifications Copyright 2014-2015 the openage authors.
# See copying.md for further legal info.

# - Find opus library
# Find the native Opus headers and libraries.
# This module defines
#  OPUS_INCLUDE_DIRS   - where to find opus/opus.h, opus/opusfile.h, etc
#  OPUS_LIBRARIES      - List of libraries when using libopus
#  OPUS_FOUND          - True if opus is found.

# find the opusfile header, defines our api.
find_path(OPUS_INCLUDE_DIR
	NAMES opus/opusfile.h
	DOC "Opus include directory"
)
mark_as_advanced(OPUS_INCLUDE_DIR)

# look for libopusfile, the highlevel container-aware api.
find_library(OPUSFILE_LIBRARY
	NAMES opusfile
	DOC "Path to OpusFile library"
)
mark_as_advanced(OPUSFILE_LIBRARY)

# find libopus, the core codec component.
find_library(OPUS_LIBRARY
	NAMES opus
	DOC "Path to Opus library"
)
mark_as_advanced(OPUS_LIBRARY)


# handle the QUIETLY and REQUIRED arguments and set OPUSFILE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Opus DEFAULT_MSG OPUSFILE_LIBRARY OPUS_LIBRARY OPUS_INCLUDE_DIR)

# export the variables
set(OPUS_LIBRARIES "${OPUSFILE_LIBRARY}" "${OPUS_LIBRARY}")
set(OPUS_INCLUDE_DIRS "${OPUS_INCLUDE_DIR}" "${OPUS_INCLUDE_DIR}/opus")
