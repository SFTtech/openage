# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# finds the python interpreter, install destination and extension flags.
# provides macros for defining python extension modules and pxdgen sources.
# and a 'finalize' function that must be called in the end.

function(python_init)
	# the Python version number requirement is in modules/FindPython_test.cpp
	find_package(Python REQUIRED)
	find_package(Cython 0.22 REQUIRED)

	# filled by pxdgen; written to bin/py/pxdgen_sources for use by pxdgen.py.
	set_property(GLOBAL PROPERTY SFT_PXDGEN_SOURCES)

	# filled by add_cython_modules. used by cythonize.py.
	set_property(GLOBAL PROPERTY SFT_CYTHON_MODULES)
	set_property(GLOBAL PROPERTY SFT_CYTHON_MODULES_EMBED)

	# filled by pxdgen and add_pxd. for use as depends list for cythonize.py.
	set_property(GLOBAL PROPERTY SFT_PXD_FILES)

	# filled with all .py filenames; used for installing.
	set_property(GLOBAL PROPERTY SFT_PY_FILES)

	# filled with all cython module target names; used for in-place creation.
	set_property(GLOBAL PROPERTY SFT_CYTHON_MODULE_TARGETS)

	# filled with all __pycache__ folders that have already been selected for installation.
	set_property(GLOBAL PROPERTY SFT_INSTALLED_PYCACHE_FOLDERS)


	py_get_config_var(OPT PYEXT_CXXFLAGS)
	py_get_config_var(EXT_SUFFIX PYEXT_SUFFIX)

	# fix the CXXFLAGS
	set(PYEXT_CXXFLAGS " ${PYEXT_CXXFLAGS} ") # padding required for the replacements below
	# C++ doesn't have the -Wstrict-prototypes warning
	string(REGEX REPLACE " -Wstrict-prototypes " " " PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS}")
	# thanks, but I'd like to choose my debug mode myself.
	string(REPLACE " -g " " " PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS}")

	# add our own regular C++ flags
	set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} ${CMAKE_CXX_FLAGS}")

	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		# some things clang complains about
		set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-extended-offsetof")
		set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-unneeded-internal-declaration")

		if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5)
			# doesn't have that warning yet
		else()
			# https://github.com/cython/cython/pull/402 (fix pending cython 0.23)
			set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-absolute-value")
		endif()
	endif()

	set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} -Wno-unused-function")

	set(PYTHON "${PYTHON}" PARENT_SCOPE)
	set(CYTHON "${CYTHON}" PARENT_SCOPE)
	set(CYTHON_VERSION "${CYTHON_VERSION}" PARENT_SCOPE)

	set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS}" PARENT_SCOPE)
	set(PYEXT_PYLIB "${PYTHON_LIBRARY}" PARENT_SCOPE)
	set(PYEXT_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}" PARENT_SCOPE)
	set(PYEXT_SUFFIX "${PYEXT_SUFFIX}" PARENT_SCOPE)

	if(NOT CMAKE_PY_INSTALL_PREFIX)
		py_exec("import site; print(site.getsitepackages()[0])" PREFIX)
		set(CMAKE_PY_INSTALL_PREFIX "${PREFIX}" PARENT_SCOPE)
	endif()
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

			if(NOT "${source}" MATCHES "${CMAKE_CURRENT_SOURCE_DIR}/.*\\.pyx?$")
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
				set(EMBED_NEXT FALSE)
				set(PRETTY_MODULE_PROPERTIES "${PRETTY_MODULE_PROPERTIES} [embedded interpreter]")

				set_property(GLOBAL APPEND PROPERTY SFT_CYTHON_MODULES_EMBED "${source}")
				add_executable("${TARGETNAME}" "${CPPNAME}")

				target_link_libraries("${TARGETNAME}" "${PYEXT_PYLIB}")
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
				set(NOINSTALL_NEXT FALSE)
			else()
				install(
					TARGETS "${TARGETNAME}"
					DESTINATION "${CMAKE_PY_INSTALL_PREFIX}/${REL_CURRENT_SOURCE_DIR}"
				)
			endif()

			set_target_properties("${TARGETNAME}" PROPERTIES
				COMPILE_FLAGS "${PYEXT_CXXFLAGS}"
				INCLUDE_DIRECTORIES "${PYEXT_INCLUDE_DIR}"
				OUTPUT_NAME "${OUTPUTNAME}"
			)

			if (STANDALONE_NEXT)
				set(PRETTY_MODULE_PROPERTIES "${PRETTY_MODULE_PROPERTIES} [standalone]")
				set(STANDALONE_NEXT FALSE)
			else()
				set_target_properties("${TARGETNAME}" PROPERTIES CMAKE_LINK_DEPENDS_NO_SHARED 1)
				target_link_libraries("${TARGETNAME}" "${PYEXT_LINK_LIBRARY}")
			endif()

			add_dependencies("${TARGETNAME}" cythonize)

			set_property(GLOBAL APPEND PROPERTY SFT_CYTHON_MODULE_TARGETS "${TARGETNAME}")

			pretty_print_target("cython module" "${PRETTY_MODULE_NAME}" "${PRETTY_MODULE_PROPERTIES}")
		endif()
	endforeach()
