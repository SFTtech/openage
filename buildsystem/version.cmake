# provides detect_version, which will store the project version to PROJECT_VERSION.

function(detect_version)
	execute_process(COMMAND git describe WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" RESULT_VARIABLE res OUTPUT_VARIABLE out ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(res EQUAL 0)
		set(PROJECT_VERSION ${out} PARENT_SCOPE)
	else()
		file(STRINGS "${CMAKE_SOURCE_DIR}/version" version_file)
		set(PROJECT_VERSION ${version_file} PARENT_SCOPE)
	endif()
endfunction()
