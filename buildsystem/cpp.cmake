# sets CXXFLAGS and compiler for the project

#TODO: integrate PGO (profile-guided optimization) build

function(cpp_init)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11" PARENT_SCOPE)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g" PARENT_SCOPE)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)

	set(CPP_SOURCE_DIR "${CMAKE_SOURCE_DIR}/cpp" PARENT_SCOPE)

	function(require_cxx_version CXXNAME MINIMAL)
		if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MINIMAL})
			message(FATAL_ERROR ">=${CXXNAME}-${MINIMAL} required (c++11, you know?), you have ${CMAKE_CXX_COMPILER_VERSION}")
		endif()
	endfunction()

	# check for compiler versions
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		require_cxx_version("gcc" 4.8)
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		require_cxx_version("clang" 3.3)
	else() #"Intel", "MSVC", etc..
		message(WARNING "Using untested compiler, at least I hope it's free software. Continue on your own, warrior.")
	endif()
endfunction()

# declare a new 'empty' executable file.
# you need to use add_sources to add source files to it, and finalize_executable to finalize it.
# then you can add libraries, include dirs, etc.
function(declare_executable binary_name)
	set_property(GLOBAL APPEND PROPERTY SFT_BINARIES ${binary_name})
endfunction()

# add source files to a binary
#
# by default, the filename is interpreted as relative to the current directory
# to specify absolute filenames, write ABSOLUTE
# to specify a file that will be auto-generated, write GENERATED
function(add_sources binary_name)
	set(generated FALSE)

	get_property(binary_list GLOBAL PROPERTY SFT_BINARIES)
	list_contains(contained ${binary_name} ${binary_list})
	if(NOT contained)
		message(FATAL_ERROR "attempting to add source to unknown binary ${binary_name}")
	endif()

	foreach(source ${ARGN})
		if(source STREQUAL GENERATED)
			set(generated TRUE)
		else()
			if(NOT IS_ABSOLUTE ${source})
				set(source ${CMAKE_CURRENT_SOURCE_DIR}/${source})
			endif()

			set_property(
				GLOBAL APPEND PROPERTY
				SFT_BINARY_SRCS_${binary_name}
				${source}
			)

			if(${generated})
				set_property(
					GLOBAL APPEND PROPERTY
					SFT_BINARY_GENERATED_SRCS_${binary_name}
					${source}
				)
			endif()
		endif()
	endforeach()
endfunction()

# finalize the executable definition,
# no sources can be added to the binary afterwards.
function(finalize_executable binary_name)
	get_property(sources GLOBAL PROPERTY SFT_BINARY_SRCS_${binary_name})
	get_property(generatedsources GLOBAL PROPERTY SFT_BINARY_GENERATED_SRCS_${binary_name})

	# mark the generated sources as GENERATED.
	if(generatedsources)
		set_source_files_properties(${generatedsources} PROPERTIES GENERATED ON)
	endif()

	# print overview of the executable's files
	message("C++ executable: ${binary_name}")
	foreach(source ${sources})
		list_contains(contained ${source} ${generatedsources})
		if(contained)
			print_filename(${source} "[gen]")
		else()
			print_filename(${source})
		endif()
	endforeach()

	# create the executable
	add_executable(${binary_name} ${sources})

	# make the binary depend on codegen iff it has any generated files
	if(generatedsources)
		add_dependencies(${binary_name} codegen)
	endif()
	message("")
endfunction()

cpp_init()
