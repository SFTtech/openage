# checks python availability,
# provides macros for defining python packages, python cpp extension modules,
# and a generator macro that must be called after including alle source dirs

# for an example usage, see py/openage/convert/CMakeLists.txt

function(python_init)
	# look for the python3.3+ executable
	find_package(PythonInterp 3.3)

	if(${PYTHONINTERP_FOUND} STREQUAL "FALSE")
		message(FATAL_ERROR "python >= 3.3 not found.")
	endif()

	# these following lists are filled by the add_py_package and add_pyext_module functions:
	set_property(GLOBAL PROPERTY "SFT_PY_PACKAGES")
	set_property(GLOBAL PROPERTY "SFT_PY_EXT_MODULES")

	# there will be more lists SFT_PY_PACKAGE_{packagename} and SFT_PY_EXT_MODULE_{extmodulename}
	# that will contain the python source files for each package and cpp source files for each ext module.
	# all of those lists will be used to generate setup.py in the generator function TODO

	set(PYTHON_SOURCE_DIR "${CMAKE_SOURCE_DIR}/py")
	set(PYTHON_SOURCE_DIR ${PYTHON_SOURCE_DIR} PARENT_SCOPE)

	set(PYTHON3 ${PYTHON_EXECUTABLE})
	set(PYTHON3 ${PYTHON3} PARENT_SCOPE)

	set(PYTHON_INVOCATION ${PYTHON3} ${BUILDSYSTEM_DIR}/runinenv "PYTHONPATH:prependpath:${PYTHON_SOURCE_DIR}" -- ${PYTHON3})
	set(PYTHON_INVOCATION ${PYTHON_INVOCATION} PARENT_SCOPE)
endfunction()


function(get_py_module_name var path)
	if(NOT IS_ABSOLUTE ${path})
		set(path ${CMAKE_CURRENT_SOURCE_DIR}/${path})
	endif()

	file(RELATIVE_PATH relpath ${PYTHON_SOURCE_DIR} ${path})

	string(REPLACE "/" "." name ${relpath})
	set(${var} ${name} PARENT_SCOPE)
endfunction()


function(get_py_module_path var name)
	string(REPLACE "." "/" relpath ${name})
	set(${var} "${PYTHON_SOURCE_DIR}/${relpath}" PARENT_SCOPE)
endfunction()


function(add_py_package name)
	# check whether the package has already been defined
	get_property(package GLOBAL PROPERTY SFT_PY_PACKAGE_${name})
	if(${package})
		message(FATAL_ERROR "py package ${name} has already been defined!")
	else()
		message("py package: ${name}")
	endif()

	set_property(GLOBAL APPEND PROPERTY SFT_PY_PACKAGES ${name})

	# find all sourcefiles for this package
	# this list of sourcefiles is not used directly for building,
	# just for determining whether a re-build is neccesary.
	get_py_module_path(package_path ${name})
	file(GLOB package_sources "${package_path}/*.py")
	set_property(GLOBAL PROPERTY SFT_PY_PACKAGE_${name})
	foreach(sourcefile ${package_sources})
		print_filename(${sourcefile})
		set_property(GLOBAL APPEND PROPERTY SFT_PY_PACKAGE_${name} "${sourcefile}")
	endforeach()
	message("")
endfunction()


function(add_pyext_module name)
	# check whether the module has already been defined
	get_property(module GLOBAL PROPERTY SFT_PY_EXT_MODULE_${name})
	if(${module})
		message(FATAL_ERROR "py cpp extension module ${name} has already been defined!")
	else()
		message("py cpp extension module: ${name}")
	endif()

	set_property(GLOBAL APPEND PROPERTY SFT_PY_EXT_MODULES ${name})

	# process the user-supplied list of cpp source files
	set_property(GLOBAL PROPERTY SFT_PY_EXT_MODULE_${name})
	foreach(sourcefile ${ARGN})
		set(sourcefile "${CMAKE_CURRENT_SOURCE_DIR}/${sourcefile}")
		print_filename(${sourcefile})
		set_property(GLOBAL APPEND PROPERTY SFT_PY_EXT_MODULE_${name} "${sourcefile}")
	endforeach()
	message("")