endfunction()


function(pxdgen)
	# add a C++ header file as pxdgen source file
	foreach(source ${ARGN})
		if(NOT IS_ABSOLUTE "${source}")
			set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
		endif()

		if(NOT "${source}" MATCHES "${CMAKE_CURRENT_SOURCE_DIR}/.*\\.h$")
			message(FATAL_ERROR "non-.h file given to pxdgen: ${source}")
		endif()

		string(REGEX REPLACE "\\.h$" ".pxd" PXDNAME "${source}")
		set_source_files_properties("${PXDNAME}" PROPERTIES GENERATED ON)

		set_property(GLOBAL APPEND PROPERTY SFT_PXDGEN_SOURCES "${source}")
		set_property(GLOBAL APPEND PROPERTY SFT_PXD_FILES "${PXDNAME}")
	endforeach()
endfunction()


function(add_pxds)
	# add a .pxd or other additional Cython source
	foreach(source ${ARGN})
		if(NOT IS_ABSOLUTE "${source}")
			set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
		endif()

		if(NOT "${source}" MATCHES "${CMAKE_CURRENT_SOURCE_DIR}/.*\\.px[id]$")
			message(FATAL_ERROR "non-pxd/pxi file given to add_pyd: ${source}")
		endif()

		set_property(GLOBAL APPEND PROPERTY SFT_PXD_FILES "${source}")
	endforeach()
endfunction()


function(add_py_modules)
	# add a .py file for installing

	file(RELATIVE_PATH REL_CURRENT_SOURCE_DIR
		"${CMAKE_SOURCE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}")

	set(NOINSTALL_NEXT FALSE)
	foreach(source ${ARGN})
		if("${source}" STREQUAL "NOINSTALL")
			set(NOINSTALL_NEXT TRUE)
		else()
			if(NOT IS_ABSOLUTE "${source}")
				set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
			endif()

			if(NOT "${source}" MATCHES "${CMAKE_CURRENT_SOURCE_DIR}/.*\\.py$")
				message(FATAL_ERROR "non-Python file given to add_py_modules: ${source}")
			endif()

			if(NOINSTALL_NEXT)
				set(NOINSTALL_NEXT FALSE)
			else()
				install(
					FILES "${source}"
					DESTINATION  "${CMAKE_PY_INSTALL_PREFIX}/${REL_CURRENT_SOURCE_DIR}"
				)

				# install the corresponding __pycache__ folder, but at most once.
				get_filename_component(DIR_NAME "${source}" DIRECTORY)
				get_property(installed_pycache_folders GLOBAL PROPERTY SFT_INSTALLED_PYCACHE_FOLDERS)
				list(FIND installed_pycache_folders "${DIR_NAME}" idx)
				if(idx LESS 0)
					install(
						DIRECTORY "${DIR_NAME}/__pycache__"
						DESTINATION  "${CMAKE_PY_INSTALL_PREFIX}/${REL_CURRENT_SOURCE_DIR}"
					)
					set_property(GLOBAL APPEND PROPERTY SFT_INSTALLED_PYCACHE_FOLDERS "${DIR_NAME}")
				endif()
			endif()

			set_property(GLOBAL APPEND PROPERTY SFT_PY_FILES "${source}")
		endif()
	endforeach()
endfunction()


function(python_finalize)
	# pxdgen (.h -> .pxd)

	get_property(pxdgen_sources GLOBAL PROPERTY SFT_PXDGEN_SOURCES)
	file(WRITE "${CMAKE_BINARY_DIR}/py/pxdgen_sources" "${pxdgen_sources}")
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
	file(WRITE "${CMAKE_BINARY_DIR}/py/cython_modules" "${cython_modules}")
	get_property(cython_modules_embed GLOBAL PROPERTY SFT_CYTHON_MODULES_EMBED)
	file(WRITE "${CMAKE_BINARY_DIR}/py/cython_modules_embed" "${cython_modules_embed}")
	get_property(pxd_list GLOBAL PROPERTY SFT_PXD_FILES)
	file(WRITE "${CMAKE_BINARY_DIR}/py/pxd_list" "${pxd_list}")
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

	get_property(py_files GLOBAL PROPERTY SFT_PY_FILES)
	file(WRITE "${CMAKE_BINARY_DIR}/py/py_files" "${py_files}")
	set(BUILDPY_TIMEFILE "${CMAKE_BINARY_DIR}/py/compilepy_timefile")
	add_custom_command(OUTPUT "${COMPILEPY_TIMEFILE}"
		COMMAND "${PYTHON}" -m buildsystem.compileall openage
		COMMAND "${CMAKE_COMMAND}" -E touch "${COMPILEPY_TIMEFILE}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIRECTORY}"
		DEPENDS ${py_files}
		COMMENT "compiling .py files to .pyc files in __pycache__"
	)
	add_custom_target(compilepy ALL DEPENDS "${COMPILEPY_TIMEFILE}")


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


function(warn_runtime_depends)
	# warns about missing runtime dependencies of openage.
	execute_process(
		COMMAND "${PYTHON}" -m buildsystem.warn_runtime_depends
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)
endfunction()


python_init()
