# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# sets CXXFLAGS and compiler for the project

#TODO: integrate PGO (profile-guided optimization) build

function(cpp_init)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

	function(require_cxx_version CXXNAME MINIMAL)
		if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MINIMAL})
			message(FATAL_ERROR ">=${CXXNAME}-${MINIMAL} required (c++14, you know?), you have ${CMAKE_CXX_COMPILER_VERSION}")
		endif()
	endfunction()


	macro(set_cxx_version_flags MINIMAL FLAGS INVERS EQTYPE)
		if(${INVERS} CMAKE_CXX_COMPILER_VERSION VERSION_${EQTYPE} ${MINIMAL})
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")
		endif()
	endmacro()

	macro(set_cxx_greater_flags MINIMAL FLAGS)
		set_cxx_version_flags(${MINIMAL} ${FLAGS} NOT LESS)
	endmacro()

	macro(set_cxx_equal_flags MINIMAL FLAGS)
		set_cxx_version_flags(${MINIMAL} ${FLAGS} "" EQUAL)
	endmacro()

	macro(set_cxx_flags FLAGS)
		set_cxx_version_flags("0.0" ${FLAGS} "" GREATER)
	endmacro()

	macro(test_cxx_flag_apply FLAG NAME DONTRUN)
		if(NOT ${DONTRUN})
			include(CheckCXXCompilerFlag)
			check_cxx_compiler_flag("${FLAG}" ${NAME})
			if(${NAME})
				set_cxx_flags("${FLAG}")
			endif()
		endif()
	endmacro()

	# check for compiler versions
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		require_cxx_version("gcc" 4.9)
		test_cxx_flag_apply("-std=c++14" GCC_SUPPORTS_CPP14 FALSE)
		set_cxx_greater_flags(4.9 "-fdiagnostics-color=auto")
		set_cxx_greater_flags(5.0 "-Wsuggest-override")

	elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		require_cxx_version("clang" 3.4)
		set_cxx_flags("-Wno-mismatched-tags")

		if(APPLE)
			set_cxx_flags("-stdlib=libc++")
		endif()

		test_cxx_flag_apply("-std=c++14" CXX_SUPPORTS_CXX14 FALSE)
		test_cxx_flag_apply("-std=c++1y" CXX_SUPPORTS_CXX1Y CXX_SUPPORTS_CXX14)

		if(NOT (CXX_SUPPORTS_CXX14 OR CXX_SUPPORTS_CXX1Y))
			message(FATAL_ERROR "compiler doesn't support c++14!")
		endif()

	else() #"Intel", "MSVC", etc..
		message(WARNING "Using untested compiler, at least I hope it's free software. Continue on your own, warrior.")
	endif()

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g" PARENT_SCOPE)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)

	set(CPP_SOURCE_DIR "${CMAKE_SOURCE_DIR}/cpp" PARENT_SCOPE)
endfunction()

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
	list_contains(contained "${binary_name}" "${binary_list}")
	if(NOT contained)
		message(FATAL_ERROR "attempting to add source to unknown binary ${binary_name}")
	endif()

	foreach(source ${ARGN})
		if(source STREQUAL GENERATED)
			set(generated TRUE)
		else()
			if(NOT IS_ABSOLUTE "${source}")
				set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
			endif()

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

			if(NOT "${CMAKE_CXX_FLAGS}" MATCHES "-fsanitize")
				if(APPLE)
					set(link_flag "-undefined,error")
				else()
					set(link_flag "--no-undefined")
				endif()
				set_target_properties("${target_name}" PROPERTIES LINK_FLAGS "-Wl,${link_flag}")
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

cpp_init()
