# Copyright 2014-2018 the openage authors. See copying.md for legal info.

function(codegen_run)
	# this function must be called once all required assets have been created, but before the executable is finalized.

	# make sure this function gets called only once
	get_property(codegen_run GLOBAL PROPERTY SFT_CODEGEN_HAS_BEEN_RUN)
	if(codegen_run)
		message(FATAL_ERROR "codegen has already been run")
	endif()
	set_property(GLOBAL PROPERTY SFT_CODEGEN_HAS_BEEN_RUN 1)

	set(CODEGEN_INVOCATION
		"${PYTHON}" -m openage codegen
		"--output-dir=${CMAKE_BINARY_DIR}"
		"--generated-list-file=${CMAKE_BINARY_DIR}/codegen_generated_files"
		"--depend-list-file=${CMAKE_BINARY_DIR}/codegen_depends")

	execute_process(COMMAND
		${CODEGEN_INVOCATION} --mode=dryrun
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		RESULT_VARIABLE COMMAND_RESULT
	)

	if(NOT ${COMMAND_RESULT} EQUAL 0)
		message(FATAL_ERROR "failed to get target list from codegen invocation")
	endif()

	FILE(READ "${CMAKE_BINARY_DIR}/codegen_generated_files" CODEGEN_TARGET_FILES)
	FILE(READ "${CMAKE_BINARY_DIR}/codegen_depends" CODEGEN_DEPENDS)
	STRING(REGEX REPLACE "\n" ";" CODEGEN_TARGET_FILES ${CODEGEN_TARGET_FILES})
	STRING(REGEX REPLACE "\n" ";" CODEGEN_DEPENDS ${CODEGEN_DEPENDS})

	# as the codegen creates all files at once,
	# let the buildsystem only depend on this single dummy file.
	# otherwise the codegen invocation would be done for each generated source.
	set(CODEGEN_TIMEFILE "${CMAKE_BINARY_DIR}/codegen_timefile")

	add_custom_command(
		OUTPUT "${CODEGEN_TIMEFILE}"
		BYPRODUCTS ${CODEGEN_TARGET_FILES}
		COMMAND ${CODEGEN_INVOCATION} --mode=codegen "--touch-file-on-cache-change=${CMAKE_CURRENT_LIST_FILE}" --force-rerun-on-generated-list-change
		COMMAND "${CMAKE_COMMAND}" -E touch "${CODEGEN_TIMEFILE}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		DEPENDS ${CODEGEN_DEPENDS}
		COMMENT "openage.codegen: generating c++ code"
	)

	add_custom_target(codegen
		DEPENDS "${CODEGEN_TIMEFILE}"
	)

	add_custom_target(cleancodegen
		COMMAND ${CODEGEN_INVOCATION} --mode=clean
		COMMAND "${CMAKE_COMMAND}" -E remove "${CODEGEN_TIMEFILE}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)

	set(CODEGEN_TARGET_TUS)
	foreach(target ${CODEGEN_TARGET_FILES})
		if("${target}" MATCHES "\\.cpp$")
			list(APPEND CODEGEN_TARGET_TUS "${target}")
		endif()
	endforeach()

	set(CODEGEN_TARGET_TUS "${CODEGEN_TARGET_TUS}" PARENT_SCOPE)
endfunction()
