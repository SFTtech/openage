# python module configurations

# filled with all .py files
set_property(GLOBAL PROPERTY SFT_PY_FILES)

function(add_py_modules)
	foreach(PY_FILE ${ARGN})
		if(NOT "${PY_FILE}" MATCHES ".*\\.py$")
			message(FATAL_ERROR "add_py_modules accepts only .py files. You provided: ${PY_FILE}")
		endif()

		get_filename_component(PY_FILE "${PY_FILE}" ABSOLUTE)
		set_property(GLOBAL APPEND PROPERTY SFT_PY_FILES "${PY_FILE}")

		get_source_file_property(DONT_INSTALL "${PY_FILE}" NOINSTALL)
		if(NOT DONT_INSTALL)
			file(RELATIVE_PATH PY_MODULE_PACKAGE_PATH "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
			install(
				FILES "${PY_FILE}"
				DESTINATION "${CMAKE_PY_INSTALL_PREFIX}/${PY_MODULE_PACKAGE_PATH}"
			)
		endif()
	endforeach()
endfunction()

# create python config file
get_config_option_string()
configure_file(config.py.in "${CMAKE_CURRENT_SOURCE_DIR}/config.py")

set_source_files_properties(devmode.py PROPERTIES NOINSTALL TRUE)

add_py_modules(
	__init__.py
	__main__.py
	assets.py
	config.py
	devmode.py
)

add_subdirectory(cabextract)
add_subdirectory(codegen)
add_subdirectory(convert)
add_subdirectory(cppinterface)
add_subdirectory(game)
add_subdirectory(log)
add_subdirectory(nyan)
add_subdirectory(util)
add_subdirectory(testing)

# Now that all the py files have been registered, we can check for
# any inconsistencies in python file listing.
include(CheckPythonFileListing.cmake)

# Create a install rule for __pycache__ dirs.
# The code in InstallPyCacheDirs.cmake is executed at install time
configure_file(InstallPyCacheDirs.cmake.in InstallPyCacheDirs.cmake @ONLY)
install(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/InstallPyCacheDirs.cmake")

# Replicates the python source tree in build directory (for facilitating out of source builds)
get_property(PY_FILES GLOBAL PROPERTY SFT_PY_FILES)
add_custom_target(replicatepy
	COMMAND ${CMAKE_COMMAND}
	            -D PYTHON_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
	            -D DESTINATION_DIR="${CMAKE_BINARY_DIR}/$<CONFIG>"
	            -P "${CMAKE_CURRENT_SOURCE_DIR}/ReplicatePythonSourceTree.cmake"
	DEPENDS ${PY_FILES}
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
