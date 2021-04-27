# Copyright 2021-2021 the openage authors. See copying.md for legal info.

"""
Lookup dicts for the EnumLookupMember instances.
"""

GRAPHICS_LAYER = {
    0: "TERRAIN",      # cliff
    1: "GRASS_PATCH",
    2: "DE2_CLIFF",
    3: "AOE1_DIRT",
    4: "DE1_DESTRUCTION",
    5: "SHADOW",       # farm fields as well
    6: "RUBBLE",
    7: "PLANT",
    9: "SWGB_EFFECT",
    10: "UNIT_LOW",    # constructions, dead units, tree stumps, flowers, paths
    11: "FISH",
    18: "SWGB_LAYER1",
    19: "CRATER",      # rugs
    20: "UNIT",        # buildings, units, damage flames, animations (mill)
    21: "BLACKSMITH",  # blacksmith smoke
    22: "BIRD",        # hawk
    30: "PROJECTILE",  # and explosions
    31: "SWGB_FLYING",
}

RESOURCE_TYPES = {
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
    219: "DE2_UNKNOWN_219",
    220: "RELIC_FOOD_PRODUCTION_RATE",
    221: "DE2_UNKNOWN_221",
    222: "DE2_UNKNOWN_222",
    223: "DE2_UNKNOWN_223",
    224: "DE2_UNKNOWN_224",
    225: "DE2_UNKNOWN_225",
    226: "DE2_UNKNOWN_226",
    227: "DE2_UNKNOWN_227",
    228: "DE2_UNKNOWN_228",
    229: "DE2_UNKNOWN_229",
    230: "DE2_UNKNOWN_230",
    231: "DE2_UNKNOWN_231",
    232: "DE2_UNKNOWN_232",
    233: "DE2_UNKNOWN_233",
    234: "FIRST_CRUSADE",
    235: "DE2_UNKNOWN_235",
    236: "BURGUNDIAN_VINEYARDS",
    237: "DE2_UNKNOWN_237",
    238: "DE2_UNKNOWN_238",
    239: "DE2_UNKNOWN_239",
    240: "DE2_UNKNOWN_240",
    241: "DE2_UNKNOWN_241",
    242: "DE2_UNKNOWN_242",
    243: "DE2_UNKNOWN_243",
    244: "DE2_UNKNOWN_244",
    245: "DE2_UNKNOWN_245",
    246: "DE2_UNKNOWN_246",
    247: "DE2_UNKNOWN_247",
    248: "DE2_UNKNOWN_248",
    249: "DE2_UNKNOWN_249",
    250: "DE2_UNKNOWN_250",
}

EFFECT_APPLY_TYPE = {
    # unused assignage: a = -1, b = -1, c = -1, d = 0
    -1: "DISABLED",
    # if a != -1: a == unit_id, else b == unit_class_id; c =
    # attribute_id, d = new_value
    0: "ATTRIBUTE_ABSSET",
    # a == resource_id, if b == 0: then d = absval, else d = relval
    # (for inc/dec)
    1: "RESOURCE_MODIFY",
    # a == unit_id, if b == 0: disable unit, else b == 1: enable
    # unit
    2: "UNIT_ENABLED",
    # a == old_unit_id, b == new_unit_id
    3: "UNIT_UPGRADE",
    # if a != -1: unit_id, else b == unit_class_id; c=attribute_id,
    # d=relval
    4: "ATTRIBUTE_RELSET",
    # if a != -1: unit_id, else b == unit_class_id; c=attribute_id,
    # d=factor
    5: "ATTRIBUTE_MUL",
    # a == resource_id, d == factor
    6: "RESOURCE_MUL",

    # a == unit_id, b == building_id, c == amount
    7: "SPAWN_UNIT",

    # same as 0-6 but applied to team members
    10: "TEAM_ATTRIBUTE_ABSSET",
    11: "TEAM_RESOURCE_MODIFY",
    12: "TEAM_UNIT_ENABLED",
    13: "TEAM_UNIT_UPGRADE",
    14: "TEAM_ATTRIBUTE_RELSET",
    15: "TEAM_ATTRIBUTE_MUL",
    16: "TEAM_RESOURCE_MUL",

    # same as 0-6 but applied to enemies
    20: "ENEMY_ATTRIBUTE_ABSSET",
    21: "ENEMY_RESOURCE_MODIFY",
    22: "ENEMY_UNIT_ENABLED",
    23: "ENEMY_UNIT_UPGRADE",
    24: "ENEMY_ATTRIBUTE_RELSET",
    25: "ENEMY_ATTRIBUTE_MUL",
    26: "ENEMY_RESOURCE_MUL",

    # these are only used in technology trees, 103 even requires
    # one
    # a == research_id, b == resource_id, if c == 0: d==absval
    # else: d == relval
    101: "TECHCOST_MODIFY",
    102: "TECH_TOGGLE",       # d == research_id
    103: "TECH_TIME_MODIFY",  # a == research_id, if c == 0: d==absval else d==relval

    # attribute_id:
    # 0: hit points
    # 1: line of sight
    # 2: garrison capacity
    # 3: unit size x
    # 4: unit size y
    # 5: movement speed
    # 6: rotation speed
    # 7: unknown
    # 8: armor                           # real_val = val + (256 * armor_id)
    # 9: attack                          # real_val = val + (256 * attack_id)
    # 10: attack reloading time
    # 11: accuracy percent
    # 12: max range
    # 13: working rate
    # 14: resource carriage
    # 15: default armor
    # 16: projectile unit
    # 17: upgrade graphic (icon), graphics angle
    # 18: terrain restriction to multiply damage received (always sets)
    # 19: intelligent projectile aim 1=on, 0=off
    # 20: minimum range
    # 21: first resource storage
    # 22: blast width (area damage)
    # 23: search radius
    # 80: boarding energy reload speed
    # 100: resource cost
    # 101: creation time
    # 102: number of garrison arrows
    # 103: food cost
    # 104: wood cost
    # 105: stone cost
    # 106: gold cost
    # 107: max total projectiles
    # 108: garrison healing rate
    # 109: regeneration rate
}

