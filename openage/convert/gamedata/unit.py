# Copyright 2013-2017 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R,too-many-lines

from ..dataformat.exportable import Exportable
from ..dataformat.member_access import READ, READ_EXPORT, READ_UNKNOWN
from ..dataformat.members import EnumLookupMember, ContinueReadMember, IncludeMembers, SubdataMember


class UnitCommand(Exportable):
    """
    also known as "Task" according to ES debug code,
    this structure is the master for spawn-unit actions.
    """
    name_struct        = "unit_command"
    name_struct_file   = "unit"
    struct_description = "a command a single unit may receive by script or human."
    
    data_format = []
    data_format.append((READ, "command_used", "int16_t"))                  # Type (0 = Generic, 1 = Tribe)
    data_format.append((READ_EXPORT, "id", "int16_t"))                     # Identity
    data_format.append((READ_UNKNOWN, None, "int8_t"))                     # IsDefault
    data_format.append((READ_EXPORT, "type", EnumLookupMember(
        raw_type    = "int16_t",
        type_name   = "command_ability",
        lookup_dict = {
            # the Action-Types found under RGE namespace:
            0: "UNUSED",
            1: "MOVE_TO",
            2: "FOLLOW",
            3: "GARRISON",  # also known as "Enter"
            4: "EXPLORE",
            5: "GATHER",
            6: "NATURAL_WONDERS_CHEAT",  # also known as "Graze"
            7: "COMBAT",       # this is converted to action-type 9 when once instanciated
            8: "MISSILE",      # for projectiles
            9: "ATTACK",
            10: "BIRD",        # flying.
            11: "PREDATOR",    # scares other animals when hunting
            12: "TRANSPORT",
            13: "GUARD",
            14: "TRANSPORT_OVER_WALL",
            20: "RUN_AWAY",
            21: "MAKE",
            # Action-Types found under TRIBE namespace:
            101: "BUILD",
            102: "MAKE_OBJECT",
            103: "MAKE_TECH",
            104: "CONVERT",
            105: "HEAL",
            106: "REPAIR",
            107: "CONVERT_AUTO",  # "Artifact": can get auto-converted
            108: "DISCOVERY",
            109: "SHOOTING_RANGE_RETREAT",
            110: "HUNT",
            111: "TRADE",
            120: "WONDER_VICTORY_GENERATE",
            121: "DESELECT_ON_TASK",
            122: "LOOT",
            123: "HOUSING",
            124: "PACK",
            125: "UNPACK_ATTACK",
            130: "OFF_MAP_TRADE_0",
            131: "OFF_MAP_TRADE_1",
            132: "PICKUP_UNIT",
            133: "PICKUP_133",
            134: "PICKUP_134",
            135: "KIDNAP_UNIT",
            136: "DEPOSIT_UNIT",
            149: "SHEAR",
            150: "REGENERATION",
            151: "FEITORIA",
            768: "UNKNOWN_768",
            1024: "UNKNOWN_1024",
            },
        )))
    data_format.append((READ_EXPORT, "class_id", "int16_t"))
    data_format.append((READ_EXPORT, "unit_id", "int16_t"))
    data_format.append((READ_EXPORT, "terrain_id", "int16_t"))
    data_format.append((READ_EXPORT, "attribute_id1", "int16_t"))          # carry resource
    data_format.append((READ_EXPORT, "attribute_id2", "int16_t"))          # resource that multiplies the amount you can gather
    data_format.append((READ_EXPORT, "attribute_id3", "int16_t"))          # drop resource
    data_format.append((READ_EXPORT, "attribute_id4", "int16_t"))
    data_format.append((READ_EXPORT, "work_value1", "float"))              # quantity
    data_format.append((READ_EXPORT, "work_value2", "float"))              # execution radius?
    data_format.append((READ_EXPORT, "work_range", "float"))
    data_format.append((READ, "search_mode", "int8_t"))
    data_format.append((READ, "search_time", "float"))
    data_format.append((READ, "combat_level", "int8_t"))                   # type defined in `selection_type`
    data_format.append((READ, "combat_mode", "int8_t"))
    data_format.append((READ, "work_mode1", "int16_t"))
    data_format.append((READ, "work_mode2", "int16_t"))
    data_format.append((READ_EXPORT, "owner_type", EnumLookupMember(
        # what can be selected as a target for the unit command?
        raw_type    = "int8_t",
        type_name   = "selection_type",
        lookup_dict = {
            0: "ANY_0",               # select anything
            1: "OWNED_UNITS",         # your own things
            2: "NEUTRAL_ENEMY",       # enemy and neutral things (->attack)
            3: "NOTHING",
            4: "GAIA_OWNED_ALLY",     # any of gaia, owned or allied things
            5: "GAYA_NEUTRAL_ENEMY",  # any of gaia, neutral or enemy things
            6: "NOT_OWNED",           # all things that aren't yours
            7: "ANY_7",
        },
    )))
    data_format.append((READ, "holding_mode", "int8_t"))              # TODO: what does it do? right click?
    data_format.append((READ, "state_build", "int8_t"))
    data_format.append((READ_EXPORT, "move_sprite_id", "int16_t"))    # walking with tool but no resource
    data_format.append((READ_EXPORT, "work_sprite_id1", "int16_t"))   # proceeding resource gathering or attack
    data_format.append((READ_EXPORT, "work_sprite_id2", "int16_t"))   # actual execution or transformation graphic
    data_format.append((READ_EXPORT, "carry_sprite_id", "int16_t"))   # display resources in hands
    data_format.append((READ_EXPORT, "work_sound_id1", "int16_t"))    # sound to play when execution starts
    data_format.append((READ_EXPORT, "work_sound_id2", "int16_t"))    # sound to play on resource drop


class UnitHeader(Exportable):
    name_struct        = "unit_header"
    name_struct_file   = "unit"
    struct_description = "stores a bunch of unit commands."

    data_format = []
    data_format.append((READ, "exists", ContinueReadMember("uint8_t")))
    data_format.append((READ, "unit_command_count", "uint16_t"))
    data_format.append((READ_EXPORT, "unit_commands", SubdataMember(
        ref_type=UnitCommand,
        length="unit_command_count",
    )))


