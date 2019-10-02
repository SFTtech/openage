# Copyright 2019-2019 the openage authors. See copying.md for legal info.

# builds the documentation after running doxygen with sphinx

# check for sphinx + extensions
find_package(Sphinx REQUIRED)
        # recommonmark autodoc breathe graphviz sphinx-markdown-tables sphinx-rtd-theme)

function(sphinx_configure)
    if(${SPHINX_FOUND})

    # configure sphinx conf.py.in for webdoc build
    configure_file("${BUILDSYSTEM_DIR}/templates/webdoc/conf.py.in" "${WEBDOC_PATH}/conf.py" @ONLY)

    # arguments from the functions call
    #foreach(folder ${ARGN})
    #    set(SPHINX_INCLUDE_FOLDERS "${DOXYGEN_SCAN_FOLDERS} \"${CMAKE_CURRENT_SOURCE_DIR}/${folder}\"")
    #endforeach()

    endif()
endfunction()

# Notes
#if(NOT DEFINED SPHINX_THEME)
#    set(SPHINX_THEME default)
#endif()

#if(NOT DEFINED SPHINX_THEME_DIR)
#    set(SPHINX_THEME_DIR)
#endif()
