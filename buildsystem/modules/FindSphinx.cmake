# This file was taken from cmb,
# Copyright 2012-2012 Kitware Inc. 28 Corporate Drive
# It's licensed under the terms of Computational Model Builder (CMB) Version 1.0 license.
# Modifications Copyright 2019-2019 the openage authors.
# See copying.md for further legal info.

# - This module looks for Sphinx
# Find the Sphinx documentation generator
#
# This modules defines
# SPHINX_EXECUTABLE
# SPHINX_FOUND
set(_python_paths )
#find_package(PythonInterp)
if(PYTHON_EXECUTABLE)
    get_filename_component(_python_dir "${PYTHON_EXECUTABLE}" DIRECTORY)
    list(APPEND _python_paths
            "${_python_dir}"
            "${_python_dir}/bin"
            "${_python_dir}/Scripts"
            "%APPDATA%\\Python"
            )
endif()


find_program(SPHINX_EXECUTABLE
        NAMES sphinx-build
        HINTS
        ${_python_paths}
        PATHS
        ~/.local/bin
        /bin
        /usr/bin
        /usr/local/bin
        /opt/local/bin
        %APPDATA%\\Python
        DOC "Sphinx documentation generator"
        )

if( NOT SPHINX_EXECUTABLE )
    set(_Python_VERSIONS
            3.7.4 3.7.3 3.6.9 3 2.7
            )
    foreach( _version ${_Python_VERSIONS} )
        set( _sphinx_NAMES sphinx-build-${_version} )
        find_program( SPHINX_EXECUTABLE
                NAMES ${_sphinx_NAMES}
                HINTS
                ${_python_paths}
                PATHS
                ~/.local/bin
                /bin
                /usr/bin
                /usr/local/bin
                /opt/local/bin
                %APPDATA%\\Python
                DOC "Sphinx documentation generator"
                )
    endforeach()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sphinx DEFAULT_MSG SPHINX_EXECUTABLE)
mark_as_advanced(SPHINX_EXECUTABLE)
