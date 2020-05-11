# Copyright 2020-2020 the openage authors. See copying.md for legal info.

# provides verbosity settings to debug cmake, compiler and linker

# cmake_print_properties and cmake_print_variables helpers
include(CMakePrintHelpers)

##################################################
# DEBUGGING CMAKE
#
# Read the debug documentation under doc/debug.md
#
# Variables: Print single variables to console
#	cmake_print_variables(MY_VARIABLE)

# Properties: Print properties of targets to console:
#	cmake_print_properties(
#			TARGETS "${TARGETNAME}"
#			PROPERTIES
#			OUTPUT_NAME
#			COMPILE_OPTIONS
#			COMPILE_FLAGS
#			LINK_OPTIONS
#	)


if("${DEBUG_VERBOSITY}" STREQUAL "low")
    message(STATUS "
        Debug verbosity: LOW
        Change it with:
        -DDEBUG_VERBOSITY=[low,medium,high,spam]
    ")

    # Makefile: Increase verbosity of Make output
    set(CMAKE_VERBOSE_MAKEFILE "on")

elseif("${DEBUG_VERBOSITY}" STREQUAL "medium")
    message(STATUS "
        Debug verbosity: MEDIUM
        -DDEBUG_VERBOSITY=[low,medium,high,spam]
    ")

    # Makefile: Increase verbosity of Make output
    set(CMAKE_VERBOSE_MAKEFILE "on")

    # Compiler: Add verbose flags compiler
    add_compile_options("-v")

elseif("${DEBUG_VERBOSITY}" STREQUAL "high")
    message(STATUS "
        Debug verbosity: HIGH
        -DDEBUG_VERBOSITY=[low,medium,high,spam]
    ")

    # Makefile: Increase verbosity of Make output
    set(CMAKE_VERBOSE_MAKEFILE "on")

    # Compiler: Add verbose flags compiler
    add_compile_options("-v")

    # BEWARE: Also turns on the property output
    # of different targets (cmake_print_properties)

elseif("${DEBUG_VERBOSITY}" STREQUAL "spam")
    message(STATUS "
        Debug verbosity: SPAM
        -DDEBUG_VERBOSITY=[low,medium,high,spam]
    ")

    # Makefile: Increase verbosity of Make output
    set(CMAKE_VERBOSE_MAKEFILE "on")

    # Compiler: Add verbose flags compiler
    add_compile_options("-v")

    # Linker: Add verbose flags to linker
    add_link_options("LINKER:-V")

    # BEWARE: Also turns on the property output
    # of different targets (cmake_print_properties)

else()
    message(STATUS "
        Debug verbosity: Quiet
        -DDEBUG_VERBOSITY=[low,medium,high,spam]
    ")

endif()
