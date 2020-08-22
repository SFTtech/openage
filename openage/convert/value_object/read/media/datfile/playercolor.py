# Copyright 2013-2020 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from .....entity_object.conversion.genie_structure import GenieStructure
from ....init.game_version import GameEdition
from ....read.member_access import READ_GEN
from ....read.value_members import MemberTypes as StorageType


class PlayerColor(GenieStructure):
    name_struct        = "player_color"
    name_struct_file   = name_struct
    struct_description = "describes player color settings."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """

        if game_version[0] not in (GameEdition.ROR, GameEdition.AOE1DE):
            data_format = [
                (READ_GEN, "id", StorageType.ID_MEMBER, "int32_t"),
                # palette index offset, where the 8 player colors start
                (READ_GEN, "player_color_base", StorageType.ID_MEMBER, "int32_t"),
                # palette index
                (READ_GEN, "outline_color", StorageType.ID_MEMBER, "int32_t"),
                (READ_GEN, "unit_selection_color1", StorageType.ID_MEMBER, "int32_t"),
                (READ_GEN, "unit_selection_color2", StorageType.ID_MEMBER, "int32_t"),
                # palette index
                (READ_GEN, "minimap_color1", StorageType.ID_MEMBER, "int32_t"),
                (READ_GEN, "minimap_color2", StorageType.ID_MEMBER, "int32_t"),
                (READ_GEN, "minimap_color3", StorageType.ID_MEMBER, "int32_t"),
                (READ_GEN, "statistics_text_color", StorageType.ID_MEMBER, "int32_t"),
            ]
        else:
            data_format = [
                (READ_GEN, "name", StorageType.STRING_MEMBER, "char[30]"),
                (READ_GEN, "id", StorageType.ID_MEMBER, "int16_t"),
                (READ_GEN, "resource_id", StorageType.ID_MEMBER, "int16_t"),
                (READ_GEN, "minimap_color", StorageType.ID_MEMBER, "uint8_t"),
                # 0 transform
                # 1 transform player color
                # 2 shadow
                # 3 translucent
                (READ_GEN, "type", StorageType.ID_MEMBER, "uint8_t"),
            ]

        return data_format
