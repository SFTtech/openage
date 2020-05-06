# Copyright 2013-2019 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from openage.convert.dataformat.version_detect import GameEdition

from ..dataformat.genie_structure import GenieStructure
from ..dataformat.member_access import READ, READ_GEN, SKIP
from ..dataformat.read_members import SubdataMember, EnumLookupMember
from ..dataformat.value_members import MemberTypes as StorageType


class TechResourceCost(GenieStructure):
    name_struct        = "tech_resource_cost"
    name_struct_file   = "research"
    struct_description = "amount definition for a single type resource for researches."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "type_id", StorageType.ID_MEMBER, EnumLookupMember(
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
                    198: "AK_UNUSED_198",
                    199: "AK_UNUSED_199",
                    200: "AK_UNUSED_200",
                    201: "AK_UNUSED_201",
                    202: "AK_UNUSED_202",
                    203: "AK_UNUSED_203",
                    204: "AK_UNUSED_204",
                    205: "AK_FEITORIA_FOOD_PRODUCTIVITY",
                    206: "AK_FEITORIA_WOOD_PRODUCTIVITY",
                    207: "AK_FEITORIA_GOLD_PRODUCTIVITY",
                    208: "AK_FEITORIA_STONE_PRODUCTIVITY",
                    209: "RAJ_REVEAL_ENEMY_TOWN_CENTER",
                    210: "RAJ_REVEAL_RELIC",
                    211: "DE2_UNKNOWN_211",
                    212: "DE2_UNKNOWN_212",
                    213: "DE2_UNKNOWN_213",
                    214: "DE2_UNKNOWN_214",
                    215: "DE2_UNKNOWN_215",
                    216: "DE2_UNKNOWN_216",
                    217: "DE2_UNKNOWN_217",
                    218: "DE2_UNKNOWN_218",
                }
            )),  # see unit/resource_cost
            (READ_GEN, "amount", StorageType.INT_MEMBER, "int16_t"),
            (READ_GEN, "enabled", StorageType.BOOLEAN_MEMBER, "int8_t"),
        ]

        return data_format


class Tech(GenieStructure):
    name_struct        = "tech"
    name_struct_file   = "research"
    struct_description = "one researchable technology."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        if game_version[0] not in (GameEdition.ROR, GameEdition.AOE1DE):
            data_format = [
                # research ids of techs that are required for activating the possible research
                (READ_GEN, "required_techs", StorageType.ARRAY_ID, "int16_t[6]"),
            ]
        else:
            data_format = [
                # research ids of techs that are required for activating the possible research
                (READ_GEN, "required_techs", StorageType.ARRAY_ID, "int16_t[4]"),
            ]

        data_format.extend([
            (READ_GEN, "research_resource_costs", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=TechResourceCost,
                length=3,
            )),
            (READ_GEN, "required_tech_count", StorageType.INT_MEMBER, "int16_t"),       # a subset of the above required techs may be sufficient, this defines the minimum amount
        ])

        if game_version[0] not in (GameEdition.ROR, GameEdition.AOE1DE):
            data_format.extend([
                (READ_GEN, "civilization_id", StorageType.ID_MEMBER, "int16_t"),           # id of the civ that gets this technology
                (READ_GEN, "full_tech_mode", StorageType.BOOLEAN_MEMBER, "int16_t"),       # 1: research is available when the full tech tree is activated on game start, 0: not
            ])

        data_format.extend([
            (READ_GEN, "research_location_id", StorageType.ID_MEMBER, "int16_t"),      # unit id, where the tech will appear to be researched
            (READ_GEN, "language_dll_name", StorageType.ID_MEMBER, "uint16_t"),
            (READ_GEN, "language_dll_description", StorageType.ID_MEMBER, "uint16_t"),
            (READ_GEN, "research_time", StorageType.INT_MEMBER, "int16_t"),            # time in seconds that are needed to finish this research
            (READ_GEN, "tech_effect_id", StorageType.ID_MEMBER, "int16_t"),            # techage id that actually contains the research effect information
            (READ_GEN, "tech_type", StorageType.ID_MEMBER, "int16_t"),                 # 0: normal tech, 2: show in Age progress bar
            (READ_GEN, "icon_id", StorageType.ID_MEMBER, "int16_t"),                   # frame id - 1 in icon slp (57029)
            (READ_GEN, "button_id", StorageType.ID_MEMBER, "int8_t"),                  # button id as defined in the unit.py button matrix
            (READ_GEN, "language_dll_help", StorageType.ID_MEMBER, "int32_t"),         # 100000 + the language file id for the name/description
            (READ_GEN, "language_dll_techtree", StorageType.ID_MEMBER, "int32_t"),     # 149000 + lang_dll_description
            (READ_GEN, "hotkey", StorageType.ID_MEMBER, "int32_t"),                    # -1 for every tech
        ])

        if game_version[0] in (GameEdition.AOE1DE, GameEdition.AOE2DE):
            data_format.extend([
                (SKIP, "name_length_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ, "name_length", StorageType.INT_MEMBER, "uint16_t"),
                (READ_GEN, "name", StorageType.STRING_MEMBER, "char[name_length]"),
            ])

            if game_version[0] is GameEdition.AOE2DE:
                data_format.extend([
                    (READ_GEN, "repeatable", StorageType.INT_MEMBER, "int8_t"),
                ])

        else:
            data_format.extend([
                (READ, "name_length", StorageType.INT_MEMBER, "uint16_t"),
                (READ_GEN, "name", StorageType.STRING_MEMBER, "char[name_length]"),
            ])

            if game_version[0] is GameEdition.SWGB:
                data_format.extend([
                    (READ, "name2_length", StorageType.INT_MEMBER, "uint16_t"),
                    (READ_GEN, "name2", StorageType.STRING_MEMBER, "char[name2_length]"),
                ])

        return data_format
