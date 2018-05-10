# Copyright 2015-2016 the openage authors. See copying.md for legal info.

find_path(EPOXY_INCLUDE_DIRS epoxy/gl.h
	HINTS
	$ENV{EPOXY_DIR}
	PATH_SUFFIXES include src
	PATHS
	/usr/include
	/usr/local/include
	/sw/include
	/opt/local/include
	/usr/freeware/include
)

find_library(EPOXY_LIBRARIES
	NAMES epoxy
	HINTS
	$ENV{EPOXY_DIR}
	PATH_SUFFIXES lib64 lib
	PATHS
	/usr/lib
	/usr/local/lib
	/sw
	/usr/freeware
)

include("FindPackageHandleStandardArgs")
find_package_handle_standard_args(Epoxy DEFAULT_MSG EPOXY_LIBRARIES EPOXY_INCLUDE_DIRS)

mark_as_advanced(EPOXY_LIBRARIES EPOXY_INCLUDE_DIRS)
