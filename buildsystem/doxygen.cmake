# Copyright 2014-2019 the openage authors. See copying.md for legal info.

# Doxygen integration

# enable doxygen for all given folder names
function(doxygen_init)
	# check for doxygen + extensions
#	find_package(Doxygen
#			REQUIRED dot)
	find_package(Doxygen)

	if(DOXYGEN_FOUND)
		find_file(DOT dot HINTS "/usr/bin/dot")

		if(NOT ${DOT} STREQUAL "DOT-NOTFOUND")
			set(DOT_EXECUTABLE "${DOT}" PARENT_SCOPE)
			set(HAVE_DOT "YES" PARENT_SCOPE)
		else()
			set(DOT_EXECUTABLE "/bin/false" PARENT_SCOPE)
			set(HAVE_DOT "NO" PARENT_SCOPE)
			message(WARNING "graphviz dot couldn't be found, you won't have cool graphs in the docs.")
		endif()

	endif()

	set(DOXYGEN_EXECUTABLE "${DOXYGEN_EXECUTABLE}" PARENT_SCOPE)
	set(DOXYGEN_FOUND "${DOXYGEN_FOUND}" PARENT_SCOPE)
endfunction()

function(doxygen_offline_configure)

	# check if we are building just doxygen or also webdocs
	if(${DOXYGEN_FOUND})
		# add doc target
		add_custom_target(doc
			"${DOXYGEN_EXECUTABLE}" "${CMAKE_BINARY_DIR}/Doxyfile"
			WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
			COMMENT "generating docs (via Doxygen)"
			VERBATIM
		)

		# create doc folder name list
		foreach(folder ${ARGN})
			set(DOXYGEN_SCAN_FOLDERS "${DOXYGEN_SCAN_FOLDERS} \"${CMAKE_CURRENT_SOURCE_DIR}/${folder}\"")
		endforeach()

		# adapt doxygen config
		configure_file("${BUILDSYSTEM_DIR}/templates/Doxyfile.in" "${CMAKE_BINARY_DIR}/Doxyfile" @ONLY)

    else()
		# add no-op doc target
		add_custom_target(doc
			COMMAND echo "When you configured the project, Doxygen could not be found."
			COMMAND echo "Install Doxygen and `./configure` again, then try to generate docs."
			COMMAND false
			VERBATIM
		)
		message(WARNING "doxygen couldn't be found, you won't be able to generate docs")
	endif()
endfunction()

function(doxygen_webdoc_configure)

	if(${DOXYGEN_FOUND})

		find_program(DOXYPYPY_EXECUTABLE
					NAMES doxypypy
					PATHS
					~/.local/bin
					/bin
					/usr/bin
					/usr/local/bin
					/opt/local/bin
					DOC "doxypypy python file documentation with doxygen"
					)

		# create webdoc folder name list
		# Todo: Check in which form is really needed here
		foreach(folder ${ARGN})
		set(DOXYGEN_SCAN_FOLDERS "${DOXYGEN_SCAN_FOLDERS} \"${CMAKE_CURRENT_SOURCE_DIR}/${folder}\"")
		endforeach()


		# configure doxygen config for webdoc build
		configure_file("${BUILDSYSTEM_DIR}/templates/webdoc/Doxyfile.in" "${WEBDOC_PATH}/Doxyfile" @ONLY)
		configure_file("${BUILDSYSTEM_DIR}/templates/webdoc/py_filter.sh.in" "${WEBDOC_PATH}/py_filter.sh" @ONLY)

	endif()
endfunction()

doxygen_init()
