# This file was taken from ulrichard's FTGL fork,
# Copyright 2011-2011 Richard Ulrich.
# It's licensed under the terms of the 3-clause OpenBSD license.
# Modifications Copyright 2014-2014 the openage authors.
# See copying.md for further legal info.

# - Locate FTGL library
# This module defines
#  FTGL_LIBRARIES, the library to link against
#  FTGL_FOUND, if false, do not try to link to FTGL
#  FTGL_INCLUDE_DIRS, where to find headers.
#
# $FTGL_DIR is an environment variable that points to the main ftgl directory.

#=============================================================================
# Copyright 2011 Richard Ulrich.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

# Created by Richard Ulrich.

find_path(FTGL_INCLUDE_DIR FTGL/ftgl.h
	HINTS
	$ENV{FTGL_DIR}
	PATH_SUFFIXES include src
	PATHS
	/usr/include
	/usr/local/include
	/sw/include
	/opt/local/include
	/usr/freeware/include
)

find_library(FTGL_LIBRARY
	NAMES ftgl libftgl ftgl_static
	HINTS
	$ENV{FTGL_DIR}
	PATH_SUFFIXES lib64 lib
	PATHS
	/usr/lib
	/usr/local/lib
	/sw
	/usr/freeware
)

# set the user variables
if(FTGL_INCLUDE_DIR)
	set(FTGL_INCLUDE_DIRS "${FTGL_INCLUDE_DIR}")
endif()
set(FTGL_LIBRARIES "${FTGL_LIBRARY}")

# handle the QUIETLY and REQUIRED arguments and set FTGL_FOUND to TRUE if
# all listed variables are TRUE
include("FindPackageHandleStandardArgs")
find_package_handle_standard_args(FTGL  DEFAULT_MSG  FTGL_LIBRARY  FTGL_INCLUDE_DIR)

mark_as_advanced(FTGL_LIBRARY FTGL_INCLUDE_DIR)
