# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# - Try to find opusfile
# Once done this will define
#
# OPUSFILE_FOUND - system has libopusfile
# OPUSFILE_INCLUDE_DIRS - the libopusfile include directory
# OPUSFILE_LIBRARIES - The libopusfile libraries

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules (OPUSFILE opusfile)
  list(APPEND OPUSFILE_INCLUDE_DIRS ${OPUSFILE_INCLUDEDIR})
endif()

if(NOT OPUSFILE_FOUND)
  find_path(OPUSFILE_INCLUDE_DIRS opus/opusfile.h)
  find_library(OPUSFILE_LIBRARIES opusfile)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OPUSFILE DEFAULT_MSG OPUSFILE_INCLUDE_DIRS OPUSFILE_LIBRARIES)

mark_as_advanced(OPUSFILE_INCLUDE_DIRS OPUSFILE_LIBRARIES)
