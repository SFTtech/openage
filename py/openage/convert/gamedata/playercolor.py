# Copyright 2013-2014 the openage authors. See copying.md for legal info.

from .. import dataformat
from struct import Struct, unpack_from
from ..util import dbg

from .empiresdat import endianness


class PlayerColor(dataformat.Exportable):
    name_struct        = "player_color"
    name_struct_file   = name_struct
    struct_description = "describes player color settings."

    data_format = (
        (dataformat.READ_EXPORT, "id", "int32_t"),
        (dataformat.READ_EXPORT, "palette", "int32_t"),       #palette index offset, where the 8 player colors start
        (dataformat.READ_EXPORT, "color", "int32_t"),         #palette index
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ_EXPORT, "minimap_color", "int32_t"), #palette index
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ_EXPORT, "statistics_text_color", "int32_t"),
    )