class ResourceStorage(Exportable):
    name_struct        = "resource_storage"
    name_struct_file   = "unit"
    struct_description = "determines the resource storage capacity for one unit mode."
    
    data_format = []
    data_format.append((READ, "type", "int16_t"))
    data_format.append((READ, "amount", "float"))
    data_format.append((READ, "used_mode", EnumLookupMember(
        raw_type    = "int8_t",
        type_name   = "resource_handling",
        lookup_dict = {
            0: "DECAYABLE",
            1: "KEEP_AFTER_DEATH",
            2: "RESET_ON_DEATH_INSTANT",
            4: "RESET_ON_DEATH_WHEN_COMPLETED",
            },
        )))


class DamageGraphic(Exportable):
    name_struct        = "damage_graphic"
    name_struct_file   = "unit"
    struct_description = "stores one possible unit image that is displayed at a given damage percentage."

    data_format = []
    data_format.append((READ_EXPORT, "graphic_id", "int16_t"))
    data_format.append((READ_EXPORT, "damage_percent", "int8_t"))
    data_format.append((READ, "apply_mode_old", "int8_t"))    # gets overwritten in aoe memory by the real apply_mode:
    data_format.append((READ_EXPORT, "apply_mode", EnumLookupMember(
        raw_type    = "int8_t",
        type_name   = "damage_draw_type",
        lookup_dict = {
            0: "TOP",      # adds graphics on top (e.g. flames)
            1: "RANDOM",   # adds graphics on top randomly
            2: "REPLACE",  # replace original graphics (e.g. damaged walls)
            },
        )))


class HitType(Exportable):
    name_struct        = "hit_type"
    name_struct_file   = "unit"
    struct_description = "stores attack amount for a damage type."

    data_format = []
    data_format.append((READ, "type_id", EnumLookupMember(
        raw_type    = "int16_t",
        type_name   = "hit_class",
        lookup_dict = {
            -1: "NONE",
            0: "UNKNOWN_0",
            1: "INFANTRY",
            2: "SHIP_TURTLE",
            3: "UNITS_PIERCE",
            4: "UNITS_MELEE",
            5: "WAR_ELEPHANT",
            8: "CAVALRY",
            11: "BUILDINGS_NO_PORT",
            13: "STONE_DEFENSES",
            14: "UNKNOWN_14",
            15: "ARCHERS",
            16: "SHIPS_CAMELS_SABOTEURS",
            17: "RAMS",
            18: "TREES",
            19: "UNIQUE_UNITS",
            20: "SIEGE_WEAPONS",
            21: "BUILDINGS",
            22: "WALLS_GATES",
            23: "UNKNOWN_23",
            24: "BOAR",
            25: "MONKS",
            26: "CASTLE",
            27: "SPEARMEN",
            28: "CAVALRY_ARCHER",
            29: "EAGLE_WARRIOR",
            30: "UNKNOWN_30",
            },
    )))
    data_format.append((READ, "amount", "int16_t"))


