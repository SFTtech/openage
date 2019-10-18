# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from openage.convert.dataformat.genie_structure import GenieStructure
from .member_access import NOREAD_EXPORT


class MultisubtypeBaseFile(GenieStructure):
    """
    class that describes the format
    for the base-file pointing to the per-subtype files.
    """

    name_struct_file   = "util"
    name_struct        = "multisubtype_ref"
    struct_description = "format for multi-subtype references"

    data_format = (
        (NOREAD_EXPORT, "subtype", None, "std::string"),
        (NOREAD_EXPORT, "filename", None, "std::string"),
    )
