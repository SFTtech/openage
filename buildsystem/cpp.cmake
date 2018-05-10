# Copyright 2014-2017 the openage authors. See copying.md for legal info.

# declare a new 'empty' executable file.
# you need to use add_sources to add source files to it, and finalize_binary to finalize it.
# then you can add libraries, include dirs, etc.
function(declare_binary binary_name)
	set_property(GLOBAL APPEND PROPERTY SFT_BINARIES "${binary_name}")
endfunction()


# add source files to a binary
#
# by default, the filename is interpreted as relative to the current directory
# to specify absolute filenames, write ABSOLUTE
# to specify a file that will be auto-generated, write GENERATED
function(add_sources binary_name)
	set(generated FALSE)

	get_property(binary_list GLOBAL PROPERTY SFT_BINARIES)
	list(FIND binary_list "${binary_name}" index)
	if(index EQUAL -1)
		message(FATAL_ERROR "attempting to add source to unknown binary ${binary_name}")
	endif()

	foreach(source ${ARGN})
		if(source STREQUAL GENERATED)
			set(generated TRUE)
		else()
			if(NOT IS_ABSOLUTE "${source}")
				set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
			endif()
			file(TO_CMAKE_PATH "${source}" source)

			set_property(
				GLOBAL APPEND PROPERTY
				"SFT_BINARY_SRCS_${binary_name}"
				"${source}"
			)

			if(generated)
				set_property(
					GLOBAL APPEND PROPERTY
					"SFT_BINARY_GENERATED_SRCS_${binary_name}"
					"${source}"
				)
			endif()
		endif()
	endforeach()
endfunction()

# finalize the executable definition,
# no sources can be added to the binary afterwards.
# include paths, libraries etc can only be specified afterwards.
# type must be one of {library, executable}.
# additional flags may be specified:
#
#  - allow_no_undefined (only valid for libraries):
#     the linker will allow no undefined symbols (same linker errors as for executables)
#     is ignored when any sanitizers are active.
function(finalize_binary target_name output_name type)
	get_property(sources GLOBAL PROPERTY SFT_BINARY_SRCS_${target_name})
	get_property(generatedsources GLOBAL PROPERTY SFT_BINARY_GENERATED_SRCS_${target_name})

	# mark the generated sources as GENERATED.
	if(generatedsources)
		set_source_files_properties(${generatedsources} PROPERTIES GENERATED ON)
	endif()

	# print overview of the binary's files
	list(LENGTH sources source_file_count)
	list(LENGTH generatedsources generated_source_file_count)
	set(pretty_binary_properties "[${source_file_count} sources]")
	if (generatedsources)
		set(pretty_binary_properties "${pretty_binary_properties} [${generated_source_file_count} generated]")
	endif()

	pretty_print_target("cpp ${type}" "${target_name}" "${pretty_binary_properties}")

	# create the executable
	if(type STREQUAL "executable")
		add_executable("${target_name}" ${sources})
	elseif(type STREQUAL "library")
		add_library("${target_name}" SHARED ${sources})
	endif()

	foreach(flag ${ARGN})
		if(flag STREQUAL "allow_no_undefined")
			if(NOT type STREQUAL "library")
				message(FATAL_ERROR "finalize_binary flag 'allow_no_undefined' is only valid for libraries!")
			endif()

			if(NOT "${CMAKE_CXX_FLAGS}" MATCHES "-fsanitize" AND NOT MSVC)
				if(APPLE)
					set(link_flag "-undefined,error")
				else()
					set(link_flag "--no-undefined")
				endif()
				set_target_properties("${target_name}" PROPERTIES
					COMPILE_FLAGS "${EXTRA_FLAGS}"
					LINK_FLAGS "-Wl,${link_flag}"
				)
			endif()
		else()
			message(FATAL_ERROR "finalize_binary flag unknown: ${flag}")
		endif()
	endforeach()

	set_target_properties("${target_name}" PROPERTIES OUTPUT_NAME "${output_name}")

	# make the binary depend on codegen iff it has any generated files
	if(generatedsources)
		add_dependencies("${target_name}" codegen)
	endif()
endfunction()
