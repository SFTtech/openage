# Copyright 2015-2021 the openage authors. See copying.md for legal info.

"""
Contains the listing of all code generator invocations.
"""


def generate_all(projectdir):
    """
    Generates all source files in targetdir.
    """
    from .cpp_testlist import generate_testlist
    generate_testlist(projectdir)

    from .coord import generate_coord_basetypes
    generate_coord_basetypes(projectdir)