class ResourceCost(Exportable):
    name_struct        = "resource_cost"
    name_struct_file   = "unit"
    struct_description = "stores cost for one resource for creating the unit."

    data_format = []
    data_format.append((READ, "type_id", EnumLookupMember(
        raw_type = "int16_t",
        type_name = "resource_types",
        lookup_dict = {
            -1: "NONE",
            0: "FOOD_STORAGE",
            1: "WOOD_STORAGE",
            2: "STONE_STORAGE",
            3: "GOLD_STORAGE",
            4: "POPULATION_HEADROOM",
            5: "CONVERSION_RANGE",
            6: "CURRENT_AGE",
            7: "OWNED_RELIC_COUNT",
            8: "TRADE_BONUS",
            9: "TRADE_GOODS",
            10: "TRADE_PRODUCTION",
            11: "POPULATION",           # both current population and population headroom
            12: "CORPSE_DECAY_TIME",
            13: "DISCOVERY",
            14: "RUIN_MONUMENTS_CAPTURED",
            15: "MEAT_STORAGE",
            16: "BERRY_STORAGE",
            17: "FISH_STORAGE",
            18: "UNKNOWN_18",           # in starwars: power core range
            19: "TOTAL_UNITS_OWNED",    # or just military ones? used for counting losses
            20: "UNITS_KILLED",
            21: "RESEARCHED_TECHNOLOGIES_COUNT",
            22: "MAP_EXPLORED_PERCENTAGE",
            23: "CASTLE_AGE_TECH_INDEX",      # default: 102
            24: "IMPERIAL_AGE_TECH_INDEX",    # default: 103
            25: "FEUDAL_AGE_TECH_INDEX",      # default: 101
            26: "ATTACK_WARNING_SOUND",
            27: "ENABLE_MONK_CONVERSION",
            28: "ENABLE_BUILDING_CONVERSION",
            30: "BUILDING_COUNT",              # default: 500
            31: "FOOD_COUNT",
            32: "BONUS_POPULATION",
            33: "MAINTENANCE",
            34: "FAITH",
            35: "FAITH_RECHARGE_RATE",  # default: 1.6
            36: "FARM_FOOD_AMOUNT",     # default: 175
            37: "CIVILIAN_POPULATION",
            38: "UNKNOWN_38",           # starwars: shields for bombers/fighters
            39: "ALL_TECHS_ACHIEVED",   # default: 178
            40: "MILITARY_POPULATION",  # -> largest army
            41: "UNITS_CONVERTED",      # monk success count
            42: "WONDERS_STANDING",
            43: "BUILDINGS_RAZED",
            44: "KILL_RATIO",
            45: "SURVIVAL_TO_FINISH",   # bool
            46: "TRIBUTE_FEE",          # default: 0.3
            47: "GOLD_MINING_PRODUCTIVITY",  # default: 1
            48: "TOWN_CENTER_UNAVAILABLE",  # -> you may build a new one
            49: "GOLD_COUNTER",
            50: "REVEAL_ALLY",          # bool, ==cartography discovered
            51: "HOUSES_COUNT",
            52: "MONASTERY_COUNT",
            53: "TRIBUTE_SENT",
            54: "RUINES_CAPTURED_ALL",  # bool
            55: "RELICS_CAPTURED_ALL",  # bool
            56: "ORE_STORAGE",
            57: "CAPTURED_UNITS",
            58: "DARK_AGE_TECH_INDEX",  # default: 104
            59: "TRADE_GOOD_QUALITY",   # default: 1
            60: "TRADE_MARKET_LEVEL",
            61: "FORMATIONS",
            62: "BUILDING_HOUSING_RATE",  # default: 20
            63: "GATHER_TAX_RATE",        # default: 32000
            64: "GATHER_ACCUMULATOR",
            65: "SALVAGE_DECAY_RATE",     # default: 5
            66: "ALLOW_FORMATION",        # bool, something with age?
            67: "ALLOW_CONVERSIONS",      # bool
            68: "HIT_POINTS_KILLED",      # unused
            69: "KILLED_PLAYER_1",        # bool
            70: "KILLED_PLAYER_2",        # bool
            71: "KILLED_PLAYER_3",        # bool
            72: "KILLED_PLAYER_4",        # bool
            73: "KILLED_PLAYER_5",        # bool
            74: "KILLED_PLAYER_6",        # bool
            75: "KILLED_PLAYER_7",        # bool
            76: "KILLED_PLAYER_8",        # bool
            77: "CONVERSION_RESISTANCE",
            78: "TRADE_VIG_RATE",             # default: 0.3
            79: "STONE_MINING_PRODUCTIVITY",  # default: 1
            80: "QUEUED_UNITS",
            81: "TRAINING_COUNT",
            82: "START_PACKED_TOWNCENTER",   # or raider, default: 2
            83: "BOARDING_RECHARGE_RATE",
            84: "STARTING_VILLAGERS",        # default: 3
            85: "RESEARCH_COST_MULTIPLIER",
            86: "RESEARCH_TIME_MULTIPLIER",
            87: "CONVERT_SHIPS_ALLOWED",     # bool
            88: "FISH_TRAP_FOOD_AMOUNT",     # default: 700
            89: "HEALING_RATE_MULTIPLIER",
            90: "HEALING_RANGE",
            91: "STARTING_FOOD",
            92: "STARTING_WOOD",
            93: "STARTING_STONE",
            94: "STARTING_GOLD",
            95: "TOWN_CENTER_PACKING",        # or raider, default: 3
            96: "BERSERKER_HEAL_TIME",        # in seconds
            97: "DOMINANT_ANIMAL_DISCOVERY",  # bool, sheep/turkey
            98: "SCORE_OBJECT_COST",          # object cost summary, economy?
            99: "SCORE_RESEARCH",
            100: "RELIC_GOLD_COLLECTED",
            101: "TRADE_PROFIT",
            102: "TRIBUTE_P1",
            103: "TRIBUTE_P2",
            104: "TRIBUTE_P3",
            105: "TRIBUTE_P4",
            106: "TRIBUTE_P5",
            107: "TRIBUTE_P6",
            108: "TRIBUTE_P7",
            109: "TRIBUTE_P8",
            110: "KILL_SCORE_P1",
            111: "KILL_SCORE_P2",
            112: "KILL_SCORE_P3",
            113: "KILL_SCORE_P4",
            114: "KILL_SCORE_P5",
            115: "KILL_SCORE_P6",
            116: "KILL_SCORE_P7",
            117: "KILL_SCORE_P8",
            118: "RAZING_COUNT_P1",
            119: "RAZING_COUNT_P2",
            120: "RAZING_COUNT_P3",
            121: "RAZING_COUNT_P4",
            122: "RAZING_COUNT_P5",
            123: "RAZING_COUNT_P6",
            124: "RAZING_COUNT_P7",
            125: "RAZING_COUNT_P8",
            126: "RAZING_SCORE_P1",
            127: "RAZING_SCORE_P2",
            128: "RAZING_SCORE_P3",
            129: "RAZING_SCORE_P4",
            130: "RAZING_SCORE_P5",
            131: "RAZING_SCORE_P6",
            132: "RAZING_SCORE_P7",
            133: "RAZING_SCORE_P8",
            134: "STANDING_CASTLES",
            135: "RAZINGS_HIT_POINTS",
            136: "KILLS_BY_P1",
            137: "KILLS_BY_P2",
            138: "KILLS_BY_P3",
            139: "KILLS_BY_P4",
            140: "KILLS_BY_P5",
            141: "KILLS_BY_P6",
            142: "KILLS_BY_P7",
            143: "KILLS_BY_P8",
            144: "RAZINGS_BY_P1",
            145: "RAZINGS_BY_P2",
            146: "RAZINGS_BY_P3",
            147: "RAZINGS_BY_P4",
            148: "RAZINGS_BY_P5",
            149: "RAZINGS_BY_P6",
            150: "RAZINGS_BY_P7",
            151: "RAZINGS_BY_P8",
            152: "LOST_UNITS_SCORE",
            153: "LOST_BUILDINGS_SCORE",
            154: "LOST_UNITS",
            155: "LOST_BUILDINGS",
            156: "TRIBUTE_FROM_P1",
            157: "TRIBUTE_FROM_P2",
            158: "TRIBUTE_FROM_P3",
            159: "TRIBUTE_FROM_P4",
            160: "TRIBUTE_FROM_P5",
            161: "TRIBUTE_FROM_P6",
            162: "TRIBUTE_FROM_P7",
            163: "TRIBUTE_FROM_P8",
            164: "SCORE_UNITS_CURRENT",
            165: "SCORE_BUILDINGS_CURRENT",         # default: 275
            166: "COLLECTED_FOOD",
            167: "COLLECTED_WOOD",
            168: "COLLECTED_STONE",
            169: "COLLECTED_GOLD",
            170: "SCORE_MILITARY",
            171: "TRIBUTE_RECEIVED",
            172: "SCORE_RAZINGS",
            173: "TOTAL_CASTLES",
            174: "TOTAL_WONDERS",
            175: "SCORE_ECONOMY_TRIBUTES",
            176: "CONVERT_ADJUSTMENT_MIN",          # used for resistance against monk conversions
            177: "CONVERT_ADJUSTMENT_MAX",
            178: "CONVERT_RESIST_ADJUSTMENT_MIN",
            179: "CONVERT_RESIST_ADJUSTMENT_MAX",
            180: "CONVERT_BUILDIN_MIN",             # default: 15
            181: "CONVERT_BUILDIN_MAX",             # default: 25
            182: "CONVERT_BUILDIN_CHANCE",          # default: 25
            183: "REVEAL_ENEMY",
            184: "SCORE_SOCIETY",                   # wonders, castles
            185: "SCORE_FOOD",
            186: "SCORE_WOOD",
            187: "SCORE_STONE",
            188: "SCORE_GOLD",
            189: "CHOPPING_PRODUCTIVITY",           # default: 1
            190: "FOOD_GATHERING_PRODUCTIVITY",     # default: 1
            191: "RELIC_GOLD_PRODUCTION_RATE",      # default: 30
            192: "CONVERTED_UNITS_DIE",             # bool
            193: "THEOCRACY_ACTIVE",                # bool
            194: "CRENELLATIONS_ACTIVE",            # bool
            195: "CONSTRUCTION_RATE_MULTIPLIER",    # except for wonders
            196: "HUN_WONDER_BONUS",
            197: "SPIES_DISCOUNT",                  # or atheism_active?
            }
    )))
    data_format.append((READ, "amount", "int16_t"))
    data_format.append((READ, "enabled", "int16_t"))


