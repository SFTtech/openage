# Copyright 2015-2025 the openage authors. See copying.md for legal info.

# Find Python
# ~~~~~~~~~~~
#
# Find the Python interpreter, and related info.
#
# This is a wrapper around FindPython3.cmake,
# which sets many more variables:
# https://cmake.org/cmake/help/latest/module/FindPython3.html
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

set(PYTHON_USED_VERSION "${PYTHON_MIN_VERSION}")

# You can manually pass the directory to an interpreter
# by defining PYTHON_DIR or passing -DPYTHON_DIR="<DIRECTORY>"
# to CMake. It's used as a hint where to look at
# https://cmake.org/cmake/help/latest/module/FindPython3.html#hints
if(PYTHON_DIR)
	set(Python3_ROOT_DIR "${PYTHON_DIR}")

elseif(Python3_ROOT_DIR OR Python3_EXECUTABLE)
	# python paths given directly

else()
	# when there are multiple pythons, preferrably use the version of
	# the default `python3` executable.
	execute_process(
		COMMAND "python3" -c "import platform; print(platform.python_version())"
		OUTPUT_VARIABLE PYVER_OUTPUT
		RESULT_VARIABLE PYVER_RETVAL
	)

	# if a system version exists, check if it's compatible with our min requirements
	if(PYVER_RETVAL EQUAL 0)
		string(REGEX MATCH "^[0-9]+\\.[0-9]+" PYTHON_USED_VERSION "${PYVER_OUTPUT}")

		if(PYTHON_USED_VERSION VERSION_GREATER_EQUAL PYTHON_MIN_VERSION)
			# set EXACT so we get the system version from find_package
			set(need_exact_version "EXACT")

		else()
			# search for alternatives if version doesn't fulfill min requirements
			set(PYTHON_USED_VERSION "${PYTHON_MIN_VERSION}")

		endif()
	endif()
endif()
###############################################################

# Never use the Windows Registry to find python
set(Python3_FIND_REGISTRY "NEVER")

# use cmake's FindPython3 to locate library and interpreter
find_package(Python3 ${PYTHON_USED_VERSION} ${need_exact_version} COMPONENTS Interpreter Development NumPy)

# python version string to cpython api test in modules/FindPython_test.cpp
# we use the solution from CPython's header (see https://github.com/SFTtech/openage/issues/1438#issuecomment-1036311012):
# define PY_VERSION_HEX ((PY_MAJOR_VERSION << 24) | \
#                       (PY_MINOR_VERSION << 16) | \
#                       (PY_MICRO_VERSION <<  8) | \
#                       (PY_RELEASE_LEVEL <<  4) | \
#                       (PY_RELEASE_SERIAL << 0))
math(EXPR BIT_SHIFT_HEX "${Python3_VERSION_MAJOR} << 24 | ${Python3_VERSION_MINOR} << 16" OUTPUT_FORMAT HEXADECIMAL)
set(PYTHON_MIN_VERSION_HEX "${BIT_SHIFT_HEX}")


# there's a static_assert that tests the Python version.
# that way, we verify the interpreter and the library version.
# (the interpreter provided us the library location)

if(WIN32 AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set(TEMP_CMAKE_TRY_COMPILE_CONFIGURATION ${CMAKE_TRY_COMPILE_CONFIGURATION})
	set(CMAKE_TRY_COMPILE_CONFIGURATION "Release")
endif()

try_compile(PYTHON_TEST_RESULT
	"${CMAKE_BINARY_DIR}"
	SOURCES "${CMAKE_CURRENT_LIST_DIR}/FindPython_test.cpp"
	LINK_LIBRARIES Python3::Python
	CXX_STANDARD ${CMAKE_CXX_STANDARD}
	COMPILE_DEFINITIONS "-DTARGET_VERSION=${PYTHON_MIN_VERSION_HEX}"
	OUTPUT_VARIABLE PYTHON_TEST_OUTPUT
)

if(WIN32 AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set(CMAKE_TRY_COMPILE_CONFIGURATION ${TEMP_CMAKE_TRY_COMPILE_CONFIGURATION})
endif()


if(NOT PYTHON_TEST_RESULT)
	message(STATUS "!! No suitable Python interpreter was found !!

We need a Python interpreter >= ${PYTHON_MIN_VERSION} that is shipped with libpython and header files.
You can tell the python find module where your python is installed:
  Executable location: -DPython3_EXECUTABLE=/path/to/executable
  Root directory: -DPython3_ROOT_DIR=/directory/of/executable/

We tried with Python ${PYTHON_FIND_VERSION}, but test compilation failed:

${PYTHON_TEST_OUTPUT}
")
	message(FATAL_ERROR "No suitable Python was found!")

elseif(PYTHON_TEST_RESULT)
	# Interfacing
	# Python.cmake vars <= Python3.cmake vars
	set(PYTHON ${Python3_EXECUTABLE} CACHE FILEPATH "Location of the Python interpreter" FORCE)
	set(PYTHON_FOUND ${Python3_Interpreter_FOUND})
	set(PYTHON_LIBRARIES ${Python3_LIBRARIES} CACHE STRING "Linker invocation for the Python library" FORCE)
	set(PYTHON_INCLUDE_DIRS ${Python3_INCLUDE_DIRS} CACHE PATH "Location of the Python include dir" FORCE)
	set(PYTHON_VERSION_STRING ${Python3_VERSION})

	# Numpy.cmake vars <= Python3.cmake vars
	set(NUMPY_FOUND ${Python3_NumPy_FOUND})
	set(NUMPY_VERSION ${Python3_NumPy_VERSION})
	set(NUMPY_INCLUDE_DIR ${Python3_NumPy_INCLUDE_DIRS} CACHE STRING "Linker invocation for the NumPy library" FORCE)

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
	# uses py_exec to determine a config var as in sysconfig.get_config_var().
	py_exec(
		"from sysconfig import get_config_var; print(get_config_var('${VAR}'))"
		RESULT
	)

	set("${RESULTVAR}" "${RESULT}" PARENT_SCOPE)
endfunction()
