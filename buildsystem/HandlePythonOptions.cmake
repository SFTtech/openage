# Copyright 2015-2017 the openage authors. See copying.md for legal info.

# finds the python interpreter, install destination and extension flags.

# the Python version number requirement is in modules/FindPython_test.cpp
find_package(Python 3.4 REQUIRED)
find_package(Cython 0.25 REQUIRED)
find_package(Numpy REQUIRED)

py_get_config_var(OPT PYEXT_CXXFLAGS)
py_get_config_var(EXT_SUFFIX PYEXT_SUFFIX)

# fix the CXXFLAGS
set(PYEXT_CXXFLAGS " ${PYEXT_CXXFLAGS} ") # padding required for the replacements below
# C++ doesn't have the -Wstrict-prototypes warning
string(REGEX REPLACE " -Wstrict-prototypes " " " PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS}")
# nope, we're not building with -Wall because I don't like numpy api warnings...
string(REGEX REPLACE " -Wall " " " PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS}")
# thanks, but I'd like to choose my debug mode myself.
string(REPLACE " -g " " " PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# suppress #warning about deprecated numpy api
	set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-cpp")
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	# suppress #warning about deprecated numpy api
	set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-#warnings")
endif()

# add our own regular C++ flags
set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} ${CMAKE_CXX_FLAGS}")

set(PYEXT_LIBRARY "${PYTHON_LIBRARY}")
set(PYEXT_INCLUDE_DIRS "${PYTHON_INCLUDE_DIR};${NUMPY_INCLUDE_DIR}")

if(NOT CMAKE_PY_INSTALL_PREFIX)
	py_exec("import site, os; print(os.path.normpath(site.getsitepackages()[0]))" PREFIX)
	set(CMAKE_PY_INSTALL_PREFIX "${PREFIX}")
endif()
