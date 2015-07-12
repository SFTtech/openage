# Copyright 2015-2015 the openage authors. See copying.md for legal info.

# Find Python
# ~~~~~~~~~~~
# Find the Python interpreter, and related info.
#
# This file defines the following variables:
#
# PYTHON_FOUND       - True when python was found.
# PYTHON             - The full path to the Python interpreter.
# PYTHON_INCLUDE_DIR - Include path for Python extensions.
# PYTHON_LIBRARY     - Linker flags for Python extensions.
#
# Also defines py_exec and py_get_config_var.
#
# You can manually pass an interpreter by defining PYTHON.

function(py_exec STATEMENTS RESULTVAR)
	# executes some python statement(s), and returns the result in RESULTVAR.
	# aborts with a fatal error on error.
	# no single quotes are allowed in STATEMENTS.
	exec_program(
		"${PYTHON}" ARGS -c "'${STATEMENTS}'"
		OUTPUT_VARIABLE PY_OUTPUT
		RETURN_VALUE PY_RETVAL
	)

	if (NOT PY_RETVAL EQUAL 0)
		message(FATAL_ERROR "failed:\n${PYTHON} -c '${STATEMENTS}'\n${PY_OUTPUT}")
	endif()

	set("${RESULTVAR}" "${PY_OUTPUT}" PARENT_SCOPE)
endfunction()


function(py_get_config_var VAR RESULTVAR)
	# uses py_exec to determine a config var as in distutils.sysconfig.get_config_var().
	py_exec(
		"from distutils.sysconfig import get_config_var; print(get_config_var(\"${VAR}\"))"
		RESULT
	)

	set("${RESULTVAR}" "${RESULT}" PARENT_SCOPE)
endfunction()

# collect a list of possible python interpreters from all sorts of sources,
# in the hope that one of them will have associated libs and headers.
set(PYTHON_INTERPRETERS)

# user-specified or from previous run
if(PYTHON)
	list(APPEND PYTHON_INTERPRETERS "${PYTHON}")
endif()

# cmake's built-in finder
find_package(PythonInterp 3.4 QUIET)
if(PYTHONINTERP_FOUND)
	list(APPEND PYTHON_INTERPRETERS "${PYTHON_EXECUTABLE}")
endif()
unset(PYTHONINTERP_FOUND)
unset(PYTHON_EXECUTABLE)
unset(PYTHON_VERSION_STRING)
unset(PYTHON_VERSION_MAJOR)
unset(PYTHON_VERSION_MINOR)
unset(PYTHON_VERSION_PATCH)

# general POSIX / GNU paths
file(GLOB _tmp "/usr/bin/python*" "/usr/local/bin/python*")
list(APPEND PYTHON_INTERPRETERS ${_tmp})

# OSX-specific paths
foreach(dirname
	/usr/local/Frameworks
	~/Library/Frameworks
	/System/Library/Frameworks
)
	file(GLOB _tmp "${dirname}/Python.framework/Versions/*/bin/python*")
	list(APPEND PYTHON_INTERPRETERS ${_tmp})
endforeach()

# test all the found interpreters; break on success.
unset(PYTHON_TEST_ERRORS)
unset(PYTHON_INTERP)
foreach(PYTHON ${PYTHON_INTERPRETERS})
	# the above globbing patterns might have caught some files
	# like /usr/bin/python-config; skip those.
	if(PYTHON MATCHES ".*-dbg$" OR NOT PYTHON MATCHES "^.*/[^/]*-[^/]*$")

		# ask the interpreter for the essential extension-building flags
		py_get_config_var(INCLUDEPY PYTHON_INCLUDE_DIR)
		py_get_config_var(BLDLIBRARY PYTHON_LIBRARY)

		# there's a static_assert that tests the Python version.
		try_compile(PYTHON_TEST_RESULT
			"${CMAKE_BINARY_DIR}"
			"${CMAKE_CURRENT_LIST_DIR}/FindPython_test.cpp"
			LINK_LIBRARIES "${PYTHON_LIBRARY}"
			CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${PYTHON_INCLUDE_DIR}"
			OUTPUT_VARIABLE PYTHON_TEST_OUTPUT
		)

		if(PYTHON_TEST_RESULT)
			set(PYTHON_INTERP "${PYTHON}")
			break()
		else()
			set(PYTHON_TEST_ERRORS "${PYTHON_TEST_ERRORS}candidate ${PYTHON}:\n${PYTHON_TEST_OUTPUT}\n\n")
		endif()

	endif()
endforeach()

if(PYTHON_INTERP)
	set(PYTHON "${PYTHON_INTERP}")
else()
	message("No suitable Python interpreter found.")
	message("We need an interpreter that is shipped with libpython and header files.")
	message("Specify your own with -DPYTHON=/path/to/file")
	message("${PYTHON_TEST_ERRORS}")
	set(PYTHON "")
	set(PYTHON_INCLUDE_DIR "")
	set(PYTHON_LIBRARY "")
endif()

unset(PYTHON_TEST_RESULT)
unset(PYTHON_TEST_OUTPUT)
unset(PYTHON_INTERP)
unset(_tmp)
mark_as_advanced(PYTHON_TEST_ERRORS)
mark_as_advanced(PYTHON_INTERPRETERS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Python REQUIRED_VARS PYTHON PYTHON_INCLUDE_DIR PYTHON_LIBRARY)
