# Copyright 2013-2014 the openage authors. See copying.md for legal info.

from ..dataformat.exportable import Exportable
from ..dataformat.members import SubdataMember, EnumLookupMember
from ..dataformat.member_access import READ, READ_EXPORT, READ_UNKNOWN


class GraphicDelta(Exportable):
    name_struct        = "graphic_delta"
    name_struct_file   = "graphic"
    struct_description = "delta definitions for ingame graphics files."

    data_format = (
        (READ, "graphic_id",  "int16_t"),
        (READ_UNKNOWN, None,  "int16_t"),
        (READ_UNKNOWN, None,  "int16_t"),
        (READ_UNKNOWN, None,  "int16_t"),
        (READ, "direction_x", "int16_t"),
        (READ, "direction_y", "int16_t"),
        (READ_UNKNOWN, None,  "int16_t"),
        (READ_UNKNOWN, None,  "int16_t"),
    )


class SoundProp(Exportable):
    name_struct        = "sound_prop"
    name_struct_file   = "graphic"
    struct_description = "sound id and delay definition for graphics sounds."

    data_format = (
        (READ, "sound_delay", "int16_t"),
        (READ, "sound_id",    "int16_t"),
    )


class GraphicAttackSound(Exportable):
    name_struct        = "graphic_attack_sound"
    name_struct_file   = "graphic"
    struct_description = "attack sounds for a given graphics file."

    data_format = (
        (READ, "sound_props", SubdataMember(
            ref_type=SoundProp,
            length=3,
        )),
    )


class Graphic(Exportable):
    name_struct        = "graphic"
    name_struct_file   = name_struct
    struct_description = "metadata for ingame graphics files."

    data_format = (
        (READ_EXPORT, "name0", "char[21]"),
        (READ_EXPORT, "name1", "char[13]"),
        (READ_EXPORT, "slp_id", "int32_t"),
        (READ_UNKNOWN, None, "int8_t"),
        (READ_UNKNOWN, None, "int8_t"),                 # somehow correlated to the forced player color
        (READ_EXPORT, "layer", EnumLookupMember(
            raw_type    = "int8_t",
            type_name   = "graphics_layer",
            lookup_dict = {
                0: "TERRAIN",      # cliff
                5: "SHADOW",       # farm fields as well
                6: "RUBBLE",
                10: "UNIT_LOW",    # constructions, dead units, tree stumps, flowers, paths
                11: "FISH",
                19: "CRATER",      # rugs
                20: "UNIT",        # buildings, units, damage flames, animations (mill)
                21: "BLACKSMITH",  # blacksmith smoke
                22: "BIRD",        # hawk
                30: "PROJECTILE",  # and explosions
            }
        )),
        (READ_EXPORT, "player_color", "int8_t"),        # force given player color
        (READ_EXPORT, "adapt_color", "int8_t"),         # playercolor can be changed on sight (like sheep)
        (READ_EXPORT, "replay", "uint8_t"),             # loop animation
        (READ, "coordinates", "int16_t[4]"),
        (READ, "delta_count", "uint16_t"),
        (READ_EXPORT, "sound_id", "int16_t"),
        (READ_EXPORT, "attack_sound_used", "uint8_t"),
        (READ_EXPORT, "frame_count", "uint16_t"),
        (READ_EXPORT, "angle_count", "uint16_t"),
        (READ, "speed_adjust", "float"),                # multiplies the speed of the unit this graphic is applied to
        (READ_EXPORT, "frame_rate", "float"),           # playtime for one frame in seconds
        (READ_EXPORT, "replay_delay", "float"),         # seconds to wait before current_frame=0 again
        (READ_EXPORT, "sequence_type", "int8_t"),
        (READ_EXPORT, "id", "int16_t"),
        (READ_EXPORT, "mirroring_mode", "int8_t"),
        (READ, "graphic_deltas", SubdataMember(
            ref_type=GraphicDelta,
            length="delta_count",
        )),
        (READ_UNKNOWN, None, "int8_t"),                 # maybe something for the sprite editor of aoe:hd?

        # if attack_sound_used:
        (READ, "graphic_attack_sounds", SubdataMember(
            ref_type=GraphicAttackSound,
            length=lambda o: "angle_count" if o.attack_sound_used != 0 else 0,
        )),
    )
