# Copyright 2015-2018 the openage authors. See copying.md for legal info.

# Find Python
# ~~~~~~~~~~~
# Find the Python interpreter, and related info.
#
# You can manually pass an interpreter by defining PYTHON:
# it's used as a manual override,
# and no further search is performed.
#
# This file defines the following variables:
#
# PYTHON_FOUND        - True when python was found.
# PYTHON              - The full path to the Python interpreter.
# PYTHON_INCLUDE_DIRS - Include path for Python extensions.
# PYTHON_LIBRARIES    - Library and Linker options for Python extensions.
#
# Also defines py_exec and py_get_config_var.
#


# python version string to test for
set(PYTHON_MIN_VERSION "${Python_FIND_VERSION_MAJOR}.${Python_FIND_VERSION_MINOR}")

# set the python version for the cpython api test
set(PYTHON_MIN_VERSION_HEX "0x0${Python_FIND_VERSION_MAJOR}0${Python_FIND_VERSION_MINOR}0000")

function(py_exec STATEMENTS RESULTVAR)
	# executes some python statement(s), and returns the result in RESULTVAR.
	# aborts with a fatal error on error.
	# no single quotes are allowed in STATEMENTS.
	execute_process(
		COMMAND "${PYTHON}" -c "${STATEMENTS}"
		OUTPUT_VARIABLE PY_OUTPUT
		RESULT_VARIABLE PY_RETVAL
	)

	if (NOT PY_RETVAL EQUAL 0)
		message(FATAL_ERROR "failed:\n${PYTHON} -c '${STATEMENTS}'\n${PY_OUTPUT}")
	endif()

	string(STRIP "${PY_OUTPUT}" PY_OUTPUT_STRIPPED)

	set("${RESULTVAR}" "${PY_OUTPUT_STRIPPED}" PARENT_SCOPE)
endfunction()

function(py_get_config_var VAR RESULTVAR)
	# uses py_exec to determine a config var as in distutils.sysconfig.get_config_var().
	py_exec(
		"from distutils.sysconfig import get_config_var; print(get_config_var('${VAR}'))"
		RESULT
	)

	set("${RESULTVAR}" "${RESULT}" PARENT_SCOPE)
endfunction()

function(find_python_interpreter_builtin)
	find_package(PythonInterp "${PYTHON_MIN_VERSION}" QUIET)
	if(PYTHONINTERP_FOUND)
		list(APPEND PYTHON_INTERPRETERS "${PYTHON_EXECUTABLE}")
	endif()
	set(PYTHON_INTERPRETERS "${PYTHON_INTERPRETERS}" PARENT_SCOPE)
endfunction()

function(find_python_interpreters_env)
	# execute `/usr/bin/env python`
	execute_process(
		COMMAND /usr/bin/env python3 -c "print(__import__('sys').executable, end='')"
		OUTPUT_VARIABLE SYSTEM_PYTHON_FROM_ENV
		RESULT_VARIABLE SYSTEM_PYTHON_RESULT
	)
	set(PYTHON_INTERPRETERS ${PYTHON_INTERPRETERS} "${SYSTEM_PYTHON_FROM_ENV}" PARENT_SCOPE)
endfunction()

function(find_python_interpreters)
	if(${ARGC} LESS 1)
		message(WARNING "find_python_interpreters requires atleast one pattern")
		return()
	endif()

	foreach(PATTERN ${ARGN})
		file(GLOB interpreter_glob "${PATTERN}")
		foreach(interpreter ${interpreter_glob})
			# resolve symlinks and get the full path of the interpreter
			get_filename_component(interpreter "${interpreter}" REALPATH)

			# the above globbing patterns might have caught some files
			# like /usr/bin/python-config; skip those.
			if("${interpreter}" MATCHES ".*-dbg$" OR NOT "${interpreter}" MATCHES "^.*/[^/]*-[^/]*$")
				list(APPEND PYTHON_INTERPRETERS "${interpreter}")
			endif()
		endforeach()
	endforeach()

	set(PYTHON_INTERPRETERS "${PYTHON_INTERPRETERS}" PARENT_SCOPE)
endfunction()

# collect a list of possible python interpreters from all sorts of sources,
# in the hope that one of them will have associated libs and headers.
set(PYTHON_INTERPRETERS)

# user-specified or from previous run, add it first
# so it has highest priority.
if(PYTHON)
	list(APPEND PYTHON_INTERPRETERS "${PYTHON}")
else()
	# From /usr/bin/env's
	find_python_interpreters_env()

	# From known python locations
	find_python_interpreters(
		# general POSIX / GNU paths
		"/usr/bin/python*"
		"/usr/local/bin/python*"
		# OSX-specific paths
		"/usr/local/Frameworks/Python.framework/Versions/*/bin/python*"
		"~/Library/Frameworks/Python.framework/Versions/*/bin/python*"
		"/System/Library/Frameworks/Python.framework/Versions/*/bin/python*"
	)

	if(NOT PYTHON_INTERPRETERS)
		# use cmake's built-in finder
		find_python_interpreter_builtin()
	endif()
endif()

