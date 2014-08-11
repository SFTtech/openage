import dataformat
from struct import Struct, unpack_from
import util
from util import dbg, zstr


class UnitCommand(dataformat.Exportable):
    name_struct        = "unit_command"
    name_struct_file   = "unit"
    struct_description = "a command a single unit may recieve by script or human."

    data_format = (
        (dataformat.READ, "command_used", "int16_t"),                  #always 1
        (dataformat.READ_EXPORT, "id", "int16_t"),                     #command id
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_EXPORT, "type", "int16_t"),
        (dataformat.READ_EXPORT, "class_id", "int16_t"),
        (dataformat.READ_EXPORT, "unit_id", "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ_EXPORT, "ressource_in", "int16_t"),
        (dataformat.READ_EXPORT, "ressource_productivity", "int16_t"), #resource that multiplies the amount you can gather
        (dataformat.READ_EXPORT, "ressource_out", "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ_EXPORT, "work_rate_multiplier", "float"),
        (dataformat.READ_EXPORT, "execution_radius", "float"),
        (dataformat.READ_EXPORT, "extra_range", "float"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "float"),
        (dataformat.READ, "selection_enabled", "int8_t"),              #1=allows to select a target, type defined in `selection_type`
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ_EXPORT, "targets_allowed", dataformat.EnumLookupMember(
            raw_type    = "int8_t",      #what can be selected as a target for the unit command?
            type_name   = "selection_type",
            lookup_dict = {
                0: "ANY_0",              #select anything
                1: "OWNED_UNITS",        #your own things
                2: "NEUTRAL_ENEMY",      #enemy and neutral things (->attack)
                3: "NOTHING",
                4: "GAIA_OWNED_ALLY",    #any of gaia, owned or allied things
                5: "GAYA_NEUTRAL_ENEMY", #any of gaia, neutral or enemy things
                6: "NOT_OWNED",          #all things that aren't yours
                7: "ANY_7",
            },
        )),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "tool_graphic_id", "int16_t"),               #walking with tool but no ressource
        (dataformat.READ, "proceed_graphic_id", "int16_t"),            #proceeding ressource gathering or attack
        (dataformat.READ, "action_graphic_id", "int16_t"),             #actual execution or transformation graphic
        (dataformat.READ, "carrying_graphic_id", "int16_t"),           #display ressources in hands
        (dataformat.READ, "execution_sound_id", "int16_t"),            #sound to play when execution starts
        (dataformat.READ, "ressource_deposit_sound_id", "int16_t"),    #sound to play on ressource drop
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
    struct_description = "determines the resource storage capacity for one unit mode."

    data_format = (
        (dataformat.READ, "type", "int16_t"),
        (dataformat.READ, "amount", "float"),
        (dataformat.READ, "used_mode", dataformat.EnumLookupMember(
            raw_type    = "int8_t",
            type_name   = "ressource_handling",
            lookup_dict = {
                0: "DECAYABLE",
                1: "KEEP_AFTER_DEATH",
                2: "RESET_ON_DEATH_INSTANT",
                4: "RESET_ON_DEATH_WHEN_COMPLETED",
            },
        )),
    )


class DamageGraphic(dataformat.Exportable):
    name_struct        = "damage_graphic"
    name_struct_file   = "unit"
    struct_description = "stores one possible unit image that is displayed at a given damage percentage."

    data_format = (
        (dataformat.READ_EXPORT, "graphic_id", "int16_t"),
        (dataformat.READ_EXPORT, "damage_percent", "int8_t"),
        (dataformat.READ_EXPORT, "apply_mode", dataformat.EnumLookupMember(
            raw_type    = "int8_t",
            type_name   = "damage_draw_type",
            lookup_dict = {
                0: "ADD_FLAMES_0",
                1: "ADD_FLAMES_1",
                2: "REPLACE",
            },
        )),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
    )


