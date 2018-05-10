# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Some types that are not available in Cython's shipped include files.
"""


# in some cases, pointers can't be properly used for template arguments.
ctypedef void *voidptr
