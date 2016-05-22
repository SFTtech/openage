# Copyright 2015-2016 the openage authors. See copying.md for legal info.

include(CMakeParseArguments)

# Adds target for creating a cython module.
#
#     add_cython_module(target_name
#                       [EMBED] [STANDALONE] [NOINSTALL]
#                       [SOURCES source [source [source ... ]]]
#                       [PACKAGE package]
#                       [OUTPUT_NAME output_name])
#
# By Default, the target created is linked against the `libopenage` shared library,
# except in cases where STANDALONE is specified.
#
# If EMBED is specified, the target is created as an EXECUTABLE instead
# of a MODULE, and the executable is embedded with the python interpreter. In
# other words, the target will be linked against the python libraries.
#
# If STANDALONE is specified, as mentioned above, the target will not be linked
# against the `libopenage` shared library.
#
# If NOINSTALL is specified, an install rule for this target will not be created.
#
function(add_cython_module TARGET_NAME)
	# Use the built-in awesome cmake_parse_arguments function
	set(OPTION_KEYWORDS EMBED STANDALONE NOINSTALL)
	set(ONE_VALUE_KEYWORDS PACKAGE OUTPUT_NAME)
	set(MULTI_VALUE_KEYWORDS SOURCES)
	cmake_parse_arguments(CYTHON_MODULE "${OPTION_KEYWORDS}" "${ONE_VALUE_KEYWORDS}" "${MULTI_VALUE_KEYWORDS}" ${ARGN})

	if(NOT CYTHON_MODULE_PACKAGE)
		file(RELATIVE_PATH CYTHON_MODULE_DESTINATION "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	else()
		string(REGEX REPLACE "\\." "/" CYTHON_MODULE_DESTINATION "${CYTHON_MODULE_PACKAGE}")
	endif()

	set(SOURCE_FILES ${CYTHON_MODULE_SOURCES} ${CYTHON_MODULE_UNPARSED_ARGUMENTS})
	# Atleast one source file is required
	if(NOT SOURCE_FILES)
		message(FATAL_ERROR "No sources provided for cython module")
	endif()

	# Separate .py/.pyx and .cpp files
	foreach(SOURCE_FILE ${SOURCE_FILES})
		get_filename_component(SOURCE_FILE "${SOURCE_FILE}" ABSOLUTE)

		if("${SOURCE_FILE}" MATCHES ".*\\.pyx?$")
			file(RELATIVE_PATH PY_SOURCE_RELATIVE_PATH "${CMAKE_SOURCE_DIR}" "${SOURCE_FILE}")

			string(REGEX REPLACE "\\.pyx?$" ".cpp" GENERATED_CPP_SOURCE_FILE "${PY_SOURCE_RELATIVE_PATH}")
			set(GENERATED_CPP_SOURCE_FILE "${CMAKE_BINARY_DIR}/${GENERATED_CPP_SOURCE_FILE}")

			list(APPEND PY_SOURCE_FILES ${PY_SOURCE_RELATIVE_PATH})
			list(APPEND GENERATED_CPP_SOURCE_FILES "${GENERATED_CPP_SOURCE_FILE}")
		elseif("${SOURCE_FILE}" MATCHES ".*\\.cpp$")
			list(APPEND CPP_SOURCE_FILES "${SOURCE_FILE}")
		else()
			message(FATAL_ERROR
				"add_cython_module accepts only .py/.pyx/.cpp source files. You provided: ${SOURCE_FILE}")
		endif()
	endforeach()

	# Atleast one .py/.pyx file should be given to add_cython_module.
	# Otherwise, is it really a cython module? Think about it.
	if(NOT PY_SOURCE_FILES)
		message(FATAL_ERROR "add_cython_module requires atleast one .py/.pyx source file.")
	endif()

	if(CYTHON_MODULE_EMBED)
		set(EMBED_ARG "--embed")
	endif()
	string(REPLACE ";" " " PY_SOURCES ${PY_SOURCE_FILES})

	# The dependency file will be generated the first time around.
	set(DEPENDENCY_FILE "${CMAKE_CURRENT_BINARY_DIR}/cythonize_${TARGET_NAME}.depends")
	if(EXISTS "${DEPENDENCY_FILE}")
		file(READ "${DEPENDENCY_FILE}" CYTHON_MODULE_DEPENDS)
		string(REGEX REPLACE "\n" ";" CYTHON_MODULE_DEPENDS ${CYTHON_MODULE_DEPENDS})
	endif()
	add_custom_command(OUTPUT ${GENERATED_CPP_SOURCE_FILES}
		COMMAND "${PYTHON}" -m buildsystem.cythonize
			--build-dir="${CMAKE_BINARY_DIR}" --depends="${DEPENDENCY_FILE}" ${EMBED_ARG} "${PY_SOURCES}"
		DEPENDS pxdgen ${CYTHON_MODULE_DEPENDS}
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)

	if(CYTHON_MODULE_EMBED)
		add_executable("${TARGET_NAME}" ${CPP_SOURCE_FILES} ${GENERATED_CPP_SOURCE_FILES})
		# TODO: use full ldflags and cflags provided by python${VERSION}-config
		target_link_libraries("${TARGET_NAME}" "${PYEXT_LIBRARY}")
	else()
		add_library("${TARGET_NAME}" MODULE ${CPP_SOURCE_FILES} ${GENERATED_CPP_SOURCE_FILES})
		set_target_properties("${TARGET_NAME}" PROPERTIES PREFIX "" SUFFIX "${PYEXT_SUFFIX}")
	endif()
	add_dependencies("${TARGET_NAME}" replicatepy)

	if(NOT CYTHON_MODULE_STANDALONE)
		set_target_properties("${TARGET_NAME}" PROPERTIES LINK_DEPENDS_NO_SHARED 1)
		target_link_libraries("${TARGET_NAME}" "${PYEXT_LINK_LIBRARY}")
	endif()

	if(NOT CYTHON_MODULE_EMBED AND APPLE)
		# Since this module is not embedded with python interpreter,
		# Mac OS X requires a link flag to resolve undefined symbols
		set_target_properties("${TARGET_NAME}" PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif()

	set_target_properties("${TARGET_NAME}" PROPERTIES
		COMPILE_FLAGS "${PYEXT_CXXFLAGS}"
		INCLUDE_DIRECTORIES "${PYEXT_INCLUDE_DIRS}"
	)

	if(CYTHON_MODULE_OUTPUT_NAME)
		set_target_properties("${TARGET_NAME}" PROPERTIES OUTPUT_NAME "${CYTHON_MODULE_OUTPUT_NAME}")
	endif()

	# We will create the output of cython modules in the specified package directory
	set(TARGET_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${CYTHON_MODULE_DESTINATION}")
	set_target_properties("${TARGET_NAME}" PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${TARGET_OUTPUT_DIRECTORY}"
		LIBRARY_OUTPUT_DIRECTORY "${TARGET_OUTPUT_DIRECTORY}"
		RUNTIME_OUTPUT_DIRECTORY "${TARGET_OUTPUT_DIRECTORY}"
	)
	# Same as above, but for multi-config cmake generators
	foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER "${CONFIG_TYPE}" CONFIG_TYPE_UPPER)
		set(TARGET_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CONFIG_TYPE}/${CYTHON_MODULE_DESTINATION}")

		set_target_properties("${TARGET_NAME}" PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} "${TARGET_OUTPUT_DIRECTORY}"
			LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} "${TARGET_OUTPUT_DIRECTORY}"
			RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} "${TARGET_OUTPUT_DIRECTORY}"
		)
	endforeach()

	# Create an install rule, if the user did not specify NOINSTALL
	if(NOT CYTHON_MODULE_NOINSTALL)
		install(
			TARGETS "${TARGET_NAME}"
			DESTINATION "${CMAKE_PY_INSTALL_PREFIX}/${CYTHON_MODULE_DESTINATION}"
		)
	endif()

	# Populate the target pretty properties and print to console
	set(PRETTY_TARGET_PROPERTIES "")
	macro(check_and_add_pretty_target_property PROPERTY VALUE)
		if(CYTHON_MODULE_${PROPERTY})
			set(PRETTY_TARGET_PROPERTIES "${PRETTY_TARGET_PROPERTIES} [${VALUE}]")
		endif()
	endmacro()
	check_and_add_pretty_target_property(EMBED "embedded interpreter")
	check_and_add_pretty_target_property(STANDALONE "standalone")
	check_and_add_pretty_target_property(NOINSTALL "noinstall")
	pretty_print_target("cython module" "${TARGET_NAME}" "${PRETTY_TARGET_PROPERTIES}")
endfunction()
