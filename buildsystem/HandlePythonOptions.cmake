# Copyright 2015-2017 the openage authors. See copying.md for legal info.

# finds the python interpreter, install destination and extension flags.

# the Python version number requirement is in modules/FindPython_test.cpp
find_package(Python ${PYTHON_MIN_VERSION} REQUIRED)
find_package(Cython ${CYTHON_MIN_VERSION} REQUIRED)
find_package(Numpy REQUIRED)

py_get_config_var(EXT_SUFFIX PYEXT_SUFFIX)

# This is the only useful thing after cleaning up what python suggests
set(PYEXT_CXXFLAGS "-fwrapv")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# suppress #warning about deprecated numpy api
	set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-cpp")
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	# suppress #warning about deprecated numpy api
	set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-#warnings")
endif()

set(PYEXT_LIBRARY "${PYTHON_LIBRARIES}")
set(PYEXT_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS};${NUMPY_INCLUDE_DIR}")

if(NOT CMAKE_PY_INSTALL_PREFIX)
	if(MSVC)
		set(CMAKE_PY_INSTALL_PREFIX "python")
	else()
		py_exec("import site, os; print(os.path.normpath(site.getsitepackages()[0]))" PREFIX)
		set(CMAKE_PY_INSTALL_PREFIX "${PREFIX}")
	endif()
endif()