CONNECTION_MODE = {
    0: "AGE",
    1: "BUILDING",
    2: "UNIT",
    3: "RESEARCH",
}

COMMAND_ABILITY = {
    # the Action-Types found under RGE namespace:
    -1: "SWGB_UNUSED",
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
}

OWNER_TYPE = {
    0: "ANY_0",               # select anything
    1: "OWNED_UNITS",         # your own things
    2: "NEUTRAL_ENEMY",       # enemy and neutral things (->attack)
    3: "NOTHING",
    4: "GAIA_OWNED_ALLY",     # any of gaia, owned or allied things
    5: "GAYA_NEUTRAL_ENEMY",  # any of gaia, neutral or enemy things
    6: "NOT_OWNED",           # all things that aren't yours
    7: "ANY_7",
}

RESOURCE_HANDLING = {
    0: "DECAYABLE",
    1: "KEEP_AFTER_DEATH",
    2: "RESET_ON_DEATH_INSTANT",
    4: "RESET_ON_DEATH_WHEN_COMPLETED",
    8: "DE2_UNKNOWN_8",
    32: "DE2_UNKNOWN_32",
}

DAMAGE_DRAW_TYPE = {
    0: "TOP",      # adds graphics on top (e.g. flames)
    1: "RANDOM",   # adds graphics on top randomly
    2: "REPLACE",  # replace original graphics (e.g. damaged walls)
}

ARMOR_CLASS = {
    -1: "NONE",
    0: "UNKNOWN_0",
    1: "INFANTRY",
    2: "SHIP_TURTLE",
    3: "UNITS_PIERCE",
    4: "UNITS_MELEE",
    5: "WAR_ELEPHANT",
    6: "SWGB_ASSAULT_MECHANIC",
    7: "SWGB_DECIMATOR",
    8: "CAVALRY",
    9: "SWGB_SHIPS",
    10: "SWGB_SUBMARINE",
    11: "BUILDINGS_NO_PORT",
    12: "AOE1_VILLAGER",
    13: "STONE_DEFENSES",
    14: "HD_PREDATOR",
    15: "ARCHERS",
    16: "SHIPS_CAMELS_SABOTEURS",
    17: "RAMS",
    18: "TREES",
    19: "UNIQUE_UNITS",
    20: "SIEGE_WEAPONS",
    21: "BUILDINGS",
    22: "WALLS_GATES",
    23: "HD_GUNPOWDER",
    24: "BOAR",
    25: "MONKS",
    26: "CASTLE",
    27: "SPEARMEN",
    28: "CAVALRY_ARCHER",
    29: "EAGLE_WARRIOR",
    30: "HD_CAMEL",
    31: "DE2_UNKOWN_31",
    32: "DE2_UNKOWN_32",
    33: "DE2_UNKOWN_33",
    34: "DE2_UNKOWN_34",
    35: "DE2_UNKOWN_35",
    36: "DE2_UNKOWN_36",
}

UNIT_CLASSES = {
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
    17: "SWGB_TRANSPORT_SHIP",
    18: "PRIEST",
    19: "TRADE_CART",
    20: "TRANSPORT_BOAT",
    21: "FISHING_BOAT",
    22: "WAR_BOAT",
    23: "CONQUISTADOR",
    24: "AOE1_WAR_ELEPPHANT",
    25: "SWGB_SHORE_FISH",
    26: "SWGB_MARKER",
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
    60: "SWGB_LIVESTOCK",
    61: "HORSE",
    62: "SWGB_AIR_CRUISER",
    63: "SWGB_GEONOSIAN",
    64: "SWGB_JEDI_STARFIGHTER",
}

