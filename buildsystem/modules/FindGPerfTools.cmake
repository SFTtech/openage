# This file was taken from VAST,
# Copyright 2014-2014 Matthias Vallentin.
# It's licensed under the terms of the 3-clause BSD license.
# Modifications Copyright 2014-2020 the openage authors.
# See copying.md for further legal info.

# Tries to find Gperftools.
#
# Usage of this module as follows:
#
#     find_package(Gperftools)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  Gperftools_ROOT_DIR  Set this variable to the root installation of
#                       Gperftools if the module has problems finding
#                       the proper installation path.
#
# Variables defined by this module:
#
#  GPERFTOOLS_FOUND              System has Gperftools libs/headers
#  GPERFTOOLS_LIBRARIES          The Gperftools libraries (tcmalloc & profiler)
#  GPERFTOOLS_TCMALLOC           The tcmalloc library
#  GPERFTOOLS_PROFILER           The profiler library
#  GPERFTOOLS_INCLUDE_DIR        The location of Gperftools headers

find_library(GPERFTOOLS_TCMALLOC
	NAMES tcmalloc
	HINTS ${Gperftools_ROOT_DIR}/lib)

find_library(GPERFTOOLS_PROFILER
	NAMES profiler
	HINTS ${Gperftools_ROOT_DIR}/lib)

find_library(GPERFTOOLS_TCMALLOC_AND_PROFILER
	NAMES tcmalloc_and_profiler
	HINTS ${Gperftools_ROOT_DIR}/lib)

find_path(GPERFTOOLS_INCLUDE_DIR
	NAMES gperftools/heap-profiler.h
	HINTS ${Gperftools_ROOT_DIR}/include)

set(GPERFTOOLS_LIBRARIES ${GPERFTOOLS_TCMALLOC_AND_PROFILER})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
	GPerfTools
	DEFAULT_MSG
	GPERFTOOLS_LIBRARIES
	GPERFTOOLS_INCLUDE_DIR)

mark_as_advanced(
	Gperftools_ROOT_DIR
	GPERFTOOLS_TCMALLOC
	GPERFTOOLS_PROFILER
	GPERFTOOLS_TCMALLOC_AND_PROFILER
	GPERFTOOLS_LIBRARIES
	GPERFTOOLS_INCLUDE_DIR)
