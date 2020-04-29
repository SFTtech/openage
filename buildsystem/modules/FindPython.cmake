# Copyright 2015-2020 the openage authors. See copying.md for legal info.

# Find Python
# ~~~~~~~~~~~
#
# Find the Python interpreter, and related info.
#
# This is a wrapper around FindPython3.cmake,
# which sets many more variables:
# https://cmake.org/cmake/help/v3.12/module/FindPython3.html
#
# This file defines the following variables:
#
# PYTHON_FOUND        - True when python was found.
# PYTHON              - The full path to the Python interpreter.
# PYTHON_INCLUDE_DIRS - Include path for Python extensions.
# PYTHON_LIBRARIES    - Library and Linker options for Python extensions.
#
# Also defines py_exec and py_get_config_var.


###############################################################
# You can manually pass the directory to an interpreter
# by defining PYTHON_DIR or passing -DPYTHON_DIR="<DIRECTORY>"
# to CMake. It's used as a hint where to look at
if(PYTHON_DIR)
	set(Python3_ROOT_DIR "${PYTHON_DIR}")
endif()
###############################################################


# Never use the Windows Registry to find python
set(Python3_FIND_REGISTRY "NEVER")

# use cmake's FindPython3 to locate library and interpreter
find_package(Python3 ${PYTHON_MIN_VERSION} COMPONENTS Interpreter Development)


# python version string to cpython api test in modules/FindPython_test.cpp
set(PYTHON_MIN_VERSION_HEX "0x0${Python3_VERSION_MAJOR}0${Python3_VERSION_MINOR}0000")

# there's a static_assert that tests the Python version.
# that way, we verify the interpreter and the library version.
# (the interpreter provided us the library location)
try_compile(PYTHON_TEST_RESULT
	"${CMAKE_BINARY_DIR}"
	SOURCES "${CMAKE_CURRENT_LIST_DIR}/FindPython_test.cpp"
	LINK_LIBRARIES Python3::Python
	CXX_STANDARD 17
	COMPILE_DEFINITIONS "-DTARGET_VERSION=${PYTHON_MIN_VERSION_HEX}"
	OUTPUT_VARIABLE PYTHON_TEST_OUTPUT
)

if(NOT PYTHON_TEST_RESULT)
	message(WARNING "!! No suitable Python interpreter was found !!\n")
	message(WARNING "We need a Python interpreter >= 3.6 that is shipped with libpython and header files.\n")
	message(WARNING "Specify the directory to your own with -DPYTHON_DIR=/dir/of/executable\n\n\n")

elseif(PYTHON_TEST_RESULT)
	# Interfacing
	# Python.cmake vars <= Python3.cmake vars
	set(PYTHON ${Python3_EXECUTABLE} CACHE FILEPATH "Location of the Python interpreter" FORCE)
	set(PYTHON_FOUND ${Python3_Interpreter_FOUND})
	set(PYTHON_LIBRARIES ${Python3_LIBRARIES} CACHE STRING "Linker invocation for the Python library" FORCE)
	set(PYTHON_INCLUDE_DIRS ${Python3_INCLUDE_DIRS} CACHE PATH "Location of the Python include dir" FORCE)
	set(PYTHONLIBS_VERSION_STRING ${Python3_VERSION})

	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(Python REQUIRED_VARS PYTHON PYTHON_INCLUDE_DIRS PYTHON_LIBRARIES)
endif()

unset(PYTHON_TEST_RESULT)
unset(PYTHON_TEST_OUTPUT)


# helper functions

function(py_exec STATEMENTS RESULTVAR)
	# executes some python statement(s), and returns the result in RESULTVAR.
	# aborts with a fatal error on error.
	# no single quotes are allowed in STATEMENTS.
	execute_process(
		COMMAND "${PYTHON}" -c "${STATEMENTS}"
		OUTPUT_VARIABLE PY_OUTPUT
		RESULT_VARIABLE PY_RETVAL
	)

	if(NOT PY_RETVAL EQUAL 0)
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
