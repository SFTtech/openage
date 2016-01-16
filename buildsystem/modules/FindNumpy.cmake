# Copyright 2016-2016 the openage authors. See copying.md for legal info.

# This module defines:
#
# NUMPY_INCLUDE_DIR
# NUMPY_FOUND
#
# Depends on: Python

py_exec("import numpy; print(numpy.get_include())" NUMPY_INCLUDE_DIR)


include("FindPackageHandleStandardArgs")
find_package_handle_standard_args(numpy REQUIRED_VARS NUMPY_INCLUDE_DIR)

mark_as_advanced(NUMPY_INCLUDE_DIR)
