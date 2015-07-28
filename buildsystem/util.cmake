# Copyright 2014-2015 the openage authors. See copying.md for legal info.

set(AUTOGEN_WARNING "warning: auto-generated file by cmake. look at the template file instead.")

function(pretty_print_target type targetname targetproperties)
	# pretty-prints a new target; tries to group target types to make stuff look more clean.
	get_property(previous_type GLOBAL PROPERTY SFT_TARGETPRINTER_CURRENT_TYPE)
	if(NOT "${type}" STREQUAL "${previous_type}")
		message("")
		message("${type}")
	endif()
	set_property(GLOBAL PROPERTY SFT_TARGETPRINTER_CURRENT_TYPE "${type}")

	string(STRIP "${targetname}" targetname)
	string(STRIP "${targetproperties}" targetproperties)

	string(LENGTH "${targetname}" targetnamelength)
	foreach(tmp RANGE ${targetnamelength} 50)
		set(targetname "${targetname} ")
	endforeach()

	set(pretty_message "${targetname} ${targetproperties}")
	string(STRIP "${pretty_message}" pretty_message)
	message("\t${pretty_message}")
endfunction()

function(list_contains var value)
	set(${var} FALSE PARENT_SCOPE)
	foreach(value2 ${ARGN})
		if(${value} STREQUAL ${value2})
			set(${var} TRUE PARENT_SCOPE)
		endif()
	endforeach()
endfunction()

function(disallow_in_source_builds)
	if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
		if(ALLOW_IN_SOURCE_BUILD)
			message("in-source build")
		else()
			message("


In-source builds are disallowed. They are a source of infinite pain,
- cluttering up the source folder
- causing cmake to silently refuse creating out-of-source builds
- overwriting the root Makefile (which contains cleaninsourcebuild)

To perform an out-of-source build,
- mkdir bin; cd bin; cmake ..
- or use ./configure, which will do this automatically

If you really want to perform an in-source build, use -DALLOW_IN_SOURCE_BUILD=TRUE

You need to run 'make cleaninsourcebuild' right now, since this (failed)
attempt already produced traces of an in-source build.
cmake will attempt in-source-builds instead of regular ones until you clean
those traces (remove CMakeCache.txt and CMakeFiles).



")
			message(FATAL_ERROR "aborting")
		endif()
	endif()
endfunction()
