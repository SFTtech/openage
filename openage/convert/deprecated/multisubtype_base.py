# Copyright 2014-2020 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R
# REFA: Whole file -> REMOVE

from ..dataformat.genie_structure import GenieStructure
from ..value_object.dataformat.member_access import NOREAD_EXPORT


class MultisubtypeBaseFile(GenieStructure):
    """
    class that describes the format
    for the base-file pointing to the per-subtype files.
    """

    name_struct_file   = "util"
    name_struct        = "multisubtype_ref"
    struct_description = "format for multi-subtype references"

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = (
            (NOREAD_EXPORT, "subtype", None, "std::string"),
            (NOREAD_EXPORT, "filename", None, "std::string"),
        )

        return data_format