ELEVATION_MODES = {
    0: "NONE",                  # gates, farms, walls, towers
    2: "ZERO_ELEV_DIFFERENCE",  # towncenter, port, trade workshop
    3: "ONE_ELEV_DIFFERENCE",   # everything else
}

FOG_VISIBILITY = {
    0: "INVISIBLE",     # people etc
    1: "VISIBLE",       # buildings
    2: "VISIBLE_IF_ALIVE",
    3: "ONLY_IN_FOG",
    4: "DOPPELGANGER",
}

TERRAIN_RESTRICTIONS = {
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
    0x09: "SWGB_ONLY_WATER0",
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
    0x16: "SWGB_GRASS_SHORE",
    0x17: "SWGB_ANY",
    0x18: "SWGB_ONLY_WATER1",
    0x19: "SWGB_LAND_IMPASSABLE_WATER0",
    0x1A: "SWGB_LAND_IMPASSABLE_WATER1",
    0x1B: "SWGB_DEEP_WATER",
    0x1C: "SWGB_WASTELAND",
    0x1D: "SWGB_ICE",
    0x1E: "DE2_UNKNOWN",
    0x1F: "SWGB_WATER2",
    0x20: "SWGB_ROCK4",
}

BLAST_DEFENSE_TYPES = {
    0: "UNIT_0",    # projectile, dead, fish, relic, tree, gate, towncenter
    1: "OTHER",     # 'other' things with multiple rotations
    2: "BUILDING",  # buildings, gates, walls, towncenter, fishtrap
    3: "UNIT_3"     # boar, farm, fishingship, villager, tradecart, sheep, turkey,
                    # archers, junk, ships, monk, siege
}

COMBAT_LEVELS = {
    0: "PROJECTILE_DEAD_RESOURCE",
    1: "BOAR",
    2: "BUILDING",
    3: "CIVILIAN",
    4: "MILITARY",
    5: "OTHER",
}

INTERACTION_MODES = {
    0: "NOTHING_0",
    1: "BIRD",
    2: "SELECTABLE",
    3: "SELECT_ATTACK",
    4: "SELECT_ATTACK_MOVE",
    5: "SELECT_MOVE",
}

MINIMAP_MODES = {
    0: "NO_DOT_0",
    1: "SQUARE_DOT",             # turns white when selected
    2: "DIAMOND_DOT",            # dito
    3: "DIAMOND_DOT_KEEPCOLOR",  # doesn't turn white when selected
    4: "LARGEDOT",               # observable by all players, no attacked-blinking
    5: "NO_DOT_5",
    6: "NO_DOT_6",
    7: "NO_DOT_7",
    8: "NO_DOT_8",
    9: "NO_DOT_9",
    10: "NO_DOT_10",
}

UNIT_LEVELS = {
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
}

OBSTRUCTION_TYPES = {
    0: "PASSABLE",   # farm, gate, dead bodies, town center
    2: "BUILDING",
    3: "BERSERK",
    5: "UNIT",
    10: "MOUNTAIN",  # mountain (matches occlusion_mask)
}

SELECTION_EFFECTS = {
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
}

ATTACK_MODES = {
    0: "NO",         # no attack
    1: "FOLLOWING",  # by following
    2: "RUN",        # run when attacked
    3: "UNKNOWN3",
    4: "ATTACK",
}

BOUNDARY_IDS = {
    -1: "NONE",
    4: "BUILDING",
    6: "DOCK",
    10: "WALL",
}

BLAST_OFFENSE_TYPES = {
    0: "RESOURCES",
    1: "TREES",
    2: "NEARBY_UNITS",
    3: "TARGET_ONLY",
    6: "UNKNOWN_6",
}

CREATABLE_TYPES = {
    0: "NONHUMAN",  # building, animal, ship
    1: "VILLAGER",  # villager, king
    2: "MELEE",     # soldier, siege, predator, trader
    3: "MOUNTED",   # camel rider
    4: "RELIC",
    5: "RANGED_PROJECTILE",  # archer
    6: "RANGED_MAGIC",       # monk
    21: "TRANSPORT_SHIP",
}

GARRISON_TYPES = {
    0x00: "NONE",
    0x01: "VILLAGER",
    0x02: "INFANTRY",
    0x04: "CAVALRY",
    0x07: "SWGB_NO_JEDI",
    0x08: "MONK",
    0x0b: "NOCAVALRY",
    0x0f: "ALL",
    0x10: "SWGB_LIVESTOCK",
}
