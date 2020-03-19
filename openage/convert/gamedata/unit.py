# Copyright 2013-2020 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R,too-many-lines

from ..dataformat.genie_structure import GenieStructure
from ..dataformat.member_access import READ, READ_EXPORT
from ..dataformat.value_members import MemberTypes as StorageType
from ..dataformat.read_members import EnumLookupMember, ContinueReadMember, IncludeMembers, SubdataMember


class UnitCommand(GenieStructure):
    """
    also known as "Task" according to ES debug code,
    this structure is the master for spawn-unit actions.
    """
    name_struct = "unit_command"
    name_struct_file = "unit"
    struct_description = "a command a single unit may receive by script or human."

    data_format = [
        # Type (0 = Generic, 1 = Tribe)
        (READ, "command_used", StorageType.INT_MEMBER, "int16_t"),
        (READ_EXPORT, "command_id", StorageType.ID_MEMBER, "int16_t"),
        (READ, "is_default", StorageType.BOOLEAN_MEMBER, "int8_t"),
        (READ_EXPORT, "type", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int16_t",
            type_name="command_ability",
            lookup_dict={
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
        )),
        (READ_EXPORT, "class_id", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "unit_id", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "terrain_id", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "resource_in", StorageType.INT_MEMBER, "int16_t"),        # carry resource
        # resource that multiplies the amount you can gather
        (READ_EXPORT, "resource_multiplier", StorageType.INT_MEMBER, "int16_t"),
        (READ_EXPORT, "resource_out", StorageType.INT_MEMBER, "int16_t"),       # drop resource
        (READ_EXPORT, "unused_resource", StorageType.INT_MEMBER, "int16_t"),
        (READ_EXPORT, "work_value1", StorageType.FLOAT_MEMBER, "float"),        # quantity
        (READ_EXPORT, "work_value2", StorageType.FLOAT_MEMBER, "float"),        # execution radius?
        (READ_EXPORT, "work_range", StorageType.FLOAT_MEMBER, "float"),
        (READ, "search_mode", StorageType.BOOLEAN_MEMBER, "int8_t"),
        (READ, "search_time", StorageType.FLOAT_MEMBER, "float"),
        (READ, "enable_targeting", StorageType.BOOLEAN_MEMBER, "int8_t"),
        (READ, "combat_level_flag", StorageType.ID_MEMBER, "int8_t"),
        (READ, "gather_type", StorageType.INT_MEMBER, "int16_t"),
        (READ, "work_mode2", StorageType.INT_MEMBER, "int16_t"),
        (READ_EXPORT, "owner_type", StorageType.ID_MEMBER, EnumLookupMember(
            # what can be selected as a target for the unit command?
            raw_type="int8_t",
            type_name="selection_type",
            lookup_dict={
                0: "ANY_0",               # select anything
                1: "OWNED_UNITS",         # your own things
                2: "NEUTRAL_ENEMY",       # enemy and neutral things (->attack)
                3: "NOTHING",
                4: "GAIA_OWNED_ALLY",     # any of gaia, owned or allied things
                5: "GAYA_NEUTRAL_ENEMY",  # any of gaia, neutral or enemy things
                6: "NOT_OWNED",           # all things that aren't yours
                7: "ANY_7",
            },
        )),
        # TODO: what does it do? right click?
        (READ, "carry_check", StorageType.BOOLEAN_MEMBER, "int8_t"),
        (READ, "state_build", StorageType.BOOLEAN_MEMBER, "int8_t"),
        # walking with tool but no resource
        (READ_EXPORT, "move_sprite_id", StorageType.ID_MEMBER, "int16_t"),
        # proceeding resource gathering or attack
        (READ_EXPORT, "proceed_sprite_id", StorageType.ID_MEMBER, "int16_t"),
        # actual execution or transformation graphic
        (READ_EXPORT, "work_sprite_id", StorageType.ID_MEMBER, "int16_t"),
        # display resources in hands
        (READ_EXPORT, "carry_sprite_id", StorageType.ID_MEMBER, "int16_t"),
        # sound to play when execution starts
        (READ_EXPORT, "resource_gather_sound_id", StorageType.ID_MEMBER, "int16_t"),
        # sound to play on resource drop
        (READ_EXPORT, "resource_deposit_sound_id", StorageType.ID_MEMBER, "int16_t"),
    ]


class UnitHeader(GenieStructure):
    name_struct = "unit_header"
    name_struct_file = "unit"
    struct_description = "stores a bunch of unit commands."

    data_format = [
        (READ, "exists", StorageType.BOOLEAN_MEMBER, ContinueReadMember("uint8_t")),
        (READ, "unit_command_count", StorageType.INT_MEMBER, "uint16_t"),
        (READ_EXPORT, "unit_commands", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=UnitCommand,
            length="unit_command_count",
        )),
    ]


# Only used in SWGB
class UnitLine(GenieStructure):
    name_struct        = "unit_line"
    name_struct_file   = "unit_lines"
    struct_description = "stores refernces to units in SWGB."

    data_format = [
        (READ, "name_length", StorageType.INT_MEMBER, "uint16_t"),
        (READ, "name", StorageType.STRING_MEMBER, "char[name_length]"),
        (READ, "unit_ids_counter", StorageType.INT_MEMBER, "uint16_t"),
        (READ, "unit_ids", StorageType.ARRAY_ID, "int16_t[unit_ids_counter]"),
    ]