class BuildingAnnex(Exportable):

    name_struct        = "building_annex"
    name_struct_file   = "unit"
    struct_description = "a possible building annex."

    data_format = []
    data_format.append((READ_EXPORT, "unit_id",    "int16_t"))
    data_format.append((READ_EXPORT, "misplaced0", "float"))
    data_format.append((READ_EXPORT, "misplaced1", "float"))


class UnitObject(Exportable):
    """
    base properties for every unit entry.
    """

    name_struct        = "unit_object"
    name_struct_file   = "unit"
    struct_description = "base properties for all units."

    data_format = []
    data_format.append((READ, "name_length", "uint16_t"))
    data_format.append((READ_EXPORT, "id0", "int16_t"))
    data_format.append((READ_EXPORT, "language_dll_name", "uint16_t"))
    data_format.append((READ_EXPORT, "language_dll_creation", "uint16_t"))
    data_format.append((READ_EXPORT, "unit_class", EnumLookupMember(
            raw_type    = "int16_t",
            type_name   = "unit_classes",
            lookup_dict = {
                -1: "NONE",
                0: "ARCHER",
                1: "ARTIFACT",
                2: "TRADE_BOAT",
                3: "BUILDING",
                4: "CIVILIAN",
                5: "SEA_FISH",
                6: "SOLDIER",
                7: "BERRY_BUSH",
                8: "STONE_MINE",
                9: "PREY_ANIMAL",
                10: "PREDATOR_ANIMAL",
                11: "OTHER",
                12: "CAVALRY",
                13: "SIEGE_WEAPON",
                14: "TERRAIN",
                15: "TREES",
                16: "UNKNOWN_16",
                18: "PRIEST",
                19: "TRADE_CART",
                20: "TRANSPORT_BOAT",
                21: "FISHING_BOAT",
                22: "WAR_BOAT",
                23: "CONQUISTADOR",
                27: "WALLS",
                28: "PHALANX",
                29: "ANIMAL_DOMESTICATED",
                30: "FLAGS",
                32: "GOLD_MINE",
                33: "SHORE_FISH",
                34: "CLIFF",
                35: "PETARD",
                36: "CAVALRY_ARCHER",
                37: "DOLPHIN",
                38: "BIRDS",
                39: "GATES",
                40: "PILES",
                41: "PILES_OF_RESOURCE",
                42: "RELIC",
                43: "MONK_WITH_RELIC",
                44: "HAND_CANNONEER",
                45: "TWO_HANDED_SWORD",
                46: "PIKEMAN",
                47: "SCOUT_CAVALRY",
                48: "ORE_MINE",
                49: "FARM",
                50: "SPEARMAN",
                51: "PACKED_SIEGE_UNITS",
                52: "TOWER",
                53: "BOARDING_BOAT",
                54: "UNPACKED_SIEGE_UNITS",
                55: "SCORPION",
                56: "RAIDER",
                57: "CAVALRY_RAIDER",
                58: "SHEEP",
                59: "KING",
                61: "HORSE",
            },
        )))
    data_format.append((READ_EXPORT, "graphic_standing0", "int16_t"))
    data_format.append((READ_EXPORT, "graphic_standing1", "int16_t"))
    data_format.append((READ_EXPORT, "graphic_dying0", "int16_t"))
    data_format.append((READ_EXPORT, "graphic_dying1", "int16_t"))
    data_format.append((READ, "death_mode", "int8_t"))                  # 1 = become `dead_unit_id` (reviving does not make it usable again)
    data_format.append((READ_EXPORT, "hit_points", "int16_t"))          # unit health. -1=insta-die
    data_format.append((READ, "line_of_sight", "float"))
    data_format.append((READ, "garrison_capacity", "int8_t"))           # number of units that can garrison in there
    data_format.append((READ_EXPORT, "radius_x", "float"))              # size of the unit
    data_format.append((READ_EXPORT, "radius_y", "float"))
    data_format.append((READ_EXPORT, "radius_z", "float"))
    data_format.append((READ_EXPORT, "sound_creation0", "int16_t"))
    data_format.append((READ_EXPORT, "sound_creation1", "int16_t"))
    data_format.append((READ_EXPORT, "dead_unit_id", "int16_t"))        # unit id to become on death
    data_format.append((READ, "placement_mode", "int8_t"))              # 0=placable on top of others in scenario editor, 5=can't
    data_format.append((READ, "air_mode", "int8_t"))                    # 1=no footprints
    data_format.append((READ, "icon_id", "int16_t"))                    # frame id of the icon slp (57029) to place on the creation button
    data_format.append((READ, "hidden_in_editor", "int8_t"))
    data_format.append((READ, "portrait_icon_id", "int16_t"))
    data_format.append((READ, "enabled", "int16_t"))                    # 0=unlocked by research, 1=insta-available
    data_format.append((READ, "placement_side_terrain0", "int16_t"))    # terrain id that's needed somewhere on the foundation (e.g. dock water)
    data_format.append((READ, "placement_side_terrain1", "int16_t"))    # second slot for ^
    data_format.append((READ, "placement_terrain0", "int16_t"))         # terrain needed for placement (e.g. dock: water)
    data_format.append((READ, "placement_terrain1", "int16_t"))         # alternative terrain needed for placement (e.g. dock: shallows)
    data_format.append((READ, "clearance_size_x", "float"))             # minimum space required to allow placement in editor
    data_format.append((READ, "clearance_size_y", "float"))
    data_format.append((READ_EXPORT, "building_mode", EnumLookupMember(
            raw_type    = "int8_t",
            type_name   = "building_modes",
            lookup_dict = {
                0: "NON_BUILDING",    # gates, farms, walls, towers
                2: "TRADE_BUILDING",  # towncenter, port, trade workshop
                3: "ANY",
            },
        )))
    data_format.append((READ_EXPORT, "visible_in_fog", EnumLookupMember(
            raw_type    = "int8_t",
            type_name   = "fog_visibility",
            lookup_dict = {
                0: "INVISIBLE",     # people etc
                1: "VISIBLE",       # buildings
                3: "ONLY_IN_FOG",
            },
        )))
    data_format.append((READ_EXPORT, "terrain_restriction", EnumLookupMember(
            raw_type    = "int16_t",      # determines on what type of ground the unit can be placed/walk
            type_name   = "ground_type",  # is actually the id of the terrain_restriction entry!
            lookup_dict = {
                -0x01: "NONE",
                0x00: "ANY",
                0x01: "SHORELINE",
                0x02: "WATER",
                0x03: "WATER_SHIP_0x03",
                0x04: "FOUNDATION",
                0x05: "NOWHERE",              # can't place anywhere
                0x06: "WATER_DOCK",           # shallow water for dock placement
                0x07: "SOLID",
                0x08: "NO_ICE_0x08",
                0x0A: "NO_ICE_0x0A",
                0x0B: "FOREST",
                0x0C: "UNKNOWN_0x0C",
                0x0D: "WATER_0x0D",           # great fish
                0x0E: "UNKNOWN_0x0E",
                0x0F: "WATER_SHIP_0x0F",      # transport ship
                0x10: "GRASS_SHORELINE",      # for gates and walls
                0x11: "WATER_ANY_0x11",
                0x12: "UNKNOWN_0x12",
                0x13: "FISH_NO_ICE",
                0x14: "WATER_ANY_0x14",
                0x15: "WATER_SHALLOW",
            },
        )))
    data_format.append((READ_EXPORT, "movement_type", "int8_t"))    # flying, etc. TODO: lookup dict
    data_format.append((READ_EXPORT, "resource_capacity", "int16_t"))
    data_format.append((READ_EXPORT, "resource_decay", "float"))                 # when animals rot, their resources decay
    data_format.append((READ_EXPORT, "blast_defense_level", EnumLookupMember(
            # receive blast damage from units that have lower or same blast_attack_level.
            raw_type    = "int8_t",
            type_name   = "blast_types",
            lookup_dict = {
                0: "UNIT_0",    # projectile, dead, fish, relic, tree, gate, towncenter
                1: "OTHER",     # 'other' things with multiple rotations
                2: "BUILDING",  # buildings, gates, walls, towncenter, fishtrap
                3: "UNIT_3",    # boar, farm, fishingship, villager, tradecart, sheep, turkey, archers, junk, ships, monk, siege
            }
        )))
    data_format.append((READ_EXPORT, "combat_level", EnumLookupMember(
            raw_type = "int8_t",
            type_name = "combat_levels",
            lookup_dict = {
                0: "PROJECTILE_DEAD_RESOURCE",
                1: "BOAR",
                2: "BUILDING",
                3: "CIVILIAN",
                4: "MILITARY",
                5: "OTHER",
            }
        )))
    data_format.append((READ_EXPORT, "interaction_mode", EnumLookupMember(
            # what can be done with this unit?
            raw_type    = "int8_t",
            type_name   = "interaction_modes",
            lookup_dict = {
                0: "NOTHING_0",
                1: "NOTHING_1",
                2: "SELECTABLE",
                3: "SELECT_ATTACK",
                4: "SELECT_ATTACK_MOVE",
                5: "SELECT_MOVE",
            },
        )))
    data_format.append((READ_EXPORT, "map_draw_level", EnumLookupMember(
            # how does the unit show up on the minimap?
            raw_type    = "int8_t",
            type_name   = "minimap_modes",
            lookup_dict = {
                0: "NO_DOT_0",
                1: "SQUARE_DOT",   # turns white when selected
                2: "DIAMOND_DOT",  # dito
                3: "DIAMOND_DOT_KEEPCOLOR",  # doesn't turn white when selected
                4: "LARGEDOT",   # observable by all players, no attacked-blinking
                5: "NO_DOT_5",
                6: "NO_DOT_6",
                7: "NO_DOT_7",
                8: "NO_DOT_8",
                9: "NO_DOT_9",
                10: "NO_DOT_10",
            },
        )))
    data_format.append((READ_EXPORT, "unit_level", EnumLookupMember(
            # selects the available ui command buttons for the unit
            raw_type    = "int8_t",
            type_name   = "command_attributes",
            lookup_dict = {
                0: "LIVING",                # commands: delete, garrison, stop, attributes: hit points
                1: "ANIMAL",                # animal
                2: "NONMILITARY_BULIDING",  # civilian building (build page 1)
                3: "VILLAGER",              # villager
                4: "MILITARY_UNIT",         # military unit
                5: "TRADING_UNIT",          # trading unit
                6: "MONK_EMPTY",            # monk
                7: "TRANSPORT_SHIP",        # transport ship
                8: "RELIC",                 # relic / monk with relic
                9: "FISHING_SHIP",          # fishing ship
                10: "MILITARY_BUILDING",    # military building (build page 2)
                11: "SHIELDED_BUILDING",    # shield building (build page 3)
                12: "UNKNOWN_12",
            },
        )))
    data_format.append((READ, "attack_reaction", "float"))
    data_format.append((READ_EXPORT, "minimap_color", "int8_t"))        # palette color id for the minimap
    data_format.append((READ_EXPORT, "language_dll_help", "uint16_t"))  # help text for this unit, stored in the translation dll.
    data_format.append((READ, "hot_keys", "int16_t[4]"))                # language dll dependent (kezb lazouts!)
    data_format.append((READ_UNKNOWN, None, "int8_t"))
    data_format.append((READ_UNKNOWN, None, "int8_t"))
    data_format.append((READ, "unselectable", "uint8_t"))
    data_format.append((READ_UNKNOWN, None, "int8_t"))
    data_format.append((READ_UNKNOWN, None, "int8_t"))
    data_format.append((READ_UNKNOWN, None, "int8_t"))

    # bit 0 == 1 && val != 7: mask shown behind buildings,
    # bit 0 == 0 && val != {6, 10}: no mask displayed,
    # val == {-1, 7}: in open area mask is partially displayed
    # val == {6, 10}: building, causes mask to appear on units behind it
    data_format.append((READ, "selection_mask", "int8_t"))
    data_format.append((READ, "selection_shape_type", "int8_t"))
    data_format.append((READ_EXPORT, "selection_shape", "int8_t"))            # 0=square, 1<=round

    # bitfield of unit attributes:
    # bit 0: allow garrison,
    # bit 1: don't join formation,
    # bit 2: stealth unit,
    # bit 3: detector unit,
    # bit 4: mechanical unit,
    # bit 5: biological unit,
    # bit 6: self-shielding unit,
    # bit 7: invisible unit
    data_format.append((READ, "attribute", "uint8_t"))
    data_format.append((READ, "civilisation", "int8_t"))
    data_format.append((READ, "attribute_piece", "int16_t"))   # leftover from attribute+civ variable
    data_format.append((READ_EXPORT, "selection_effect", EnumLookupMember(
            # things that happen when the unit was selected
            raw_type = "int8_t",
            type_name = "selection_effects",
            lookup_dict = {
                0: "NONE",
                1: "HPBAR_ON_OUTLINE_DARK",  # permanent, editor only
                2: "HPBAR_ON_OUTLINE_NORMAL",
                3: "HPBAR_OFF_SELECTION_SHADOW",
                4: "HPBAR_OFF_OUTLINE_NORMAL",
                5: "HPBAR_ON_5",
                6: "HPBAR_OFF_6",
                7: "HPBAR_OFF_7",
                8: "HPBAR_ON_8",
                9: "HPBAR_ON_9",
            },
        )))
    data_format.append((READ, "editor_selection_color", "uint8_t"))  # 0: default, -16: fish trap, farm, 52: deadfarm, OLD-*, 116: flare, whale, dolphin -123: fish
    data_format.append((READ_EXPORT, "selection_shape_x", "float"))
    data_format.append((READ_EXPORT, "selection_shape_y", "float"))
    data_format.append((READ_EXPORT, "selection_shape_z", "float"))
    data_format.append((READ_EXPORT, "resource_storage", SubdataMember(
            ref_type=ResourceStorage,
            length=3,
        )))
    data_format.append((READ, "damage_graphic_count", "int8_t"))
    data_format.append((READ_EXPORT, "damage_graphic", SubdataMember(
            ref_type=DamageGraphic,
            length="damage_graphic_count",
        )))
    data_format.append((READ_EXPORT, "sound_selection", "int16_t"))
    data_format.append((READ_EXPORT, "sound_dying", "int16_t"))
    data_format.append((READ_EXPORT, "attack_mode", EnumLookupMember(  # maybe obsolete, as it's copied when converting the unit
            raw_type = "int8_t",     # things that happen when the unit was selected
            type_name = "attack_modes",
            lookup_dict = {
                0: "NO",         # no attack
                1: "FOLLOWING",  # by following
                2: "RUN",        # run when attacked
                3: "UNKNOWN3",
                4: "ATTACK",
            },
        )))


    data_format.append((READ_UNKNOWN, None, "int8_t"))         # maybe: 1: yup, you may eat/gather it.
    data_format.append((READ_EXPORT, "name", "char[name_length]"))
    data_format.append((READ_EXPORT, "id1", "int16_t"))
    data_format.append((READ_EXPORT, "id2", "int16_t"))


