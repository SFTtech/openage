# Copyright 2017-2017 the openage authors. See copying.md for legal info.

# Keeps all required CPack configuration in one place and include(CPack)

if(MSVC)
	# HACKHACK: Update the following if vcpkg breaks compatibility for
	# internal directories, scripts, or variables.
	set(vcpkg_dir "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
	find_program(windeployqt windeployqt.exe)

	configure_file(
		"${BUILDSYSTEM_DIR}/templates/ForwardVariables.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/ForwardVariables.cmake"
	)
	install(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/ForwardVariables.cmake")
	install(SCRIPT "${BUILDSYSTEM_DIR}/scripts/EmbedWinDependencies.cmake")
	foreach(_UTILITY ${REQUIRED_UTILITIES})
		install(FILES "${${_UTILITY}_EXECUTABLE}" DESTINATION "${CMAKE_INSTALL_BINDIR}")
	endforeach()
	configure_file(
		"${BUILDSYSTEM_DIR}/templates/openage.bat.in"
		"${CMAKE_CURRENT_BINARY_DIR}/openage.bat"
	)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/openage.bat" DESTINATION ".")

	include(InstallRequiredSystemLibraries)
endif()

set(CPACK_PACKAGE_VENDOR "The openage authors")
# TODO: set package version components instead
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Free (as in freedom) open source clone of the Age of Empires II engine 🚀")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/copying.md")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
include(CPack)