# After resolving symlinks, the list of interpreters contains duplicates
list(REMOVE_DUPLICATES PYTHON_INTERPRETERS)

# Retain only the proper python interpreters
foreach(INTERPRETER ${PYTHON_INTERPRETERS})

	# python* matches pythontex.py, which we never ever want.
	if(INTERPRETER MATCHES "pythontex\[0-9]?\.py")
		list(REMOVE_ITEM PYTHON_INTERPRETERS "${INTERPRETER}")
		continue()
	endif()

	# test for validity of the interpreter
	set(PY_OUTPUT_TEST "rofl, lol")

	execute_process(COMMAND
		"${INTERPRETER}" -c "print('${PY_OUTPUT_TEST}'); exit(42)"
		OUTPUT_VARIABLE TEST_OUTPUT
		RESULT_VARIABLE TEST_RETVAL
	)

	if(NOT TEST_OUTPUT STREQUAL "${PY_OUTPUT_TEST}\n" OR NOT TEST_RETVAL EQUAL 42)
		# not a python interpreter
		message("-- Dropping invalid python interpreter '${INTERPRETER}'")
		list(REMOVE_ITEM PYTHON_INTERPRETERS "${INTERPRETER}")
	endif()
endforeach()

# test all the found interpreters; break on success.
foreach(PYTHON ${PYTHON_INTERPRETERS})

	# If the current python interpreter equals the one found at the very beginning with PythonInterp,
	# we can use the PythonLibs find-module to find the matching libraries.
	# Otherwise we ask that interpreter where its matching libraries are.
	if (PYTHON STREQUAL PYTHON_EXECUTABLE)
		find_package(PythonLibs REQUIRED)
	else()
		# ask the interpreter for the essential extension-building flags
		py_get_config_var(INCLUDEPY PYTHON_INCLUDE_DIRS)
		py_get_config_var(LIBDIR PYTHON_LIBRARY_DIR)
		py_get_config_var(LIBPL PYTHON_LIBPL)
		py_get_config_var(LDLIBRARY PYTHON_LIBRARY_NAME)
		py_get_config_var(VERSION PYTHON_VERSION)

		find_library(PYTHON_LIBRARIES "${PYTHON_LIBRARY_NAME}" "libpython${PYTHON_VERSION}.dylib"
			PATHS "${PYTHON_LIBRARY_DIR}" "${PYTHON_LIBPL}"
		)

	endif()

	# there's a static_assert that tests the Python version.
	# that way, we verify the interpreter and the library version.
	# (the interpreter provided us the library location)
	try_compile(PYTHON_TEST_RESULT
		"${CMAKE_BINARY_DIR}"
		SOURCES "${CMAKE_CURRENT_LIST_DIR}/FindPython_test.cpp"
		LINK_LIBRARIES ${PYTHON_LIBRARIES}
		CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${PYTHON_INCLUDE_DIRS}" "-DCMAKE_CXX_STANDARD=14"
		COMPILE_DEFINITIONS "-DTARGET_VERSION=${PYTHON_MIN_VERSION_HEX}"
		OUTPUT_VARIABLE PYTHON_TEST_OUTPUT
	)

	if(PYTHON_TEST_RESULT)
		message("-- Using python interpreter: ${PYTHON}")

		set(PYTHON_INTERP "${PYTHON}")
		break()
	else()
		set(PYTHON_TEST_ERRORS "${PYTHON_TEST_ERRORS}python candidate ${PYTHON}:\n${PYTHON_TEST_OUTPUT}\n\n")
	endif()
endforeach()

if(NOT PYTHON_INTERP)
	if(PYTHON_TEST_ERRORS)
		message("Python interpreter candidates:")
		message("${PYTHON_TEST_ERRORS}")
	endif()
	message("No suitable Python interpreter found.")
	message("We need an interpreter that is shipped with libpython and header files.")
	message("Specify your own with -DPYTHON=/path/to/executable\n\n\n")
	set(PYTHON_INTERP "")
	set(PYTHON_INCLUDE_DIRS "")
	set(PYTHON_LIBRARIES "")

	unset(PYTHON CACHE)
	unset(PYTHON_LIBRARIES CACHE)
	unset(PYTHON_INCLUDE_DIRS CACHE)
endif()

if(NOT PYTHON)
	# if python was not set before, force-set it now.
	set(LOL_FORCE "FORCE")
endif()

set(PYTHON "${PYTHON_INTERP}" CACHE FILEPATH "Location of the Python interpreter" ${LOL_FORCE})
set(PYTHON_LIBRARIES "${PYTHON_LIBRARIES}" CACHE STRING "Linker invocation for the Python library" ${LOL_FORCE})
set(PYTHON_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS}" CACHE PATH "Location of the Python include dir" ${LOL_FORCE})


unset(LOL_FORCE)
unset(PYTHON_TEST_RESULT)
unset(PYTHON_TEST_OUTPUT)
unset(PYTHON_INTERP)
mark_as_advanced(PYTHON_TEST_ERRORS)
mark_as_advanced(PYTHON_INTERPRETERS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Python REQUIRED_VARS PYTHON PYTHON_INCLUDE_DIRS PYTHON_LIBRARIES)