endfunction()


function(python_modules_in_prefix prefix varname)
	# stores all pure python modules that lie inside prefix to ${varname}
	set(modules)

	get_property(packages GLOBAL PROPERTY SFT_PY_PACKAGES)
	foreach(package ${packages})
		string(FIND "${package}" "${prefix}" match)
		if(${match} EQUAL 0)
			get_property(sourcefiles GLOBAL PROPERTY SFT_PY_PACKAGE_${package})
			foreach(sourcefile ${sourcefiles})
				list(APPEND modules "${sourcefile}")
			endforeach()
		endif()
	endforeach()

	set(${varname} "${modules}" PARENT_SCOPE)
endfunction()


function(process_python_modules)
	set(all_sourcefiles)

	set(pkg_src "\n")
	get_property(packages GLOBAL PROPERTY SFT_PY_PACKAGES)
	foreach(package ${packages})
		set(pkg_src "${pkg_src}    '${package}': [")
		get_property(sourcefiles GLOBAL PROPERTY SFT_PY_PACKAGE_${package})
		foreach(sourcefile ${sourcefiles})
			set(pkg_src "${pkg_src}'${sourcefile}', ")
			list(APPEND all_sourcefiles "${sourcefile}")
		endforeach()
		set(pkg_src "${pkg_src}],\n")
	endforeach()

	set(ext_src "\n")
	get_property(modules GLOBAL PROPERTY SFT_PY_EXT_MODULES)
	foreach(module ${modules})
		set(ext_src "${ext_src}    '${module}': [")
		get_property(sourcefiles GLOBAL PROPERTY SFT_PY_EXT_MODULE_${module})
		foreach(sourcefile ${sourcefiles})
			set(ext_src "${ext_src}'${sourcefile}', ")
			list(APPEND all_sourcefiles "${sourcefile}")
		endforeach()
		set(ext_src "${ext_src}],\n")
	endforeach()

	set(SETUP_PY_IN "${BUILDSYSTEM_DIR}/templates/setup.py.in")
	set(SETUP_PY    "${CMAKE_BINARY_DIR}/py/setup.py")
	set(PY_TIMEFILE "${CMAKE_BINARY_DIR}/py/timefile")

	set(SETUP_INVOCATION ${PYTHON3} ${BUILDSYSTEM_DIR}/runinenv CXX=${CMAKE_CXX_COMPILER} CC=${CMAKE_C_COMPILER} -- ${PYTHON3} ${SETUP_PY})

	# create setup.py file for python module creation
	#
	# * the time stamp file is used for rebuilding
	# * all python-C extensions are built in-place
	#   so they can be used for imports in the development tree
	# * the pure python modules are compiled to pyc
	configure_file(${SETUP_PY_IN} ${SETUP_PY})
	add_custom_command(OUTPUT ${PY_TIMEFILE}
			COMMAND ${SETUP_INVOCATION} build_ext --inplace
			COMMAND ${SETUP_INVOCATION} build
			COMMAND ${CMAKE_COMMAND} -E touch ${PY_TIMEFILE}
			DEPENDS ${all_sourcefiles}
			COMMENT "building python modules (via setup.py)")

	add_custom_target(pymodules ALL DEPENDS ${PY_TIMEFILE})

	add_custom_target(cleanpymodules
		COMMAND ${SETUP_INVOCATION} clean --all
		COMMAND ${CMAKE_COMMAND} -E remove ${PY_TIMEFILE}
	)

	# create call to setup.py when installing
	# * set the install prefix at configure time
	# * evaluate the temporary install destination parameter DESTDIR at 'make install'-time
	install(CODE "execute_process(COMMAND ${PYTHON3} ${SETUP_PY} install --prefix=${CMAKE_INSTALL_PREFIX} --root=\$ENV{DESTDIR})")
endfunction()

python_init()
