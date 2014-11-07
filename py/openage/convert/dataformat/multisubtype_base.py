# Copyright 2014-2014 the openage authors. See copying.md for legal info.

from .exportable import Exportable
from .member_access import NOREAD_EXPORT

class MultisubtypeBaseFile(Exportable):
    """
    class that describes the format
    for the base-file pointing to the per-subtype files.
    """

    name_struct_file   = "util"
    name_struct        = "multisubtype_ref"
    struct_description = "format for multi-subtype references"

    data_format = (
        (NOREAD_EXPORT, "subtype", "std::string"),
        (NOREAD_EXPORT, "filename", "std::string"),
    )
