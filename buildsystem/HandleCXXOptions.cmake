# Copyright 2015-2016 the openage authors. See copying.md for legal info.

# sets CXXFLAGS and compiler for the project

#TODO: integrate PGO (profile-guided optimization) build

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

macro(set_linker_flags FLAGS)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FLAGS}")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${FLAGS}")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${FLAGS}")
	set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${FLAGS}")
endmacro()

macro(set_cxx_optimize_flags FLAGS)
	set(${BUILD_TYPE_CXX_FLAGS} "${${BUILD_TYPE_CXX_FLAGS}} ${FLAGS}")
endmacro()


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

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

# optimization settings.
# TODO: multi-configuration support for xcode, vstudio, ...
#       the following code just makes sense for single-config
#       generation, e.g. makefiles.
#       we'd have to perform the flag generation for other types as well.

# these flags will now be extended by the following code.
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG")

# If CXX_OPTIMIZATION_LEVEL was not provided, default to auto
if(NOT CXX_OPTIMIZATION_LEVEL)
	set(CXX_OPTIMIZATION_LEVEL "auto")
endif()

if(${CXX_OPTIMIZATION_LEVEL} STREQUAL "auto")
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set(${CXX_OPTIMIZATION_LEVEL} "g")
	else()
		set(${CXX_OPTIMIZATION_LEVEL} "3")
	endif()
endif()

if(${CXX_OPTIMIZATION_LEVEL} STREQUAL "0")
	set_cxx_optimize_flags("-O0")
elseif(${CXX_OPTIMIZATION_LEVEL} STREQUAL "1")
	set_cxx_optimize_flags("-O1")
elseif(${CXX_OPTIMIZATION_LEVEL} STREQUAL "2")
	set_cxx_optimize_flags("-O2")
elseif(${CXX_OPTIMIZATION_LEVEL} STREQUAL "3")
	set_cxx_optimize_flags("-O3")
elseif(${CXX_OPTIMIZATION_LEVEL} STREQUAL "g")
	if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
		set_cxx_optimize_flags("-Og")
	else()
		set_cxx_optimize_flags("-O1")
	endif()
elseif(${CXX_OPTIMIZATION_LEVEL} STREQUAL "max")
	set_cxx_optimize_flags("-O3 -march=native")

	if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
		include(ProcessorCount)
		ProcessorCount(N)
		if(NOT N EQUAL 0)
			set_cxx_optimize_flags("-flto=${N}")
			set_linker_flags("-flto=${N}")
		endif()
	endif()
endif()


# sanitizing options

if(NOT CXX_SANITIZE_MODE)
	set(CXX_SANITIZE_MODE "none")
endif()

if (${CXX_SANITIZE_MODE} STREQUAL "none")
	# Do nothing
	if(${CXX_SANITIZE_FATAL})
		message(WARNING "CXX_SANITIZE_FATAL is only valid when CXX_SANITIZE_MODE is not none")
	endif()
else()
	set_cxx_flags("-fno-omit-frame-pointer")
	if(${CXX_SANITIZE_FATAL} AND ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
		set_cxx_flags("-fno-sanitize-recover")
	endif()

	if (${CXX_SANITIZE_MODE} STREQUAL "yes")
		set_cxx_flags("-fsanitize=address")
		set_cxx_flags("-fsanitize=undefined")
	elseif(${CXX_SANITIZE_MODE} STREQUAL "mem")
		if(NOT APPLE)
			set_cxx_flags("-fsanitize=memory")
		endif()
	elseif(${CXX_SANITIZE_MODE} STREQUAL "thread")
		if(NOT APPLE)
			set_cxx_flags("-fsanitize=thread")
		endif()
		if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
			set_cxx_flags("-fPIC")
			set_cxx_flags("-pie")
		endif()
	else()
		message(WARNING "Unknown sanitizer mode provided: ${CXX_SANITIZE_MODE}")
	endif()
endif()
