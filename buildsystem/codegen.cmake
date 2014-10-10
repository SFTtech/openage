function(codegen_init)
	set(CODEGEN_INVOCATION ${PYTHON_INVOCATION} -m openage.codegen --target-cache=${CMAKE_BINARY_DIR}/codegen_target_cache --depend-cache=${CMAKE_BINARY_DIR}/codegen_depend_cache --cpp-src-dir=${CPP_SOURCE_DIR})

	execute_process(COMMAND
		${CODEGEN_INVOCATION}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		RESULT_VARIABLE COMMAND_RESULT
	)

	if(NOT ${COMMAND_RESULT} EQUAL 0)
		message(FATAL_ERROR "failed to get codegen filelist via convert script invocation")
	endif()


	FILE(READ "${CMAKE_BINARY_DIR}/codegen_target_cache" CODEGEN_TARGET_FILES)
	FILE(READ "${CMAKE_BINARY_DIR}/codegen_depend_cache" CODEGEN_DEPENDS)
	STRING(REGEX REPLACE "\n" ";" CODEGEN_TARGET_FILES "${CODEGEN_TARGET_FILES}")
	STRING(REGEX REPLACE "\n" ";" CODEGEN_DEPENDS "${CODEGEN_DEPENDS}")

	add_custom_command(OUTPUT ${CODEGEN_TARGET_FILES}
		COMMAND ${CODEGEN_INVOCATION} --write-to-sourcedir --touch-file-on-cache-change=${CMAKE_CURRENT_LIST_FILE} --force-rerun-on-targetcache-change
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS ${CODEGEN_DEPENDS}
		COMMENT running code generation
	)

	set(CODEGEN_TARGET_TUS)
	foreach(target ${CODEGEN_TARGET_FILES})
		if(${target} MATCHES \\.cpp$)
			list(APPEND CODEGEN_TARGET_TUS ${target})
		endif()
	endforeach()

	add_custom_target(codegen ALL DEPENDS ${CODEGEN_TARGET_FILES})

	set(CODEGEN_TARGET_TUS ${CODEGEN_TARGET_TUS} PARENT_SCOPE)
endfunction()

codegen_init()
