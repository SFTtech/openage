# Copyright 2013-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.exportable import Exportable
from ..dataformat.member_access import READ_EXPORT, READ_UNKNOWN


class PlayerColor(Exportable):
    name_struct        = "player_color"
    name_struct_file   = name_struct
    struct_description = "describes player color settings."

    data_format = []
    data_format.append((READ_EXPORT, "id", "int32_t"))
    data_format.append((READ_EXPORT, "palette", "int32_t"))        # palette index offset, where the 8 player colors start
    data_format.append((READ_EXPORT, "color", "int32_t"))          # palette index
    data_format.append((READ_UNKNOWN, None, "int32_t"))
    data_format.append((READ_UNKNOWN, None, "int32_t"))
    data_format.append((READ_EXPORT, "minimap_color", "int32_t"))  # palette index
    data_format.append((READ_UNKNOWN, None, "int32_t"))
    data_format.append((READ_UNKNOWN, None, "int32_t"))
    data_format.append((READ_EXPORT, "statistics_text_color", "int32_t"))