class AnimatedUnit(UnitObject):
    """
    type_id >= 20
    Animated master object
    """

    name_struct        = "animated_unit"
    name_struct_file   = "unit"
    struct_description = "adds speed property to units."

    data_format = []
    data_format.append((READ_EXPORT, None, IncludeMembers(cls=UnitObject)))
    data_format.append((READ_EXPORT, "speed", "float"))


class DoppelgangerUnit(AnimatedUnit):
    """
    type_id >= 25
    """

    name_struct        = "doppelganger_unit"
    name_struct_file   = "unit"
    struct_description = "weird doppelganger unit thats actually the same as an animated unit."

    data_format = [
        (READ_EXPORT, None, IncludeMembers(cls=AnimatedUnit)),
    ]


class MovingUnit(DoppelgangerUnit):
    """
    type_id >= 30
    Moving master object
    """

    name_struct        = "moving_unit"
    name_struct_file   = "unit"
    struct_description = "adds walking graphics, rotations and tracking properties to units."

    data_format = []
    data_format.append((READ_EXPORT, None, IncludeMembers(cls=DoppelgangerUnit)))
    data_format.append((READ_EXPORT, "walking_graphics0", "int16_t"))
    data_format.append((READ_EXPORT, "walking_graphics1", "int16_t"))
    data_format.append((READ, "turn_speed", "float"))
    data_format.append((READ, "size_class", "int8_t"))
    data_format.append((READ, "trail_unit_id", "int16_t"))             # unit id for the ground traces
    data_format.append((READ, "trail_opsions", "uint8_t"))             # ground traces: -1: no tracking present, 2: projectiles with tracking unit
    data_format.append((READ, "trail_spacing", "float"))               # ground trace spacing: 0: no tracking, 0.5: trade cart, 0.12: some projectiles, 0.4: other projectiles
    data_format.append((READ, "move_algorithm", "int8_t"))
    data_format.append((READ, "rotation_angles", "float[5]"))


