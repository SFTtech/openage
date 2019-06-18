# Copyright 2017-2019 the openage authors. See copying.md for legal info.

if(NOT forward_variables)
	message(FATAL_ERROR "CMake configuration variables not available. Please include(ForwardVariables.cmake)")
endif()

include("${buildsystem_dir}/modules/DownloadCache.cmake")
if(sizeof_void_p EQUAL 8)
	set(py_arch "amd64")
else()
	set(py_arch "win32")
endif()
set(python_embed_zip "python-${py_version}-embed-${py_arch}.zip")
set(python_zip_download_path "${downloads_dir}/${python_embed_zip}")
# TODO: disable downloading for CI builds. <Need CI first>
download_cache(
	"https://www.python.org/ftp/python/${py_version}/${python_embed_zip}"
	"${python_zip_download_path}"
)

execute_process(COMMAND
	"${CMAKE_COMMAND}" -E tar xf "${python_zip_download_path}"
	WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${py_install_prefix}"
)

execute_process(COMMAND
	"${python}" "${buildsystem_dir}/scripts/copy_modules.py"
	numpy PIL pyreadline readline
	"${CMAKE_INSTALL_PREFIX}/${py_install_prefix}"
)
