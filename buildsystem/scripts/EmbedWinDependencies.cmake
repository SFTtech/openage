# Copyright 2017-2019 the openage authors. See copying.md for legal info.

if(NOT forward_variables)
	message(FATAL_ERROR "CMake configuration variables not available. Please include(ForwardVariables.cmake)")
endif()

get_filename_component(cl_directory "${cmake_linker}" DIRECTORY)
find_program(dumpbin dumpbin.exe PATHS "${cl_directory}")
if(NOT dumpbin)
	message(SEND_ERROR "Cannot locate dumpbin.")
	return()
endif()

# function to wrap the dependency resolving and copying logic on Windows.
# Inspired by `applocal.ps1` from vcpkg which recursively descends into the
# dependency tree of a binary and copies each to the target directory.
function(resolve binary)
	get_filename_component(bin_dir "${binary}" DIRECTORY)
	execute_process(COMMAND "${dumpbin}" /DEPENDENTS "${binary}"
		RESULT_VARIABLE dump_result
		OUTPUT_VARIABLE dump_output
	)
	if(NOT dump_result EQUAL 0)
		message(SEND_ERROR "dumpbin returned ${dump_result}.")
		return()
	endif()
	string(REGEX MATCHALL "    [^\\\n]+\\.dll" dll_list ${dump_output})

	# manually add qt dlls
	list(APPEND dll_list "Qt5Widgets.dll")
	list(APPEND dll_list "qtquickcontrolsplugin.dll")
<<<<<<< HEAD
=======
	list(APPEND dll_list "qwindows.dll")
>>>>>>> ae1b144bb... Use applocal.ps1 to resolve dependencies

	foreach(item ${dll_list})
		string(STRIP "${item}" dll_name)

		#qtquickcontrolsplugin.dll
<<<<<<< HEAD
		if(dll_name EQUAL "qtquickcontrolsplugin.dll")
			set(dll_src_path "${vcpkg_dir}/qml/QtQuick/Controls/${dll_name}")
=======
		if("${dll_name}" EQUAL "qtquickcontrolsplugin.dll")
			set(dll_src_path "${vcpkg_dir}/qml/QtQuick/Controls/${dll_name}")
		elseif("${dll_name}" EQUAL "qwindows.dll")
			set(dll_src_path "${vcpkg_dir}/plugins/platforms/${dll_name}")
>>>>>>> ae1b144bb... Use applocal.ps1 to resolve dependencies
		else()
			set(dll_src_path "${vcpkg_dir}/bin/${dll_name}")
		endif()

		set(dll_dest_path "${bin_dir}/${dll_name}")
		if(NOT EXISTS "${dll_dest_path}" AND EXISTS "${dll_src_path}")
			message(STATUS "Copying ${dll_name}")
			file(COPY "${dll_src_path}" DESTINATION "${bin_dir}")
			resolve("${dll_dest_path}")
		endif()
	endforeach()
endfunction()

foreach(file ${CMAKE_INSTALL_MANIFEST_FILES})
	if(file MATCHES "\\.dll$")
		if(windeployqt)
			resolve("${file}")
			execute_process(COMMAND "${CMAKE_COMMAND}" -E env "PATH=${vcpkg_dir}/bin;$ENV{PATH}"
				"${windeployqt}"
				--qmldir "${CMAKE_INSTALL_PREFIX}/${asset_dir}/qml"
				--dir "${CMAKE_INSTALL_PREFIX}/${py_install_prefix}"
				"${file}"
			)
		else()
			message(STATUS "Applocal used with ${file}!")
			set(APPLOCAL_SCRIPT "${vcpkg_dir}/../../scripts/buildsystems/msbuild/applocal.ps1")
			set(POWERSHELL_COMMAND "C:/Windows/System32/WindowsPowerShell/v1.0/powershell.exe")
			message(STATUS "Applocal used with ${POWERSHELL_COMMAND}!")
			message(STATUS "Use this command manually for diagnosing: ${POWERSHELL_COMMAND} ${APPLOCAL_SCRIPT} -targetBinary ${file} -installedDir ${vcpkg_dir}/bin -tlogFile ${APPLOCAL_LOGFILE_DIR}/applocal.log -copiedFilesLog ${APPLOCAL_LOGFILE_DIR}/applocal_copied_files.log!")
			execute_process(COMMAND "${POWERSHELL_COMMAND}" "${APPLOCAL_SCRIPT}" 
					-targetBinary "${file}" 
					-installedDir "${vcpkg_dir}/bin" 
					-tlogFile "${APPLOCAL_LOGFILE_DIR}/applocal.log" 
					-copiedFilesLog "${APPLOCAL_LOGFILE_DIR}/applocal_copied_files.log"
					ERROR_VARIABLE "${APPLOCAL_LOGFILE_DIR}/applocal_error.log"
					OUTPUT_FILE "${APPLOCAL_LOGFILE_DIR}/applocal_output.log"
			)
		endif()
	endif()
endforeach()

# The fonts directory defaults to the same directory as the executable.
# In this case, it is python.exe at `py_install_prefix`.
file(COPY "${vcpkg_dir}/tools/fontconfig/fonts"
	DESTINATION "${CMAKE_INSTALL_PREFIX}/${py_install_prefix}"
)

include("${CMAKE_CURRENT_LIST_DIR}/EmbedFont.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/EmbedPython.cmake")