class ResourceStorage(GenieStructure):
    name_struct = "resource_storage"
    name_struct_file = "unit"
    struct_description = "determines the resource storage capacity for one unit mode."

    data_format = [
        (READ, "type", StorageType.ID_MEMBER, "int16_t"),
        (READ, "amount", StorageType.FLOAT_MEMBER, "float"),
        (READ, "used_mode", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int8_t",
            type_name="resource_handling",
            lookup_dict={
                0: "DECAYABLE",
                1: "KEEP_AFTER_DEATH",
                2: "RESET_ON_DEATH_INSTANT",
                4: "RESET_ON_DEATH_WHEN_COMPLETED",
            },
        )),
    ]


class DamageGraphic(GenieStructure):
    name_struct = "damage_graphic"
    name_struct_file = "unit"
    struct_description = "stores one possible unit image that is displayed at a given damage percentage."

    data_format = [
        (READ_EXPORT, "graphic_id", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "damage_percent", StorageType.INT_MEMBER, "int8_t"),
        # gets overwritten in aoe memory by the real apply_mode:
        (READ, "old_apply_mode", StorageType.ID_MEMBER, "int8_t"),
        (READ_EXPORT, "apply_mode", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int8_t",
            type_name="damage_draw_type",
            lookup_dict={
                0: "TOP",      # adds graphics on top (e.g. flames)
                1: "RANDOM",   # adds graphics on top randomly
                2: "REPLACE",  # replace original graphics (e.g. damaged walls)
            },
        )),
    ]


class HitType(GenieStructure):
    name_struct = "hit_type"
    name_struct_file = "unit"
    struct_description = "stores attack amount for a damage type."

    data_format = [
        (READ, "type_id", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int16_t",
            type_name="hit_class",
            lookup_dict={
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
        )),
        (READ, "amount", StorageType.INT_MEMBER, "int16_t"),
    ]


class ResourceCost(GenieStructure):
    name_struct = "resource_cost"
    name_struct_file = "unit"
    struct_description = "stores cost for one resource for creating the unit."

    data_format = [
        (READ, "type_id", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int16_t",
            type_name="resource_types",
            lookup_dict={
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
                # used for resistance against monk conversions
                176: "CONVERT_ADJUSTMENT_MIN",
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
        )),
        (READ, "amount", StorageType.INT_MEMBER, "int16_t"),
        (READ, "enabled", StorageType.BOOLEAN_MEMBER, "int16_t"),
    ]


class BuildingAnnex(GenieStructure):

    name_struct = "building_annex"
    name_struct_file = "unit"
    struct_description = "a possible building annex."

    data_format = [
        (READ_EXPORT, "unit_id", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "misplaced0", StorageType.FLOAT_MEMBER, "float"),
        (READ_EXPORT, "misplaced1", StorageType.FLOAT_MEMBER, "float"),
    ]