class ActionUnit(MovingUnit):
    """
    type_id >= 40
    Action master object
    """

    name_struct        = "action_unit"
    name_struct_file   = "unit"
    struct_description = "adds search radius and work properties, as well as movement sounds."

    data_format = []
    data_format.append((READ_EXPORT, None, IncludeMembers(cls=MovingUnit)))
    # callback unit action id when found.
    # monument and sheep: 107 = enemy convert.
    # all auto-convertible units: 0, most other units: -1
    data_format.append((READ, "default_task_id", "int16_t"))             # e.g. when sheep are discovered
    data_format.append((READ, "search_radius", "float"))
    data_format.append((READ_EXPORT, "work_rate", "float"))
    data_format.append((READ_EXPORT, "drop_site0", "int16_t"))           # unit id where gathered resources shall be delivered to
    data_format.append((READ_EXPORT, "drop_site1", "int16_t"))           # alternative unit id
    data_format.append((READ_EXPORT, "task_by_group", "int8_t"))         # if a task is not found in the current unit, other units with the same group id are tried.
                                                          # 1: male villager; 2: female villager; 3+: free slots
                                                          # basically this creates a "swap group id" where you can place different-graphic units together.
    data_format.append((READ_EXPORT, "command_sound_id", "int16_t"))     # sound played when a command is instanciated
    data_format.append((READ_EXPORT, "stop_sound_id", "int16_t"))        # sound when the command is done (e.g. unit stops at target position)
    data_format.append((READ, "run_pattern", "int8_t"))                  # how animals run around randomly


