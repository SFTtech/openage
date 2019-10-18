# Copyright 2013-2017 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from openage.convert.dataformat.genie_structure import GenieStructure
from ..dataformat.member_access import READ, READ_EXPORT
from ..dataformat.value_members import MemberTypes as StorageType


class PlayerColor(GenieStructure):
    name_struct        = "player_color"
    name_struct_file   = name_struct
    struct_description = "describes player color settings."

    # TODO: Enable conversion for AOE1; replace 9 values below
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format = [
    #         (READ_EXPORT, "id", "int32_t"),
    #         (READ_EXPORT, "player_color_base", "int32_t"),       # palette index offset, where the 8 player colors start
    #         (READ_EXPORT, "outline_color", "int32_t"),           # palette index
    #         (READ, "unit_selection_color1", "int32_t"),
    #         (READ, "unit_selection_color2", "int32_t"),
    #         (READ_EXPORT, "minimap_color1", "int32_t"),          # palette index
    #         (READ, "minimap_color2", "int32_t"),
    #         (READ, "minimap_color3", "int32_t"),
    #         (READ_EXPORT, "statistics_text_color", "int32_t"),
    #     ]
    # else:
    #     data_format = [
    #         (READ, "name", "char[30]"),
    #         (READ, "id_short", "int16_t"),
    #         (READ, "color", "uint8_t"),
    #         (READ, "type", "uint8_t"),                        # 0 transform, 1 transform player color, 2 shadow, 3 translucent
    #     ]
    # ===========================================================================
    data_format = [
        (READ_EXPORT, "id", StorageType.ID_MEMBER, "int32_t"),
        (READ_EXPORT, "player_color_base", StorageType.ID_MEMBER, "int32_t"),       # palette index offset, where the 8 player colors start
        (READ_EXPORT, "outline_color", StorageType.ID_MEMBER, "int32_t"),           # palette index
        (READ, "unit_selection_color1", StorageType.ID_MEMBER, "int32_t"),
        (READ, "unit_selection_color2", StorageType.ID_MEMBER, "int32_t"),
        (READ_EXPORT, "minimap_color1", StorageType.ID_MEMBER, "int32_t"),          # palette index
        (READ, "minimap_color2", StorageType.ID_MEMBER, "int32_t"),
        (READ, "minimap_color3", StorageType.ID_MEMBER, "int32_t"),
        (READ_EXPORT, "statistics_text_color", StorageType.ID_MEMBER, "int32_t"),
    ]
