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
        (dataformat.READ, "unknown", "int8_t"),
        (dataformat.READ, "unknown", "int8_t"),
    )


class HitType:
    def read(self, raw, offset, cls=None):
        #int16_t used_for_class_id;
        #int16_t amount;
        hit_type_struct = Struct(endianness + "2h")

        pc = hit_type_struct.unpack_from(raw, offset)
        offset += hit_type_struct.size

        self.type_id = pc[0]
        self.amount  = pc[1]

        return offset


class RessourceCost:
    def read(self, raw, offset, cls=None):
        #int16_t type;
        #int16_t amount;
        #int16_t enabled;
        ressource_cost_struct = Struct(endianness + "3h")

        pc = ressource_cost_struct.unpack_from(raw, offset)
        offset += ressource_cost_struct.size

        self.type_id = pc[0]
        self.amount  = pc[1]
        self.enabled = pc[2]

        return offset


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
        (dataformat.READ, "unknown", "int16_t"),
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
        (dataformat.READ, "unknown", "int8_t"),
        (dataformat.READ, "interaction_mode", "int8_t"),
        (dataformat.READ, "minimap_mode", "int8_t"),
        (dataformat.READ, "command_attribute", "int16_t"),
        (dataformat.READ, "unknown", "int16_t"),
        (dataformat.READ, "unknown", "int16_t"),
        (dataformat.READ, "language_dll_help", "uint16_t"),
        (dataformat.READ, "hot_keys", "int16_t[4]"),
        (dataformat.READ, "unknown", "int8_t"),
        (dataformat.READ, "unknown", "int8_t"),
        (dataformat.READ, "unselectable", "bool"),
        (dataformat.READ, "unknown", "int8_t"),
        (dataformat.READ, "unknown", "int8_t"),
        (dataformat.READ, "unknown", "int8_t"),
        (dataformat.READ, "selection_mask", "int8_t"),
        (dataformat.READ, "selection_shape_type", "int8_t"),
        (dataformat.READ, "selection_shape", "int8_t"),
        (dataformat.READ, "attribute", "int8_t"),
        (dataformat.READ, "civilisation", "int8_t"),
        (dataformat.READ, "unknown", "int16_t"),
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
        (dataformat.READ, "name", dataformat.CharArrayMember(
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
        (dataformat.READ, None, dataformat.ParentMember(cls=UnitObject)),
        (dataformat.READ, "speed", "float"),
    )

    def __init__(self, **args):
        super().__init__(**args)

    def read(self, raw, offset, cls=None):
        offset = super().read(raw, offset, cls=UnitObject)

        #float speed;
        tmp_struct = Struct(endianness + "f")
        pc = tmp_struct.unpack_from(raw, offset)
        offset += tmp_struct.size

        self.speed, = pc

        return offset


class UnitDoppelganger(UnitFlag):
    """
    type_id >= 25
    """

    name_struct        = "unit_doppelganger"
    name_struct_file   = "unit"
    struct_description = "weird doppelganger unit thats actually the same as a flag unit."

    data_format = (
        (None, None, None),
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
        (None, None, None),
    )

    def __init__(self):
        super().__init__()

    def read(self, raw, offset, cls=None):
        offset = super().read(raw, offset)

        #int16_t walking_graphics0;
        #int16_t walking_graphics1;
        #float rotation_speed;
        #int8_t unknown;
        #int16_t tracking_unit;
        #bool tracking_unit_used;
        #float tracking_unit_density;
        #float unknown;
        #int8_t unknown[17];
        tmp_struct = Struct(endianness + "2h f b h ? 2f 17b")
        pc = tmp_struct.unpack_from(raw, offset)
        offset += tmp_struct.size

        self.walking_graphics0     = pc[0]
        self.walking_graphics1     = pc[1]
        self.rotation_speed        = pc[2]
        #self. = pc[2]
        self.tracking_unit         = pc[3]
        self.tracking_unit_used    = pc[4]
        self.tracking_unit_density = pc[5]
        #self. = pc[6]
        #self. = pc[7:(7+17)]

        return offset


class UnitBird(UnitDeadOrFish):
    """
    type_id >= 40
    """

    name_struct        = "unit_bird"
    name_struct_file   = "unit"
    struct_description = "adds search radius and work properties, as well as movement sounds."

    data_format = (
        (None, None, None),
    )

    def __init__(self):
        super().__init__()

    def read(self, raw, offset, cls=None):
        offset = super().read(raw, offset)

        #int16_t sheep_conversion;
        #float search_radius;
        #float work_rate;
        #int16_t drop_site0;
        #int16_t drop_site1;
        #int8_t villager_mode;
        #int16_t move_sound;
        #int16_t stop_sound;
        #int8_t animal_mode;
        tmp_struct = Struct(endianness + "h 2f 2h b 2h b")
        pc = tmp_struct.unpack_from(raw, offset)
        offset += tmp_struct.size

        self.sheep_conversion = pc[0]
        self.search_radius    = pc[1]
        self.work_rate        = pc[2]
        self.drop_site0       = pc[3]
        self.drop_site1       = pc[4]
        self.villager_mode    = pc[5]
        self.move_sound       = pc[6]
        self.stop_sound       = pc[7]
        self.animal_mode      = pc[8]

        return offset


class UnitMovable(UnitBird):
    """
    type_id >= 60
    """

    name_struct        = "unit_movable"
    name_struct_file   = "unit"
    struct_description = "adds attack and armor properties to units."

    data_format = (
        (None, None, None),
    )

    def __init__(self):
        super().__init__()

    def read(self, raw, offset, cls=None):
        offset = super().read(raw, offset)

        #int16_t default_armor;
        #uint16_t attack_count;
        tmp_struct = Struct(endianness + "h H")
        pc = tmp_struct.unpack_from(raw, offset)
        offset += tmp_struct.size

        self.default_armor = pc[0]
        self.attack_count  = pc[1]

        self.attack = list()
        for i in range(self.attack_count):
            t = HitType()
            offset_tmp = t.read(raw, offset)
            self.attack.append(t)
            offset = offset_tmp

        #uint16_t armor_count;
        tmp_struct = Struct(endianness + "H")
        pc = tmp_struct.unpack_from(raw, offset)
        offset += tmp_struct.size

        self.armor_count = pc[0]

        self.armor = list()
        for i in range(self.armor_count):
            t = HitType()
            offset_tmp = t.read(raw, offset)
            self.armor.append(t)
            offset = offset_tmp

        #int16_t unknown;
        #float max_range;
        #float blast_radius;
        #float reload_time0;
        #int16_t projectile_unit_id;
        #int16_t accuracy_percent;
        #int8_t tower_mode;
        #int16_t delay;
        #float graphics_displacement[3];
        #int8_t blast_level;
        #float min_range;
        #float garnison_recovery_rate;
        #int16_t attack_graphic;
        #int16_t melee_armor_displayed;
        #int16_t attack_displayed;
        #int16_t range_displayed;
        #float reload_time1;
        tmp_struct = Struct(endianness + "h 3f 2h b h 3f b 2f 4h xx f")
        pc = tmp_struct.unpack_from(raw, offset)
        offset += tmp_struct.size - 2


        #self. = pc[0]
        self.max_range              = pc[1]
        self.blast_radius           = pc[2]
        self.reload_time0           = pc[3]
        self.projectile_unit_id     = pc[4]
        self.accuracy_percent       = pc[5]
        self.tower_mode             = pc[6]
        self.delay                  = pc[7]
        self.graphics_displacement  = pc[8:(8+3)]
        self.blast_level            = pc[11]
        self.min_range              = pc[12]
        self.garnison_recovery_rate = pc[13]
        self.attack_graphic         = pc[14]
        self.melee_armor_displayed  = pc[15]
        self.attack_displayed       = pc[16]
        self.range_displayed        = pc[17]
        self.reload_time1           = pc[18]

        return offset


class UnitProjectile(UnitMovable):
    """
    type_id == 60
    """

    name_struct        = "unit_projectile"
    name_struct_file   = "unit"
    struct_description = "adds projectile specific unit properties."

    data_format = (
        (None, None, None),
    )

    def __init__(self):
        super().__init__()

    def read(self, raw, offset, cls=None):
        offset = super().read(raw, offset)

        #int8_t stretch_mode;
        #int8_t compensation_mode;
        #int8_t drop_animation_mode;
        #int8_t penetration_mode;
        #int8_t unknown;
        #float projectile_arc;
        tmp_struct = Struct(endianness + "5b x x f")
        pc = tmp_struct.unpack_from(raw, offset)
        offset += tmp_struct.size

        self.stretch_mode        = pc[0]
        self.compensation_mode   = pc[1]
        self.drop_animation_mode = pc[2]
        self.penetration_mode    = pc[3]
        #self. = pc[4]
        self.projectile_arc      = pc[5]

        return offset


class UnitLiving(UnitMovable):
    """
    type_id >= 70
    """

    name_struct        = "unit_living"
    name_struct_file   = "unit"
    struct_description = "adds creation location and garnison unit properties."

    data_format = (
        (None, None, None),
    )

    def __init__(self):
        super().__init__()

    def read(self, raw, offset, cls=None):
        offset = super().read(raw, offset)

        self.ressource_cost = list()
        for i in range(3):
            t = RessourceCost()
            offset = t.read(raw, offset)
            self.ressource_cost.append(t)

        #int16_t train_time;
        #int16_t train_location_id;
        #int8_t button_id;
        #int32_t unknown;
        #int32_t unknown;
        #int8_t missile_graphic_delay;
        #int8_t hero_mode;
        #int16_t garnison_graphic0;
        #int16_t garnison_graphic1;
        #float attack_missile_duplication0;
        #int8_t attack_missile_duplication1;
        #float attack_missile_duplication_spawning[3];
        #int32_t attack_missile_duplication_unit;
        #int32_t attack_missile_duplication_graphic;
        #int8_t unknown;
        #int16_t pierce_armor_displayed;
        tmp_struct = Struct(endianness + "2h b 2i 2b 2h xx f b 3f 2i b h")
        pc = tmp_struct.unpack_from(raw, offset)
        offset += tmp_struct.size


        self.train_time                          = pc[0]
        self.train_location_id                   = pc[1]
        self.button_id                           = pc[2]
        #self. = pc[3]
        #self. = pc[4]
        self.missile_graphic_delay               = pc[5]
        self.hero_mode                           = pc[6]
        self.garnison_graphic0                   = pc[7]
        self.garnison_graphic1                   = pc[8]
        self.attack_missile_duplication0         = pc[9]
        self.attack_missile_duplication1         = pc[10]
        self.attack_missile_duplication_spawning = pc[11:(11+3)]
        self.attack_missile_duplication_unit     = pc[14]
        self.attack_missile_duplication_graphic  = pc[15]
        #self. = pc[16]
        self.pierce_armor_displayed              = pc[17]

        return offset


class UnitBuilding(UnitLiving):
    """
    type_id >= 80
    """

    name_struct        = "unit_building"
    name_struct_file   = "unit"
    struct_description = "construction graphics and garnison building properties for units."

    data_format = (
        (None, None, None),
    )

    def __init__(self):
        super().__init__()

    def read(self, raw, offset, cls=None):
        offset = super().read(raw, offset)

        #int16_t construction_graphic_id;
        #int16_t snow_graphic_id;
        #int16_t adjacent_mode;
        #int8_t unknown;
        #int8_t unknown;
        #int16_t stack_unit_id;
        #int16_t terrain_id;
        #int16_t unknown;
        #int16_t research_id;
        #int8_t unknown;
        tmp_struct = Struct(endianness + "3h 2b 4h b")
        pc = tmp_struct.unpack_from(raw, offset)
        offset += tmp_struct.size

        self.construction_graphic_id = pc[0]
        self.snow_graphic_id         = pc[1]
        self.adjacent_mode           = pc[2]
        #self. = pc[3]
        #self. = pc[4]
        self.stack_unit_id           = pc[5]
        self.terrain_id              = pc[6]
        #self. = pc[7]
        #self.research_id             = pc[8]
        #self. = pc[9]

        self.building_annex = list()
        for i in range(4):
            t = BuildingAnnex()
            offset_tmp = t.read(raw, offset)
            offset = offset_tmp

            self.building_annex.append(t)

        #int16_t head_unit;
        #int16_t transform_unit;
        #int16_t unknown;
        #int16_t construction_sound_id;
        #int8_t garnison_type;
        #float garnison_heal_rate;
        #int32_t unknown;
        #int16_t unknown;
        #int8_t unknown[6];
        tmp_struct = Struct(endianness + "4h b f i h 6b")
        pc = tmp_struct.unpack_from(raw, offset)
        offset += tmp_struct.size

        self.head_unit = pc[0]
        self.transform_unit = pc[1]
        #self. = pc[2]
        self.construction_sound_id = pc[3]
        self.garnison_type = pc[4]
        self.garnison_heal_rate = pc[5]
        #self. = pc[6]
        #self. = pc[7]
        #self. = pc[8:(8+6)]

        return offset


class UnitTree(UnitObject):
    """
    type_id = 90
    """

    name_struct        = "unit_tree"
    name_struct_file   = "unit"
    struct_description = "just a tree unit."

    data_format = ()

    def __init__(self):
        super().__init__()


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
