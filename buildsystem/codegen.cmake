# Copyright 2014-2014 the openage authors. See copying.md for legal info.

function(codegen_run)
	# this function must be called once all required assets have been created, but before the executable is finalized.

	# make sure this function gets called only once
	get_property(codegen_run GLOBAL PROPERTY SFT_CODEGEN_HAS_BEEN_RUN)
	if(codegen_run)
		message(FATAL_ERROR codegen has already been run)
	endif()
	set_property(GLOBAL PROPERTY SFT_CODEGEN_HAS_BEEN_RUN 1)

	set(CODEGEN_INVOCATION ${PYTHON_INVOCATION} -m openage.codegen "--target-cache=${CMAKE_BINARY_DIR}/codegen_target_cache" "--depend-cache=${CMAKE_BINARY_DIR}/codegen_depend_cache" "--cpp-src-dir=${CPP_SOURCE_DIR}")

	execute_process(COMMAND
		${CODEGEN_INVOCATION}
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		RESULT_VARIABLE COMMAND_RESULT
	)

	if(NOT ${COMMAND_RESULT} EQUAL 0)
		message(FATAL_ERROR "failed to get target list from convert script invocation")
	endif()


	FILE(READ "${CMAKE_BINARY_DIR}/codegen_target_cache" CODEGEN_TARGET_FILES)
	FILE(READ "${CMAKE_BINARY_DIR}/codegen_depend_cache" CODEGEN_DEPENDS)
	STRING(REGEX REPLACE "\n" ";" CODEGEN_TARGET_FILES ${CODEGEN_TARGET_FILES})
	STRING(REGEX REPLACE "\n" ";" CODEGEN_DEPENDS ${CODEGEN_DEPENDS})
	set(CODEGEN_TIMEFILE "${CMAKE_BINARY_DIR}/codegen_timefile")

	add_custom_command(
		OUTPUT "${CODEGEN_TIMEFILE}"
		COMMAND ${CODEGEN_INVOCATION} --write-to-sourcedir "--touch-file-on-cache-change=${CMAKE_CURRENT_LIST_FILE}" --force-rerun-on-targetcache-change
		COMMAND "${CMAKE_COMMAND}" -E touch "${CODEGEN_TIMEFILE}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		DEPENDS ${CODEGEN_DEPENDS}
		COMMENT "generating c++ code (via py/openage/codegen)"
	)

	add_custom_target(codegen DEPENDS "${CODEGEN_TIMEFILE}")

	add_custom_target(cleancodegen
		COMMAND ${CODEGEN_INVOCATION} --clean
		COMMAND "${CMAKE_COMMAND}" -E remove "${CODEGEN_TIMEFILE}"
	)

	set(CODEGEN_TARGET_TUS)
	foreach(target ${CODEGEN_TARGET_FILES})
		if("${target}" MATCHES "\\.cpp$")
			list(APPEND CODEGEN_TARGET_TUS "${target}")
		endif()
	endforeach()

	set(CODEGEN_TARGET_TUS "${CODEGEN_TARGET_TUS}" PARENT_SCOPE)
endfunction()
