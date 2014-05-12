import dataformat
from struct import Struct, unpack_from
import util
from util import dbg, zstr


from .empiresdat import endianness


class UnitHeader:
    def read(self, raw, offset, cls=None):
        #bool exists;
        unit_header_header_struct0 = Struct(endianness + "?")

        pc = unit_header_header_struct0.unpack_from(raw, offset)
        offset += unit_header_header_struct0.size

        self.exists = pc[0]

        if self.exists == True:
            unit_header_header_struct1 = Struct(endianness + "H")

            pc = unit_header_header_struct1.unpack_from(raw, offset)
            offset += unit_header_header_struct1.size

            self.unit_command_count = pc[0]

            self.unit_command = list()
            for i in range(self.unit_command_count):
                t = UnitCommand()
                offset = t.read(raw, offset)
                self.unit_command.append(t)

        return offset


class UnitHeaderData:
    def read(self, raw, offset, cls=None):
        #uint32_t unit_count;
        header_struct = Struct(endianness + "I")

        pc = header_struct.unpack_from(raw, offset)
        offset += header_struct.size

        self.unit_count = pc[0]

        self.unit_header = list()
        for i in range(self.unit_count):
            t = UnitHeader()
            offset = t.read(raw, offset)
            self.unit_header.append(t)

        return offset


class UnitCommand:
    def read(self, raw, offset, cls=None):
        #int16_t one;
        #int16_t uid;
        #int8_t unknown;
        #int16_t type;
        #int16_t class_id;
        #int16_t unit_id;
        #int16_t unknown;
        #int16_t ressource_in;
        #int16_t sub_type;
        #int16_t ressource_out;
        #int16_t unknown;
        #float work_rate_multiplier;
        #float execution_radius;
        #float extra_range;
        #int8_t unknown;
        #float unknown;
        #int8_t unknown;
        #int8_t unknown;
        #int32_t unknown;
        #int8_t unknown;
        #int8_t unknown;
        #int8_t unknown;
        #int16_t graphic[6];
        unit_command_struct = Struct(endianness + "2h b 8h 3f b f 2b i 3b 6h")

        pc = unit_command_struct.unpack_from(raw, offset)
        offset += unit_command_struct.size

        self.one = pc[0]
        self.uid = pc[1]
        #self. = pc[2]
        self.class_id = pc[3]
        self.unit_id = pc[4]
        #self. = pc[5]
        self.ressource_in = pc[6]
        self.ressource_out = pc[7]
        #self. = pc[8]
        self.work_rate_multiplier = pc[9]
        self.execution_radius = pc[10]
        self.extra_range = pc[11]
        #self. = pc[12]
        #self. = pc[13]
        #self. = pc[14]
        #self. = pc[15]
        #self. = pc[16]
        #self. = pc[17]
        #self. = pc[18]
        #self. = pc[19]
        self.graphic = pc[20:(20+6)]

        return offset


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
    name_struct        = "ressource_storage"
    name_struct_file   = "unit"
    struct_description = "stores the resource storage capacity for one unit mode."

    data_format = (
        (dataformat.READ, "graphic_id", "int16_t"),
        (dataformat.READ, "damage_percent", "int8_t"),
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
        (dataformat.READ, "id0", "int16_t"),
        (dataformat.READ, "language_dll_name", "uint16_t"),
        (dataformat.READ, "language_dll_creation", "uint16_t"),
        (dataformat.READ, "unit_class", "int16_t"),
        (dataformat.READ, "graphic_standing0", "int16_t"),
        (dataformat.READ, "graphic_standing1", "int16_t"),
        (dataformat.READ, "graphic_dying0", "int16_t"),
        (dataformat.READ, "graphic_dying1", "int16_t"),
        (dataformat.READ, "death_mode", "int8_t"),
        (dataformat.READ, "hit_points", "int16_t"),
        (dataformat.READ, "line_of_sight", "float"),
        (dataformat.READ, "garnison_capacity", "int8_t"),
        (dataformat.READ, "radius_size0", "float"),
        (dataformat.READ, "radius_size1", "float"),
        (dataformat.READ, "hp_bar_height0", "float"),
        (dataformat.READ, "sound_train0", "int16_t"),
        (dataformat.READ, "sound_train1", "int16_t"),
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
        (dataformat.READ, "language_dll_help", "uint16_t"),
        (dataformat.READ, "hot_keys", "int16_t[4]"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "unselectable", "bool"),
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
        (dataformat.READ, "ressource_storage", dataformat.SubdataMember(
            ref_type=RessourceStorage,
            length=3,
        )),
        (dataformat.READ, "damage_graphic_count", "int8_t"),
        (dataformat.READ, "damage_graphic", dataformat.SubdataMember(
            ref_type=DamageGraphic,
            length="damage_graphic_count",
        )),
        (dataformat.READ, "sound_selection", "int16_t"),
        (dataformat.READ, "sound_dying", "int16_t"),
        (dataformat.READ, "attack_mode", "int16_t"),
        (dataformat.READ_EXPORT, "name", dataformat.CharArrayMember(
            length="name_length",
        )),
        (dataformat.READ, "id1", "int16_t"),
        (dataformat.READ, "id2", "int16_t"),
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
        (dataformat.READ, "speed", "float"),
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
        (dataformat.READ, "walking_graphics0", "int16_t"),
        (dataformat.READ, "walking_graphics1", "int16_t"),
        (dataformat.READ, "rotation_speed", "float"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "tracking_unit", "int16_t"),
        (dataformat.READ, "tracking_unit_used", "bool"),
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
        (dataformat.READ, "attack_graphic", "int16_t"),
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


class Unit(dataformat.Exportable):

    name_struct        = "unit"
    name_struct_file   = "unit"
    struct_description = "container for all possible unit types."

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

    data_format = (
        (dataformat.READ_EXPORT, "unit_type", dataformat.EnumMember(
            type_name="unit_types",
            values=unit_type_lookup.values(),
        )),
        (dataformat.READ_EXPORT, "type_data", dataformat.MultisubtypeMember(
            type_name="unit_type_data",
            class_lookup=unit_type_class_lookup,
            type_to="unit_type",
        )),
    )

    def __init__(self):
        #create dict with unit_type => []
        #created units will be placed in here.
        self.type_data = util.gen_dict_key2lists(self.unit_type_lookup.values())

    def read(self, raw, offset, cls=None):

        #int8_t type_id;
        unit_type_struct = Struct(endianness + "b")

        unit_type_id, = unit_type_struct.unpack_from(raw, offset)
        offset += unit_type_struct.size

        #depending on the type id the matching unit class will be selected
        self.unit_type = self.unit_type_lookup[unit_type_id]

        new_unit = self.unit_type_class_lookup[self.unit_type]()
        new_unit.unit_type = unit_type_id
        offset = new_unit.read(raw, offset)
        self.type_data[self.unit_type].append(new_unit)

        return offset