class HitType(dataformat.Exportable):
    name_struct        = "hit_type"
    name_struct_file   = "unit"
    struct_description = "stores attack amount for a damage type."

    data_format = (
        (dataformat.READ, "type_id", dataformat.EnumLookupMember(
            raw_type    = "int16_t",
            type_name   = "hit_class",
            lookup_dict = {
                -1: "NONE",
                1: "INFANTRY",
                2: "SHIP_TURTLE",
                3: "UNITS_PIERCE",
                4: "UNITS_MELEE",
                5: "WAR_ELEPHANT",
                8: "CAVALRY",
                11: "BUILDINGS_NO_PORT",
                13: "STONE_DEFENSES",
                15: "ARCHERS",
                16: "SHIPS_CAMELS_SABOTEURS",
                17: "RAMS",
                18: "TREES",
                19: "UNIQUE_UNITS",
                20: "SIEGE_WEAPONS",
                21: "BUILDINGS",
                22: "WALLS_GATES",
                24: "BOAR",
                25: "MONKS",
                26: "CASTLE",
                27: "SPEARMEN",
                28: "CAVALRY_ARCHER",
                29: "EAGLE_WARRIOR",
            },
        )),
        (dataformat.READ, "amount", "int16_t"),
    )


class RessourceCost(dataformat.Exportable):
    name_struct        = "ressource_cost"
    name_struct_file   = "unit"
    struct_description = "stores cost for one ressource for creating the unit."

    data_format = (
        (dataformat.READ, "type_id", dataformat.EnumLookupMember(
            raw_type = "int16_t",
            type_name = "ressource_types",
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
                11: "POPULATION",         #both current population and population headroom
                12: "CORPSE_DECAY_TIME",
                13: "DISCOVERY",
                14: "RUIN_MONUMENTS_CAPTURED", #unused
                15: "PREDATOR_ANIMAL_FOOD",
                16: "CROPS",
                17: "FISH_STORAGE",
                18: "UNKNOWN_18",
                19: "TOTAL_UNITS_OWNED",   #or just military ones? used for counting losses
                20: "UNITS_KILLED",
                21: "RESEARCHED_TECHNOLOGIES_COUNT",
                23: "TECHNOLOGY_ID_0",     #default: 102
                24: "TECHNOLOGY_ID_1",     #default: 103
                25: "TECHNOLOGY_ID_2",     #default: 101
                27: "ATONEMENT",           #bool
                28: "REDEMPTION",          #bool
                30: "VAL_500",             #default: 500
                32: "BONUS_POPULATION",
                35: "FAITH_RECHARGE_RATE", #default: 1.6
                36: "FARM_FOOD_AMOUNT",    #default: 175
                37: "CIVILIAN_POPULATION",
                38: "UNKNOWN_38",
                39: "ALL_TECHS_ACHIEVED",  #default: 178
                40: "MILITARY_POPULATION", #-> largest army
                41: "UNITS_CONVERTED",
                42: "WONDERS_STANDING",
                43: "BUILDINGS_RAZED",
                44: "KILL_RATIO",
                45: "SURVIVAL_TO_FINISH",  #bool
                46: "TRIBUTE_FEE",         #default: 0.3
                47: "GOLD_MINING_PRODUCTIVITY", #default: 1
                48: "TOWN_CENTER_AVAILABLE",
                49: "GOLD_COUNTER",
                50: "REVEAL_ALLY",         #bool, ==cartography discovered
                51: "HOUSES_UNUSED",
                52: "MONASTERY_COUNT",
                53: "TRIBUTE_SENT",
                54: "RUINES_CAPTURED_ALL", #bool
                55: "RELICS_CAPTURED_ALL", #bool
                56: "LOAD_STORAGE",        #or unit unload room?
                57: "CAPTURED_UNITS",
                58: "DARK_AGE",            #default: 104
                59: "TRADE_GOOD_QUALITY",  #default: 1
                60: "TRADE_MARKET_LEVEL",
                61: "FORMATIONS",
                62: "BUILDING_HOUSING_RATE", #default: 20
                63: "GATHER_TAX_RATE",       #default: 32000
                64: "GATHER_ACCUMULATOR",
                65: "SALVAGE_DECAY_RATE",    #default: 5
                66: "ALLOW_FORMATION",       #bool, something with age?
                67: "CONVERSIONS",           #bool?
                68: "HIT_POINTS_KILLED",     #unused
                69: "KILLED_PLAYER_1",       #bool
                70: "KILLED_PLAYER_2",       #bool
                71: "KILLED_PLAYER_3",       #bool
                72: "KILLED_PLAYER_4",       #bool
                73: "KILLED_PLAYER_5",       #bool
                74: "KILLED_PLAYER_6",       #bool
                75: "KILLED_PLAYER_7",       #bool
                76: "KILLED_PLAYER_8",       #bool
                77: "CONVERSION_RESISTANCE",
                78: "TRADE_FEE",             #default: 0.3
                79: "STONE_MINING_PRODUCTIVITY", #default: 1
                80: "QUEUED_UNITS",
                81: "TRAINING_COUNT",
                82: "START_PACKED_TOWNCENTER",   #or raider, default: 2
                83: "BOARDING_RECHARGE_RATE",
                84: "STARTING_VILLAGERS",        #default: 3
                85: "RESEARCH_COST_MULTIPLIER",
                86: "RESEARCH_TIME_MULTIPLIER",
                87: "CONVERT_SHIPS_ABILITY",     #bool
                88: "FISH_TRAP_FOOD_AMOUNT",     #default: 700
                89: "BONUS_HEALING_RATE",
                90: "HEALING_RANGE",
                91: "BONUS_STARTING_FOOD",
                92: "BONUS_STARTING_WOOD",
                93: "BONUS_STARTING_STONE",
                94: "BONUS_STARTING_GOLD",
                95: "TOWN_CENTER_PACKING",       #or raider, default: 3
                96: "SELF_HEALING_SECONDS_BERSERKER",
                97: "ANIMAL_DISCOVERY_DOMINANT_LOS", #bool, sheep/turkey
                98: "SCORE_ECONOMY",                 #object cost summary
                99: "SCORE_TECHNOLOGY",
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
                165: "SCORE_BUILDINGS_CURRENT",         #default: 275
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
                176: "CONVERT_ADJUSTMENT_MIN",          #used for resistance against monk conversions
                177: "CONVERT_ADJUSTMENT_MAX",
                178: "CONVERT_RESIST_ADJUSTMENT_MIN",
                179: "CONVERT_RESIST_ADJUSTMENT_MAX",
                180: "CONVERT_BUILDIN_MIN",             #default: 15
                181: "CONVERT_BUILDIN_MAX",             #default: 25
                182: "CONVERT_BUILDIN_CHANCE",          #default: 25
                183: "REVEAL_ENEMY",
                184: "SCORE_SOCIETY",
                185: "SCORE_FOOD",
                186: "SCORE_WOOD",
                187: "SCORE_STONE",
                188: "SCORE_GOLD",
                189: "CHOPPING_PRODUCTIVITY",           #default: 1
                190: "FOOD_GATHERING_PRODUCTIVITY",     #default: 1
                191: "RELIC_GOLD_PRODUCTION_RATE",      #default: 30
                192: "HERESY_ACTIVE",                   #bool
                193: "THEOCRACY_ACTIVE",                #bool
                194: "CRENELLATIONS_ACTIVE",            #bool
                195: "CONSTRUCTION_RATE",               #except for wonders
                196: "WONDER_BONUS",
                197: "SPIES_DISCOUNT",                  #or atheism_active?
            }
        )),
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
        (dataformat.READ_EXPORT, "unit_class", dataformat.EnumLookupMember(
            raw_type    = "int16_t",
            type_name   = "unit_classes",
            lookup_dict = {
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
                18: "PRIEST",
                19: "TRADE_CART",
                20: "TRANSPORT_BOAT",
                21: "FISHING_BOAT",
                22: "WAR_BOAT",
                23: "CONQUISTADOR",
                27: "WALLS",
                28: "PHALANX",
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
        )),
        (dataformat.READ_EXPORT, "graphic_standing0", "int16_t"),
        (dataformat.READ_EXPORT, "graphic_standing1", "int16_t"),
        (dataformat.READ_EXPORT, "graphic_dying0", "int16_t"),
        (dataformat.READ_EXPORT, "graphic_dying1", "int16_t"),
        (dataformat.READ, "death_mode", "int8_t"),                  #1 = become `dead_unit_id` (reviving does not make it usable again)
        (dataformat.READ_EXPORT, "hit_points", "int16_t"),          #unit health. -1=insta-die
        (dataformat.READ, "line_of_sight", "float"),
        (dataformat.READ, "garrison_capacity", "int8_t"),           #number of units that can garrison in there
        (dataformat.READ_EXPORT, "radius_size0", "float"),          #size of the unit
        (dataformat.READ_EXPORT, "radius_size1", "float"),
        (dataformat.READ, "hp_bar_height0", "float"),               #vertical hp bar distance from ground
        (dataformat.READ_EXPORT, "sound_creation0", "int16_t"),
        (dataformat.READ_EXPORT, "sound_creation1", "int16_t"),
        (dataformat.READ, "dead_unit_id", "int16_t"),               #unit id to become on death
        (dataformat.READ, "placement_mode", "int8_t"),              #0=placable on top of others in scenario editor, 5=can't
        (dataformat.READ, "air_mode", "int8_t"),                    #1=no footprints
        (dataformat.READ, "icon_id", "int16_t"),                    #frame id of the icon slp (57029) to place on the creation button
        (dataformat.READ, "hidden_in_editor", "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ, "enabled", "int16_t"),                    #0=unlocked by research, 1=insta-available
        (dataformat.READ, "placement_bypass_terrain0", "int16_t"),  #terrain id that's needed somewhere on the foundation (e.g. dock water)
        (dataformat.READ, "placement_bypass_terrain1", "int16_t"),  #second slot for ^
        (dataformat.READ, "placement_terrain0", "int16_t"),         #terrain needed for placement (e.g. dock: water)
        (dataformat.READ, "placement_terrain1", "int16_t"),         #alternative terrain needed for placement (e.g. dock: shallows)
        (dataformat.READ, "editor_radius0", "float"),
        (dataformat.READ, "editor_radius1", "float"),
        (dataformat.READ_EXPORT, "building_mode", dataformat.EnumLookupMember(
            raw_type    = "int8_t",
            type_name   = "building_modes",
            lookup_dict = {
                0: "NON_BUILDING",    #gates, farms, walls, towers
                2: "TRADE_BUILDING",  #towncenter, port, trade workshop
                3: "ANY",
            },
        )),
        (dataformat.READ_EXPORT, "visible_in_fog", dataformat.EnumLookupMember(
            raw_type    = "int8_t",
            type_name   = "fog_visibility",
            lookup_dict = {
                0: "INVISIBLE",     #people etc
                1: "VISIBLE",       #buildings
                3: "ONLY_IN_FOG",
            },
        )),
        (dataformat.READ_EXPORT, "terrain_restriction", dataformat.EnumLookupMember(
            raw_type    = "int16_t",      #determines on what type of ground the unit can be placed/walk
            type_name   = "ground_type",  #is actually the id of the terrain_restriction entry!
            lookup_dict = {
                0x00: "ANY",
                0x01: "SHORELINE",
                0x02: "WATER",
                0x03: "WATER_SHIP_0x03",
                0x04: "FOUNDATION",
                0x05: "NOWHERE",              #can't place anywhere
                0x06: "WATER_DOCK",           #shallow water for dock placement
                0x07: "SOLID",
                0x08: "NO_ICE_0x08",
                0x0A: "NO_ICE_0x0A",
                0x0B: "FOREST",
                0x0C: "UNKNOWN_0x0C",
                0x0D: "WATER_0x0D",           #great fish
                0x0E: "UNKNOWN_0x0E",
                0x0F: "WATER_SHIP_0x0F",      #transport ship
                0x10: "GRASS_SHORELINE",      #for gates and walls
                0x11: "WATER_ANY_0x11",
                0x12: "UNKNOWN_0x12",
                0x13: "FISH_NO_ICE",
                0x14: "WATER_ANY_0x14",
                0x15: "WATER_SHALLOW",
            },
        )),
        (dataformat.READ_EXPORT, "fly_mode", "int8_t"),
        (dataformat.READ_EXPORT, "ressource_capacity", "int16_t"),
        (dataformat.READ_EXPORT, "ressource_decay", "float"),                 #when animals rot, their ressources decay
        (dataformat.READ_EXPORT, "blast_type", dataformat.EnumLookupMember(
            raw_type    = "int8_t",
            type_name   = "blast_types",
            lookup_dict = {
                0: "UNIT_0",   #projectile, dead, fish, relic, tree, gate, towncenter
                1: "OTHER",    #'other' things with multiple rotations
                2: "BUILDING", #buildings, gates, walls, towncenter, fishtrap
                3: "UNIT_3",   #boar, farm, fishingship, villager, tradecart, sheep, turkey, archers, junk, ships, monk, siege
            }
        )),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_EXPORT, "interaction_mode", dataformat.EnumLookupMember(
            raw_type    = "int8_t",  #what can be done with this unit?
            type_name   = "interaction_modes",
            lookup_dict = {
                0: "NOTHING_0",
                1: "NOTHING_1",
                2: "SELECTABLE",
                3: "SELECT_ATTACK",
                4: "SELECT_ATTACK_MOVE",
                5: "SELECT_MOVE",
            },
        )),
        (dataformat.READ_EXPORT, "minimap_mode", dataformat.EnumLookupMember(
            raw_type    = "int8_t",        #how does the unit show up on the minimap
            type_name   = "minimap_modes",
            lookup_dict = {
                0: "NO_DOT_0",
                1: "SQUARE_DOT",  #turns white when selected
                2: "DIAMOND_DOT", #dito
                3: "DIAMOND_DOT_KEEPCOLOR",
                4: "LARGEDOT_0",  #observable by all players, no attack-blinking
                5: "LARGEDOT_1",
                6: "NO_DOT_6",
                7: "NO_DOT_7",
                8: "NO_DOT_8",
                9: "NO_DOT_9",
                10: "NO_DOT_10",
            },
        )),
        (dataformat.READ_EXPORT, "command_attribute", dataformat.EnumLookupMember(
            raw_type    = "int16_t",             #selects the available ui command buttons for the unit
            type_name   = "command_attributes",
            lookup_dict = {
                0: "LIVING",               #commands: delete, garrison, stop, attributes: hit points
                1: "ANIMAL",               #animal
                2: "NONMILITARY_BULIDING", #nonmilitary building (build page 1)
                3: "VILLAGER",             #villager
                4: "MILITARY_UNIT",        #military unit
                5: "TRADING_UNIT",         #trading unit
                6: "MONK_EMPTY",           #monk
                7: "TRANSPORT_SHIP",       #transport ship
                8: "RELIC",                #relic / monk with relic
                9: "FISHING_SHIP",         #fishing ship
                10: "MILITARY_BUILDING",   #military building (build page 2)
                11: "SHIELDED_BUILDING",   #shield building (build page 3)
            },
        )),
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

        #bit 0 == 1 && val != 7: mask shown behind buildings,
        #bit 0 == 0 && val != {6, 10}: no mask displayed,
        #val == {-1, 7}: in open area mask is partially displayed
        #val == {6, 10}: building, causes mask to appear on units behind it
        (dataformat.READ, "selection_mask", "int8_t"),
        (dataformat.READ, "selection_shape_type", "int8_t"),
        (dataformat.READ, "selection_shape", "int8_t"),            #0=square, 1<=round

        #bitfield of unit attributes:
        #bit 0: allow garrison,
        #bit 1: don't join formation,
        #bit 2: stealth unit,
        #bit 3: detector unit,
        #bit 4: mechanical unit,
        #bit 5: biological unit,
        #bit 6: self-shielding unit,
        #bit 7: invisible unit
        (dataformat.READ, "attribute", "int8_t"),
        (dataformat.READ, "civilisation", "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ_EXPORT, "selection_effect", dataformat.EnumLookupMember(
            raw_type = "int8_t",     #things that happen when the unit was selected
            type_name = "selection_effects",
            lookup_dict = {
                0: "NONE",
                1: "HPBAR_ON_OUTLINE_DARK",  #permanent, editor only
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
        (dataformat.READ, "editor_selection_color", "uint8_t"), #0: default, -16: fish trap, farm, 52: deadfarm, OLD-*, 116: flare, whale, dolphin -123: fish
        (dataformat.READ, "selection_radius0", "float"),
        (dataformat.READ, "selection_radius1", "float"),
        (dataformat.READ, "hp_bar_height1", "float"),           #vertical hp bar distance from ground
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
        (dataformat.READ_EXPORT, "attack_mode", "int16_t"),     #0: no attack, 1: attack by following, 2: run when attacked, 3:?, 4: attack
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
        (dataformat.READ, "tracking_unit_id", "int16_t"),          #unit id what for the ground traces are for
        (dataformat.READ, "tracking_unit_used", "uint8_t"),        #-1: no tracking present, 2: projectiles with tracking unit
        (dataformat.READ, "tracking_unit_density", "float"),       #0: no tracking, 0.5: trade cart, 0.12: some projectiles, 0.4: other projectiles
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
        (dataformat.READ, "sheep_conversion", "int16_t"), #0=can be converted by unit command 107 (you found sheep!!1)
        (dataformat.READ, "search_radius", "float"),
        (dataformat.READ, "work_rate", "float"),
        (dataformat.READ, "drop_site0", "int16_t"),       #unit id where gathered ressources shall be delivered to
        (dataformat.READ, "drop_site1", "int16_t"),       #alternative unit id
        (dataformat.READ_EXPORT, "villager_mode", "int8_t"),     #unit can switch villager type (holza? gathara!) 1=male, 2=female
        (dataformat.READ_EXPORT, "move_sound", "int16_t"),
        (dataformat.READ_EXPORT, "stop_sound", "int16_t"),
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
        (dataformat.READ_EXPORT, "interaction_type", dataformat.EnumLookupMember(
            raw_type    = "int16_t",
            type_name   = "interaction_types",
            lookup_dict = {
                -1: "UNIT",
                 4: "BUILDING",
                 6: "DOCK",
                10: "WALL",
            },
        )),
        (dataformat.READ, "max_range", "float"),
        (dataformat.READ, "blast_radius", "float"),
        (dataformat.READ, "reload_time0", "float"),
        (dataformat.READ, "projectile_unit_id", "int16_t"),
        (dataformat.READ, "accuracy_percent", "int16_t"),       #probablity of attack hit
        (dataformat.READ, "tower_mode", "int8_t"),
        (dataformat.READ, "delay", "int16_t"),                  #delay in frames before projectile is shot
        (dataformat.READ, "projectile_graphics_displacement_lr", "float"),
        (dataformat.READ, "projectile_graphics_displacement_distance", "float"),
        (dataformat.READ, "projectile_graphics_displacement_height", "float"),
        (dataformat.READ_EXPORT, "blast_level", dataformat.EnumLookupMember(
            raw_type    = "int8_t",
            type_name   = "range_damage_type",
            lookup_dict = {
                0: "RESSOURCES",
                1: "TREES",
                2: "NEARBY_UNITS",
                3: "TARGET_ONLY",
            },
        )),
        (dataformat.READ, "min_range", "float"),
        (dataformat.READ, "garrison_recovery_rate", "float"),
        (dataformat.READ_EXPORT, "attack_graphic", "int16_t"),
        (dataformat.READ, "melee_armor_displayed", "int16_t"),
        (dataformat.READ, "attack_displayed", "int16_t"),
        (dataformat.READ, "range_displayed", "float"),
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
        (dataformat.READ, "stretch_mode", "int8_t"),         #1 = projectile falls vertically to the bottom of the map
        (dataformat.READ, "compensation_mode", "int8_t"),
        (dataformat.READ, "drop_animation_mode", "int8_t"),  #1 = disappear on hit
        (dataformat.READ, "penetration_mode", "int8_t"),     #1 = pass through hit object
        (dataformat.READ_UNKNOWN, None, "int8_t"),
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
    struct_description = "adds creation location and garrison unit properties."

    data_format = (
        (dataformat.READ_EXPORT, None, dataformat.IncludeMembers(cls=UnitMovable)),
        (dataformat.READ, "ressource_cost", dataformat.SubdataMember(ref_type=RessourceCost, length=3)),
        (dataformat.READ, "creation_time", "int16_t"),         #in seconds
        (dataformat.READ, "creation_location_id", "int16_t"),  #e.g. 118 = villager

        #where to place the button with the given icon
        #creation page:
        #+------------------------+
        #| 01 | 02 | 03 | 04 | 05 |
        #|----|----|----|----|----|
        #| 06 | 07 | 08 | 09 | 10 |
        #|----|----|----|----|----|
        #| 11 | 12 | 13 | 14 | 15 |
        #+------------------------+
        #
        #additional page (dock):
        #+------------------------+
        #| 21 | 22 | 23 | 24 | 25 |
        #|----|----|----|----|----|
        #| 26 | 27 | 28 | 29 | 30 |
        #|----|----|----|----|----|
        #| 31 | 32 | 33 | 34 | 35 |
        #+------------------------+
        (dataformat.READ, "creation_button_id", "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ, "missile_graphic_delay", "int8_t"),          #delay before the projectile is fired.
        (dataformat.READ, "hero_mode", "int8_t"), #if building: "others" tab in editor, if living unit: "heroes" tab, regenerate health + monk immunity
        (dataformat.READ, "garrison_graphic", "int32_t"),              #graphic to display when units are garrisoned
        (dataformat.READ, "attack_missile_duplication_min", "float"),  #projectile duplication when nothing garrisoned
        (dataformat.READ, "attack_missile_duplication_max", "int8_t"), #duplication when fully garrisoned
        (dataformat.READ, "attack_missile_duplication_spawning_width", "float"),
        (dataformat.READ, "attack_missile_duplication_spawning_length", "float"),
        (dataformat.READ, "attack_missile_duplication_spawning_randomness", "float"), #placement randomness, 0=from single spot, 1=random, 1<less random
        (dataformat.READ, "attack_missile_duplication_unit_id", "int32_t"),
        (dataformat.READ, "attack_missile_duplication_graphic_id", "int32_t"),
        (dataformat.READ, "dynamic_image_update", "int8_t"),     #determines adjacent unit graphics, if 1: building can adapt graphics by adjacent buildings
        (dataformat.READ, "pierce_armor_displayed", "int16_t"),  #unit stats display of pierce armor
    )

    def __init__(self):
        super().__init__()


class UnitBuilding(UnitLiving):
    """
    type_id >= 80
    """

    name_struct        = "unit_building"
    name_struct_file   = "unit"
    struct_description = "construction graphics and garrison building properties for units."

    data_format = (
        (dataformat.READ_EXPORT, None, dataformat.IncludeMembers(cls=UnitLiving)),
        (dataformat.READ_EXPORT, "construction_graphic_id", "int16_t"),
        (dataformat.READ, "snow_graphic_id", "int16_t"),
        (dataformat.READ, "adjacent_mode", "int16_t"),           #1=adjacent units may change the graphics
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "stack_unit_id", "int16_t"),           #second building to place directly on top
        (dataformat.READ_EXPORT, "terrain_id", "int16_t"),       #change underlying terrain to this id when building completed
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ, "research_id", "int16_t"),             #research_id to be enabled when building creation
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_EXPORT, "building_annex", dataformat.SubdataMember(ref_type=BuildingAnnex, length=4)),
        (dataformat.READ, "head_unit_id", "int16_t"),            #building at which an annex building is attached to
        (dataformat.READ, "transform_unit_id", "int16_t"),       #destination unit id when unit shall transform (e.g. unpack)
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ, "construction_sound_id", "int16_t"),
        (dataformat.READ_EXPORT, "garrison_type", dataformat.EnumLookupMember(
            raw_type    = "int8_t",
            type_name   = "garrison_types",
            lookup_dict = {  #TODO: create bitfield
                0x00: "NONE",
                0x01: "VILLAGER",
                0x02: "INFANTRY",
                0x04: "CAVALRY",
                0x08: "MONK",
                0x0b: "NOCAVALRY",
                0x0f: "ALL",
            },
        )),
        (dataformat.READ, "garrison_heal_rate", "float"),
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
