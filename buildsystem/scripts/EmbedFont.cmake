# Copyright 2017-2017 the openage authors. See copying.md for legal info.

if(NOT forward_variables)
	message(FATAL_ERROR "CMake configuration variables not available. Please include(ForwardVariables.cmake)")
endif()

include("${buildsystem_dir}/modules/DownloadCache.cmake")
set(dejavu_dir "${downloads_dir}/DejaVu")
set(dejavu_zip "dejavu-fonts-ttf-2.37.tar.bz2")
set(dejavu_zip_download_path "${dejavu_dir}/${dejavu_zip}")
# TODO: disable downloading for CI builds. <Need CI first>
download_cache(
	"https://github.com/dejavu-fonts/dejavu-fonts/releases/download/version_2_37/${dejavu_zip}"
	"${dejavu_zip_download_path}"
)

execute_process(COMMAND
	"${CMAKE_COMMAND}" -E tar xf "${dejavu_zip_download_path}"
	WORKING_DIRECTORY "${dejavu_dir}"
)

file(GLOB_RECURSE dejavu_fonts "${dejavu_dir}/DejaVuSerif*.ttf")
file(COPY ${dejavu_fonts} DESTINATION "${CMAKE_INSTALL_PREFIX}/share/fonts")

file(GLOB_RECURSE dejavu_conf "${dejavu_dir}/*dejavu-serif.conf")
file(COPY ${dejavu_conf} DESTINATION "${CMAKE_INSTALL_PREFIX}/${py_install_prefix}/fonts/conf.d")
