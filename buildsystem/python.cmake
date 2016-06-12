# Copyright 2014-2016 the openage authors. See copying.md for legal info.

# provides macros for defining python extension modules and pxdgen sources.
# and a 'finalize' function that must be called in the end.

function(python_init)
	# filled by pxdgen; written to bin/py/pxdgen_sources for use by pxdgen.py.
	set_property(GLOBAL PROPERTY SFT_PXDGEN_SOURCES)

	# filled by add_cython_modules. used by cythonize.py.
	set_property(GLOBAL PROPERTY SFT_CYTHON_MODULES)
	set_property(GLOBAL PROPERTY SFT_CYTHON_MODULES_EMBED)

	# list of pxd files added manually
	# filled by add_pxd only. for use as depends list for cythonize.py.
	set_property(GLOBAL PROPERTY SFT_PXD_FILES)

	# filled by pxdgen. for use as output list for the pxdgen target.
	set_property(GLOBAL PROPERTY SFT_GENERATED_PXD_FILES)

	# filled with all .py filenames; used for installing.
	set_property(GLOBAL PROPERTY SFT_PY_FILES)

	# filled with the relative source dirs of the above py files.
	set_property(GLOBAL PROPERTY SFT_PY_FILES_RELSRCDIRS)

	# filled with all .py filenames that should not be installed
	set_property(GLOBAL PROPERTY SFT_PY_FILES_NOINSTALL)

	# filled with all cython module target names; used for in-place creation.
	set_property(GLOBAL PROPERTY SFT_CYTHON_MODULE_TARGETS)
endfunction()


function(add_cython_modules)
	# adds a new module for cython compilation, by relative filename.
	# synoposis:
	# add_cython_modules(
	#    test.pyx
	#    EMBED __main__.pyx
	#    STANDALONE EMBED foo/bar.pyx
	#    NOINSTALL STANDALONE foo/test.pyx
	# )
	#
	# test.pyx is compiled to a shared library linked against PYEXT_LINK_LIBRARY
	# __main__.pyx is compiled to a executable with embedded python interpreter,
	# linked against libpython and PYEXT_LINK_LIBRARY.
	# foo/bar.pyx is compiled to a executable with embedded pytthon interpreter,
	# linked only against libpython.
	# foo/test.pyx is compiled to a shared library linked against nothing, and will
	# not be installed.

	file(RELATIVE_PATH REL_CURRENT_SOURCE_DIR
		"${CMAKE_SOURCE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}")

	set(EMBED_NEXT FALSE)
	set(STANDALONE_NEXT FALSE)
	set(NOINSTALL_NEXT FALSE)
	foreach(source ${ARGN})
		if(source STREQUAL "EMBED")
			set(EMBED_NEXT TRUE)
		elseif(source STREQUAL "STANDALONE")
			set(STANDALONE_NEXT TRUE)
		elseif(source STREQUAL "NOINSTALL")
			set(NOINSTALL_NEXT TRUE)
		else()
			if(NOT IS_ABSOLUTE "${source}")
				set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
			endif()

			if(NOT "${source}" MATCHES ".*\\.pyx?$")
				message(FATAL_ERROR "non-.py/.pyx file given to add_cython_modules: ${source}")
			endif()

			get_filename_component(OUTPUTNAME "${source}" NAME_WE)
			string(REGEX REPLACE "\\.pyx?$" ".cpp" CPPNAME "${source}")
			set_source_files_properties("${CPPNAME}" PROPERTIES GENERATED ON)

			# construct some hopefully unique target name
			file(RELATIVE_PATH TARGETNAME "${CMAKE_SOURCE_DIR}" "${source}")
			string(REGEX REPLACE "\\.pyx?$" "" TARGETNAME "${TARGETNAME}")
			string(REGEX REPLACE "/" "_" TARGETNAME "${TARGETNAME}")

			# generate the pretty module name
			file(RELATIVE_PATH PRETTY_MODULE_NAME "${CMAKE_SOURCE_DIR}" "${source}")
			string(REGEX REPLACE "\\.pyx?$" "" PRETTY_MODULE_NAME "${PRETTY_MODULE_NAME}")
			string(REGEX REPLACE "/" "." PRETTY_MODULE_NAME "${PRETTY_MODULE_NAME}")
			set(PRETTY_MODULE_PROPERTIES "")

			if(EMBED_NEXT)
				set(PRETTY_MODULE_PROPERTIES "${PRETTY_MODULE_PROPERTIES} [embedded interpreter]")

				set_property(GLOBAL APPEND PROPERTY SFT_CYTHON_MODULES_EMBED "${source}")
				add_executable("${TARGETNAME}" "${CPPNAME}")

				# TODO: use full ldflags and cflags provided by python${VERSION}-config
				target_link_libraries("${TARGETNAME}" "${PYEXT_LIBRARY}")
			else()
				set_property(GLOBAL APPEND PROPERTY SFT_CYTHON_MODULES "${source}")
				add_library("${TARGETNAME}" MODULE "${CPPNAME}")

				set_target_properties("${TARGETNAME}" PROPERTIES
					PREFIX ""
					SUFFIX "${PYEXT_SUFFIX}"
				)
			endif()

			if(NOINSTALL_NEXT)
				set(PRETTY_MODULE_PROPERTIES "${PRETTY_MODULE_PROPERTIES} [noinstall]")
			else()
				install(
					TARGETS "${TARGETNAME}"
					DESTINATION "${CMAKE_PY_INSTALL_PREFIX}/${REL_CURRENT_SOURCE_DIR}"
				)
			endif()

			set_target_properties("${TARGETNAME}" PROPERTIES
				COMPILE_FLAGS "${PYEXT_CXXFLAGS}"
				INCLUDE_DIRECTORIES "${PYEXT_INCLUDE_DIRS}"
				OUTPUT_NAME "${OUTPUTNAME}"
			)

			if (STANDALONE_NEXT)
				set(PRETTY_MODULE_PROPERTIES "${PRETTY_MODULE_PROPERTIES} [standalone]")
			else()
				set_target_properties("${TARGETNAME}" PROPERTIES LINK_DEPENDS_NO_SHARED 1)
				target_link_libraries("${TARGETNAME}" "${PYEXT_LINK_LIBRARY}")
			endif()

			# Since this module is not embedded with python interpreter,
			# Mac OS X requires a link flag to resolve undefined symbols
			if(NOT EMBED_NEXT AND APPLE)
				set_target_properties("${TARGETNAME}" PROPERTIES LINK_FLAGS "-undefined dynamic_lookup" )
			endif()

			add_dependencies("${TARGETNAME}" cythonize)

			set_property(GLOBAL APPEND PROPERTY SFT_CYTHON_MODULE_TARGETS "${TARGETNAME}")

			pretty_print_target("cython module" "${PRETTY_MODULE_NAME}" "${PRETTY_MODULE_PROPERTIES}")

			# Reset the flags before processing the next cython module
			set(EMBED_NEXT FALSE)
			set(NOINSTALL_NEXT FALSE)
			set(STANDALONE_NEXT FALSE)
		endif()
	endforeach()
