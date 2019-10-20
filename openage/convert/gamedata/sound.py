# Copyright 2013-2019 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from openage.convert.dataformat.genie_structure import GenieStructure
from openage.convert.dataformat.read_members import SubdataMember
from ..dataformat.member_access import READ_EXPORT, READ
from ..dataformat.value_members import MemberTypes as StorageType


class SoundItem(GenieStructure):
    name_struct        = "sound_item"
    name_struct_file   = "sound"
    struct_description = "one possible file for a sound."

    data_format = []

    # TODO: Enable conversion for SWGB; replace "filename"
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.append((READ_EXPORT, "filename", "char[27]"))
    # else:
    #     data_format.append((READ_EXPORT, "filename", "char[13]"))
    # ===========================================================================
    data_format.append((READ_EXPORT, "filename", StorageType.STRING_MEMBER, "char[13]"))

    data_format.extend([
        (READ_EXPORT, "resource_id", StorageType.ID_MEMBER, "int32_t"),
        (READ_EXPORT, "probablilty", StorageType.INT_MEMBER, "int16_t"),
    ])

    # TODO: Enable conversion for AOE1; replace "civilisation", "icon_set"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.extend([
    #         (READ_EXPORT, "civilisation", "int16_t"),
    #         (READ,        "icon_set",     "int16_t"),
    #     ])
    # ===========================================================================
    data_format.extend([
        (READ_EXPORT, "civilisation_id", StorageType.ID_MEMBER, "int16_t"),
        (READ, "icon_set", StorageType.ID_MEMBER, "int16_t"),
    ])


class Sound(GenieStructure):
    name_struct        = "sound"
    name_struct_file   = "sound"
    struct_description = "describes a sound, consisting of several sound items."

    data_format = [
        (READ_EXPORT, "sound_id", StorageType.ID_MEMBER, "int16_t"),
        (READ, "play_delay", StorageType.INT_MEMBER, "int16_t"),
        (READ_EXPORT, "file_count", StorageType.INT_MEMBER, "uint16_t"),
        (READ, "cache_time", StorageType.INT_MEMBER, "int32_t"),                   # always 300000
        (READ_EXPORT, "sound_items", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=SoundItem,
            ref_to="id",
            length="file_count",
        )),
    ]
