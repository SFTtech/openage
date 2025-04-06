# Copyright 2015-2025 the openage authors. See copying.md for legal info.

# finds the python interpreter, install destination and extension flags.

# the Python version number requirement is in modules/FindPython_test.cpp
find_package(Python ${PYTHON_MIN_VERSION} REQUIRED)

find_package(Cython ${CYTHON_MIN_VERSION})
if(NOT CYTHON_FOUND)
	message("Checking for alternative Cython fallback version (>=${CYTHON_MIN_VERSION_FALLBACK} AND <=${CYTHON_MAX_VERSION_FALLBACK})")
	find_package(Cython ${CYTHON_MIN_VERSION_FALLBACK} QUIET)
	if(CYTHON_VERSION VERSION_LESS ${CYTHON_MIN_VERSION} AND CYTHON_VERSION VERSION_GREATER ${CYTHON_MAX_VERSION_FALLBACK})
		message(FATAL_ERROR "Cython version ${CYTHON_VERSION} is not compatible")
	else()
		message("Compatible Cython version ${CYTHON_VERSION} found")
	endif()
endif()

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
message("PYTHON_LIBRARIES: " "${PYTHON_LIBRARIES}")
#Windows always uses optimized version of Python lib
if(WIN32 AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	#get index of string "optimized" and increment it by 1 so index points at the path of the optimized lib
	list(FIND PYEXT_LIBRARY "optimized" _index)
	if(${_index} GREATER -1)
		MATH(EXPR _index "${_index}+1")
		list(GET PYEXT_LIBRARY ${_index} PYEXT_LIBRARY)
	endif()
	message("force linking to python release lib, instead of debug lib when cythonising")
	set(force_optimized_lib_flag "--force_optimized_lib")
endif()

set(PYEXT_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS};${NUMPY_INCLUDE_DIR}")

if(NOT CMAKE_PY_INSTALL_PREFIX)
	if(MSVC)
		set(CMAKE_PY_INSTALL_PREFIX "python")
	else()
		# get site-packages directory, prepended with cmake's install prefix
		py_exec("import sys, sysconfig, os; print(os.path.join('${CMAKE_INSTALL_PREFIX}', os.path.relpath(sysconfig.get_path('purelib'), os.path.normpath(sys.prefix))))" PREFIX)
		set(CMAKE_PY_INSTALL_PREFIX "${PREFIX}")
	endif()
endif()
