# provides detect_version, which will store the project version to PROJECT_VERSION.

function(init_version)
	set(res 1)
	if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/.git)
		# if .git exists, try running git describe
		execute_process(COMMAND git describe WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" RESULT_VARIABLE res OUTPUT_VARIABLE version ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	endif()

	if(NOT res EQUAL 0)
		# if git describe didn't run or failed, read openage_version
		file(STRINGS "${CMAKE_SOURCE_DIR}/openage_version" version)
	endif()

	if(NOT version)
		message(FATAL_ERROR "failed to determine project version")
	endif()

	message("${PROJECT_NAME} ${version}\n")

	set(PROJECT_VERSION ${version} PARENT_SCOPE)
endfunction()

init_version()
