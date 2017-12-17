# Copyright 2013-2017 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.exportable import Exportable
from ..dataformat.members import SubdataMember, EnumLookupMember
from ..dataformat.member_access import READ, READ_EXPORT


class GraphicDelta(Exportable):
    name_struct        = "graphic_delta"
    name_struct_file   = "graphic"
    struct_description = "delta definitions for ingame graphics files."

    data_format = []
    data_format.append((READ_EXPORT, "graphic_id",  "int16_t"))
    data_format.append((READ, "padding_1",  "int16_t"))
    data_format.append((READ, "sprite_ptr",  "int32_t"))
    data_format.append((READ_EXPORT, "offset_x", "int16_t"))
    data_format.append((READ_EXPORT, "offset_y", "int16_t"))
    data_format.append((READ, "display_angle",  "int16_t"))
    data_format.append((READ, "padding_2",  "int16_t"))


class SoundProp(Exportable):
    name_struct        = "sound_prop"
    name_struct_file   = "graphic"
    struct_description = "sound id and delay definition for graphics sounds."

    data_format = []
    data_format.append((READ, "sound_delay", "int16_t"))
    data_format.append((READ, "sound_id",    "int16_t"))


class GraphicAttackSound(Exportable):
    name_struct        = "graphic_attack_sound"
    name_struct_file   = "graphic"
    struct_description = "attack sounds for a given graphics file."

    data_format = [
        (READ, "sound_props", SubdataMember(
            ref_type=SoundProp,
            length=3,
        )),
    ]


class Graphic(Exportable):
    name_struct        = "graphic"
    name_struct_file   = name_struct
    struct_description = "metadata for ingame graphics files."

    data_format = []
    data_format.append((READ_EXPORT, "name", "char[21]"))             # internal name: e.g. ARRG2NNE = archery range feudal Age north european
    data_format.append((READ_EXPORT, "filename", "char[13]"))
    data_format.append((READ_EXPORT, "slp_id", "int32_t"))             # id of the graphics file in the drs
    data_format.append((READ, "is_loaded", "int8_t"))                  # unused
    data_format.append((READ, "old_color_flag", "int8_t"))             # unused
    data_format.append((READ_EXPORT, "layer", EnumLookupMember(        # originally 40 layers, higher -> drawn on top
            raw_type    = "int8_t",                     # -> same layer -> order according to map position.
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
        )))
    data_format.append((READ_EXPORT, "player_color", "int8_t"))        # force given player color
    data_format.append((READ_EXPORT, "adapt_color", "int8_t"))         # playercolor can be changed on sight (like sheep)
    data_format.append((READ_EXPORT, "transparent_selection", "uint8_t"))             # loop animation
    data_format.append((READ, "coordinates", "int16_t[4]"))
    data_format.append((READ_EXPORT, "delta_count", "uint16_t"))
    data_format.append((READ_EXPORT, "sound_id", "int16_t"))
    data_format.append((READ_EXPORT, "attack_sound_used", "uint8_t"))
    data_format.append((READ_EXPORT, "frame_count", "uint16_t"))       # number of frames per angle
    data_format.append((READ_EXPORT, "angle_count", "uint16_t"))       # number of heading angles stored, some of the frames must be mirrored
    data_format.append((READ, "speed_adjust", "float"))                # multiplies the speed of the unit this graphic is applied to
    data_format.append((READ_EXPORT, "frame_rate", "float"))           # frame rate in seconds
    data_format.append((READ_EXPORT, "replay_delay", "float"))         # seconds to wait before current_frame=0 again
    data_format.append((READ_EXPORT, "sequence_type", "int8_t"))
    data_format.append((READ_EXPORT, "id", "int16_t"))
    data_format.append((READ_EXPORT, "mirroring_mode", "int8_t"))
    data_format.append((READ, "editor_flag", "int8_t"))        # sprite editor thing for AoK
    data_format.append((READ_EXPORT, "graphic_deltas", SubdataMember(
            ref_type=GraphicDelta,
            length="delta_count",
        )))

        # if attack_sound_used:
    data_format.append((READ, "graphic_attack_sounds", SubdataMember(
            ref_type=GraphicAttackSound,
            length=lambda o: "angle_count" if o.attack_sound_used != 0 else 0,
        )))
