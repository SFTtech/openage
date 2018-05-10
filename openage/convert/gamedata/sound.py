# Copyright 2013-2017 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.exportable import Exportable
from ..dataformat.members import SubdataMember
from ..dataformat.member_access import READ_EXPORT, READ


class SoundItem(Exportable):
    name_struct        = "sound_item"
    name_struct_file   = "sound"
    struct_description = "one possible file for a sound."

    data_format = (
        (READ_EXPORT, "filename",     "char[13]"),
        (READ_EXPORT, "resource_id",  "int32_t"),
        (READ_EXPORT, "probablilty",  "int16_t"),
        (READ_EXPORT, "civilisation", "int16_t"),
        (READ,        "player_id",    "int16_t"),
    )


class Sound(Exportable):
    name_struct        = "sound"
    name_struct_file   = "sound"
    struct_description = "describes a sound, consisting of several sound items."

    data_format = (
        (READ_EXPORT, "id", "int16_t"),
        (READ, "play_at_update_count", "int16_t"),
        (READ_EXPORT, "item_count", "uint16_t"),
        (READ, "cache_time", "int32_t"),  # always 300000
        (READ_EXPORT, "sound_items", SubdataMember(
            ref_type=SoundItem,
            ref_to="id",
            length="item_count",
        )),
    )
