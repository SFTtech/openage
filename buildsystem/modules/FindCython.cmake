# Copyright 2015-2016 the openage authors. See copying.md for legal info.

# This module defines:
#
#  CYTHON_FOUND
#  CYTHON_VERSION          - e.g. '0.23dev'
#  CYTHON                  - invocation (using python)
#
# It depends on:
#
#  PYTHON3

# set the python version for the cpython api test
set(PYTHON_MIN_VERSION_HEX "0x0${Python3_VERSION_MAJOR}0${Python3_VERSION_MINOR}0000")

function(py_exec STATEMENTS RESULTVAR)
	# executes some python statement(s), and returns the result in RESULTVAR.
	# aborts with a fatal error on error.
	# no single quotes are allowed in STATEMENTS.
	execute_process(
		COMMAND "${Python3_EXECUTABLE}" -c "${STATEMENTS}"
		OUTPUT_VARIABLE PY_OUTPUT
		RESULT_VARIABLE PY_RETVAL
	)

	if (NOT PY_RETVAL EQUAL 0)
		message(FATAL_ERROR "failed:\n${Python3_EXECUTABLE} -c '${STATEMENTS}'\n${PY_OUTPUT}")
	endif()

	string(STRIP "${PY_OUTPUT}" PY_OUTPUT_STRIPPED)

	set("${RESULTVAR}" "${PY_OUTPUT_STRIPPED}" PARENT_SCOPE)
endfunction()

set(CYTHON "${Python3_EXECUTABLE} -m cython")
py_exec("import cython; print(cython.__version__)" CYTHON_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cython VERSION_VAR CYTHON_VERSION REQUIRED_VARS CYTHON)
