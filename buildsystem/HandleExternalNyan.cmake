# Copyright 2020-2020 the openage authors. See copying.md for legal info.

message(CHECK_START "Looking for nyan library ...")

# first, try to locate nyan directly
# this discovers the system package or the user-registry package
find_package(nyan CONFIG)

if(nyan_FOUND)
message(CHECK_PASS "found")
endif()

# if this didn't work, we can download nyan like a git submodule.
# this is the treeish to be checked out.
if(NOT DEFINED NYAN_CLONE_VERSION)
    set(NYAN_CLONE_VERSION origin/master)
endif()

option(
        DOWNLOAD_NYAN
        "whether to clone the nyan project in case it is not found"
        OFF
)

option(
        FORCE_DOWNLOAD_NYAN
        "Force the download and usage of the nyan project"
        OFF
)

option(
        DISABLE_SUBPROJECT_UPDATES
        "Disable the automatic update of subprojects over the internet"
        OFF
)

# if nyan was not found, consider downloading it as subproject
# only use the subproject mode if it was requested
# or if it was used before.
if((NOT nyan_FOUND AND DOWNLOAD_NYAN) OR FORCE_DOWNLOAD_NYAN)
    message(STATUS "Downloading nyan as submodule project...")

    if(DISABLE_SUBPROJECT_UPDATES)
        set(DISABLE_NYAN_UPDATES "DISABLE_UPDATES")
    endif()

    fetch_project(
            NAME nyan
            ${DISABLE_NYAN_UPDATES}
            GIT_REPOSITORY https://github.com/SFTtech/nyan
            GIT_TAG ${NYAN_CLONE_VERSION}
    )

    # don't register nyan to the userpackage-repo!
    set(REGISTER_USERPACKAGE OFF)
    # don't generate the `doc` target again (name conflict!)
    set(DOXYGEN_ENABLE OFF)

    # register the targets
    add_subdirectory(${nyan_SOURCE_DIR} ${nyan_BINARY_DIR})

    if(nyan_FOUND)
        message(CHECK_PASS "nyan processed successfully!")
    endif()

elseif(NOT nyan_FOUND)
    message(CHECK_FAIL "Could not find the cmake package configuration file \"nyanConfig.cmake\".")
    message(FATAL_ERROR "
  To find it, you have several options:
  * If your distribution provides it, install \"nyan\" through the package manager.
  * If you want openage to automatically download \"nyan\", append `-DDOWNLOAD_NYAN=YES` to the cmake invocation or use `./configure --download-nyan`.
  * If you want to build nyan manually, follow the build instructions:
      [[  doc/building.md#nyan-installation  ]]
  * If you already built nyan but it still can't be found (cmake package repo fails):
    * Try to set \"nyan_DIR\" to the nyan build directory (it contains nyanConfig.cmake)
      either through:  \"./configure $youroptions -- -Dnyan_DIR=/home/dev/nyan/build\"
      or:              \"cmake $yourotheroptions -Dnyan_DIR=/home/dev/nyan/build ..\"

  In case of other problems, please try to figure them out (and tell us what you did).
  Contact information is in README.md.
")
endif()
