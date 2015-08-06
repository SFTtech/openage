# Copyright 2015-2015 the openage authors. See copying.md for legal info.

# This module defines:
#
#  CYTHON_FOUND
#  CYTHON_VERSION          - e.g. '0.23dev'
#  CYTHON                  - invocation (using python)
#
# It depends on:
#
#  PYTHON

set(CYTHON "${PYTHON} -m cython")
py_exec("import cython; print(cython.__version__)" CYTHON_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cython VERSION_VAR CYTHON_VERSION REQUIRED_VARS CYTHON)
