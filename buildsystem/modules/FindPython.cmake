# Find Python
# ~~~~~~~~~~~
# Find the Python interpreter and related Python directories.
#
# This file defines the following variables:
#
# PYTHON_FOUND       - True when python was found
#
# PYTHON_EXECUTABLE  - The full path to the Python interpreter.
#
# PYTHON_INCLUDE_DIR - Directory holding the include files.
#
# PYTHON_LIBRARY     - Location of the Python library.

function(find_frameworks list framework_name)
	set(_hints
			/usr/local/Frameworks
			~/Library/Frameworks
			/System/Library/Frameworks
	)
	foreach(_hint ${_hints})
		set(_current_folder "${_hint}/${framework_name}.framework")
		if(IS_DIRECTORY ${_current_folder})
			SET(_list ${_list} ${_current_folder})
		endif()
	endforeach()
	set(${list} ${_list} PARENT_SCOPE)
endfunction()

function(find_python_framework framework_paths)
	find_frameworks(PYTHON_FRAMEWORKS "Python")
	foreach(_framework_path ${PYTHON_FRAMEWORKS})
		file(GLOB children RELATIVE ${_framework_path}/Versions ${_framework_path}/Versions/*)
		foreach(child ${children})
			if(IS_DIRECTORY ${_framework_path}/Versions/${child})
				if((${Python_FIND_VERSION} VERSION_EQUAL ${child}) OR ((NOT Python_FIND_VERSION_EXACT) AND (NOT ${Python_FIND_VERSION} VERSION_GREATER ${child})))
					SET(_matching_framework_paths ${_matching_framework_paths} "${_framework_path}/Versions/${child}")
				endif()
			endif()
		endforeach()
	endforeach()
	set(${framework_paths} ${_matching_framework_paths} PARENT_SCOPE)
endfunction()

if (EXISTS "${PYTHON_EXECUTABLE}" AND EXISTS "${PYTHON_INCLUDE_DIR}" AND EXISTS "${PYTHON_LIBRARY}")
	set(PYTHON_FOUND TRUE)
endif()
if(APPLE AND NOT PYTHON_FOUND)
	find_python_framework(PYTHON_FRAMEWORK_PATHS)
	if(PYTHON_FRAMEWORK_PATHS)
		list(GET PYTHON_FRAMEWORK_PATHS 0 PYTHON_FRAMEWORK)
		get_filename_component(PYTHON_VERSION "${PYTHON_FRAMEWORK}" NAME)
		set(PYTHON_EXECUTABLE "${PYTHON_FRAMEWORK}/bin/python${PYTHON_VERSION}")
		set(PYTHON_INCLUDE_DIR "${PYTHON_FRAMEWORK}/Headers")
		find_library(PYTHON_LIBRARY "python${PYTHON_VERSION}" PATHS "${PYTHON_FRAMEWORK}/lib")
		if(EXISTS "${PYTHON_EXECUTABLE}" AND EXISTS "${PYTHON_INCLUDE_DIR}" AND EXISTS "${PYTHON_LIBRARY}")
			set(PYTHON_FOUND TRUE)
		endif()
	endif()
endif()
if(NOT PYTHON_FOUND)
	set(Python_ADDITIONAL_VERSIONS 3.4)
	# find py libs
	find_package(PythonLibs 3.3 REQUIRED)
	# on some distros (ubuntu 13.10) there seems to be additional clutter at the end,
	# which needs to be stripped to prevent find_package(PythonInterp) from erroring.
	if(PYTHONLIBS_VERSION_STRING MATCHES "^([0-9]+\\.[0-9]+\\.[0-9]+)([^0-9].*)?$")
		set(PYTHON_SANITIZED_VERSION "${CMAKE_MATCH_1}")
	else()
		message(WARNING "PYTHONLIBS_VERSION_STRING is ${PYTHONLIBS_VERSION_STRING}, expected major.minor.patch*")
		set(PYTHON_SANITIZED_VERSION "${PYTHONLIBS_VERSION_STRING}")
	endif()
	# look for the py interpreter that fits to the py libs
	find_package(PythonInterp "${PYTHON_SANITIZED_VERSION}" EXACT REQUIRED)
	if(PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND)
		set(PYTHON_FOUND TRUE)
	endif()
endif()
if(Python_FIND_REQUIRED AND NOT PYTHON_FOUND)
	if(Python_FIND_VERSION_EXACT)
		message(FATAL_ERROR "Could not find Python exactly of version ${Python_FIND_VERSION}.")
	else()
		message(FATAL_ERROR "Could not find Python of version ${Python_FIND_VERSION} or greater.")
	endif()
endif()
