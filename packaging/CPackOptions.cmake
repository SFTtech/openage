# Copyright 2019-2019 the openage authors. See copying.md for legal info.

# Keeps all required CPack configuration for the Generators in one place and
# CPack calls this file for every generator, keep this in mind, if you use
# variables or functions outside of the conditional branch


########################################################################
# Continuous Integration

# Filename addition for architecture, set from environment variables
set(CPACK_PACKAGE_ARCHITECTURE "$ENV{TARGET_PLATFORM}")

# Filename addition for nightly-builds, set from environment variables
if(DEFINED ENV{IS_NIGHTLY})
    set(PACKAGE_VERSION_STRING "${CPACK_VERSION_FULL_STRING}")
    set(PACKAGE_NIGHTLY "_NIGHTLY")
else()
    set(PACKAGE_VERSION_STRING "v${PROJECT_VERSION}")
    message(DEBUG "${PACKAGE_VERSION_STRING}")
    set(PACKAGE_NIGHTLY "")
endif()


if(CPACK_GENERATOR MATCHES "NSIS")

    # Explanations to the following commands you can find here:
    # https://cmake.org/cmake/help/latest/cpack_gen/nsis.html#cpack_gen:CPack%20NSIS%20Generator

    ########################################################################
    # Filename
    message(DEBUG "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PACKAGE_VERSION_STRING}-${CPACK_PACKAGE_ARCHITECTURE}-installer${PACKAGE_NIGHTLY}")


    ########################################################################
    # Installer

    set(CPACK_NSIS_MODIFY_PATH "OFF")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
    set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME} v${CPACK_PACKAGE_VERSION}")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME} v${CPACK_PACKAGE_VERSION}")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/SFTtech/openage")
    set(CPACK_NSIS_HELP_LINK "https://github.com/SFTtech/openage/blob/master/doc/troubleshooting.md")

    # TODO: create welcome page
    #set(CPACK_RESOURCE_FILE_WELCOME "")


    ########################################################################
    # Shortcuts

    # TODO: create startmenu shortcut
    #set(CPACK_PACKAGE_EXECUTABLES "")
    # TODO: create desktop shortcut
    #set(CPACK_CREATE_DESKTOP_LINKS "")

    # Link to launcher (exe) can go here, *.bat is not working
    #set(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_CURRENT_BINARY_DIR}\\\\openage.bat")
    #SET(CPACK_PACKAGE_EXECUTABLES "openage.bat;openage")

endif()


if(CPACK_GENERATOR MATCHES "7Z")
    message(DEBUG "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PACKAGE_VERSION_STRING}-${CPACK_PACKAGE_ARCHITECTURE}-portable${PACKAGE_NIGHTLY}")
endif()
