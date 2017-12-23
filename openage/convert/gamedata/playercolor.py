# Copyright 2013-2017 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.exportable import Exportable
from ..dataformat.member_access import READ, READ_EXPORT


class PlayerColor(Exportable):
    name_struct        = "player_color"
    name_struct_file   = name_struct
    struct_description = "describes player color settings."

    data_format = []

    # TODO: Enable conversion for AOE1; replace 9 values below
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.append((READ_EXPORT, "id", "int32_t"))
    #     data_format.append((READ_EXPORT, "player_color_base", "int32_t"))    # palette index offset, where the 8 player colors start
    #     data_format.append((READ_EXPORT, "outline_color", "int32_t"))        # palette index
    #     data_format.append((READ, "unit_selection_color1", "int32_t"))
    #     data_format.append((READ, "unit_selection_color2", "int32_t"))
    #     data_format.append((READ_EXPORT, "minimap_color1", "int32_t"))       # palette index
    #     data_format.append((READ, "minimap_color2", "int32_t"))
    #     data_format.append((READ, "minimap_color3", "int32_t"))
    #     data_format.append((READ_EXPORT, "statistics_text_color", "int32_t"))
    # else:
    #     data_format.append((READ, "name", "char[30]"))
    #     data_format.append((READ, "id_short", "int16_t"))
    #     data_format.append((READ, "color", "uint8_t"))
    #     data_format.append((READ, "type", "uint8_t"))                        # 0 transform, 1 transform player color, 2 shadow, 3 translucent
    # ===========================================================================
    data_format.append((READ_EXPORT, "id", "int32_t"))
    data_format.append((READ_EXPORT, "player_color_base", "int32_t"))       # palette index offset, where the 8 player colors start
    data_format.append((READ_EXPORT, "outline_color", "int32_t"))           # palette index
    data_format.append((READ, "unit_selection_color1", "int32_t"))
    data_format.append((READ, "unit_selection_color2", "int32_t"))
    data_format.append((READ_EXPORT, "minimap_color1", "int32_t"))          # palette index
    data_format.append((READ, "minimap_color2", "int32_t"))
    data_format.append((READ, "minimap_color3", "int32_t"))
    data_format.append((READ_EXPORT, "statistics_text_color", "int32_t"))
