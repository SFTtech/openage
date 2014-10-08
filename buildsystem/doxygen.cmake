# Doxygen integration

find_package(Doxygen)
if(DOXYGEN_FOUND)
	find_file(DOT dot HINTS /usr/bin/dot)

	set(DOT_EXECUTABLE "/bin/false")
	set(HAVE_DOT "NO")

	if(NOT ${DOT} STREQUAL "DOT-NOTFOUND")
		set(DOT_EXECUTABLE "${DOT}")
		set(HAVE_DOT "YES")
		message("found dot: ${DOT_EXECUTABLE}")
	else()
		message(WARNING "graphviz dot couldn't be found, you won't have cool graphs in the docs.")
	endif()

	# adapt doxygen config
	configure_file(${BUILDSYSTEM_DIR}/templates/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)

	# add doc target
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "generating docs with Doxygen" VERBATIM
	)
endif()