endfunction()


function(pxdgen)
	# add a C++ header file as pxdgen source file
	foreach(source ${ARGN})
		if(NOT IS_ABSOLUTE "${source}")
			set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
		endif()

		if(NOT "${source}" MATCHES ".*\\.h$")
			message(FATAL_ERROR "non-.h file given to pxdgen: ${source}")
		endif()

		string(REGEX REPLACE "\\.h$" ".pxd" PXDNAME "${source}")
		set_source_files_properties("${PXDNAME}" PROPERTIES GENERATED ON)

		set_property(GLOBAL APPEND PROPERTY SFT_PXDGEN_SOURCES "${source}")
		set_property(GLOBAL APPEND PROPERTY SFT_GENERATED_PXD_FILES "${PXDNAME}")
	endforeach()
endfunction()


function(add_pxds)
	# add a .pxd or other additional Cython source
	foreach(source ${ARGN})
		if(NOT IS_ABSOLUTE "${source}")
			set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
		endif()

		if(NOT "${source}" MATCHES ".*\\.px[id]$")
			message(FATAL_ERROR "non-pxd/pxi file given to add_pyd: ${source}")
		endif()

		set_property(GLOBAL APPEND PROPERTY SFT_PXD_FILES "${source}")
	endforeach()
endfunction()