class ProjectileUnit(ActionUnit):
    """
    type_id >= 60
    Projectile master object
    """

    name_struct        = "projectile_unit"
    name_struct_file   = "unit"
    struct_description = "adds attack and armor properties to units."

    data_format = []
    data_format.append((READ_EXPORT, None, IncludeMembers(cls=ActionUnit)))
    data_format.append((READ, "default_armor", "int16_t"))
    data_format.append((READ, "attack_count", "uint16_t"))
    data_format.append((READ, "attacks", SubdataMember(ref_type=HitType, length="attack_count")))
    data_format.append((READ, "armor_count", "uint16_t"))
    data_format.append((READ, "armors", SubdataMember(ref_type=HitType, length="armor_count")))
    data_format.append((READ_EXPORT, "boundary_id", EnumLookupMember(
            # the damage received by this unit is multiplied by
            # the accessible values on the specified terrain restriction
            raw_type    = "int16_t",
            type_name   = "boundary_ids",
            lookup_dict = {
                -1: "NONE",
                4: "BUILDING",
                6: "DOCK",
                10: "WALL",
            },
        )))
    data_format.append((READ_EXPORT, "weapon_range_max", "float"))
    data_format.append((READ, "blast_range", "float"))
    data_format.append((READ, "attack_speed", "float"))                    # = "reload time"
    data_format.append((READ_EXPORT, "missile_unit_id", "int16_t"))        # which projectile to use?
    data_format.append((READ, "base_hit_chance", "int16_t"))               # probablity of attack hit in percent
    data_format.append((READ, "break_off_combat", "int8_t"))               # = tower mode?
    data_format.append((READ, "fire_missile_at_frame", "int16_t"))         # the frame number at which the missile is fired, = delay
    data_format.append((READ, "weapon_offset", "float[3]"))                # graphics displacement in x, y and z
    data_format.append((READ_EXPORT, "blast_level_offence", EnumLookupMember(
            # blasts damage units that have higher or same blast_defense_level
            raw_type    = "int8_t",
            type_name   = "range_damage_type",
            lookup_dict = {
                0: "RESOURCES",
                1: "TREES",
                2: "NEARBY_UNITS",
                3: "TARGET_ONLY",
                6: "UNKNOWN_6",
            },
        )))
    data_format.append((READ, "weapon_range_min", "float"))                # minimum range that this projectile requests for display
    data_format.append((READ, "accuracy_dispersion", "float"))
    data_format.append((READ_EXPORT, "fight_sprite_id", "int16_t"))
    data_format.append((READ, "melee_armor_displayed", "int16_t"))
    data_format.append((READ, "attack_displayed", "int16_t"))
    data_format.append((READ, "range_displayed", "float"))
    data_format.append((READ, "reload_time_displayed", "float"))


class MissileUnit(ProjectileUnit):
    """
    type_id == 60
    Missile master object
    """

    name_struct        = "missile_unit"
    name_struct_file   = "unit"
    struct_description = "adds missile specific unit properties."

    data_format = []
    data_format.append((READ_EXPORT, None, IncludeMembers(cls=ProjectileUnit)))
    data_format.append((READ, "stretch_mode", "int8_t"))         # 0 = default; 1 = projectile falls vertically to the bottom of the map; 3 = teleporting projectiles
    data_format.append((READ, "smart_mode", "int8_t"))           # "better aiming". tech attribute 19 changes this: 0 = shoot at current pos; 1 = shoot at predicted pos
    data_format.append((READ, "drop_animation_mode", "int8_t"))  # 1 = disappear on hit
    data_format.append((READ, "penetration_mode", "int8_t"))     # 1 = pass through hit object; 0 = stop projectile on hit; (only for graphics, not pass-through damage)
    data_format.append((READ_UNKNOWN, None, "int8_t"))
    data_format.append((READ_EXPORT, "projectile_arc", "float"))


