# Copyright 2015-2016 the openage authors. See copying.md for legal info.

get_property(LISTED_PY_FILES GLOBAL PROPERTY SFT_PY_FILES)
file(GLOB_RECURSE ACTUAL_PY_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.py")

foreach(PY_FILE ${ACTUAL_PY_FILES})
	list(FIND LISTED_PY_FILES "${PY_FILE}" LIST_INDEX)
	if(LIST_INDEX EQUAL -1)
		file(RELATIVE_PATH PY_FILE_RELATIVE_PATH "${CMAKE_SOURCE_DIR}" "${PY_FILE}")
		list(APPEND UNLISTED_PY_FILES "${PY_FILE_RELATIVE_PATH}")
	endif()
endforeach()

foreach(PY_FILE ${LISTED_PY_FILES})
	# Listed files may contain generated files in the binary dir
	if(PY_FILE MATCHES "${CMAKE_CURRENT_SOURCE_DIR}/*")
		list(FIND ACTUAL_PY_FILES "${PY_FILE}" LIST_INDEX)
		if(LIST_INDEX EQUAL -1)
			file(RELATIVE_PATH PY_FILE_RELATIVE_PATH "${CMAKE_SOURCE_DIR}" "${PY_FILE}")
			list(APPEND NONEXISTENT_PY_FILES "${PY_FILE_RELATIVE_PATH}")
		endif()
	endif()
endforeach()

if(UNLISTED_PY_FILES OR NONEXISTENT_PY_FILES)
	message("\nNoticed inconsistency in python file listing.")

	macro(print_inconsistent_files LIST_TYPE DESCRIPTION)
		if(${LIST_TYPE}_PY_FILES)
			message("  -- ${DESCRIPTION}")
			foreach(PY_FILE ${${LIST_TYPE}_PY_FILES})
				message("\t${PY_FILE}")
			endforeach()
			message("")
		endif()
	endmacro()

	print_inconsistent_files(UNLISTED "Unlisted files (list these files using add_py_modules)")
	print_inconsistent_files(NONEXISTENT "Nonexistent files (remove these files from add_py_modules)")

	message(FATAL_ERROR "Aborting.")
endif()
