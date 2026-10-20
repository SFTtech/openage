# Copyright 2026+ the openage authors. See copying.md for legal info.

# GCC fails with "redefinition of ..." when a translation unit includes a
# standard header after importing a module whose interface uses the same
# standard declarations.
# Given a header unit of <bits/stdc++.h>, GCC translates every standard
# #include into its import, and imports merge in any order.
# CMake can't build header units, but GCC looks for them in the repository
# root of CMake's module mappers: the build directory.
# CMake's dependency scan runs without mapper, so it never reports the header
# unit, which CMake couldn't resolve.
#
# TODO: remove when GCC merges textual definitions after imports.

function(use_std_header_unit target)
	if(TARGET std_header_unit)
		add_dependencies("${target}" std_header_unit)
	endif()
endfunction()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	return()
endif()

set(std_header_files "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/std_header_unit")

# importers get the header unit's macros, so build it with the global flags
separate_arguments(std_header_flags UNIX_COMMAND
	"${CMAKE_CXX_FLAGS} ${${BUILD_TYPE_CXX_FLAGS}} ${CMAKE_CXX${CMAKE_CXX_STANDARD}_EXTENSION_COMPILE_OPTION} ${CMAKE_CXX_COMPILE_OPTIONS_PIC}"
)

file(CONFIGURE OUTPUT "${std_header_files}.cpp" CONTENT "#include <bits/stdc++.h>\n")
execute_process(
	COMMAND "${CMAKE_CXX_COMPILER}" ${std_header_flags} -E -M "${std_header_files}.cpp"
	OUTPUT_VARIABLE std_header_deps
	COMMAND_ERROR_IS_FATAL ANY
)
string(REGEX MATCH "/[^ \n\\\\]*/bits/stdc\\+\\+\\.h" std_header "${std_header_deps}")
if(NOT std_header)
	message(FATAL_ERROR "<bits/stdc++.h> not found for the standard library header unit")
endif()

# GCC names the header unit after the header path, with '..' as ',,'
set(std_header_cmi "${std_header}")
while(std_header_cmi MATCHES "/\\.\\./")
	string(REPLACE "/../" "/,,/" std_header_cmi "${std_header_cmi}")
endwhile()
set(std_header_cmi "${CMAKE_BINARY_DIR}${std_header_cmi}.gcm")

file(CONFIGURE OUTPUT "${std_header_files}.modmap" CONTENT "${std_header} ${std_header_cmi}\n")

add_custom_command(
	OUTPUT "${std_header_cmi}"
	COMMAND "${CMAKE_CXX_COMPILER}" ${std_header_flags}
		-fmodules "-fmodule-mapper=${std_header_files}.modmap"
		-MD -MF "${std_header_files}.d" -MT "${std_header_cmi}" -Mno-modules
		-x c++-system-header bits/stdc++.h
	DEPFILE "${std_header_files}.d"
	COMMENT "Building C++ header unit <bits/stdc++.h>"
	VERBATIM
)
add_custom_target(std_header_unit DEPENDS "${std_header_cmi}")