class LivingUnit(ProjectileUnit):
    """
    type_id >= 70
    """

    name_struct        = "living_unit"
    name_struct_file   = "unit"
    struct_description = "adds creation location and garrison unit properties."

    data_format = []
    data_format.append((READ_EXPORT, None, IncludeMembers(cls=ProjectileUnit)))
    data_format.append((READ_EXPORT, "resource_cost", SubdataMember(ref_type=ResourceCost, length=3)))
    data_format.append((READ_EXPORT, "creation_time", "int16_t"))         # in seconds
    data_format.append((READ_EXPORT, "creation_location_id", "int16_t"))  # e.g. 118 = villager

    # where to place the button with the given icon
    # creation page:
    # +------------------------+
    # | 01 | 02 | 03 | 04 | 05 |
    # |----|----|----|----|----|
    # | 06 | 07 | 08 | 09 | 10 |
    # |----|----|----|----|----|
    # | 11 | 12 | 13 | 14 | 15 |
    # +------------------------+
    #
    # additional page (dock):
    # +------------------------+
    # | 21 | 22 | 23 | 24 | 25 |
    # |----|----|----|----|----|
    # | 26 | 27 | 28 | 29 | 30 |
    # |----|----|----|----|----|
    # | 31 | 32 | 33 | 34 | 35 |
    # +------------------------+
    data_format.append((READ, "creation_button_id", "int8_t"))
    data_format.append((READ_UNKNOWN, None, "float"))
    data_format.append((READ_UNKNOWN, None, "float"))
    data_format.append((READ_EXPORT, "creatable_type", EnumLookupMember(
            raw_type    = "int8_t",
            type_name   = "creatable_types",
            lookup_dict = {
                0: "NONHUMAN",  # building, animal, ship
                1: "VILLAGER",  # villager, king
                2: "MELEE",     # soldier, siege, predator, trader
                3: "MOUNTED",   # camel rider
                4: "RELIC",
                5: "RANGED_PROJECTILE",  # archer
                6: "RANGED_MAGIC",       # monk
                21: "TRANSPORT_SHIP",
            },
        )))
    data_format.append((READ, "hero_mode", "int8_t"))                           # if building: "others" tab in editor, if living unit: "heroes" tab, regenerate health + monk immunity
    data_format.append((READ_EXPORT, "garrison_graphic", "int32_t"))            # graphic to display when units are garrisoned
    data_format.append((READ, "attack_projectile_count", "float"))              # projectile count when nothing garrisoned, including both normal and duplicated projectiles
    data_format.append((READ, "attack_projectile_max_count", "int8_t"))         # total projectiles when fully garrisoned
    data_format.append((READ, "attack_projectile_spawning_area_width", "float"))
    data_format.append((READ, "attack_projectile_spawning_area_length", "float"))
    data_format.append((READ, "attack_projectile_spawning_area_randomness", "float"))  # placement randomness, 0=from single spot, 1=random, 1<less random
    data_format.append((READ, "attack_projectile_secondary_unit_id", "int32_t"))   # uses its own attack values
    data_format.append((READ, "special_graphic_id", "int32_t")) # used just before unit reaches its target enemy, configuration:
    data_format.append((READ, "special_activation", "int8_t"))  # determines adjacent unit graphics, if 1: building can adapt graphics by adjacent buildings
                                                                # 0: default: only works when facing the hit angle.
                                                                # 1: block: activates special graphic when receiving damage and not pursuing the attacker.
                                                                # while idle, blocking decreases damage taken by 1/3.
                                                                # also: a wall changes the graphics (when not-an-end piece) because of this.
                                                                # 2: counter charge: activates special graphic when idle and enemy is near.
                                                                # while idle, attacks back once on first received hit.
                                                                # enemy must be unit type 70 and have less than 0.2 max range.
                                                                # 3: charge: activates special graphic when closer than two tiles to the target.
                                                                # deals 2X damage on 1st hit

    data_format.append((READ, "pierce_armor_displayed", "int16_t"))  # unit stats display of pierce armor


class BuildingUnit(LivingUnit):
    """
    type_id >= 80
    """

    name_struct        = "building_unit"
    name_struct_file   = "unit"
    struct_description = "construction graphics and garrison building properties for units."

    data_format = []
    data_format.append((READ_EXPORT, None, IncludeMembers(cls=LivingUnit)))
    data_format.append((READ_EXPORT, "construction_graphic_id", "int16_t"))
    data_format.append((READ, "snow_graphic_id", "int16_t"))
    data_format.append((READ, "adjacent_mode", "int8_t"))            # 1=adjacent units may change the graphics
    data_format.append((READ, "icon_disabler", "int16_t"))
    data_format.append((READ, "disappears_when_built", "int8_t"))
    data_format.append((READ_EXPORT, "stack_unit_id", "int16_t"))    # second building to place directly on top
    data_format.append((READ_EXPORT, "foundation_terrain_id", "int16_t"))  # change underlying terrain to this id when building completed
    data_format.append((READ, "overlay_id", "int16_t"))              # deprecated terrain-like structures knowns as "Overlays" from alpha AOE used for roads
    data_format.append((READ, "research_id", "int16_t"))             # research_id to be enabled when building creation
    data_format.append((READ, "annex_mode", "int8_t"))
    data_format.append((READ_EXPORT, "building_annex", SubdataMember(ref_type=BuildingAnnex, length=4)))
    data_format.append((READ, "head_unit_id", "int16_t"))            # building at which an annex building is attached to
    data_format.append((READ, "transform_unit_id", "int16_t"))       # destination unit id when unit shall transform (e.g. unpack)
    data_format.append((READ_UNKNOWN, None, "int16_t"))              # unit_id for something unknown
    data_format.append((READ, "construction_sound_id", "int16_t"))
    data_format.append((READ_EXPORT, "garrison_type", EnumLookupMember(
            raw_type    = "int8_t",
            type_name   = "garrison_types",
            lookup_dict = {  # TODO: create bitfield
                0x00: "NONE",
                0x01: "VILLAGER",
                0x02: "INFANTRY",
                0x04: "CAVALRY",
                0x08: "MONK",
                0x0b: "NOCAVALRY",
                0x0f: "ALL",
            },
        )))
    data_format.append((READ, "garrison_heal_rate", "float"))
    data_format.append((READ_UNKNOWN, None, "float"))              # (unknown garrison value)
    data_format.append((READ, "salvage_unit_id", "int16_t"))       # id of the unit used for salvages
    data_format.append((READ, "salvage_attributes", "int8_t[6]"))  # list of attributes for salvages


class TreeUnit(UnitObject):
    """
    type_id == 90
    """

    name_struct        = "tree_unit"
    name_struct_file   = "unit"
    struct_description = "just a tree unit."

    data_format = [
        (READ_EXPORT, None, IncludeMembers(cls=UnitObject)),
    ]


# unit type id => human readable name
# used as member name in the resulting struct
unit_type_lookup = {
    10: "object",
    20: "animated",
    25: "doppelganger",
    30: "moving",
    40: "action",
    60: "missile",
    70: "living",
    80: "building",
    90: "tree",
}


# name => attribute class
unit_type_class_lookup = {
    "object":         UnitObject,
    "animated":       AnimatedUnit,
    "doppelganger":   DoppelgangerUnit,
    "moving":         MovingUnit,
    "action":         ActionUnit,
    "missile":        MissileUnit,
    "living":         LivingUnit,
    "building":       BuildingUnit,
    "tree":           TreeUnit,
}
