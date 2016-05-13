# Copyright 2014-2016 the openage authors. See copying.md for legal info.

# provides functions for defining pxdgen sources and a 'finalize' function that must be called in the end.

# filled by pxdgen.
set_property(GLOBAL PROPERTY SFT_PXDGEN_SOURCES)

function(pxdgen)
	# add a C++ header file as pxdgen source file
	foreach(HEADER_SOURCE ${ARGN})
		if(NOT "${HEADER_SOURCE}" MATCHES ".*\\.h$")
			message(FATAL_ERROR "pxdgen accepts only .h files. You provided: ${HEADER_SOURCE}")
		endif()

		get_filename_component(HEADER_SOURCE "${HEADER_SOURCE}" ABSOLUTE)
		set_property(GLOBAL APPEND PROPERTY SFT_PXDGEN_SOURCES "${HEADER_SOURCE}")
	endforeach()
endfunction()