function(add_py_modules)
	# add a .py file for installing

	file(RELATIVE_PATH REL_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

	set(NOINSTALL_NEXT FALSE)
	foreach(source ${ARGN})
		if(source STREQUAL "NOINSTALL")
			set(NOINSTALL_NEXT TRUE)
		else()
			if(NOT ${source} MATCHES ".*\\.py$")
				message(FATAL_ERROR "non-Python file given to add_py_modules: ${source}")
			endif()

			if(NOT IS_ABSOLUTE ${source})
				set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
			endif()

			if(NOINSTALL_NEXT)
				set_property(GLOBAL APPEND PROPERTY SFT_PY_FILES_NOINSTALL "${source}")
				set(NOINSTALL_NEXT FALSE)
			endif()

			set_property(GLOBAL APPEND PROPERTY SFT_PY_FILES "${source}")
			set_property(GLOBAL APPEND PROPERTY SFT_PY_FILES_RELSRCDIRS "${REL_CURRENT_SOURCE_DIR}")
		endif()
	endforeach()
endfunction()


function(python_finalize)
	# pxdgen (.h -> .pxd)

	get_property(pxdgen_sources GLOBAL PROPERTY SFT_PXDGEN_SOURCES)
	write_on_change("${CMAKE_BINARY_DIR}/py/pxdgen_sources" "${pxdgen_sources}")
	set(PXDGEN_TIMEFILE "${CMAKE_BINARY_DIR}/py/pxdgen_timefile")
	add_custom_command(OUTPUT "${PXDGEN_TIMEFILE}"
		COMMAND "${PYTHON}" -m buildsystem.pxdgen
		--file-list "${CMAKE_BINARY_DIR}/py/pxdgen_sources"
		COMMAND "${CMAKE_COMMAND}" -E touch "${PXDGEN_TIMEFILE}"
		DEPENDS ${pxdgen_sources} "${CMAKE_BINARY_DIR}/py/pxdgen_sources"
		COMMENT "pxdgen: generating .pxd files from headers"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)
	add_custom_target(pxdgen ALL DEPENDS "${PXDGEN_TIMEFILE}")


	# cythonize (.pyx -> .cpp)

	get_property(cython_modules GLOBAL PROPERTY SFT_CYTHON_MODULES)
	write_on_change("${CMAKE_BINARY_DIR}/py/cython_modules" "${cython_modules}")
	get_property(cython_modules_embed GLOBAL PROPERTY SFT_CYTHON_MODULES_EMBED)
	write_on_change("${CMAKE_BINARY_DIR}/py/cython_modules_embed" "${cython_modules_embed}")
	get_property(pxd_list GLOBAL PROPERTY SFT_PXD_FILES)
	get_property(generated_pxd_list GLOBAL PROPERTY SFT_GENERATED_PXD_FILES)
	write_on_change("${CMAKE_BINARY_DIR}/py/pxd_list" "${pxd_list};${generated_pxd_list}")
	set(CYTHONIZE_TIMEFILE "${CMAKE_BINARY_DIR}/py/cythonize_timefile")
	add_custom_command(OUTPUT "${CYTHONIZE_TIMEFILE}"
		COMMAND "${PYTHON}" -m buildsystem.cythonize
		"${CMAKE_BINARY_DIR}/py/cython_modules"
		"${CMAKE_BINARY_DIR}/py/cython_modules_embed"
		"${CMAKE_BINARY_DIR}/py/pxd_list"
		COMMAND "${CMAKE_COMMAND}" -E touch "${CYTHONIZE_TIMEFILE}"
		DEPENDS
		"${PXDGEN_TIMEFILE}"
		${cython_modules}
		${cython_modules_embed}
		${pxd_list}
		"${CMAKE_BINARY_DIR}/py/cython_modules"
		"${CMAKE_BINARY_DIR}/py/cython_modules_embed"
		"${CMAKE_BINARY_DIR}/py/pxd_list"
		COMMENT "cythonize.py: compiling .pyx files to .cpp"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)
	add_custom_target(cythonize ALL DEPENDS "${CYTHONIZE_TIMEFILE}")


	# py compile (.py -> .pyc)

	set_property(GLOBAL APPEND PROPERTY SFT_PY_FILES_RELSRCDIRS "lol test")

	get_property(py_files GLOBAL PROPERTY SFT_PY_FILES)
	get_property(py_files_srcdirs GLOBAL PROPERTY SFT_PY_FILES_RELSRCDIRS)
	get_property(py_files_noinstall GLOBAL PROPERTY SFT_PY_FILES_NOINSTALL)

	write_on_change("${CMAKE_BINARY_DIR}/py/py_files" "${py_files}")
	set(COMPILEPY_TIMEFILE "${CMAKE_BINARY_DIR}/py/compilepy_timefile")
	add_custom_command(OUTPUT "${COMPILEPY_TIMEFILE}"
		COMMAND "${PYTHON}" -m buildsystem.compilepy
		"${CMAKE_BINARY_DIR}/py/py_files"
		"${CMAKE_SOURCE_DIR}"
		"${CMAKE_SOURCE_DIR}"
		COMMAND "${CMAKE_COMMAND}" -E touch "${COMPILEPY_TIMEFILE}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		DEPENDS "${CMAKE_BINARY_DIR}/py/py_files" ${py_files}
		COMMENT "compiling .py files to .pyc files"
	)
	add_custom_target(compilepy ALL DEPENDS "${COMPILEPY_TIMEFILE}")

	# determine the compiled file name for all source files
	# avoid doing this call for each file, instead, batch-process.
	# BUT: i'm unsure when exactly this is called again
	#      (if a new file was added to the py_files?)
	py_exec("from importlib import util; print(';'.join(util.cache_from_source(f) for f in open('${CMAKE_BINARY_DIR}/py/py_files').read().split(';')))" py_compiled_files)

	list(LENGTH py_files py_files_count)
	math(EXPR py_files_count_range "${py_files_count} - 1")

	foreach(idx RANGE ${py_files_count_range})
		list(GET py_files ${idx} pyfile)
		list(GET py_compiled_files ${idx} pycfile)
		list(GET py_files_srcdirs ${idx} pyrelsrcdir)

		# install if it's not in that list.
		list(FIND py_files_noinstall "${pyfile}" do_install)
		if(do_install)
			install(
				FILES "${pyfile}"
				DESTINATION  "${CMAKE_PY_INSTALL_PREFIX}/${pyrelsrcdir}"
			)
			install(
				FILES "${pycfile}"
				DESTINATION "${CMAKE_PY_INSTALL_PREFIX}/${pyrelsrcdir}/__pycache__"
			)
		endif()
	endforeach()


	# inplace module install (bin/module.so -> module.so)

	get_property(cython_module_targets GLOBAL PROPERTY SFT_CYTHON_MODULE_TARGETS)
	set(cython_module_files_expr)
	foreach(cython_module_target ${cython_module_targets})
		list(APPEND cython_module_files_expr "$<TARGET_FILE:${cython_module_target}>")
	endforeach()
	file(GENERATE
		OUTPUT "${CMAKE_BINARY_DIR}/py/inplace_module_list"
		CONTENT "${cython_module_files_expr}"
	)
	set(INPLACEMODULES_TIMEFILE "${CMAKE_BINARY_DIR}/py/inplacemodules_timefile")
	add_custom_command(OUTPUT "${INPLACEMODULES_TIMEFILE}"
		COMMAND "${PYTHON}" -m buildsystem.inplacemodules
		"${CMAKE_BINARY_DIR}/py/inplace_module_list"
		"${CMAKE_BINARY_DIR}"
		DEPENDS
		"${CMAKE_BINARY_DIR}/py/inplace_module_list"
		${cython_module_targets}
		COMMAND "${CMAKE_COMMAND}" -E touch "${INPLACEMODULES_TIMEFILE}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		COMMENT "creating in-place modules"
	)
	add_custom_target(inplacemodules ALL DEPENDS "${INPLACEMODULES_TIMEFILE}")


	# cleaning of all in-sourcedir stuff

	add_custom_target(cleancython
		COMMAND "${PYTHON}" -m buildsystem.inplacemodules --clean
		"${CMAKE_BINARY_DIR}/py/inplace_module_list"
		"${CMAKE_BINARY_DIR}"
		COMMAND "${PYTHON}" -m buildsystem.cythonize --clean
		"${CMAKE_BINARY_DIR}/py/cython_modules"
		"${CMAKE_BINARY_DIR}/py/cython_modules_embed"
		"${CMAKE_BINARY_DIR}/py/pxd_list"
		# general deleters to catch files that have already been un-listed.
		COMMAND find openage -name "*.cpp" -type f -print -delete
		COMMAND find openage -name "*.html" -type f -print -delete
		COMMAND find openage -name "*.so" -type f -print -delete
		COMMAND "${CMAKE_COMMAND}" -E remove "${CYTHONIZE_TIMEFILE}"
		COMMAND "${CMAKE_COMMAND}" -E remove "${INPLACEMODULES_TIMEFILE}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)

	add_custom_target(cleanpxdgen
		COMMAND find libopenage -name "*.pxd" -type f -print -delete
		COMMAND find libopenage -name "__init__.py" -type f -print -delete
		COMMAND "${CMAKE_COMMAND}" -E remove "${PXDGEN_TIMEFILE}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)


	# check for any unlisted .py files, and error.

	execute_process(
		COMMAND "${PYTHON}" -m buildsystem.check_py_file_list
		${CMAKE_BINARY_DIR}/py/py_files
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		RESULT_VARIABLE res
	)
	if(NOT res EQUAL 0)
		message(FATAL_ERROR ".py file listing inconsistent")
	endif()
endfunction()

python_init()
