import dataformat
from struct import Struct, unpack_from
import util
from util import dbg, zstr


class UnitCommand(dataformat.Exportable):
    name_struct        = "unit_command"
    name_struct_file   = "unit"
    struct_description = "a command a single unit may recieve by script or human."

    data_format = (
        (dataformat.READ, "one", "int16_t"),
        (dataformat.READ_EXPORT, "uid", "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_EXPORT, "type", "int16_t"),
        (dataformat.READ_EXPORT, "class_id", "int16_t"),
        (dataformat.READ_EXPORT, "unit_id", "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ_EXPORT, "ressource_in", "int16_t"),
        (dataformat.READ_EXPORT, "sub_type", "int16_t"),
        (dataformat.READ_EXPORT, "ressource_out", "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ_EXPORT, "work_rate_multiplier", "float"),
        (dataformat.READ_EXPORT, "execution_radius", "float"),
        (dataformat.READ_EXPORT, "extra_range", "float"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "float"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "graphic", "int16_t[6]"),
    )


class UnitHeader(dataformat.Exportable):
    name_struct        = "unit_header"
    name_struct_file   = "unit"
    struct_description = "stores a bunch of unit commands."

    data_format = (
        (dataformat.READ, "exists", dataformat.ContinueReadMember("uint8_t")),
        (dataformat.READ, "unit_command_count", "uint16_t"),
        (dataformat.READ_EXPORT, "unit_commands", dataformat.SubdataMember(
            ref_type=UnitCommand,
            length="unit_command_count",
        )),
    )


class RessourceStorage(dataformat.Exportable):
    name_struct        = "ressource_storage"
    name_struct_file   = "unit"
    struct_description = "stores the resource storage capacity for one unit mode."

    data_format = (
        (dataformat.READ, "a", "int16_t"),
        (dataformat.READ, "b", "float"),
        (dataformat.READ, "c", "int8_t"),
    )


class DamageGraphic(dataformat.Exportable):
    name_struct        = "damage_graphic"
    name_struct_file   = "unit"
    struct_description = "stores one building image that shows a destroyed foundation."

    data_format = (
        (dataformat.READ_EXPORT, "graphic_id", "int16_t"),
        (dataformat.READ_EXPORT, "damage_percent", "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
    )


class HitType(dataformat.Exportable):
    name_struct        = "hit_type"
    name_struct_file   = "unit"
    struct_description = "stores attack amount for a damage type."

    data_format = (
        (dataformat.READ, "type_id", "int16_t"), #used for class id
        (dataformat.READ, "amount", "int16_t"),
    )


class RessourceCost(dataformat.Exportable):
    name_struct        = "ressource_cost"
    name_struct_file   = "unit"
    struct_description = "stores cost for one ressource for creating the unit."

    data_format = (
        (dataformat.READ, "type_id", "int16_t"),
        (dataformat.READ, "amount", "int16_t"),
        (dataformat.READ, "enabled", "int16_t"),
    )


class BuildingAnnex(dataformat.Exportable):

    name_struct        = "building_annex"
    name_struct_file   = "unit"
    struct_description = "a possible building annex."

    data_format = (
        (dataformat.READ_EXPORT, "unit_id",    "int16_t"),
        (dataformat.READ_EXPORT, "misplaced0", "float"),
        (dataformat.READ_EXPORT, "misplaced1", "float"),
    )

    def __init__(self, **args):
        super().__init__(**args)


class UnitObject(dataformat.Exportable):
    """
    base properties for every unit entry.
    """

    name_struct        = "unit_object"
    name_struct_file   = "unit"
    struct_description = "base properties for all units."

    data_format = (
        (dataformat.READ, "name_length", "uint16_t"),
        (dataformat.READ_EXPORT, "id0", "int16_t"),
        (dataformat.READ_EXPORT, "language_dll_name", "uint16_t"),
        (dataformat.READ_EXPORT, "language_dll_creation", "uint16_t"),
        (dataformat.READ, "unit_class", "int16_t"),
        (dataformat.READ_EXPORT, "graphic_standing0", "int16_t"),
        (dataformat.READ_EXPORT, "graphic_standing1", "int16_t"),
        (dataformat.READ_EXPORT, "graphic_dying0", "int16_t"),
        (dataformat.READ_EXPORT, "graphic_dying1", "int16_t"),
        (dataformat.READ, "death_mode", "int8_t"),
        (dataformat.READ, "hit_points", "int16_t"),
        (dataformat.READ, "line_of_sight", "float"),
        (dataformat.READ, "garnison_capacity", "int8_t"),
        (dataformat.READ, "radius_size0", "float"),
        (dataformat.READ, "radius_size1", "float"),
        (dataformat.READ, "hp_bar_height0", "float"),
        (dataformat.READ_EXPORT, "sound_train0", "int16_t"),
        (dataformat.READ_EXPORT, "sound_train1", "int16_t"),
        (dataformat.READ, "dead_unit_id", "int16_t"),
        (dataformat.READ, "placement_mode", "int8_t"),
        (dataformat.READ, "air_mode", "int8_t"),
        (dataformat.READ, "icon_id", "int16_t"),
        (dataformat.READ, "hidden_in_editor", "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ, "enabled", "int16_t"),
        (dataformat.READ, "placement_by_pass_terrain0", "int16_t"),
        (dataformat.READ, "placement_by_pass_terrain1", "int16_t"),
        (dataformat.READ, "placement_terrain0", "int16_t"),
        (dataformat.READ, "placement_terrain1", "int16_t"),
        (dataformat.READ, "editor_radius0", "float"),
        (dataformat.READ, "editor_radius1", "float"),
        (dataformat.READ, "building_mode", "int8_t"),
        (dataformat.READ, "visible_in_fog", "int8_t"),
        (dataformat.READ, "terrain_restriction", "int16_t"),
        (dataformat.READ, "fly_mode", "int8_t"),
        (dataformat.READ, "ressource_capacity", "int16_t"),
        (dataformat.READ, "ressource_decay", "float"),
        (dataformat.READ, "blast_type", "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "interaction_mode", "int8_t"),
        (dataformat.READ, "minimap_mode", "int8_t"),
        (dataformat.READ, "command_attribute", "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ_EXPORT, "language_dll_help", "uint16_t"),
        (dataformat.READ, "hot_keys", "int16_t[4]"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "unselectable", "uint8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "selection_mask", "int8_t"),
        (dataformat.READ, "selection_shape_type", "int8_t"),
        (dataformat.READ, "selection_shape", "int8_t"),
        (dataformat.READ, "attribute", "int8_t"),
        (dataformat.READ, "civilisation", "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ, "selection_effect", "int8_t"),
        (dataformat.READ, "editor_selection_color", "uint8_t"),
        (dataformat.READ, "selection_radius0", "float"),
        (dataformat.READ, "selection_radius1", "float"),
        (dataformat.READ, "hp_bar_height1", "float"),
        (dataformat.READ_EXPORT, "ressource_storage", dataformat.SubdataMember(
            ref_type=RessourceStorage,
            length=3,
        )),
        (dataformat.READ, "damage_graphic_count", "int8_t"),
        (dataformat.READ_EXPORT, "damage_graphic", dataformat.SubdataMember(
            ref_type=DamageGraphic,
            length="damage_graphic_count",
        )),
        (dataformat.READ_EXPORT, "sound_selection", "int16_t"),
        (dataformat.READ_EXPORT, "sound_dying", "int16_t"),
        (dataformat.READ, "attack_mode", "int16_t"),
        (dataformat.READ_EXPORT, "name", "char[name_length]"),
        (dataformat.READ_EXPORT, "id1", "int16_t"),
        (dataformat.READ_EXPORT, "id2", "int16_t"),
    )

    def __init__(self, **args):
        super().__init__(**args)


class UnitFlag(UnitObject):
    """
    type_id >= 20
    """

    name_struct        = "unit_flag"
    name_struct_file   = "unit"
    struct_description = "adds speed property to units."

    data_format = (
        (dataformat.READ_EXPORT, None, dataformat.IncludeMembers(cls=UnitObject)),
        (dataformat.READ_EXPORT, "speed", "float"),
    )

    def __init__(self, **args):
        super().__init__(**args)


class UnitDoppelganger(UnitFlag):
    """
    type_id >= 25
    """

    name_struct        = "unit_doppelganger"
    name_struct_file   = "unit"
    struct_description = "weird doppelganger unit thats actually the same as a flag unit."

    data_format = (
        (dataformat.READ_EXPORT, None, dataformat.IncludeMembers(cls=UnitFlag)),
    )

    def __init__(self):
        super().__init__()


class UnitDeadOrFish(UnitDoppelganger):
    """
    type_id >= 30
    """

    name_struct        = "unit_dead_or_fish"
    name_struct_file   = "unit"
    struct_description = "adds walking graphics, rotations and tracking properties to units."

    data_format = (
        (dataformat.READ_EXPORT, None, dataformat.IncludeMembers(cls=UnitDoppelganger)),
        (dataformat.READ_EXPORT, "walking_graphics0", "int16_t"),
        (dataformat.READ_EXPORT, "walking_graphics1", "int16_t"),
        (dataformat.READ, "rotation_speed", "float"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "tracking_unit", "int16_t"),
        (dataformat.READ, "tracking_unit_used", "uint8_t"),
        (dataformat.READ, "tracking_unit_density", "float"),
        (dataformat.READ_UNKNOWN, None, "float"),
        (dataformat.READ_UNKNOWN, None, "int8_t[17]"),
    )

    def __init__(self, **args):
        super().__init__(**args)


class UnitBird(UnitDeadOrFish):
    """
    type_id >= 40
    """

    name_struct        = "unit_bird"
    name_struct_file   = "unit"
    struct_description = "adds search radius and work properties, as well as movement sounds."

    data_format = (
        (dataformat.READ_EXPORT, None, dataformat.IncludeMembers(cls=UnitDeadOrFish)),
        (dataformat.READ, "sheep_conversion", "int16_t"),
        (dataformat.READ, "search_radius", "float"),
        (dataformat.READ, "work_rate", "float"),
        (dataformat.READ, "drop_site0", "int16_t"),
        (dataformat.READ, "drop_site1", "int16_t"),
        (dataformat.READ, "villager_mode", "int8_t"),
        (dataformat.READ, "move_sound", "int16_t"),
        (dataformat.READ, "stop_sound", "int16_t"),
        (dataformat.READ, "animal_mode", "int8_t"),
    )

    def __init__(self, **args):
        super().__init__(**args)


class UnitMovable(UnitBird):
    """
    type_id >= 60
    """

    name_struct        = "unit_movable"
    name_struct_file   = "unit"
    struct_description = "adds attack and armor properties to units."

    data_format = (
        (dataformat.READ_EXPORT, None, dataformat.IncludeMembers(cls=UnitBird)),
        (dataformat.READ, "default_armor", "int16_t"),
        (dataformat.READ, "attack_count", "uint16_t"),
        (dataformat.READ, "attacks", dataformat.SubdataMember(ref_type=HitType, length="attack_count")),
        (dataformat.READ, "armor_count", "uint16_t"),
        (dataformat.READ, "armors", dataformat.SubdataMember(ref_type=HitType, length="armor_count")),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ, "max_range", "float"),
        (dataformat.READ, "blast_radius", "float"),
        (dataformat.READ, "reload_time0", "float"),
        (dataformat.READ, "projectile_unit_id", "int16_t"),
        (dataformat.READ, "accuracy_percent", "int16_t"),
        (dataformat.READ, "tower_mode", "int8_t"),
        (dataformat.READ, "delay", "int16_t"),
        (dataformat.READ, "graphics_displacement", "float[3]"),
        (dataformat.READ, "blast_level", "int8_t"),
        (dataformat.READ, "min_range", "float"),
        (dataformat.READ, "garnison_recovery_rate", "float"),
        (dataformat.READ_EXPORT, "attack_graphic", "int16_t"),
        (dataformat.READ, "melee_armor_displayed", "int16_t"),
        (dataformat.READ, "attack_displayed", "int16_t"),
        (dataformat.READ, "range_displayed", "int16_t"),
        (dataformat.READ, "reload_time1", "float"),
    )

    def __init__(self):
        super().__init__()


class UnitProjectile(UnitMovable):
    """
    type_id == 60
    """

    name_struct        = "unit_projectile"
    name_struct_file   = "unit"
    struct_description = "adds projectile specific unit properties."

    data_format = (
        (dataformat.READ_EXPORT, None, dataformat.IncludeMembers(cls=UnitMovable)),
        (dataformat.READ, "stretch_mode", "int8_t"),
        (dataformat.READ, "compensation_mode", "int8_t"),
        (dataformat.READ, "drop_animation_mode", "int8_t"),
        (dataformat.READ, "penetration_mode", "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "uint16_t"),
        (dataformat.READ, "projectile_arc", "float"),
    )

    def __init__(self):
        super().__init__()


class UnitLiving(UnitMovable):
    """
    type_id >= 70
    """

    name_struct        = "unit_living"
    name_struct_file   = "unit"
    struct_description = "adds creation location and garnison unit properties."

    data_format = (
        (dataformat.READ_EXPORT, None, dataformat.IncludeMembers(cls=UnitMovable)),
        (dataformat.READ, "ressource_cost", dataformat.SubdataMember(ref_type=RessourceCost, length=3)),
        (dataformat.READ, "train_time", "int16_t"),
        (dataformat.READ, "train_location_id", "int16_t"),
        (dataformat.READ, "button_id", "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ, "missile_graphic_delay", "int8_t"),
        (dataformat.READ, "hero_mode", "int8_t"),
        (dataformat.READ, "garnison_graphic0", "int16_t"),
        (dataformat.READ, "garnison_graphic1", "int16_t"),
        (dataformat.READ_UNKNOWN, None, "uint16_t"),               #TODO: verify, probably one byte off...
        (dataformat.READ, "attack_missile_duplication0", "float"),
        (dataformat.READ, "attack_missile_duplication1", "int8_t"),
        (dataformat.READ, "attack_missile_duplication_spawning", "float[3]"),
        (dataformat.READ, "attack_missile_duplication_unit", "int32_t"),
        (dataformat.READ, "attack_missile_duplication_graphic", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "pierce_armor_displayed", "int16_t"),
    )

    def __init__(self):
        super().__init__()


class UnitBuilding(UnitLiving):
    """
    type_id >= 80
    """

    name_struct        = "unit_building"
    name_struct_file   = "unit"
    struct_description = "construction graphics and garnison building properties for units."

    data_format = (
        (dataformat.READ_EXPORT, None, dataformat.IncludeMembers(cls=UnitLiving)),
        (dataformat.READ, "construction_graphic_id", "int16_t"),
        (dataformat.READ, "snow_graphic_id", "int16_t"),
        (dataformat.READ, "adjacent_mode", "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "stack_unit_id", "int16_t"),
        (dataformat.READ, "terrain_id", "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ, "research_id", "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "building_annex", dataformat.SubdataMember(ref_type=BuildingAnnex, length=4)),
        (dataformat.READ, "head_unit", "int16_t"),
        (dataformat.READ, "transform_unit", "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ, "construction_sound_id", "int16_t"),
        (dataformat.READ, "garnison_type", "int8_t"),
        (dataformat.READ, "garnison_heal_rate", "float"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t[6]"),
    )

    def __init__(self):
        super().__init__()


class UnitTree(UnitObject):
    """
    type_id = 90
    """

    name_struct        = "unit_tree"
    name_struct_file   = "unit"
    struct_description = "just a tree unit."

    data_format = (
        (dataformat.READ_EXPORT, None, dataformat.IncludeMembers(cls=UnitObject)),
    )

    def __init__(self, **args):
        super().__init__(**args)


unit_type_lookup = {
    10: "object",
    20: "flag",
    25: "doppelganger",
    30: "dead_or_fish",
    40: "bird",
    60: "projectile",
    70: "living",
    80: "building",
    90: "tree",
}

unit_type_class_lookup = {
    "object":       UnitObject,
    "flag":         UnitFlag,
    "doppelganger": UnitDoppelganger,
    "dead_or_fish": UnitDeadOrFish,
    "bird":         UnitBird,
    "projectile":   UnitProjectile,
    "living":       UnitLiving,
    "building":     UnitBuilding,
    "tree":         UnitTree,
}
