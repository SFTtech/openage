# CMake module for test specification
#
# adds language-specific test invocations and writes test lists to assets (tests_*)
# tightly integrated with language-specific testing code
# in the case of C++, the test list asset is used during code generation

# enable testing
function(init_testing)
	enable_testing()
endfunction()

function(add_test_lang lang name desc)
	get_property(finalized GLOBAL PROPERTY SFT_TESTS_${lang}_FINALIZED)
	if(finalized)
		message(FATAL_ERROR "Test specifications for ${lang} have been finalized!")
	endif()

	get_property(tests GLOBAL PROPERTY SFT_TESTS_${lang})
	list_contains(contained ${name} tests)
	if(contained)
		message(FATAL_ERROR "The ${lang} test ${name} has already been defined!")
	endif()

	set_property(GLOBAL APPEND PROPERTY SFT_TESTS_${lang} ${name})
	set_property(GLOBAL PROPERTY SFT_TESTS_${lang}_DESCRIPTION_${name} ${description})

	if(lang STREQUAL cpp)
		add_test(NAME ${name} COMMAND ${PROJECT_NAME} --test ${name})
	elseif(lang STREQUAL py)
		add_test(NAME ${name} COMMAND ${PYTHON_INVOCATION} -m openage.testing --test ${name})
	else()
		message(FATAL_ERROR "Unknown test language: ${lang}")
	endif()
endfunction()

function(add_test_cpp name description)
	add_test_lang(cpp ${name} ${description})
endfunction()

function(add_test_py name description)
	add_test_lang(py ${name} ${description})
endfunction()

function(finalize_tests lang)
	get_property(test_names GLOBAL PROPERTY SFT_TESTS_${lang})
	set_property(GLOBAL PROPERTY SFT_TESTS_${lang}_FINALIZED 1)

	set(testsfile "${GLOBAL_ASSET_DIR}/tests_${lang}")
	file(WRITE ${testsfile} "")

	message("${lang} tests")

	foreach(test ${test_names})
		get_property(description GLOBAL PROPERTY SFT_TESTS_${lang}_DESCRIPTION_${test})

		message("\t${test}: ${description}")

		file(APPEND ${testsfile} "${test} ${description}\n")
	endforeach()

	message("")
endfunction()

init_testing()
