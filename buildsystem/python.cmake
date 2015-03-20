# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# checks python availability,
# provides macros for defining python packages, python cpp extension modules,
# and a generator macro that must be called after including alle source dirs

# for an example usage, see py/openage/convert/CMakeLists.txt

function(python_init)
	find_package(Python 3.4 REQUIRED)

	# this list is filled by the add_py_packages function.
	# it's then written to bin/py/packages for use by setup.py.
	set_property(GLOBAL PROPERTY SFT_PY_PACKAGES)

	# this list is filled by the pxdgen function.
	# it's then written to bin/py/pxdgen_sources for use by pxdgen.
	set_property(GLOBAL PROPERTY SFT_PXDGEN_SOURCES)

	# this list is filled by the add_cython_modules function.
	# it's then written to bin/py/cython_modules for use by setup.py build_ext.
	set_property(GLOBAL PROPERTY SFT_CYTHON_MODULES)

	set(PYTHON3 "${PYTHON_EXECUTABLE}")
	set(PYTHON3 "${PYTHON3}" PARENT_SCOPE)
	set(PYTHON_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}" PARENT_SCOPE)
	set(PYTHON_VERSION_STRING "${PYTHON_VERSION_STRING}" PARENT_SCOPE)

	exec_program(
		"${PYTHON3}"
		ARGS -m cython --version
		OUTPUT_VARIABLE CYTHON_VERSION
		RETURN_VALUE CYTHON_RETVAL
	)

	if (NOT "${CYTHON_RETVAL}" EQUAL 0)
		message(FATAL_ERROR "${PYTHON3} can not call Cython")
	endif()

	set(CYTHON_VERSION "${CYTHON_VERSION}" PARENT_SCOPE)

	set(PYTHON_INVOCATION env "PYTHON_INTERP=${PYTHON3}" ${CMAKE_SOURCE_DIR}/run PARENT_SCOPE)
endfunction()


function(add_py_packages)
	foreach(package ${ARGN})
		set_property(GLOBAL APPEND PROPERTY SFT_PY_PACKAGES "${package}")
	endforeach()
endfunction()


function(add_cython_modules)
	foreach(source ${ARGN})
		if(NOT IS_ABSOLUTE "${source}")
			set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
		endif()

		set_property(GLOBAL APPEND PROPERTY SFT_CYTHON_MODULES "${source}")
	endforeach()
endfunction()


function(pxdgen)
	foreach(source ${ARGN})
		if(NOT IS_ABSOLUTE "${source}")
			set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
		endif()

		set_property(GLOBAL APPEND PROPERTY SFT_PXDGEN_SOURCES "${source}")
	endforeach()
endfunction()


function(python_finalize)
	get_property(packages GLOBAL PROPERTY SFT_PY_PACKAGES)
	file(WRITE "${CMAKE_BINARY_DIR}/py/packages" "${packages}")
	get_property(pxdgen_sources GLOBAL PROPERTY SFT_PXDGEN_SOURCES)
	file(WRITE "${CMAKE_BINARY_DIR}/py/pxdgen_sources" "${pxdgen_sources}")
	get_property(cython_modules GLOBAL PROPERTY SFT_CYTHON_MODULES)
	file(WRITE "${CMAKE_BINARY_DIR}/py/cython_modules" "${cython_modules}")

	set(PXDGEN_TIMEFILE "${CMAKE_BINARY_DIR}/py/pxdgen_timefile")
	set(PXDGEN_SOURCES_FILE "${CMAKE_BINARY_DIR}/py/pxdgen_sources")
	set(PXDGEN_DIR "${CMAKE_BINARY_DIR}/py/pxds")
	file(MAKE_DIRECTORY "${PXDGEN_DIR}")

	# cmake automatically adds -rpath to all executable it links, and removes it when installing them.
	# since the python extension modules are, however, not compiled by cmake, we need to do it manually here.
	# setup.py appends this to the linker flags if called with --inplace.
	set(CYTHON_INPLACE_LINKER_FLAGS "-Wl,-rpath,${CMAKE_BINARY_DIR}/cpp")

	set(CYTHON_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		# with our flags, clang complains about some unused functions when compiling cython-generated code.

		# TODO -Wno-absolute-value is required to avoid a warning:
		#      https://github.com/cython/cython/pull/402
		#      (pending cython 0.23)
		# TODO it's not implemented in all compilers, though.
		set(CYTHON_CXX_FLAGS "${CYTHON_CXX_FLAGS} -Wno-absolute-value -Wno-unused-function -Wno-unneeded-internal-declaration -Wno-extended-offsetof")
	endif()

	configure_file("${BUILDSYSTEM_DIR}/templates/py.conf.in" "${CMAKE_BINARY_DIR}/py/py.conf")

	message("py packages")
	foreach(package ${packages})
		message("\t${package}")
	endforeach()
	message("")

	message("cython modules")
	foreach(filename ${cython_modules})
		print_filename("${filename}")
	endforeach()
	message("")

	add_custom_command(OUTPUT "${PXDGEN_TIMEFILE}"
		COMMAND "${PYTHON3}" -m buildsystem.pxdgen --target-dir "${PXDGEN_DIR}/libopenage" --file-list "${PXDGEN_SOURCES_FILE}"
		COMMAND "${CMAKE_COMMAND}" -E touch "${PXDGEN_TIMEFILE}"
		DEPENDS ${pxdgen_sources}
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		COMMENT "pxdgen.py: generating cython .pxd files"
	)

	add_custom_target(pxdgen DEPENDS "${PXDGEN_TIMEFILE}")

	set(PY_TIMEFILE "${CMAKE_BINARY_DIR}/py/py_timefile")

	add_custom_command(OUTPUT "${PY_TIMEFILE}"
		COMMAND "${PYTHON3}" "${BUILDSYSTEM_DIR}/setup.py" build_ext --inplace
		COMMAND "${CMAKE_COMMAND}" -E touch "${PY_TIMEFILE}"
		DEPENDS ${cython_modules} ${PXDGEN_TIMEFILE} "${PROJECT_NAME}"
		COMMENT "setup.py: cython extensions"
	)

	add_custom_target(python ALL DEPENDS "${PY_TIMEFILE}"
		SOURCES ${cython_sourcefiles}
	)

	add_custom_target(cleanpython
		COMMAND "${PYTHON3}" "${BUILDSYSTEM_DIR}/setup.py" clean --all
		COMMAND "${CMAKE_COMMAND}" -E remove "${PY_TIMEFILE}"
	)

	add_custom_target(cleanpxdgen
		COMMAND "${CMAKE_COMMAND}" -E remove_directory "${PXDGEN_DIR}"
		COMMAND "${CMAKE_COMMAND}" -E remove "${PXDGEN_TIMEFILE}"
	)

	# add setup.py to the install target
	# * set the install prefix at configure time
	# * evaluate the temporary install destination parameter DESTDIR at 'make install'-time
	install(CODE "execute_process(COMMAND ${PYTHON3} \"${BUILDSYSTEM_DIR}/setup.py\" install --prefix=${CMAKE_INSTALL_PREFIX} --root=\$ENV{DESTDIR})")
endfunction()


function(warn_runtime_depends)
	execute_process(
		COMMAND "${PYTHON3}" -m buildsystem.warn_runtime_depends
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)
endfunction()


python_init()
