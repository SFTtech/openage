# Copyright 2014-2018 the openage authors. See copying.md for legal info.

# declare a new 'empty' executable file.
# you need to use add_sources to add source files to it, and finalize_binary to finalize it.
# then you can add libraries, include dirs, etc.
function(declare_binary target_name output_name type)
	# create the executable
	if(type STREQUAL "executable")
		add_executable("${target_name}" ${sources})
	elseif(type STREQUAL "library")
		add_library("${target_name}" SHARED ${sources})
	endif()

	set_target_properties("${target_name}" PROPERTIES OUTPUT_NAME "${output_name}")

	set_property(GLOBAL APPEND PROPERTY SFT_BINARIES "${target_name}")

	# process further arguments
	foreach(flag ${ARGN})
		if(flag STREQUAL "allow_no_undefined")
			if(NOT type STREQUAL "library")
				message(FATAL_ERROR "finalize_binary flag 'allow_no_undefined' is only valid for libraries!")
			endif()

			if(NOT "${CMAKE_CXX_FLAGS}" MATCHES "-fsanitize" AND NOT MSVC)
				if(APPLE)
					set(link_flag "-undefined,error")
				elseif(MINGW)
					if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
						# set(link_flag "")
					endif()
				else()
					set(link_flag "--no-undefined")
				endif()
				set_target_properties("${target_name}" PROPERTIES
					COMPILE_FLAGS "${EXTRA_FLAGS}"
					LINK_FLAGS "-Wl,${link_flag}"
				)
			endif()
		else()
			message(FATAL_ERROR "declare_binary encountered unknown flag: ${flag}")
		endif()
	endforeach()
endfunction()


# add source files to a binary
#
# by default, the filename is interpreted as relative to the current directory
# to specify absolute filenames, write ABSOLUTE
# to specify a file that will be auto-generated, write GENERATED, all files after
# this modifier are marked as generated.
function(add_sources target_name)
	set(generated FALSE)

	get_property(binary_list GLOBAL PROPERTY SFT_BINARIES)
	list(FIND binary_list "${target_name}" index)
	if(index EQUAL -1)
		message(FATAL_ERROR "attempting to add source to unknown binary ${target_name}")
	endif()

	foreach(source ${ARGN})
		if(source STREQUAL GENERATED)
			set(generated TRUE)
		else()
			if(NOT IS_ABSOLUTE "${source}")
				set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
			endif()
			file(TO_CMAKE_PATH "${source}" source)

			# add all sources as private, otherwise _ALL SOURCES_
			# would be compiled again for each library that links against
			# the $target_name
			target_sources("${target_name}" PRIVATE "${source}")

			if(generated)
				set_source_files_properties("${source}" PROPERTIES GENERATED ON)
				set_property(GLOBAL PROPERTY SFT_BINARY_HAS_GENERATED_SRCS_${target_name} TRUE)
			endif()
		endif()
	endforeach()
endfunction()


# finalize the executable definition
function(finalize_binary target_name)
	get_property(has_generated_sources GLOBAL PROPERTY SFT_BINARY_HAS_GENERATED_SRCS_${target_name})

	# make the binary depend on codegen iff it has any generated files
	#if(has_generated_sources)
	#	add_dependencies("${target_name}" codegen)
	#endif()

	pretty_print_target("cpp" "${target_name}" "[${source_file_count} sources]")
endfunction()
