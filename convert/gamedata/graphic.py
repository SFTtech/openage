import dataformat
from struct import Struct, unpack_from
from util import dbg, zstr

from .empiresdat import endianness

class GraphicDelta(dataformat.Exportable):
    name_struct        = "graphic_delta"
    name_struct_file   = "graphic"
    struct_description = "delta definitions for ingame graphics files."

    data_format = (
        (dataformat.READ, "graphic_id",  "int16_t"),
        (dataformat.READ_UNKNOWN, None,  "int16_t"),
        (dataformat.READ_UNKNOWN, None,  "int16_t"),
        (dataformat.READ_UNKNOWN, None,  "int16_t"),
        (dataformat.READ, "direction_x", "int16_t"),
        (dataformat.READ, "direction_y", "int16_t"),
        (dataformat.READ_UNKNOWN, None,  "int16_t"),
        (dataformat.READ_UNKNOWN, None,  "int16_t"),
    )


class SoundProp(dataformat.Exportable):
    name_struct        = "sound_prop"
    name_struct_file   = "graphic"
    struct_description = "sound id and delay definition for graphics sounds."

    data_format = (
        (dataformat.READ, "sound_delay", "int16_t"),
        (dataformat.READ, "sound_id",    "int16_t"),
    )


class GraphicAttackSound(dataformat.Exportable):
    name_struct        = "graphic_attack_sound"
    name_struct_file   = "graphic"
    struct_description = "attack sounds for a given graphics file."

    data_format = (
        (dataformat.READ, "sound_props", dataformat.SubdataMember(
            ref_type=SoundProp,
            length=3,
        )),
    )


class Graphic(dataformat.Exportable):
    name_struct        = "graphic"
    name_struct_file   = name_struct
    struct_description = "metadata for ingame graphics files."

    data_format = (
        (dataformat.READ, "name0", "char[21]"),
        (dataformat.READ, "name1", "char[13]"),
        (dataformat.READ, "slp_id", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "layer", "int8_t"),
        (dataformat.READ, "player_color", "int16_t"),
        (dataformat.READ, "replay", "bool"),
        (dataformat.READ, "coordinates", "int16_t[4]"),
        (dataformat.READ, "delta_count", "uint16_t"),
        (dataformat.READ, "sound_id", "int16_t"),
        (dataformat.READ, "attack_sound_used", "bool"),
        (dataformat.READ, "frame_count", "uint16_t"),
        (dataformat.READ, "angle_count", "uint16_t"),
        (dataformat.READ, "new_speed", "float"),
        (dataformat.READ, "frame_rate", "float"),
        (dataformat.READ, "replay_delay", "float"),
        (dataformat.READ, "sequence_type", "int8_t"),
        (dataformat.READ, "id", "int16_t"),
        (dataformat.READ, "mirroring_mode", "int16_t"),
        (dataformat.READ, "graphic_deltas", dataformat.SubdataMember(
            ref_type=GraphicDelta,
            length="delta_count",
        )),

        #if attack_sound_used:
        (dataformat.READ, "graphic_attack_sounds", dataformat.SubdataMember(
            ref_type=GraphicAttackSound,
            length=lambda o: "angle_count" if o.attack_sound_used else 0,
        )),
    )
