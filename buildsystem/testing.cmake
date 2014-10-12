# CMake module for test specification and registration
#
# provides generated C++ code for registering tests in a binary,
# these are tightly integrated with the testing code.


# enable testing
function(init_testing)
	enable_testing()
endfunction()

# add a new test function to the list of available tests
function(add_test_cpp binary_name name description functionname interactive)
	get_property(tests GLOBAL PROPERTY SFT_TESTS_CPP)
	list_contains(contained ${name} tests)
	if(contained)
		message(FATAL_ERROR "The test ${name} was already defined!")
	endif()

	set_property(GLOBAL APPEND PROPERTY SFT_TESTS_CPP ${name})
	set_property(GLOBAL PROPERTY SFT_TESTS_CPP_DESCRIPTION_${name} ${description})
	set_property(GLOBAL PROPERTY SFT_TESTS_CPP_FUNCTION_${name} ${functionname})
	set_property(GLOBAL PROPERTY SFT_TESTS_CPP_INTERACTIVE_${name} ${interactive})

	if(NOT ${interactive})
		add_test(NAME ${name}
			COMMAND ${binary_name} --test ${name}
		)
	endif()
endfunction()

# add a test call to a python function
# the relative path to the function needs to be given
# e.g. to call openage.convert.cabextract.tests.testcab(),
# add the following to py/openage/convert/cabextract/CMakeLists.txt:
#
#   add_py_test(tests.testcab "tests cabextract")
#
function(add_test_py name description)
	get_py_module_name(name ${name})

	set_property(GLOBAL APPEND PROPERTY SFT_TESTS_PY ${name})
	set_property(GLOBAL APPEND PROPERTY SFT_TESTS_PY_DESCRIPTION_${name} ${description})
endfunction()

# add the gathered information to the given template file for
# registration, this generates C++ code.
function(register_tests_cpp binary_name register_file_in register_file)
	set(REGISTER_TESTS "// register all defined C++ tests\n")

	message("tests for ${binary_name} (${register_file})")

	get_property(test_names GLOBAL PROPERTY SFT_TESTS_CPP)
	foreach(test ${test_names})
		get_property(description  GLOBAL PROPERTY SFT_TESTS_CPP_DESCRIPTION_${test})
		get_property(functionname GLOBAL PROPERTY SFT_TESTS_CPP_FUNCTION_${test})
		get_property(interactive  GLOBAL PROPERTY SFT_TESTS_CPP_INTERACTIVE_${test})
		set(REGISTER_TESTS "${REGISTER_TESTS}
	register_test(
		\"${test}\",
		\"${description}\",
		${interactive}, &tests::${functionname}
	);
")
		set(TEST_PROTOTYPES
			"${TEST_PROTOTYPES}\n\tbool ${functionname}(int argc, char **argv);"
		)

		message("\t${test}: ${description}")
	endforeach()

	message("")

	file(RELATIVE_PATH SOURCE_RELPATH
		"${CMAKE_CURRENT_BINARY_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}"
	)
	configure_file(
		${register_file_in}
		${CMAKE_CURRENT_BINARY_DIR}/${register_file}
	)

	# add the configured file to the source list.
	add_sources(${binary_name} ${CMAKE_CURRENT_BINARY_DIR}/${register_file})
endfunction()

function(register_tests_py)
	get_property(test_names GLOBAL PROPERTY SFT_TESTS_PY)

	set(testsfile "${GLOBAL_ASSET_DIR}/tests_python")
	file(WRITE ${testsfile} "")

	foreach(test ${test_names})
		get_property(description GLOBAL PROPERTY SFT_TESTS_PY_DESCRIPTION_${test})

		file(APPEND ${testsfile} "${test}:${description}\n")
	endforeach()
endfunction()

init_testing()
