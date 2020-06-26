# Copyright 2013-2020 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.genie_structure import GenieStructure
from ..dataformat.member_access import READ_GEN, READ, SKIP
from ..dataformat.read_members import SubdataMember
from ..dataformat.value_members import MemberTypes as StorageType
from ..dataformat.version_detect import GameEdition


class SoundItem(GenieStructure):
    name_struct        = "sound_item"
    name_struct_file   = "sound"
    struct_description = "one possible file for a sound."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = []

        if game_version[0] in (GameEdition.AOE1DE, GameEdition.AOE2DE):
            data_format.extend([
                (SKIP, "name_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ, "name_len", StorageType.INT_MEMBER, "uint16_t"),
                (READ_GEN, "name", StorageType.STRING_MEMBER, "char[name_len]"),
            ])
        elif game_version[0] is GameEdition.SWGB:
            data_format.extend([
                (READ_GEN, "filename", StorageType.STRING_MEMBER, "char[27]"),
            ])
        else:
            data_format.extend([
                (READ_GEN, "filename", StorageType.STRING_MEMBER, "char[13]"),
            ])

        data_format.extend([
            (READ_GEN, "resource_id", StorageType.ID_MEMBER, "int32_t"),
            (READ_GEN, "probablilty", StorageType.INT_MEMBER, "int16_t"),
        ])

        if game_version[0] not in (GameEdition.ROR, GameEdition.AOE1DE):
            data_format.extend([
                (READ_GEN, "civilization_id", StorageType.ID_MEMBER, "int16_t"),
                (READ_GEN, "icon_set", StorageType.ID_MEMBER, "int16_t"),
            ])

        return data_format


class Sound(GenieStructure):
    name_struct        = "sound"
    name_struct_file   = "sound"
    struct_description = "describes a sound, consisting of several sound items."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "sound_id", StorageType.ID_MEMBER, "int16_t"),
            (READ_GEN, "play_delay", StorageType.INT_MEMBER, "int16_t"),
            (READ, "file_count", StorageType.INT_MEMBER, "uint16_t"),
            (SKIP, "cache_time", StorageType.INT_MEMBER, "int32_t"),                   # always 300000
        ]

        if game_version[0] in (GameEdition.AOE1DE, GameEdition.AOE2DE):
            data_format.extend([
                (READ_GEN, "total_probability", StorageType.ID_MEMBER, "int16_t"),
            ])

        data_format.extend([
            (READ_GEN, "sound_items", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=SoundItem,
                ref_to="id",
                length="file_count",
            )),
        ])

        return data_format
