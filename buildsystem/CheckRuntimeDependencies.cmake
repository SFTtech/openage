# Copyright 2015-2016 the openage authors. See copying.md for legal info.

# python modules
# a list of imported modules may be obtained via
#
# grep -RE '^ *(import |from [^.])' | cut -d: -f2- | \
#     sed 's/^ *//g' | sort -u | grep -v openage
set(REQUIRED_PYTHON_MODULES "PIL.Image" "PIL.ImageDraw" "numpy" "pygments")

# command-line tools
set(REQUIRED_UTILITIES "opusenc")

# Checks if the specified python module exists
#
#     check_python_module_exists(<module> <exists>)
#         <module>: The python module.
#         <exists>: TRUE of FALSE based on the check.
function(check_python_module_exists MODULE EXISTS)
	set(STATEMENT "from importlib import import_module; import_module(\"${MODULE}\")")
	execute_process(
		COMMAND ${PYTHON} -c "${STATEMENT}"
		RESULT_VARIABLE PY_RESULT)

	if(PY_RESULT EQUAL 0)
		set(${EXISTS} TRUE PARENT_SCOPE)
	else()
		set(${EXISTS} FALSE PARENT_SCOPE)
	endif()
endfunction()

# loop through all required python modules to find them
foreach(_PYTHON_MODULE ${REQUIRED_PYTHON_MODULES})
	check_python_module_exists(${_PYTHON_MODULE} EXISTS)

	if(EXISTS)
		message(STATUS "Checking python3 module ${_PYTHON_MODULE} - Success")
	else()
		message(FATAL_ERROR "Checking python3 module ${_PYTHON_MODULE} - Not Found")
	endif()
endforeach()

# loop through all required utilities to find them
foreach(_UTILITY ${REQUIRED_UTILITIES})
	find_program(${_UTILITY}_EXECUTABLE
		NAMES "${_UTILITY}"
	)

	if(${_UTILITY}_EXECUTABLE)
		message(STATUS "Checking utility program ${_UTILITY} - Success")
	else()
		message(FATAL_ERROR "Checking utility program ${_UTILITY} - Not Found")
	endif()
endforeach()
