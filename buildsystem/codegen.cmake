set(CODEGEN_INVOCATION ${PYTHON_INVOCATION} -m openage.codegen --target-cache=${CMAKE_BUILD_DIR}/codegen_target_cache --depend-cache=${CMAKE_BUILD_DIR}/codegen_depend_cache --cpp-src-dir=${CPP_SOURCE_DIR})

execute_process(COMMAND
	${CODEGEN_INVOCATION}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	RESULT_VARIABLE COMMAND_RESULT
)

if(NOT ${COMMAND_RESULT} EQUAL 0)
	message(FATAL_ERROR "failed to get codegen filelist via convert script invocation")
endif()


FILE(READ "${CMAKE_BUILD_DIR}/codegen_target_cache" CODEGEN_TARGETS)
FILE(READ "${CMAKE_BUILD_DIR}/codegen_depend_cache" CODEGEN_DEPENDS)
STRING(REGEX REPLACE "\n" ";" CODEGEN_TARGETS "${codegen_targets}")
STRING(REGEX REPLACE "\n" ";" CODEGEN_DEPENDS "${CODEGEN_DEPENDS}")

add_custom_command(OUTPUT ${CODEGEN_TARGETS}
	COMMAND ${CODEGEN_INVOCATION} --write-to-sourcedir --touch-file-on-cache-change=${CMAKE_BIN_DIR} --force-rerun-on-targetcache-change
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS ${CODEGEN_DEPENDS}
	COMMENT running code generation
)

add_sources_absolute(${PROJECT_NAME}
	${GENERATED_FILES_ABSOLUTE}
)
