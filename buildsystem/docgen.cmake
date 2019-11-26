# Copyright 2019-2019 the openage authors. See copying.md for legal info.

# Implement the target 'webdoc' - documentation generation with doxygen and sphinx
# for the purpose to host online

# Set the output path
set(WEBDOC_PATH "${CMAKE_BINARY_DIR}/webdoc")
#set(WEBDOC_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/webdoc-hosting")
set(WEBDOC_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../openage-webdoc/")

# branch 'webdoc build'

if(${SPHINX_FOUND})
    add_custom_target(webdoc-doxygen
            "${DOXYGEN_EXECUTABLE}" "${WEBDOC_PATH}/Doxyfile"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/"
            VERBATIM
            COMMENT "generating webdocs... (doxygen-web)"
            )

    add_custom_target(webdoc-sphinx
            "${SPHINX_EXECUTABLE}"
            -E -a -b html
            -c "${WEBDOC_PATH}"
            -j "auto"
            # -d "${SPHINX_CACHE_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${WEBDOC_OUTPUT_PATH}/sphinx"
            COMMENT "generating webdocs... (sphinx-web)"
            )
    # Todo: Use semicolon seperated list elements like before

    # dass nyan;url steht
    # und aus dem ding kannst du ja auch wieder listenelemente auslesen
    # Projects that need to be fetched
    set(PROJECT_LIST_NAMES
            nyan
            openage-modding
            openage-data-models
            )

    # Todo: Change links to official repository
    set(PROJECT_LIST_LINKS
            "https://github.com/simonsan/nyan.git"
            "https://github.com/simonsan/openage-modding.git"
            "https://github.com/simonsan/openage-data-3dmodels.git"
            )

    # loop for projects to clone and generate docs from
    set(VAR "0")
    foreach(DL_PROJECT_NAME ${PROJECT_LIST_NAMES})
        list(GET PROJECT_LIST_LINKS "${VAR}" DL-PROJECT_GIT)
        MATH(EXPR VAR "${VAR}+1")

        # clone Repos
        fetch_project(
                    NAME ${DL_PROJECT_NAME}
                    # DISABLE_UPDATES
                    GIT_REPOSITORY ${DL-PROJECT_GIT}
                    GIT_TAG origin/master
        )

    endforeach()

    # Path to sources folder => index.rst
    message(STATUS "${nyan_SOURCE_DIR}")
    message(STATUS "${openage-modding_SOURCE_DIR}")
    message(STATUS "${openage-data-models_SOURCE_DIR}")

    # Todo: Remember
    # Set new variable
    # set(${PROJ_NAME}_SOURCE_DIR "${PROJ_SRC_DIR}" PARENT_SCOPE)

    # Todo: Check if possible to use just one function for doxygen-configure
    # configure doxygen for webdoc-build
    doxygen_webdoc_configure(libopenage/ openage/ README.md)

    # Todo: Configure Sphinx explicitly to get just document these folders \
    #       besides the ones we already use for normal documentation

    # configure sphinx for webdoc-build
    sphinx_configure("${nyan_SOURCE_DIR}" "${openage-modding_SOURCE_DIR}" "${openage-data-models_SOURCE_DIR}")

    # Todo: What about documentation of cmake? Any specialities?

endif()

# endbranch 'webdoc build'
