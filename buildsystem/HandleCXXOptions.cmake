# Copyright 2015-2019 the openage authors. See copying.md for legal info.

# sets CXXFLAGS and compiler for the project

#TODO: integrate PGO (profile-guided optimization) build


include(CheckCXXCompilerFlag)

macro(set_compiler_version_flags TYPE MINIMAL FLAGS INVERS EQTYPE)
	if(${INVERS} CMAKE_CXX_COMPILER_VERSION VERSION_${EQTYPE} ${MINIMAL})
		if(${TYPE} STREQUAL "CXX")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")
		elseif(${TYPE} STREQUAL "EXTRA")
			set(EXTRA_FLAGS "${EXTRA_FLAGS} ${FLAGS}")
		else()
			message(FATAL_ERROR "Invalid compiler flag type specified!")
		endif()
	endif()
endmacro()

macro(set_compiler_greater_flags TYPE MINIMAL FLAGS)
	set_compiler_version_flags(${TYPE} ${MINIMAL} ${FLAGS} NOT LESS)
endmacro()

macro(set_compiler_equal_flags TYPE MINIMAL FLAGS)
	set_compiler_version_flags(${TYPE} ${MINIMAL} ${FLAGS} "" EQUAL)
endmacro()

macro(set_compiler_flags TYPE FLAGS)
	set_compiler_version_flags(${TYPE} "0.0" ${FLAGS} "" GREATER)
endmacro()

macro(test_compiler_flag_apply TYPE FLAG NAME DONTRUN)
	if(NOT ${DONTRUN})
		check_cxx_compiler_flag("${FLAG}" ${NAME})
		if(${NAME})
			set_compiler_flags(${TYPE} "${FLAG}")
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
if(NOT MSVC)
	set(EXTRA_FLAGS "${EXTRA_FLAGS} -Wall -Wextra -pedantic")
endif()

# set up gold linker features
macro(try_enable_gold_linker)
	# Activate ld.gold instead of the default
	option(USE_LD_GOLD "Use GNU gold linker" ON)
	if(USE_LD_GOLD)
		if(MINGW)
			execute_process(COMMAND ${CMAKE_CXX_COMPILER} -Wl,--major-image-version,0,--minor-image-version,0 -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
		else()
			execute_process(COMMAND ${CMAKE_CXX_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
		endif()
		if("${LD_VERSION}" MATCHES "GNU gold")
			set(HAVE_LD_GOLD TRUE)
			set_linker_flags("-fuse-ld=gold")
		else()
			set(HAVE_LD_GOLD FALSE)
			message(WARNING "GNU gold linker isn't available, using the default system linker.")
		endif()
	endif()

	# do splitdebug by default when in debug mode
	set(DEBUG_FISSION_DEFAULT OFF)
	if(HAVE_LD_GOLD AND CMAKE_BUILD_TYPE STREQUAL "Debug")
		check_cxx_compiler_flag("-gsplit-dwarf" HAVE_GSPLIT_DWARF_SUPPORT)
		if(HAVE_GSPLIT_DWARF_SUPPORT)
			set(DEBUG_FISSION_DEFAULT ON)
		endif()
	endif()

	# https://gcc.gnu.org/wiki/DebugFission
	option(DEBUG_FISSION "Enable Debug Fission" DEBUG_FISSION_DEFAULT)
	if(DEBUG_FISSION)
		if(NOT HAVE_LD_GOLD)
			message(FATAL_ERROR "GNU gold linker required for Debug Fission")
		endif()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gsplit-dwarf")
		set_linker_flags("-Wl,--gdb-index")
	endif()
endmacro()

# check for compiler versions
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set_compiler_greater_flags("CXX" 4.9 "-fdiagnostics-color=auto")
	set_compiler_greater_flags("EXTRA" 5.0 "-Wsuggest-override")
	try_enable_gold_linker()

elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	set_compiler_flags("EXTRA" "-Wno-gnu-statement-expression")
	try_enable_gold_linker()

	if(APPLE)
		set_compiler_flags("CXX" "-stdlib=libc++")
	endif()

elseif(MSVC)
	# Don't worry. You're not alone. If you face an issue, just ask.

	# Enable multi processor compilation on MSVC
	set_compiler_flags("CXX" "/MP")

	return() # The following flag specifications don't apply.

else() # "Intel", etc..
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
if("${CXX_OPTIMIZATION_LEVEL}" STREQUAL "")
	set(CXX_OPTIMIZATION_LEVEL "auto")
endif()

if("${CXX_OPTIMIZATION_LEVEL}" STREQUAL "auto")
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set(CXX_OPTIMIZATION_LEVEL "g")
	else()
		set(CXX_OPTIMIZATION_LEVEL "3")
	endif()
endif()

if("${CXX_OPTIMIZATION_LEVEL}" STREQUAL "0")
	set_cxx_optimize_flags("-O0")
elseif("${CXX_OPTIMIZATION_LEVEL}" STREQUAL "1")
	set_cxx_optimize_flags("-O1")
elseif("${CXX_OPTIMIZATION_LEVEL}" STREQUAL "2")
	set_cxx_optimize_flags("-O2")
elseif("${CXX_OPTIMIZATION_LEVEL}" STREQUAL "3")
	set_cxx_optimize_flags("-O3")
elseif("${CXX_OPTIMIZATION_LEVEL}" STREQUAL "g")
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set_cxx_optimize_flags("-Og")
	else()
		set_cxx_optimize_flags("-O0")
	endif()
elseif("${CXX_OPTIMIZATION_LEVEL}" STREQUAL "max")
	set_cxx_optimize_flags("-O3 -march=native")

	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
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

if("${CXX_SANITIZE_MODE}" STREQUAL "none")
	# Do nothing
	if("${CXX_SANITIZE_FATAL}")
		message(WARNING "CXX_SANITIZE_FATAL is only valid when CXX_SANITIZE_MODE is not none")
	endif()
else()
	set_compiler_flags("CXX" "-fno-omit-frame-pointer")
	if("${CXX_SANITIZE_FATAL}" AND "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		set_compiler_flags("CXX" "-fno-sanitize-recover")
	endif()

	if("${CXX_SANITIZE_MODE}" STREQUAL "yes")
		set_compiler_flags("CXX" "-fsanitize=address")
		set_compiler_flags("CXX" "-fsanitize=undefined")
	elseif("${CXX_SANITIZE_MODE}" STREQUAL "mem")
		if(NOT APPLE)
			set_compiler_flags("CXX" "-fsanitize=memory")
		endif()
	elseif("${CXX_SANITIZE_MODE}" STREQUAL "thread")
		if(NOT APPLE)
			set_compiler_flags("CXX" "-fsanitize=thread")
		endif()
		if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
			set_compiler_flags("CXX" "-fPIC")
			set_compiler_flags("CXX" "-pie")
		endif()
	else()
		message(WARNING "Unknown sanitizer mode provided: ${CXX_SANITIZE_MODE}")
	endif()
endif()
