# Copyright 2014-2019 the openage authors. See copying.md for legal info.

# provides macros for defining python extension modules and pxdgen sources.
# and a 'finalize' function that must be called in the end.

function(python_init)
	# filled by pxdgen; written to bin/py/pxdgen_sources for use by pxdgen.py.
	set_property(GLOBAL PROPERTY SFT_PXDGEN_SOURCES)

	# filled by add_cython_modules. used by cythonize.py.
	set_property(GLOBAL PROPERTY SFT_CYTHON_MODULES)
	set_property(GLOBAL PROPERTY SFT_CYTHON_MODULES_EMBED)

	# list of pxd files added manually
	# filled by add_pxds only. for use as depends list for cythonize.py.
	set_property(GLOBAL PROPERTY SFT_PXD_FILES)

	# filled by pxdgen. for use as output list for the pxdgen target.
	set_property(GLOBAL PROPERTY SFT_GENERATED_PXD_FILES)

	# filled with all .py filenames; used for installing.
	set_property(GLOBAL PROPERTY SFT_PY_FILES)

	# filled with the relative source dirs of the above py files.
	set_property(GLOBAL PROPERTY SFT_PY_FILES_RELSRCDIRS)

	# filled with all .py filenames that should not be installed.
	set_property(GLOBAL PROPERTY SFT_PY_FILES_NOINSTALL)

	# filled with all .py filenames that are installed to the elf binary directory.
	set_property(GLOBAL PROPERTY SFT_PY_FILES_BININSTALL)

	# filled with replacement names that are used for installing each entry in SFT_PY_FILES
	# special value __nope is used if no replacement should be done.
	# thank you cmake for not providing a dict
	# where i could just look the original name up to get the replacement name.
	set_property(GLOBAL PROPERTY SFT_PY_FILES_INSTALLNAMES)

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
	# foo/bar.pyx is compiled to a executable with embedded python interpreter,
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
			set(CPPNAME "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUTNAME}.cpp")
			set_source_files_properties("${CPPNAME}" PROPERTIES GENERATED ON)

			# construct some hopefully unique target name
			set(TARGETNAME "${REL_CURRENT_SOURCE_DIR}/${OUTPUTNAME}")
			string(REPLACE "/" "_" TARGETNAME "${TARGETNAME}")

			# generate the pretty module name
			set(PRETTY_MODULE_NAME "${REL_CURRENT_SOURCE_DIR}/${OUTPUTNAME}")
			string(REPLACE "/" "." PRETTY_MODULE_NAME "${PRETTY_MODULE_NAME}")
			string(REGEX REPLACE "^\\." "" PRETTY_MODULE_NAME "${PRETTY_MODULE_NAME}")
			set(PRETTY_MODULE_PROPERTIES "")

			if(EMBED_NEXT)
				set(PRETTY_MODULE_PROPERTIES "${PRETTY_MODULE_PROPERTIES} [embedded interpreter]")

				set_property(GLOBAL APPEND PROPERTY SFT_CYTHON_MODULES_EMBED "${source}")
				add_executable("${TARGETNAME}" "${CPPNAME}")

# CLANG
#				if(MINGW)
#					set_target_properties("${TARGETNAME}" PROPERTIES LINK_FLAGS "-municode")
#				endif()

				# TODO: use full ldflags and cflags provided by python${VERSION}-config
				target_link_libraries("${TARGETNAME}" ${PYEXT_LIBRARY})
			else()
				set_property(GLOBAL APPEND PROPERTY SFT_CYTHON_MODULES "${source}")
				add_library("${TARGETNAME}" MODULE "${CPPNAME}")

