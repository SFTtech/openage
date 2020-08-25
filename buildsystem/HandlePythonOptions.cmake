# Copyright 2015-2020 the openage authors. See copying.md for legal info.

# finds the python interpreter, install destination and extension flags.

# the Python version number requirement is in modules/FindPython_test.cpp
find_package(Python ${PYTHON_MIN_VERSION} REQUIRED)
find_package(Cython ${CYTHON_MIN_VERSION} REQUIRED)

py_get_config_var(EXT_SUFFIX PYEXT_SUFFIX)
if(MINGW)
	string(REGEX REPLACE "dll" "pyd" PYEXT_SUFFIX "${PYEXT_SUFFIX}")
endif()

# This is the only useful thing after cleaning up what python suggests
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(PYEXT_CXXFLAGS "-fwrapv")
endif()


# numpy deprecated api
# http://docs.cython.org/en/latest/src/userguide/source_files_and_compilation.html#configuring-the-c-build
if(CYTHON_VERSION VERSION_GREATER_EQUAL 3)
	set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION")
else()

	# suppress #warning about deprecated numpy api
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-cpp")
	elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-#warnings")
	endif()
endif()

# silence cython+python3.8 tp_print deprecation warning
# https://github.com/cython/cython/pull/3201
# https://github.com/cython/cython/issues/3474
if(PYTHON_VER VERSION_GREATER_EQUAL 3.8 AND PYTHON_VERSION VERSION_LESS 3.9)
	set(PYEXT_CXXCLAGS "${PYEXT_CXXCLAGS}" "-Wno-deprecated-declarations")
endif()

set(PYEXT_LIBRARY "${PYTHON_LIBRARIES}")
set(PYEXT_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS};${NUMPY_INCLUDE_DIR}")

if(NOT CMAKE_PY_INSTALL_PREFIX)
	if(MSVC)
		set(CMAKE_PY_INSTALL_PREFIX "python")
	else()
		py_exec("from distutils.sysconfig import get_python_lib; print(get_python_lib())" PREFIX)
		set(CMAKE_PY_INSTALL_PREFIX "${PREFIX}")
	endif()
endif()
