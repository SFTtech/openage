# Copyright 2015-2016 the openage authors. See copying.md for legal info.

# Find Python
# ~~~~~~~~~~~
# Find the Python interpreter, and related info.
#
# This file defines the following variables:
#
# PYTHON_FOUND        - True when python was found.
# PYTHON              - The full path to the Python interpreter.
# PYTHON_INCLUDE_DIR  - Include path for Python extensions.
# PYTHON_LIBRARY      - Library and Linker options for Python extensions.
#
# Also defines py_exec and py_get_config_var.
#
# You can manually pass an interpreter by defining PYTHON.

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

function(py_get_lib_name RESULTVAR)
	# uses py_exec to compute Python's C/C++ library name, just like python-config does.
	py_get_config_var(VERSION PYTHON_VERSION)
	if(NOT "${PYTHON_VERSION}" VERSION_LESS "3.2")
		py_exec(
			"import sys; print(sys.abiflags)"
			ABIFLAGS
		)
	else()
		set(ABIFLAGS, "")
	endif()

	set("${RESULTVAR}" "python${PYTHON_VERSION}${ABIFLAGS}" PARENT_SCOPE)
endfunction()

function(find_python_interpreter_builtin)
	find_package(PythonInterp 3.4 QUIET)
	if(PYTHONINTERP_FOUND)
		list(APPEND PYTHON_INTERPRETERS "${PYTHON_EXECUTABLE}")
	endif()
	set(PYTHON_INTERPRETERS "${PYTHON_INTERPRETERS}" PARENT_SCOPE)
endfunction()

function(find_python_interpreters)
	if(${ARGC} LESS 1)
		message(WARNING "find_python_interpreters requires atleast one pattern")
		return()
	endif()

	foreach(PATTERN ${ARGN})
		file(GLOB _INTERPRETERS "${PATTERN}")
		foreach(_TMP_INTERPRETER ${_INTERPRETERS})
			# resolve symlinks and get the full path of the interpreter
			get_filename_component(_INTERPRETER ${_TMP_INTERPRETER} REALPATH)

			# the above globbing patterns might have caught some files
			# like /usr/bin/python-config; skip those.
			if(${_INTERPRETER} MATCHES ".*-dbg$" OR NOT ${_INTERPRETER} MATCHES "^.*/[^/]*-[^/]*$")
				list(APPEND PYTHON_INTERPRETERS ${_INTERPRETER})
			endif()
		endforeach()
	endforeach()

	set(PYTHON_INTERPRETERS "${PYTHON_INTERPRETERS}" PARENT_SCOPE)
endfunction()

# collect a list of possible python interpreters from all sorts of sources,
# in the hope that one of them will have associated libs and headers.
set(PYTHON_INTERPRETERS)

# user-specified or from previous run
if(PYTHON)
	list(APPEND PYTHON_INTERPRETERS "${PYTHON}")
endif()

# From cmake's built-in finder
find_python_interpreter_builtin()

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

# After resolving symlinks, the list of interpreters contains duplicates
list(REMOVE_DUPLICATES PYTHON_INTERPRETERS)

# Retain only the proper python interpreters
foreach(INTERPRETER ${PYTHON_INTERPRETERS})
	set(PY_OUTPUT_TEST "rofl, lol")
	execute_process(COMMAND
		"${INTERPRETER}" -c "print('${PY_OUTPUT_TEST}'); exit(42)"
		OUTPUT_VARIABLE TEST_OUTPUT
		RESULT_VARIABLE TEST_RETVAL
	)
	if(NOT TEST_OUTPUT STREQUAL "${PY_OUTPUT_TEST}\n" OR NOT TEST_RETVAL EQUAL 42)
		message(WARNING "Dropping invalid python interpreter '${INTERPRETER}'")
		list(REMOVE_ITEM PYTHON_INTERPRETERS "${INTERPRETER}")
	endif()
endforeach()

# test all the found interpreters; break on success.
foreach(PYTHON ${PYTHON_INTERPRETERS})
	# TODO: sort interpreters by version

	# ask the interpreter for the essential extension-building flags
	py_get_config_var(INCLUDEPY PYTHON_INCLUDE_DIR)
	py_get_config_var(LIBDIR PYTHON_LIBRARY_DIR)
	py_get_lib_name(PYTHON_LIBRARY_NAME)

	# there's a static_assert that tests the Python version.
	try_compile(PYTHON_TEST_RESULT
		"${CMAKE_BINARY_DIR}"
		SOURCES "${CMAKE_CURRENT_LIST_DIR}/FindPython_test.cpp"
		LINK_LIBRARIES "${PYTHON_LIBRARY_NAME}"
		CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${PYTHON_INCLUDE_DIR}" "-DLINK_DIRECTORIES=${PYTHON_LIBRARY_DIR}"
		OUTPUT_VARIABLE PYTHON_TEST_OUTPUT
	)

	if(PYTHON_TEST_RESULT)
		message("-- Looking for suitable python3 - Success: ${PYTHON}")
		set(PYTHON_INTERP "${PYTHON}")
		set(PYTHON_LIBRARY "-l${PYTHON_LIBRARY_NAME} -L${PYTHON_LIBRARY_DIR}")
		break()
	else()
		set(PYTHON_TEST_ERRORS "${PYTHON_TEST_ERRORS}python candidate ${PYTHON}:\n${PYTHON_TEST_OUTPUT}\n\n")
	endif()
endforeach()

if(PYTHON_INTERP)
	set(PYTHON "${PYTHON_INTERP}")
else()
	message("Python interpreter candidates:")
	message("${PYTHON_TEST_ERRORS}")
	message("No suitable Python interpreter found.")
	message("We need an interpreter that is shipped with libpython and header files.")
	message("Specify your own with -DPYTHON=/path/to/executable\n\n\n")
	set(PYTHON "")
	set(PYTHON_INCLUDE_DIR "")
	set(PYTHON_LIBRARY "")
endif()

unset(PYTHON_TEST_RESULT)
unset(PYTHON_TEST_OUTPUT)
unset(PYTHON_INTERP)
mark_as_advanced(PYTHON_TEST_ERRORS)
mark_as_advanced(PYTHON_INTERPRETERS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Python REQUIRED_VARS PYTHON PYTHON_INCLUDE_DIR PYTHON_LIBRARY)
