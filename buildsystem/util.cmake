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
