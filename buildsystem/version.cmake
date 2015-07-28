# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# provides detect_version, which will store the project version to PROJECT_VERSION.

function(init_version)
	set(res 1)
	if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/.git")
		# if .git exists, try running git describe
		execute_process(
			COMMAND git describe
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
			RESULT_VARIABLE res
			OUTPUT_VARIABLE version
			ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	endif()

	if(NOT res EQUAL 0)
		# if git describe didn't run or failed, read openage_version
		file(STRINGS "${CMAKE_SOURCE_DIR}/openage_version" version)
	endif()

	if(NOT version)
		message(FATAL_ERROR "failed to determine project version")
	endif()

	set(PROJECT_VERSION "${version}" PARENT_SCOPE)
endfunction()

# logs whether the option NAME is enabled
# sets WITH_${VARNAME} to HAVE
# errors if WANT_${VARNAME} conflicts with HAVE
function(have_config_option NAME VARNAME HAVE)
	set(WANT "${WANT_${VARNAME}}")
	set(WITH_${VARNAME} "${HAVE}" PARENT_SCOPE)

	if(HAVE)
		set_property(GLOBAL APPEND PROPERTY SFT_CONFIG_OPTIONS_ENABLED "${NAME}")

		if(NOT WANT)
			message(FATAL_ERROR "${NAME}: WANT_${VARNAME}=${WANT}, but WITH_${VARNAME}=${HAVE}")
		endif()
	else()
		set_property(GLOBAL APPEND PROPERTY SFT_CONFIG_OPTIONS_DISABLED "${NAME}")

		if(WANT STREQUAL "if_available")
			message("unavailable: ${NAME}")
		elseif(WANT)
			message(FATAL_ERROR "${NAME}: WANT_${VARNAME}=${WANT}, but WITH_${VARNAME}=${HAVE}")
		endif()
	endif()
endfunction()

function(print_config_options)
	get_property(enabled_opts GLOBAL PROPERTY SFT_CONFIG_OPTIONS_ENABLED)
	get_property(disabled_opts GLOBAL PROPERTY SFT_CONFIG_OPTIONS_DISABLED)

	message("enabled options:")
	if(enabled_opts)
		foreach(opt ${enabled_opts})
			message("\t${opt}")
		endforeach()
	else()
		message("\t<none>")
	endif()

	message("")

	message("disabled options:")
	if(disabled_opts)
		foreach(opt ${disabled_opts})
			message("\t${opt}")
		endforeach()
	else()
		message("\t<none>")
	endif()

	message("")
endfunction()

function(get_config_option_string)
	get_property(enabled_opts GLOBAL PROPERTY SFT_CONFIG_OPTIONS_ENABLED)

	if(enabled_opts)
		LIST(GET enabled_opts 0 CONFIG_OPTION_STRING)
		LIST(REMOVE_AT enabled_opts 0)

		foreach(opt ${enabled_opts})
			set(CONFIG_OPTION_STRING "${CONFIG_OPTION_STRING}, ${opt}")
		endforeach()
	else()
		set(CONFIG_OPTION_STRING "< no options enabled >")
	endif()

	set(CONFIG_OPTION_STRING "${CONFIG_OPTION_STRING}" PARENT_SCOPE)
endfunction()

init_version()
