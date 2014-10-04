# sets CXXFLAGS and compiler for the project

#TODO: integrate PGO (profile-guided optimization) build

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}")

message("compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

macro(cxx_required CXXNAME MINIMAL)
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MINIMAL})
		message(FATAL_ERROR ">=${CXXNAME}-${MINIMAL} required (c++11, you know?), you have ${CMAKE_CXX_COMPILER_VERSION}")
	endif()
endmacro()

# check for compiler versions
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	cxx_required("gcc" 4.8)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	cxx_required("clang" 3.3)
else() #"Intel", "MSVC", etc..
	message(WARNING "Using untested compiler, at least I hope it's free software. Continue on your own, warrior.")
endif()

# these environment variables are used by the python C extension builder
set(CCENV "CC=${CMAKE_C_COMPILER}")
set(CXXENV "CXX=${CMAKE_CXX_COMPILER}")


# output binary definition helpers.
# allows to add sources to a binary from anywhere.

# create a new 'empty' binary
function(create_binary binary_name)
	set_property(GLOBAL APPEND PROPERTY SFT_BINARIES ${binary_name})
	set_property(GLOBAL APPEND PROPERTY SFT_BINARY_PATH_${binary_name} ${CMAKE_CURRENT_BINARY_DIR}/${binary_name})
endfunction()

# add a source file with current source dir path
function(add_sources binary_name)
	get_property(binary_list GLOBAL PROPERTY SFT_BINARIES)
	if(NOT ${binary_list} MATCHES ${binary_name})
		message(FATAL_ERROR "adding sources to unknown binary ${binary_name}")
	endif()

	foreach(source ${ARGN})
		set_property(
			GLOBAL APPEND PROPERTY
			SFT_BINARY_SRCS_${binary_name}
			${CMAKE_CURRENT_SOURCE_DIR}/${source}
		)
	endforeach()
endfunction()

# add sources file with absolute path to a binary
function(add_sources_absolute binary_name)
	get_property(binary_list GLOBAL PROPERTY SFT_BINARIES)
	if(NOT ${binary_list} MATCHES ${binary_name})
		message(FATAL_ERROR "adding source to unknown binary ${binary_name}")
	endif()

	foreach(source ${ARGN})
		set_property(
			GLOBAL APPEND PROPERTY
			SFT_BINARY_SRCS_${binary_name}
			${source}
		)
	endforeach()
endfunction()

# finalize the executable definition,
# no sources can be added to the binary afterwards.
function(define_executable binary_name)
	get_property(sources GLOBAL PROPERTY SFT_BINARY_SRCS_${binary_name})
	message("defined binary: ${binary_name}")
	add_executable(${binary_name} ${sources})
endfunction()
