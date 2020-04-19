# Copyright 2013-2019 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.genie_structure import GenieStructure
from ..dataformat.member_access import READ, READ_EXPORT
from ..dataformat.value_members import MemberTypes as StorageType
from openage.convert.dataformat.version_detect import GameEdition


class PlayerColor(GenieStructure):
    name_struct        = "player_color"
    name_struct_file   = name_struct
    struct_description = "describes player color settings."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """

        if game_version[0] is not GameEdition.ROR:
            data_format = [
                (READ_EXPORT, "id", StorageType.ID_MEMBER, "int32_t"),
                # palette index offset, where the 8 player colors start
                (READ_EXPORT, "player_color_base", StorageType.ID_MEMBER, "int32_t"),
                # palette index
                (READ_EXPORT, "outline_color", StorageType.ID_MEMBER, "int32_t"),
                (READ, "unit_selection_color1", StorageType.ID_MEMBER, "int32_t"),
                (READ, "unit_selection_color2", StorageType.ID_MEMBER, "int32_t"),
                # palette index
                (READ_EXPORT, "minimap_color1", StorageType.ID_MEMBER, "int32_t"),
                (READ, "minimap_color2", StorageType.ID_MEMBER, "int32_t"),
                (READ, "minimap_color3", StorageType.ID_MEMBER, "int32_t"),
                (READ_EXPORT, "statistics_text_color", StorageType.ID_MEMBER, "int32_t"),
            ]
        else:
            data_format = [
                (READ, "name", StorageType.STRING_MEMBER, "char[30]"),
                (READ, "id_short", StorageType.ID_MEMBER, "int16_t"),
                (READ, "resource_id", StorageType.ID_MEMBER, "int16_t"),
                (READ, "color", StorageType.ID_MEMBER, "uint8_t"),
                # 0 transform
                # 1 transform player color
                # 2 shadow
                # 3 translucent
                (READ, "type", StorageType.ID_MEMBER, "uint8_t"),
            ]

        return data_format