class UnitObject(GenieStructure):
    """
    base properties for every unit entry.
    """

    name_struct = "unit_object"
    name_struct_file = "unit"
    struct_description = "base properties for all units."

    data_format = [
        (READ, "name_length", StorageType.INT_MEMBER, "uint16_t"),
        (READ_EXPORT, "id0", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "language_dll_name", StorageType.ID_MEMBER, "uint16_t"),
        (READ_EXPORT, "language_dll_creation", StorageType.ID_MEMBER, "uint16_t"),
        (READ_EXPORT, "unit_class", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int16_t",
            type_name="unit_classes",
            lookup_dict={
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
                16: "TREE_STUMP",
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
                31: "DEEP_SEA_FISH",
                32: "GOLD_MINE",
                33: "SHORE_FISH",
                34: "CLIFF",
                35: "PETARD",
                36: "CAVALRY_ARCHER",
                37: "DOPPELGANGER",
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
                58: "HERDABLE",
                59: "KING",
                61: "HORSE",
            },
        )),
        (READ_EXPORT, "idle_graphic0", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "idle_graphic1", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "dying_graphic", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "undead_graphic", StorageType.ID_MEMBER, "int16_t"),
        # 1 = become `dead_unit_id` (reviving does not make it usable again)
        (READ, "death_mode", StorageType.ID_MEMBER, "int8_t"),
        # unit health. -1=insta-die
        (READ_EXPORT, "hit_points", StorageType.INT_MEMBER, "int16_t"),
        (READ, "line_of_sight", StorageType.FLOAT_MEMBER, "float"),
        # number of units that can garrison in there
        (READ, "garrison_capacity", StorageType.INT_MEMBER, "int8_t"),
        # size of the unit
        (READ_EXPORT, "radius_x", StorageType.FLOAT_MEMBER, "float"),
        (READ_EXPORT, "radius_y", StorageType.FLOAT_MEMBER, "float"),
        (READ_EXPORT, "radius_z", StorageType.FLOAT_MEMBER, "float"),
        (READ_EXPORT, "train_sound_id", StorageType.ID_MEMBER, "int16_t"),
    ]

    # TODO: Enable conversion for AOE1; replace "damage_sound"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.append((READ_EXPORT, "damage_sound", "int16_t"))
    # ===========================================================================
    data_format.append((READ_EXPORT, "damage_sound_id", StorageType.ID_MEMBER, "int16_t"))

    data_format.extend([
        # unit id to become on death
        (READ_EXPORT, "dead_unit_id", StorageType.ID_MEMBER, "int16_t"),
        # 0=placable on top of others in scenario editor, 5=can't
        (READ, "placement_mode", StorageType.ID_MEMBER, "int8_t"),
        (READ, "can_be_built_on", StorageType.BOOLEAN_MEMBER, "int8_t"),  # 1=no footprints
        (READ_EXPORT, "icon_id", StorageType.ID_MEMBER, "int16_t"),      # frame id of the icon slp (57029) to place on the creation button
        (READ, "hidden_in_editor", StorageType.BOOLEAN_MEMBER, "int8_t"),
        (READ, "old_portrait_icon_id", StorageType.ID_MEMBER, "int16_t"),
        # 0=unlocked by research, 1=insta-available
        (READ, "enabled", StorageType.BOOLEAN_MEMBER, "int8_t"),
    ])

    # TODO: Enable conversion for AOE1; replace "disabled"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.append((READ, "disabled", "int8_t"))
    # ===========================================================================
    data_format.append((READ, "disabled", StorageType.BOOLEAN_MEMBER, "int8_t"))

    data_format.extend([
        # terrain id that's needed somewhere on the foundation (e.g. dock
        # water)
        (READ, "placement_side_terrain0", StorageType.ID_MEMBER, "int16_t"),
        (READ, "placement_side_terrain1", StorageType.ID_MEMBER, "int16_t"),    # second slot for ^
        # terrain needed for placement (e.g. dock: water)
        (READ, "placement_terrain0", StorageType.ID_MEMBER, "int16_t"),
        # alternative terrain needed for placement (e.g. dock: shallows)
        (READ, "placement_terrain1", StorageType.ID_MEMBER, "int16_t"),
        # minimum space required to allow placement in editor
        (READ, "clearance_size_x", StorageType.FLOAT_MEMBER, "float"),
        (READ, "clearance_size_y", StorageType.FLOAT_MEMBER, "float"),
        (READ_EXPORT, "building_mode", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int8_t",
            type_name="building_modes",
            lookup_dict={
                0: "NON_BUILDING",    # gates, farms, walls, towers
                2: "TRADE_BUILDING",  # towncenter, port, trade workshop
                3: "ANY",
            },
        )),
        (READ_EXPORT, "visible_in_fog", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int8_t",
            type_name="fog_visibility",
            lookup_dict={
                0: "INVISIBLE",     # people etc
                1: "VISIBLE",       # buildings
                3: "ONLY_IN_FOG",
            },
        )),
        (READ_EXPORT, "terrain_restriction", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int16_t",      # determines on what type of ground the unit can be placed/walk
            type_name="ground_type",  # is actually the id of the terrain_restriction entry!
            lookup_dict={
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
        )),
        # determines whether the unit can fly
        (READ_EXPORT, "fly_mode", StorageType.BOOLEAN_MEMBER, "int8_t"),
        (READ_EXPORT, "resource_capacity", StorageType.INT_MEMBER, "int16_t"),
        # when animals rot, their resources decay
        (READ_EXPORT, "resource_decay", StorageType.FLOAT_MEMBER, "float"),
        (READ_EXPORT, "blast_defense_level", StorageType.ID_MEMBER, EnumLookupMember(
            # receive blast damage from units that have lower or same
            # blast_attack_level.
            raw_type="int8_t",
            type_name="blast_types",
            lookup_dict={
                0: "UNIT_0",    # projectile, dead, fish, relic, tree, gate, towncenter
                1: "OTHER",     # 'other' things with multiple rotations
                2: "BUILDING",  # buildings, gates, walls, towncenter, fishtrap
                3: "UNIT_3",    # boar, farm, fishingship, villager, tradecart, sheep, turkey, archers, junk, ships, monk, siege
            }
        )),
        (READ_EXPORT, "combat_level", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int8_t",
            type_name="combat_levels",
            lookup_dict={
                0: "PROJECTILE_DEAD_RESOURCE",
                1: "BOAR",
                2: "BUILDING",
                3: "CIVILIAN",
                4: "MILITARY",
                5: "OTHER",
            }
        )),
        (READ_EXPORT, "interaction_mode", StorageType.ID_MEMBER, EnumLookupMember(
            # what can be done with this unit?
            raw_type="int8_t",
            type_name="interaction_modes",
            lookup_dict={
                0: "NOTHING_0",
                1: "NOTHING_1",
                2: "SELECTABLE",
                3: "SELECT_ATTACK",
                4: "SELECT_ATTACK_MOVE",
                5: "SELECT_MOVE",
            },
        )),
        (READ_EXPORT, "map_draw_level", StorageType.ID_MEMBER, EnumLookupMember(
            # how does the unit show up on the minimap?
            raw_type="int8_t",
            type_name="minimap_modes",
            lookup_dict={
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
        )),
        (READ_EXPORT, "unit_level", StorageType.ID_MEMBER, EnumLookupMember(
            # selects the available ui command buttons for the unit
            raw_type="int8_t",
            type_name="command_attributes",
            lookup_dict={
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
        )),
        (READ, "attack_reaction", StorageType.FLOAT_MEMBER, "float"),
        # palette color id for the minimap
        (READ_EXPORT, "minimap_color", StorageType.ID_MEMBER, "int8_t"),
        # help text for this unit, stored in the translation dll.
        (READ_EXPORT, "language_dll_help", StorageType.ID_MEMBER, "int32_t"),
        (READ_EXPORT, "language_dll_hotkey_text", StorageType.ID_MEMBER, "int32_t"),
        # language dll dependent (kezb lazouts!)
        (READ, "hot_keys", StorageType.ID_MEMBER, "int32_t"),
        (READ, "reclyclable", StorageType.BOOLEAN_MEMBER, "int8_t"),
        (READ, "enable_auto_gather", StorageType.BOOLEAN_MEMBER, "int8_t"),
        (READ, "doppelgaenger_on_death", StorageType.BOOLEAN_MEMBER, "int8_t"),
        (READ, "resource_gather_drop", StorageType.INT_MEMBER, "int8_t"),
    ])

    # TODO: Enable conversion for AOE1, AOK; replace 6 values below
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     # bit 0 == 1 && val != 7: mask shown behind buildings,
    #     # bit 0 == 0 && val != {6, 10}: no mask displayed,
    #     # val == {-1, 7}: in open area mask is partially displayed
    #     # val == {6, 10}: building, causes mask to appear on units behind it
    #     data_format.extend([
    #         (READ, "occlusion_mask", "int8_t"),
    #         (READ, "obstruction_type", EnumLookupMember(
    #             # selects the available ui command buttons for the unit
    #             raw_type    = "int8_t",
    #             type_name   = "obstruction_types",
    #             lookup_dict = {
    #                 0: "PASSABLE",              # farm, gate, dead bodies, town center
    #                 2: "BUILDING",
    #                 3: "BERSERK",
    #                 5: "UNIT",
    #                 10: "MOUNTAIN",             # mountain (matches occlusion_mask)
    #             },
    #         )),
    #         # There shouldn't be a value here according to genieutils
    #         # What is this?
    #         (READ_EXPORT, "selection_shape", "int8_t"),            # 0=square, 1<=round
    #     ])
    #
    #     if GameVersion-age2_aok not in game_versions:
    #         # bitfield of unit attributes:
    #         # bit 0: allow garrison,
    #         # bit 1: don't join formation,
    #         # bit 2: stealth unit,
    #         # bit 3: detector unit,
    #         # bit 4: mechanical unit,
    #         # bit 5: biological unit,
    #         # bit 6: self-shielding unit,
    #         # bit 7: invisible unit
    #         data_format.extend([
    #             (READ, "trait", "uint8_t"),
    #             (READ, "civilisation", "int8_t"),
    #             (READ, "attribute_piece", "int16_t"),    # leftover from trait+civ variable
    #         ])
    # ===========================================================================

    # bit 0 == 1 && val != 7: mask shown behind buildings,
    # bit 0 == 0 && val != {6, 10}: no mask displayed,
    # val == {-1, 7}: in open area mask is partially displayed
    # val == {6, 10}: building, causes mask to appear on units behind it
    data_format.extend([
        (READ, "occlusion_mask", StorageType.ID_MEMBER, "int8_t"),
        (READ, "obstruction_type", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int8_t",
            type_name="obstruction_types",
            lookup_dict={
                0: "PASSABLE",              # farm, gate, dead bodies, town center
                2: "BUILDING",
                3: "BERSERK",
                5: "UNIT",
                10: "MOUNTAIN",             # mountain (matches occlusion_mask)
            },
        )),
        # There shouldn't be a value here according to genieutils
        # What is this?
        (READ_EXPORT, "obstruction_class", StorageType.ID_MEMBER, "int8_t"),

        # bitfield of unit attributes:
        # bit 0: allow garrison,
        # bit 1: don't join formation,
        # bit 2: stealth unit,
        # bit 3: detector unit,
        # bit 4: mechanical unit,
        # bit 5: biological unit,
        # bit 6: self-shielding unit,
        # bit 7: invisible unit
        (READ, "trait", StorageType.ID_MEMBER, "uint8_t"),
        (READ, "civilisation", StorageType.ID_MEMBER, "int8_t"),
        # leftover from trait+civ variable
        (READ, "attribute_piece", StorageType.INT_MEMBER, "int16_t"),
    ])
    # ===========================================================================

    data_format.extend([
        (READ_EXPORT, "selection_effect", StorageType.ID_MEMBER, EnumLookupMember(
            # things that happen when the unit was selected
            raw_type="int8_t",
            type_name="selection_effects",
            lookup_dict={
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
        )),
        # 0: default, -16: fish trap, farm, 52: deadfarm, OLD-*, 116: flare,
        # whale, dolphin -123: fish
        (READ, "editor_selection_color", StorageType.ID_MEMBER, "uint8_t"),
        (READ_EXPORT, "selection_shape_x", StorageType.FLOAT_MEMBER, "float"),
        (READ_EXPORT, "selection_shape_y", StorageType.FLOAT_MEMBER, "float"),
        (READ_EXPORT, "selection_shape_z", StorageType.FLOAT_MEMBER, "float"),
        (READ_EXPORT, "resource_storage", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=ResourceStorage,
            length=3,
        )),
        (READ, "damage_graphic_count", StorageType.INT_MEMBER, "int8_t"),
        (READ_EXPORT, "damage_graphic", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=DamageGraphic,
            length="damage_graphic_count",
        )),
        (READ_EXPORT, "selection_sound_id", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "dying_sound_id", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "old_attack_mode", StorageType.ID_MEMBER, EnumLookupMember(  # obsolete, as it's copied when converting the unit
            raw_type="int8_t",     # things that happen when the unit was selected
            type_name="attack_modes",
            lookup_dict={
                0: "NO",         # no attack
                1: "FOLLOWING",  # by following
                2: "RUN",        # run when attacked
                3: "UNKNOWN3",
                4: "ATTACK",
            },
        )),

        (READ, "convert_terrain", StorageType.INT_MEMBER, "int8_t"),
        (READ_EXPORT, "name", StorageType.STRING_MEMBER, "char[name_length]"),
    ])

    # TODO: Enable conversion for SWGB
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.extend([(READ, "name2_length", "uint16_t"),
    #                         (READ, "name2", "char[name2_length]"),
    #                         (READ, "unit_line", "int16_t"),
    #                         (READ, "min_tech_level", "int8_t"),
    #     ])
    # ===========================================================================

    data_format.append((READ_EXPORT, "id1", StorageType.ID_MEMBER, "int16_t"))

    # TODO: Enable conversion for AOE1; replace "id2"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.append((READ_EXPORT, "id2", "int16_t"))
    # ===========================================================================
    data_format.append((READ_EXPORT, "id2", StorageType.ID_MEMBER, "int16_t"))


class TreeUnit(UnitObject):
    """
    type_id == 90
    """

    name_struct = "tree_unit"
    name_struct_file = "unit"
    struct_description = "just a tree unit."

    data_format = [
        (READ_EXPORT, None, None, IncludeMembers(cls=UnitObject)),
    ]


class AnimatedUnit(UnitObject):
    """
    type_id >= 20
    Animated master object
    """

    name_struct = "animated_unit"
    name_struct_file = "unit"
    struct_description = "adds speed property to units."

    data_format = [
        (READ_EXPORT, None, None, IncludeMembers(cls=UnitObject)),
        (READ_EXPORT, "speed", StorageType.FLOAT_MEMBER, "float"),
    ]


class DoppelgangerUnit(AnimatedUnit):
    """
    type_id >= 25
    """

    name_struct = "doppelganger_unit"
    name_struct_file = "unit"
    struct_description = "weird doppelganger unit thats actually the same as an animated unit."

    data_format = [
        (READ_EXPORT, None, None, IncludeMembers(cls=AnimatedUnit)),
    ]


class MovingUnit(DoppelgangerUnit):
    """
    type_id >= 30
    Moving master object
    """

    name_struct = "moving_unit"
    name_struct_file = "unit"
    struct_description = "adds walking graphics, rotations and tracking properties to units."

    data_format = [
        (READ_EXPORT, None, None, IncludeMembers(cls=DoppelgangerUnit)),
        (READ_EXPORT, "move_graphics", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "run_graphics", StorageType.ID_MEMBER, "int16_t"),
        (READ, "turn_speed", StorageType.FLOAT_MEMBER, "float"),
        (READ, "old_size_class", StorageType.ID_MEMBER, "int8_t"),
        # unit id for the ground traces
        (READ, "trail_unit_id", StorageType.ID_MEMBER, "int16_t"),
        # ground traces: -1: no tracking present, 2: projectiles with tracking unit
        (READ, "trail_opsions", StorageType.ID_MEMBER, "uint8_t"),
        # ground trace spacing: 0: no tracking, 0.5: trade cart, 0.12: some
        # projectiles, 0.4: other projectiles
        (READ, "trail_spacing", StorageType.FLOAT_MEMBER, "float"),
        (READ, "old_move_algorithm", StorageType.ID_MEMBER, "int8_t"),
    ]

    # TODO: Enable conversion for AOE1; replace 5 values below
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.extend([
    #         (READ, "turn_radius", "float"),
    #         (READ, "turn_radius_speed", "float"),
    #         (READ, "max_yaw_per_sec_moving", "float"),
    #         (READ, "stationary_yaw_revolution_time", "float"),
    #         (READ, "max_yaw_per_sec_stationary", "float"),
    #     ])
    # ===========================================================================
    data_format.extend([
        (READ, "turn_radius", StorageType.FLOAT_MEMBER, "float"),
        (READ, "turn_radius_speed", StorageType.FLOAT_MEMBER, "float"),
        (READ, "max_yaw_per_sec_moving", StorageType.FLOAT_MEMBER, "float"),
        (READ, "stationary_yaw_revolution_time", StorageType.FLOAT_MEMBER, "float"),
        (READ, "max_yaw_per_sec_stationary", StorageType.FLOAT_MEMBER, "float"),
    ])


class ActionUnit(MovingUnit):
    """
    type_id >= 40
    Action master object
    """

    name_struct = "action_unit"
    name_struct_file = "unit"
    struct_description = "adds search radius and work properties, as well as movement sounds."

    data_format = [
        (READ_EXPORT, None, None, IncludeMembers(cls=MovingUnit)),
        # callback unit action id when found.
        # monument and sheep: 107 = enemy convert.
        # all auto-convertible units: 0, most other units: -1
        # e.g. when sheep are discovered
        (READ, "default_task_id", StorageType.ID_MEMBER, "int16_t"),
        (READ, "search_radius", StorageType.FLOAT_MEMBER, "float"),
        (READ_EXPORT, "work_rate", StorageType.FLOAT_MEMBER, "float"),
        # unit id where gathered resources shall be delivered to
        (READ_EXPORT, "drop_site0", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "drop_site1", StorageType.ID_MEMBER, "int16_t"),  # alternative unit id
        # if a task is not found in the current unit, other units with the same
        # task group are tried.
        (READ_EXPORT, "task_group", StorageType.ID_MEMBER, "int8_t"),   # 1: male villager; 2: female villager; 3+: free slots
                                                                        # basically this
                                                                        # creates a "swap
                                                                        # group id" where you
                                                                        # can place
                                                                        # different-graphic
                                                                        # units together.
        # sound played when a command is instanciated
        (READ_EXPORT, "command_sound_id", StorageType.ID_MEMBER, "int16_t"),
        # sound when the command is done (e.g. unit stops at target position)
        (READ_EXPORT, "stop_sound_id", StorageType.ID_MEMBER, "int16_t"),
        # how animals run around randomly
        (READ, "run_pattern", StorageType.ID_MEMBER, "int8_t"),
    ]

    # TODO: Enable conversion for AOE1
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) in game_versions:
    #     data_format.extend([
    #         (READ_EXPORT, "unit_count", "uint16_t"),
    #         (READ_EXPORT, "unit_commands", SubdataMember(
    #             ref_type=UnitCommand,
    #             length="unit_count",
    #         )),
    #     ])
    # ===========================================================================


class ProjectileUnit(ActionUnit):
    """
    type_id >= 60
    Projectile master object
    """

    name_struct = "projectile_unit"
    name_struct_file = "unit"
    struct_description = "adds attack and armor properties to units."

    data_format = [
        (READ_EXPORT, None, None, IncludeMembers(cls=ActionUnit)),
    ]

    # TODO: Enable conversion for AOE1; replace "default_armor"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror or GameVersion.age2_aok) in game_versions:
    #     data_format.append((READ, "default_armor", "uint8_t"))
    # else:
    #     data_format.append((READ, "default_armor", "int16_t"))
    # ===========================================================================
    data_format.append((READ, "default_armor", StorageType.ID_MEMBER, "int16_t"))

    data_format.extend([
        (READ, "attack_count", StorageType.INT_MEMBER, "uint16_t"),
        (READ, "attacks", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=HitType,
            length="attack_count",
        )),
        (READ, "armor_count", StorageType.INT_MEMBER, "uint16_t"),
        (READ, "armors", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=HitType,
            length="armor_count",
        )),
        (READ_EXPORT, "boundary_id", StorageType.ID_MEMBER, EnumLookupMember(
            # the damage received by this unit is multiplied by
            # the accessible values on the specified terrain restriction
            raw_type="int16_t",
            type_name="boundary_ids",
            lookup_dict={
                -1: "NONE",
                4: "BUILDING",
                6: "DOCK",
                10: "WALL",
            },
        )),
        (READ_EXPORT, "weapon_range_max", StorageType.FLOAT_MEMBER, "float"),
        (READ, "blast_range", StorageType.FLOAT_MEMBER, "float"),
        (READ, "attack_speed", StorageType.FLOAT_MEMBER, "float"),  # = "reload time"
        # which projectile to use?
        (READ_EXPORT, "attack_projectile_primary_unit_id", StorageType.ID_MEMBER, "int16_t"),
        # probablity of attack hit in percent
        (READ, "accuracy", StorageType.INT_MEMBER, "int16_t"),
        # = tower mode?; not used anywhere
        (READ, "break_off_combat", StorageType.INT_MEMBER, "int8_t"),
        # the frame number at which the missile is fired, = delay
        (READ, "frame_delay", StorageType.INT_MEMBER, "int16_t"),
        # graphics displacement in x, y and z
        (READ, "weapon_offset", StorageType.ARRAY_FLOAT, "float[3]"),
        (READ_EXPORT, "blast_level_offence", StorageType.ID_MEMBER, EnumLookupMember(
            # blasts damage units that have higher or same blast_defense_level
            raw_type="int8_t",
            type_name="range_damage_type",
            lookup_dict={
                0: "RESOURCES",
                1: "TREES",
                2: "NEARBY_UNITS",
                3: "TARGET_ONLY",
                6: "UNKNOWN_6",
            },
        )),
        # minimum range that this projectile requests for display
        (READ, "weapon_range_min", StorageType.FLOAT_MEMBER, "float"),
    ])

    # TODO: Enable conversion for AOE1; replace "accuracy_dispersion"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.append((READ, "accuracy_dispersion", "float"))
    # ===========================================================================
    data_format.append((READ, "accuracy_dispersion", StorageType.FLOAT_MEMBER, "float"))

    data_format.extend([
        (READ_EXPORT, "attack_sprite_id", StorageType.ID_MEMBER, "int16_t"),
        (READ, "melee_armor_displayed", StorageType.INT_MEMBER, "int16_t"),
        (READ, "attack_displayed", StorageType.INT_MEMBER, "int16_t"),
        (READ, "range_displayed", StorageType.FLOAT_MEMBER, "float"),
        (READ, "reload_time_displayed", StorageType.FLOAT_MEMBER, "float"),
    ])


class MissileUnit(ProjectileUnit):
    """
    type_id == 60
    Missile master object
    """

    name_struct = "missile_unit"
    name_struct_file = "unit"
    struct_description = "adds missile specific unit properties."

    data_format = [
        (READ_EXPORT, None, None, IncludeMembers(cls=ProjectileUnit)),
        # 0 = default; 1 = projectile falls vertically to the bottom of the
        # map; 3 = teleporting projectiles
        (READ, "projectile_type", StorageType.ID_MEMBER, "int8_t"),
        # "better aiming". tech attribute 19 changes this: 0 = shoot at current pos; 1 = shoot at predicted pos
        (READ, "smart_mode", StorageType.BOOLEAN_MEMBER, "int8_t"),
        (READ, "drop_animation_mode", StorageType.ID_MEMBER, "int8_t"),  # 1 = disappear on hit
        # 1 = pass through hit object; 0 = stop projectile on hit; (only for
        # graphics, not pass-through damage)
        (READ, "penetration_mode", StorageType.ID_MEMBER, "int8_t"),
        (READ, "area_of_effect_special", StorageType.INT_MEMBER, "int8_t"),
        (READ_EXPORT, "projectile_arc", StorageType.FLOAT_MEMBER, "float"),
    ]


class LivingUnit(ProjectileUnit):
    """
    type_id >= 70
    """

    name_struct = "living_unit"
    name_struct_file = "unit"
    struct_description = "adds creation location and garrison unit properties."

    data_format = [
        (READ_EXPORT, None, None, IncludeMembers(cls=ProjectileUnit)),
        (READ_EXPORT, "resource_cost", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=ResourceCost,
            length=3,
        )),
        (READ_EXPORT, "creation_time", StorageType.INT_MEMBER, "int16_t"),     # in seconds
        (READ_EXPORT, "train_location_id", StorageType.ID_MEMBER, "int16_t"),  # e.g. 118 = villager builder

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
        (READ, "creation_button_id", StorageType.ID_MEMBER, "int8_t"),
    ]

    # TODO: Enable conversion for AOE1; replace 13 values below
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.extend([(
    #         (READ, "rear_attack_modifier", "float"),
    #         (READ, "flank_attack_modifier", "float"),
    #         (READ_EXPORT, "creatable_type", EnumLookupMember(
    #             raw_type    = "int8_t",
    #             type_name   = "creatable_types",
    #             lookup_dict = {
    #                 0: "NONHUMAN",  # building, animal, ship
    #                 1: "VILLAGER",  # villager, king
    #                 2: "MELEE",     # soldier, siege, predator, trader
    #                 3: "MOUNTED",   # camel rider
    #                 4: "RELIC",
    #                 5: "RANGED_PROJECTILE",  # archer
    #                 6: "RANGED_MAGIC",       # monk
    #                 21: "TRANSPORT_SHIP",
    #             },
    #         )),
    #         (READ, "hero_mode", "int8_t"),                           # if building: "others" tab in editor, if living unit: "heroes" tab, regenerate health + monk immunity
    #         (READ_EXPORT, "garrison_graphic", "int32_t"),            # graphic to display when units are garrisoned
    #         (READ, "attack_projectile_count", "float"),              # projectile count when nothing garrisoned, including both normal and duplicated projectiles
    #         (READ, "attack_projectile_max_count", "int8_t"),         # total projectiles when fully garrisoned
    #         (READ, "attack_projectile_spawning_area_width", "float"),
    #         (READ, "attack_projectile_spawning_area_length", "float"),
    #         (READ, "attack_projectile_spawning_area_randomness", "float"),  # placement randomness, 0=from single spot, 1=random, 1<less random
    #         (READ, "attack_projectile_secondary_unit_id", "int32_t"),       # uses its own attack values
    #         (READ, "special_graphic_id", "int32_t"),  # used just before unit reaches its target enemy, configuration:
    #         (READ, "special_activation", "int8_t"),  # determines adjacent unit graphics, if 1: building can adapt graphics by adjacent buildings
    #                                                 # 0: default: only works when facing the hit angle.
    #                                                 # 1: block: activates special graphic when receiving damage and not pursuing the attacker.
    #                                                 # while idle, blocking decreases damage taken by 1/3.
    #                                                 # also: a wall changes the graphics (when not-an-end piece) because of this.
    #                                                 # 2: counter charge: activates special graphic when idle and enemy is near.
    #                                                 # while idle, attacks back once on first received hit.
    #                                                 # enemy must be unit type 70 and have less than 0.2 max range.
    #                                                 # 3: charge: activates special graphic when closer than two tiles to the target.
    #                                                 # deals 2X damage on 1st hit
    #     ])
    # ===========================================================================
    data_format.extend([
        (READ, "rear_attack_modifier", StorageType.FLOAT_MEMBER, "float"),
        (READ, "flank_attack_modifier", StorageType.FLOAT_MEMBER, "float"),
        (READ_EXPORT, "creatable_type", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int8_t",
            type_name="creatable_types",
            lookup_dict={
                0: "NONHUMAN",  # building, animal, ship
                1: "VILLAGER",  # villager, king
                2: "MELEE",     # soldier, siege, predator, trader
                3: "MOUNTED",   # camel rider
                4: "RELIC",
                5: "RANGED_PROJECTILE",  # archer
                6: "RANGED_MAGIC",       # monk
                21: "TRANSPORT_SHIP",
            },
        )),
        # if building: "others" tab in editor, if living unit: "heroes" tab,
        # regenerate health + monk immunity
        (READ, "hero_mode", StorageType.BOOLEAN_MEMBER, "int8_t"),
        # graphic to display when units are garrisoned
        (READ_EXPORT, "garrison_graphic", StorageType.ID_MEMBER, "int32_t"),
        # projectile count when nothing garrisoned, including both normal and
        # duplicated projectiles
        (READ, "attack_projectile_count", StorageType.INT_MEMBER, "float"),
        # total projectiles when fully garrisoned
        (READ, "attack_projectile_max_count", StorageType.INT_MEMBER, "int8_t"),
        (READ, "attack_projectile_spawning_area_width", StorageType.FLOAT_MEMBER, "float"),
        (READ, "attack_projectile_spawning_area_length", StorageType.FLOAT_MEMBER, "float"),
        # placement randomness, 0=from single spot, 1=random, 1<less random
        (READ, "attack_projectile_spawning_area_randomness", StorageType.FLOAT_MEMBER, "float"),
        # uses its own attack values
        (READ, "attack_projectile_secondary_unit_id", StorageType.ID_MEMBER, "int32_t"),
        # used just before unit reaches its target enemy, configuration:
        (READ, "special_graphic_id", StorageType.ID_MEMBER, "int32_t"),
        # determines adjacent unit graphics, if 1: building can adapt graphics
        # by adjacent buildings
        (READ, "special_activation", StorageType.ID_MEMBER, "int8_t"),
        # 0: default: only works when facing the hit angle.
        # 1: block: activates special graphic when receiving damage and not pursuing the attacker.
        # while idle, blocking decreases damage taken by 1/3.
        # also: a wall changes the graphics (when not-an-end piece) because of this.
        # 2: counter charge: activates special graphic when idle and enemy is near.
        # while idle, attacks back once on first received hit.
        # enemy must be unit type 70 and have less than 0.2 max range.
        # 3: charge: activates special graphic when closer than two tiles to the target.
        # deals 2X damage on 1st
        # hit
    ])
    # ===========================================================================

    # unit stats display of pierce armor
    data_format.append((READ, "pierce_armor_displayed", StorageType.INT_MEMBER, "int16_t"))


class BuildingUnit(LivingUnit):
    """
    type_id >= 80
    """

    name_struct = "building_unit"
    name_struct_file = "unit"
    struct_description = "construction graphics and garrison building properties for units."

    data_format = [
        (READ_EXPORT, None, None, IncludeMembers(cls=LivingUnit)),
        (READ_EXPORT, "construction_graphic_id", StorageType.ID_MEMBER, "int16_t"),
    ]

    # TODO: Enable conversion for AOE1; replace "snow_graphic_id"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror or age2_aok) not in game_versions:
    #     data_format.append((READ, "snow_graphic_id", "int16_t"))
    # ===========================================================================
    data_format.append((READ, "snow_graphic_id", StorageType.ID_MEMBER, "int16_t"))

    data_format.extend([
        # 1=adjacent units may change the graphics
        (READ, "adjacent_mode", StorageType.BOOLEAN_MEMBER, "int8_t"),
        (READ, "graphics_angle", StorageType.INT_MEMBER, "int16_t"),
        (READ, "disappears_when_built", StorageType.BOOLEAN_MEMBER, "int8_t"),
        # second building to place directly on top
        (READ_EXPORT, "stack_unit_id", StorageType.ID_MEMBER, "int16_t"),
        # change underlying terrain to this id when building completed
        (READ_EXPORT, "foundation_terrain_id", StorageType.ID_MEMBER, "int16_t"),
        # deprecated terrain-like structures knowns as "Overlays" from alpha
        # AOE used for roads
        (READ, "old_overlay_id", StorageType.ID_MEMBER, "int16_t"),
        # research_id to be enabled when building creation
        (READ, "research_id", StorageType.ID_MEMBER, "int16_t"),
    ])

    # TODO: Enable conversion for AOE1; replace 5 values below
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.extend([
    #         (READ, "can_burn", "int8_t"),
    #         (READ_EXPORT, "building_annex", SubdataMember(
    #             ref_type=BuildingAnnex,
    #             length=4
    #         )),
    #         (READ, "head_unit_id", "int16_t"),            # building at which an annex building is attached to
    #         (READ, "transform_unit_id", "int16_t"),       # destination unit id when unit shall transform (e.g. unpack)
    #         (READ, "transform_sound_id", "int16_t"),
    #     ])
    # ===========================================================================
    data_format.extend([
        (READ, "can_burn", StorageType.BOOLEAN_MEMBER, "int8_t"),
        (READ_EXPORT, "building_annex", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=BuildingAnnex,
            length=4
        )),
        # building at which an annex building is attached to
        (READ, "head_unit_id", StorageType.ID_MEMBER, "int16_t"),
        # destination unit id when unit shall transform (e.g. unpack)
        (READ, "transform_unit_id", StorageType.ID_MEMBER, "int16_t"),
        (READ, "transform_sound_id", StorageType.ID_MEMBER, "int16_t"),
    ])
    # ===========================================================================

    data_format.append((READ, "construction_sound_id", StorageType.ID_MEMBER, "int16_t"))

    # TODO: Enable conversion for AOE1; replace 5 values below
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.extend([
    #         (READ_EXPORT, "garrison_type", EnumLookupMember(
    #             raw_type    = "int8_t",
    #             type_name   = "garrison_types",
    #             lookup_dict = {  # TODO: create bitfield
    #                 0x00: "NONE",
    #                 0x01: "VILLAGER",
    #                 0x02: "INFANTRY",
    #                 0x04: "CAVALRY",
    #                 0x08: "MONK",
    #                 0x0b: "NOCAVALRY",
    #                 0x0f: "ALL",
    #             },
    #         )),
    #         (READ, "garrison_heal_rate", "float"),
    #         (READ, "garrison_repair_rate", "float"),
    #         (READ, "salvage_unit_id", "int16_t"),       # id of the unit used for salvages
    #         (READ, "salvage_attributes", "int8_t[6]"),  # list of attributes for salvages (looting table)
    #     ])
    # ===========================================================================
    data_format.extend([
        (READ_EXPORT, "garrison_type", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int8_t",
            type_name="garrison_types",
            lookup_dict={  # TODO: create bitfield
                0x00: "NONE",
                0x01: "VILLAGER",
                0x02: "INFANTRY",
                0x04: "CAVALRY",
                0x08: "MONK",
                0x0b: "NOCAVALRY",
                0x0f: "ALL",
            },
        )),
        (READ, "garrison_heal_rate", StorageType.FLOAT_MEMBER, "float"),
        (READ, "garrison_repair_rate", StorageType.FLOAT_MEMBER, "float"),
        # id of the unit used for salvages
        (READ, "salvage_unit_id", StorageType.ID_MEMBER, "int16_t"),
        # list of attributes for salvages (looting table)
        (READ, "salvage_attributes", StorageType.ARRAY_INT, "int8_t[6]"),
    ])
    # ===========================================================================


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
