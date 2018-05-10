# Copyright 2017-2017 the openage authors. See copying.md for legal info.

# DownloadCache, as the name suggests, downloads a file and
# if it succeeds, caches it for subsequent runs.

# function args:
# * url: the source URL to download the file from.
# * download_path: the destination file path.
function(download_cache url download_path)
	set(download_stamp_path "${download_path}.stamp")
	if(EXISTS "${download_stamp_path}")
		message(STATUS "using cached ${download_path}")
		return()
	endif()
	file(DOWNLOAD "${url}" "${download_path}"
		STATUS download_status
		SHOW_PROGRESS
	)
	list(GET download_status 0 status_code)
	if(NOT status_code EQUAL 0)
		list(GET dowload_status 1 reason)
		message(FATAL_ERROR "Failed to download from ${url}: ${reason}")
	endif()
	file(WRITE "${download_stamp_path}" "")
endfunction()
