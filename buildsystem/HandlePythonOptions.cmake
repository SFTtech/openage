# Copyright 2015-2016 the openage authors. See copying.md for legal info.

# finds the python interpreter, install destination and extension flags.

# the Python version number requirement is in modules/FindPython_test.cpp
find_package(Python REQUIRED)
find_package(Cython 0.22 REQUIRED)

py_get_config_var(OPT PYEXT_CXXFLAGS)
py_get_config_var(EXT_SUFFIX PYEXT_SUFFIX)

# fix the CXXFLAGS
set(PYEXT_CXXFLAGS " ${PYEXT_CXXFLAGS} ") # padding required for the replacements below
# C++ doesn't have the -Wstrict-prototypes warning
string(REGEX REPLACE " -Wstrict-prototypes " " " PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS}")
# thanks, but I'd like to choose my debug mode myself.
string(REPLACE " -g " " " PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS}")

# add our own regular C++ flags
set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} ${CMAKE_CXX_FLAGS}")

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	# some things clang complains about
	set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-extended-offsetof")
	set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-unneeded-internal-declaration")

	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5)
		# doesn't have that warning yet
	else()
		# https://github.com/cython/cython/pull/402 (fix pending cython 0.23)
		set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-absolute-value")
	endif()
endif()

set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-unused-function")
set(PYEXT_LIBRARY "${PYTHON_LIBRARY}")
set(PYEXT_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}")

if(NOT CMAKE_PY_INSTALL_PREFIX)
	py_exec("import site; print(site.getsitepackages()[0])" PREFIX)
	set(CMAKE_PY_INSTALL_PREFIX "${PREFIX}")
endif()
