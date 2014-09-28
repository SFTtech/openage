# CMake module for test specification and registration
#
# provides generated C++ code for registering tests in a binary,
# these are tightly integrated with the testing code.


# enable testing
function(setup_testing)
	enable_testing()
endfunction()

# add a new test function to the list of available tests
function(add_c_test binary_name name description functionname interactive)
	message("${binary_name} test: ${name} - ${description}")

	list(FIND SFT_TESTS ${name} test_defined)
	if(NOT test_defined EQUAL -1)
		message(FATAL_ERROR "The test ${name} was already defined!")
	endif()

	set_property(GLOBAL APPEND PROPERTY SFT_TESTS ${name})
	set_property(GLOBAL PROPERTY SFT_TESTS_DESCRIPTION_${name} ${description})
	set_property(GLOBAL PROPERTY SFT_TESTS_FUNCTION_${name} ${functionname})
	set_property(GLOBAL PROPERTY SFT_TESTS_INTERACTIVE_${name} ${interactive})

	get_property(binary_path GLOBAL PROPERTY SFT_BINARY_PATH_${binary_name})

	if(NOT ${interactive})
		add_test(NAME ${name}
			COMMAND ${binary_path} --test ${name}
		)
	endif()
endfunction()

# add a test call to python code
function(add_py_test)
	message(ERROR "python tests unimplemented")
endfunction()

# add the gathered information to the given template file for
# registration, this generates C++ code.
function(register_test_cases binary_name register_file_in register_file)
	set(REGISTER_TESTS "// register all defined C++ tests\n")

	get_property(test_names GLOBAL PROPERTY SFT_TESTS)
	foreach(test ${test_names})
		get_property(description  GLOBAL PROPERTY SFT_TESTS_DESCRIPTION_${test})
		get_property(functionname GLOBAL PROPERTY SFT_TESTS_FUNCTION_${test})
		get_property(interactive  GLOBAL PROPERTY SFT_TESTS_INTERACTIVE_${test})
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
	endforeach()

	message("registering tests in ${register_file}")
	file(RELATIVE_PATH SOURCE_RELPATH
		"${CMAKE_CURRENT_BINARY_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}"
	)
	configure_file(
		${register_file_in}
		${CMAKE_CURRENT_BINARY_DIR}/${register_file}
	)

	# add the configured file to the source list.
	add_sources_absolute(
		${binary_name}
		${CMAKE_CURRENT_BINARY_DIR}/${register_file}
	)
endfunction()