# CLANG
#				if(MINGW)
#					message(NOTICE "${CMAKE_CXX_COMPILER_ID}")
#					message(NOTICE "PY_LIBRARY_NAME is ${TARGETNAME}${PYEXT_SUFFIX}")
#					set_target_properties("${TARGETNAME}" PROPERTIES LINK_FLAGS "-municode")
#				endif()

				set_target_properties("${TARGETNAME}" PROPERTIES
					PREFIX ""
					SUFFIX "${PYEXT_SUFFIX}"
				)

				if(WIN32)
					# for MingW as well
					target_link_libraries("${TARGETNAME}" ${PYEXT_LIBRARY})
				endif()
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
			if(NOT EMBED_NEXT)
				if(APPLE)
					set_target_properties("${TARGETNAME}" PROPERTIES LINK_FLAGS "-undefined dynamic_lookup" )
				elseif(MINGW)
					#set_target_properties("${TARGETNAME}" PROPERTIES LINK_FLAGS "-llibpython3.8" )
				endif()
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

		get_filename_component(source_ext "${source}" EXT)
		if(NOT "${source_ext}" STREQUAL ".h")
			message(FATAL_ERROR "non-.h file given to pxdgen: ${source}")
		endif()

		# TODO: change if multiple files with the same name are supported in a directory
		get_filename_component(source_name_without_ext "${source}" NAME_WE)
		set(PXDNAME "${CMAKE_CURRENT_BINARY_DIR}/${source_name_without_ext}.pxd")
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
	#
	# add_py_module (
	#    somefile.py
	#    NOINSTALL otherfile.py          # do not install this file
	#    BININSTALL wtf.py               # install to $PREFIX/bin/wtf.py
	#    BININSTALL lol.py AS rofl       # install lol.py to $PREFIX/bin/rofl
	# )
	#
	# because cmake is such a nice language,
	# the handling of the above is quite painful.

	file(RELATIVE_PATH REL_CURRENT_SOURCE_DIR "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

	set(NOINSTALL_NEXT FALSE)
	set(BININSTALL_NEXT FALSE)
	set(AS_NEXT FALSE)
	set(IN_BININSTALL FALSE)
	foreach(source ${ARGN})
		# keyword parsing so the next arg knows
		# what the previous keyword was.
		if(source STREQUAL "NOINSTALL")
			set(NOINSTALL_NEXT TRUE)
		elseif(source STREQUAL "BININSTALL")
			set(BININSTALL_NEXT TRUE)
		elseif(source STREQUAL "AS")
			set(AS_NEXT TRUE)
		else()
			if(NOT ${source} MATCHES ".*\\.py$" AND NOT AS_NEXT)
				message(FATAL_ERROR "non-Python file given to add_py_modules: ${source}")
			endif()

			if(IN_BININSTALL AND NOT AS_NEXT)
				# if after the source file for /bin installation no AS
				# follows, normal source listings follow.
				set(IN_BININSTALL FALSE)
			endif()

			if(NOT IS_ABSOLUTE "${source}" AND NOT AS_NEXT)
				# make the source absolute, except when source is the "AS $replacementname"
				set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
			endif()

			if(NOINSTALL_NEXT OR BININSTALL_NEXT)
				# if source should not be installed or be installed to bin/
				# it is excluded from the big python installation list
				set_property(GLOBAL APPEND PROPERTY SFT_PY_FILES_NOINSTALL "${source}")
				set(NOINSTALL_NEXT FALSE)
			endif()

			if(BININSTALL_NEXT)
				# if the file is to be installed to bin/
				set_property(GLOBAL APPEND PROPERTY SFT_PY_FILES_BININSTALL "${source}")
				set(BININSTALL_NEXT FALSE)
				set(IN_BININSTALL TRUE)
			endif()

			if(AS_NEXT)
				# the replacement name must be after the BININSTALL definition
				if(NOT IN_BININSTALL)
					message(FATAL_ERROR "you used AS without BININSTALL!")
				endif()

				set_property(GLOBAL APPEND PROPERTY SFT_PY_FILES_INSTALLNAMES "${source}")
			else()
				# in all cases except the "AS $newname",
				# add the python file to the to-compile list.

				if("${REL_CURRENT_SOURCE_DIR}" STREQUAL "")
					set(REL_CURRENT_SOURCE_DIR "./")
				endif()

				set_property(GLOBAL APPEND PROPERTY SFT_PY_FILES "${source}")
				set_property(GLOBAL APPEND PROPERTY SFT_PY_FILES_RELSRCDIRS "${REL_CURRENT_SOURCE_DIR}")
			endif()

			if(NOT IN_BININSTALL)
				# don't replace the install name if we're in the "AS $name"
				# or no AS followed the BININSTALL

				set_property(GLOBAL APPEND PROPERTY SFT_PY_FILES_INSTALLNAMES "__nope")
			endif()

			if(AS_NEXT)
				# reset the alias and bin-install flags
				# so they count only for one (the current) source file.
				set(AS_NEXT FALSE)
				set(IN_BININSTALL FALSE)
			endif()
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
		--output-dir "${CMAKE_BINARY_DIR}"
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
		# remove unneeded files from the previous builds (if any)
		COMMAND "${CMAKE_COMMAND}" -E remove -f
			"${CMAKE_BINARY_DIR}/__init__.py"
			"${CMAKE_BINARY_DIR}/__init__.pxd"
		COMMAND "${PYTHON}" -m buildsystem.cythonize
		"${CMAKE_BINARY_DIR}/py/cython_modules"
		"${CMAKE_BINARY_DIR}/py/cython_modules_embed"
		"${CMAKE_BINARY_DIR}/py/pxd_list"
		"--build-dir" "${CMAKE_BINARY_DIR}"
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
	get_property(py_files_srcdirs GLOBAL PROPERTY SFT_PY_FILES_RELSRCDIRS)
	get_property(py_files_noinstall GLOBAL PROPERTY SFT_PY_FILES_NOINSTALL)
	get_property(py_files_bininstall GLOBAL PROPERTY SFT_PY_FILES_BININSTALL)
	get_property(py_install_names GLOBAL PROPERTY SFT_PY_FILES_INSTALLNAMES)

	write_on_change("${CMAKE_BINARY_DIR}/py/py_files" "${py_files}")
	set(COMPILEPY_TIMEFILE "${CMAKE_BINARY_DIR}/py/compilepy_timefile")
	set(COMPILEPY_INVOCATION
		"${PYTHON}" -m buildsystem.compilepy
		"${CMAKE_BINARY_DIR}/py/py_files"
		"${CMAKE_SOURCE_DIR}"
		"${CMAKE_BINARY_DIR}"
	)

	# determine the compiled file name for all source files
	execute_process(COMMAND
		${COMPILEPY_INVOCATION} "--print-output-paths-only"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE py_compiled_files
		RESULT_VARIABLE COMMAND_RESULT
	)
	if(NOT ${COMMAND_RESULT} EQUAL 0)
		message(FATAL_ERROR "failed to get output list from compilepy invocation")
	endif()
	string(STRIP "${py_compiled_files}" py_compiled_files)

	add_custom_command(OUTPUT "${COMPILEPY_TIMEFILE}"
		COMMAND ${COMPILEPY_INVOCATION}
		COMMAND "${CMAKE_COMMAND}" -E touch "${COMPILEPY_TIMEFILE}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		DEPENDS "${CMAKE_BINARY_DIR}/py/py_files" ${py_files}
		COMMENT "compiling .py files to .pyc files"
	)
	add_custom_target(compilepy ALL DEPENDS "${COMPILEPY_TIMEFILE}")

	list(LENGTH py_files py_files_count)
	math(EXPR py_files_count_range "${py_files_count} - 1")

	foreach(idx RANGE ${py_files_count_range})
		list(GET py_files ${idx} pyfile)
		list(GET py_compiled_files ${idx} pycfile)
		list(GET py_files_srcdirs ${idx} pyrelsrcdir)
		list(GET py_install_names ${idx} pyinstallname)
		file(TO_CMAKE_PATH ${pycfile} pycfile)

		list(FIND py_files_noinstall "${pyfile}" do_install)
		list(FIND py_files_bininstall "${pyfile}" do_bininstall)

		if(do_install LESS 0)
			install(
				FILES "${pyfile}"
				DESTINATION  "${CMAKE_PY_INSTALL_PREFIX}/${pyrelsrcdir}"
			)
			install(
				FILES "${pycfile}"
				DESTINATION "${CMAKE_PY_INSTALL_PREFIX}/${pyrelsrcdir}/__pycache__"
			)
		elseif(do_bininstall GREATER -1)
			# install the python file to the elf binary dir

			if("${pyinstallname}" STREQUAL "__nope")
				install(
					PROGRAMS "${pyfile}"
					DESTINATION "${CMAKE_INSTALL_BINDIR}"
				)
			else()
				# optionally, rename the file:
				install(
					PROGRAMS "${pyfile}"
					RENAME "${pyinstallname}"
					DESTINATION "${CMAKE_INSTALL_BINDIR}"
				)
			endif()
		endif()
	endforeach()

	##################################################
	# code generation

	codegen_run()

	# inplace module install (bin/module.so -> module.so)
	# MSVC, Xcode (multi-config generators) produce outputs
	# in a directory different from `CMAKE_CURRENT_BINARY_DIR`.
	# link/copy the output files as required for the cython modules.

	get_property(cython_module_targets GLOBAL PROPERTY SFT_CYTHON_MODULE_TARGETS)
	set(cython_module_files_expr)
	foreach(cython_module_target ${cython_module_targets})
		list(APPEND cython_module_files_expr "$<TARGET_FILE:${cython_module_target}>")
	endforeach()

	set(INPLACEMODULES_LISTFILE "${CMAKE_BINARY_DIR}/py/inplace_module_list_$<CONFIG>")
	file(GENERATE
		OUTPUT ${INPLACEMODULES_LISTFILE}
		CONTENT "${cython_module_files_expr}"
	)
	set(INPLACEMODULES_INVOCATION
		"${PYTHON}" -m buildsystem.inplacemodules
		${INPLACEMODULES_LISTFILE}
		"$<CONFIG>"
	)
	set(INPLACEMODULES_TIMEFILE "${CMAKE_BINARY_DIR}/py/inplacemodules_timefile")
	add_custom_command(OUTPUT "${INPLACEMODULES_TIMEFILE}"
		COMMAND ${INPLACEMODULES_INVOCATION}
		DEPENDS
		${INPLACEMODULES_LISTFILE} ${cython_module_targets}
		COMMAND "${CMAKE_COMMAND}" -E touch "${INPLACEMODULES_TIMEFILE}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		COMMENT "creating in-place modules"
	)
	add_custom_target(inplacemodules ALL DEPENDS "${INPLACEMODULES_TIMEFILE}")


	# cleaning of all in-sourcedir stuff

	add_custom_target(cleancython
		COMMAND ${INPLACEMODULES_INVOCATION} --clean
		COMMAND "${PYTHON}" -m buildsystem.cythonize --clean
		"${CMAKE_BINARY_DIR}/py/cython_modules"
		"${CMAKE_BINARY_DIR}/py/cython_modules_embed"
		"${CMAKE_BINARY_DIR}/py/pxd_list"
		"--build-dir" "${CMAKE_BINARY_DIR}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)
	add_custom_command(TARGET cleancython POST_BUILD
		# general deleters to catch files that have already been un-listed.
		COMMAND find openage -name "'*.cpp'" -type f -print -delete
		COMMAND find openage -name "'*.html'" -type f -print -delete
		COMMAND find openage -name "'*.so'" -type f -print -delete
		COMMAND "${CMAKE_COMMAND}" -E remove "${CYTHONIZE_TIMEFILE}"
		COMMAND "${CMAKE_COMMAND}" -E remove "${INPLACEMODULES_TIMEFILE}"
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
	)

	add_custom_target(cleanpxdgen
		COMMAND find libopenage -name "'*.pxd'" -type f -print -delete
		COMMAND find libopenage -name "__init__.py" -type f -print -delete
		COMMAND "${CMAKE_COMMAND}" -E remove "${PXDGEN_TIMEFILE}"
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
	)


	# check for any unlisted .py files, and error.

	execute_process(
		COMMAND "${PYTHON}" -m buildsystem.check_py_file_list
		"${CMAKE_BINARY_DIR}/py/py_files"
		"${CMAKE_SOURCE_DIR}/openage"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		RESULT_VARIABLE res
	)
	if(NOT res EQUAL 0)
		message(FATAL_ERROR ".py file listing inconsistent")
	endif()
endfunction()

python_init()
