# Copyright 2014-2014 the openage authors. See copying.md for legal info.

# CMake module for test specification
#
# adds language-specific test invocations and writes test lists to assets (tests_*)
# tightly integrated with language-specific testing code
# in the case of C++, the test list asset is used during code generation

# enable testing
function(init_testing)
	enable_testing()
endfunction()

function(sft_add_test lang type name desc)
	# sanitize inputs
	if(NOT type STREQUAL test AND NOT type STREQUAL demo)
		message(FATAL_ERROR "Unknown test type: ${type}")
	endif()
	if(NOT lang STREQUAL cpp AND NOT lang STREQUAL py)
		message(FATAL_ERROR "Unknown test language: ${lang}")
	endif()

	# check whether tests for ${lang} have been finalized already
	get_property(finalized GLOBAL PROPERTY "SFT_TESTS_${lang}_FINALIZED")
	if(finalized)
		message(FATAL_ERROR "Test specifications for ${lang} have been finalized!")
	endif()

	# check whether that test has been specified already
	get_property(tests GLOBAL PROPERTY "SFT_TESTS_${lang}")
	list_contains(contained "${name}" tests)
	if(contained)
		message(FATAL_ERROR "The ${lang} test ${name} has already been defined!")
	endif()

	# append test to list
	set_property(GLOBAL APPEND PROPERTY "SFT_TESTS_${lang}" "${name}")
	set_property(GLOBAL PROPERTY "SFT_TESTS_${lang}_TYPE_${name}" "${type}")
	set_property(GLOBAL PROPERTY "SFT_TESTS_${lang}_DESCRIPTION_${name}" "${description}")

	# add test to CTest
	if(type STREQUAL test AND lang STREQUAL cpp)
		add_test(NAME "${name}" COMMAND "${PROJECT_NAME}" --test "${name}")
	elseif(type STREQUAL test AND lang STREQUAL py)
		add_test(NAME "${name}" COMMAND ${PYTHON_INVOCATION} -m openage.testing --test "${name}")
	endif()
endfunction()

# usability wrappers
function(add_test_cpp name description)
	sft_add_test(cpp test "${name}" "${description}")
endfunction()

function(add_demo_cpp name description)
	sft_add_test(cpp demo "${name}" "${description}")
endfunction()

function(add_test_py name description)
	sft_add_test(py test "${name}" "${description}")
endfunction()

function(add_demo_py name description)
	sft_add_test(py demo "${name}" "${description}")
endfunction()

function(finalize_tests lang)
	get_property(test_names GLOBAL PROPERTY "SFT_TESTS_${lang}")
	set_property(GLOBAL PROPERTY "SFT_TESTS_${lang}_FINALIZED" 1)

	set(testsfile "${GLOBAL_ASSET_DIR}/tests_${lang}")
	file(WRITE "${testsfile}" "")

	message("${lang} tests")

	foreach(test ${test_names})
		get_property(type GLOBAL PROPERTY "SFT_TESTS_${lang}_TYPE_${test}")
		get_property(description GLOBAL PROPERTY "SFT_TESTS_${lang}_DESCRIPTION_${test}")

		if(type STREQUAL demo)
			set(demostr "[demo]")
		else()
			set(demostr "")
		endif()

		message("${demostr}\t${test}: ${description}")

		file(APPEND "${testsfile}" "${test} ${type} ${description}\n")
	endforeach()

	message("")
endfunction()

init_testing()
