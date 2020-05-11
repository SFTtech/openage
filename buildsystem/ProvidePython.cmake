# Copyright 2015-2020 the openage authors. See copying.md for legal info.

# finds the python interpreter, install destination and extension flags.

# the Python version number requirement is in modules/FindPython_test.cpp
find_package(Python ${PYTHON_MIN_VERSION} REQUIRED)
find_package(Cython ${CYTHON_MIN_VERSION} REQUIRED)

py_get_config_var(EXT_SUFFIX PYEXT_SUFFIX)

set(PYEXT_LIBRARY "${PYTHON_LIBRARIES}")
set(PYEXT_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS};${NUMPY_INCLUDE_DIR}")

if(NOT CMAKE_PY_INSTALL_PREFIX)
	# TODO: Revisit to check with Windows CI if install prefix is used correctly
	py_exec("from distutils.sysconfig import get_python_lib; print(get_python_lib())" PREFIX)
	set(CMAKE_PY_INSTALL_PREFIX "${PREFIX}")
endif()
