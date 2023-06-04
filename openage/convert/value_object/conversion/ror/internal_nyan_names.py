# Copyright 2020-2023 the openage authors. See copying.md for legal info.
#
# pylint: disable=line-too-long

"""
Age of Empires games do not necessarily come with an english
translation. Therefore, we use the strings in this file to
figure out the names for a nyan object.
"""

# key: head unit id; value: (nyan object name, filename prefix)
UNIT_LINE_LOOKUPS = {
    4: ("Bowman", "bowman"),
    5: ("ImprovedBowman", "improved_bowman"),
    11: ("Ballista", "balista"),
    13: ("FishingBoat", "fishing_boat"),
    15: ("TradeBoat", "trade_boat"),
    17: ("Transport", "transport"),
    19: ("Galley", "galley"),
    25: ("ElephantArcher", "elephant_archer"),
    35: ("Catapult", "catapult"),
    37: ("Cavalry", "cavalry"),
    39: ("HorseArcher", "horse_archer"),
    40: ("Chariot", "chariot"),
    41: ("ChariotArcher", "chariot_archer"),
    46: ("WarElephant", "war_elephant"),
    73: ("Clubman", "clubman"),
    75: ("Swordsman", "swordsman"),
    93: ("Hoplite", "hoplite"),
    118: ("Villager", "villager"),
    125: ("Priest", "priest"),
    250: ("CatapultTrireme", "catapult_trireme"),
    299: ("Scout", "scout"),
    338: ("CamelRider", "camel_rider"),
    347: ("Slinger", "slinger"),
    360: ("FireGalley", "fire_galley"),
}

# key: head unit id; value: (nyan object name, filename prefix)
BUILDING_LINE_LOOKUPS = {
    0: ("Academy", "academy"),
    12: ("Barracks", "barracks"),
    45: ("Dock", "dock"),
    49: ("SiegeWorkshop", "siege_workshop"),
    50: ("Farm", "farm"),
    68: ("Granary", "granary"),
    70: ("House", "house"),
    72: ("Wall", "wall"),
    79: ("Tower", "tower"),
    82: ("GovernmentCenter", "government_center"),
    84: ("Market", "market"),
    87: ("ArcheryRange", "archery_range"),
    101: ("Stable", "stable"),
    103: ("StoragePit", "storage_pit"),
    104: ("Temple", "temple"),
    109: ("TownCenter", "town_center"),
    276: ("Wonder", "wonder"),
}

# key: (head) unit id; value: (nyan object name, filename prefix)
AMBIENT_GROUP_LOOKUPS = {
    59: ("BerryBush", "berry_bush"),
    66: ("GoldMine", "gold_mine"),
    102: ("StoneMine", "stone_mine"),
}

# key: index; value: (nyan object name, filename prefix, units belonging to group, variant type)
VARIANT_GROUP_LOOKUPS = {
    56: ("TreeOak", "tree_oak", (56, 134, 141, 144, 145, 146, 148,), "misc"),
    80: ("Shallows", "shallows", (80,), "misc"),
    113: ("TreePalm", "tree_palm", (113, 114, 121, 129, 148, 149, 150, 151, 152, 153), "misc"),
    135: ("TreeConifer", "tree_conifer", (135, 137, 138, 139,), "misc"),
    136: ("TreeSpruce", "tree_spruce", (136,), "misc"),
    140: ("TreeBeech", "tree_beech", (140,), "misc"),
    161: ("TreePine", "tree_pine", (161, 192, 193, 194, 197, 198, 203, 226, 391, 392,), "misc"),
    164: ("Cactus", "cactus", (164, 165, 166, 169,), "misc"),
    167: ("GrassClump", "grass_clump", (167, 168, 170, 171, 172, 173, 174, 175, 176, 177,), "misc"),
    178: ("DesertClump", "desert_clump", (178, 179, 180,), "misc"),
    181: ("Skeleton", "skeleton", (181, 182, 183,), "misc"),
    186: ("RockDirt", "rock_dirt", (186, 304, 305, 306,), "misc"),
    187: ("Crack", "crack", (187, 188, 189, 190, 191,), "misc"),
    184: ("RockGrass", "rock_grass", (184, 307, 308, 309, 310, 311, 312, 313,), "misc"),
    185: ("RockSand", "rock_sand", (185, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324,), "misc"),
    332: ("RockMud", "rock_mud", (332, 333, 334,), "misc"),
    343: ("TreeDead", "tree_dead", (343,), "misc"),
    385: ("RockBeach", "rock_beach", (385,), "misc"),
}

# key: head unit id; value: (nyan object name, filename prefix)
TECH_GROUP_LOOKUPS = {
    2: ("BallistaTower", "ballista_tower"),
    4: ("FishingShip", "fishing_ship"),
    5: ("MediumWarship", "medium_warship"),
    6: ("MerchantShip", "merchant_ship"),
    7: ("Trireme", "trireme"),
    8: ("HeavyTransport", "heavy_transport"),
    9: ("CatapultTrireme", "catapult_trireme"),
    11: ("StoneWall", "stone_wall"),
    12: ("SentryTower", "sentry_tower"),
    13: ("MediumWall", "medium_wall"),
    14: ("Fortifications", "fortifications"),
    15: ("GuardTower", "guard_tower"),
    16: ("WatchTower", "watch_tower"),
    18: ("Afterlife", "afterlife"),
    19: ("Monotheism", "monotheism"),
    20: ("Fanatism", "fanatism"),
    21: ("Mysticism", "mysticism"),
    22: ("Astrology", "astrology"),
    23: ("HolyWar", "holy_war"),
    24: ("Polytheism", "polytheism"),
    25: ("HeavyWarship", "heavy_warship"),
    27: ("Helepolis", "helepolis"),
    28: ("Wheel", "wheel"),
    30: ("Coinage", "coinage"),
    31: ("Plow", "plow"),
    32: ("Artisanship", "artisanship"),
    34: ("Nobility", "nobility"),
    35: ("Engineering", "engineering"),
    36: ("MassiveCatapult", "massive_catapult"),
    37: ("Alchemy", "alchemy"),
    38: ("HeavyHorseArcher", "heavy_horse_archer"),
    40: ("LeatherArmorInfantry", "leather_armor_infantry"),
    41: ("LeatherArmorArcher", "leather_armor_archer"),
    42: ("LeatherArmorCavalry", "leather_armor_cavalry"),
    43: ("ScaleArmorInfantry", "scale_armor_infantry"),
    44: ("ScaleArmorArcher", "scale_armor_archer"),
    45: ("ScaleArmorCavalry", "scale_armor_cavalry"),
    46: ("ToolWorking", "tool_working"),
    47: ("BronzeShield", "bronze_shield"),
    48: ("ChainMailInfantry", "chain_mail_infantry"),
    49: ("ChainMailArcher", "chain_mail_archer"),
    50: ("ChainMailCavalry", "chain_mail_cavalry"),
    51: ("MetalWorking", "metal_working"),
    52: ("Metalurgy", "metalurgy"),
    54: ("HeavyCatapult", "heavy_catapult"),
    56: ("ImprovedBow", "improved_bow"),
    57: ("CompositeBowman", "composite_bowman"),
    63: ("Axe", "axe"),
    64: ("ShortSword", "short_sword"),
    65: ("Broadswordsman", "broadswordsman"),
    66: ("Longswordsman", "longswordsman"),
    71: ("HeavyCavalry", "heavy_cavalry"),
    73: ("Phalanx", "phalanx"),
    77: ("Legion", "legion"),
    78: ("Cataphract", "cataphract"),
    79: ("Centurion", "centurion"),
    80: ("Irrigation", "irrigation"),
    81: ("Domestication", "domestication"),
    100: ("StoneAge", "stone_age"),
    101: ("ToolAge", "tool_age"),
    102: ("BronzeAge", "bronze_age"),
    103: ("IronAge", "iron_age"),
    106: ("Ballistics", "ballistics"),
    107: ("WoodWorking", "wood_working"),
    108: ("GoldMining", "gold_mining"),
    109: ("StoneMining", "stone_mining"),
    110: ("Craftmanship", "craftmanship"),
    111: ("SiegeCraft", "siege_craft"),
    112: ("Architecture", "architecture"),
    113: ("Aristocracy", "aristocracy"),
    114: ("Writing", "writing"),
    117: ("IronShield", "iron_shield"),
    119: ("Medicine", "medicine"),
    120: ("Martyrdom", "martyrdom"),
    121: ("Logistics", "logistics"),
    122: ("TowerShield", "tower_shield"),
    125: ("ArmoredElephant", "armored_elephant"),
    126: ("HeavyChariot", "heavy_chariot"),
}

# key: civ index; value: (nyan object name, filename prefix)
CIV_GROUP_LOOKUPS = {
    0: ("Gaia", "gaia"),
    1: ("Egyptians", "egyptians"),
    2: ("Greek", "greek"),
    3: ("Babylonians", "babylonians"),
    4: ("Assyrians", "assyrians"),
    5: ("Minoans", "minoans"),
    6: ("Hittite", "hittite"),
    7: ("Phoenicians", "phoenicians"),
    8: ("Sumerians", "sumerians"),
    9: ("Persians", "persians"),
    10: ("Shang", "shang"),
    11: ("Yamato", "yanato"),
    12: ("Choson", "choson"),
    13: ("Romans", "romans"),
    14: ("Carthage", "carthage"),
    15: ("Palmyra", "palmyra"),
    16: ("Macedonians", "macedonians"),
}

# key: civ index; value: (civ ids, nyan object name, filename prefix)
GRAPHICS_SET_LOOKUPS = {
    0: ((1, 4, 8), "MiddleEastern", "middle_eastern"),
    1: ((2, 5, 7), "Mediterranean", "mediterranean"),
    2: ((3, 6, 9), "CentralAsian", "central_asian"),
    3: ((0, 10, 11, 12), "EastAsian", "east_asian"),
    4: ((13, 14, 15, 16), "Roman", "roman"),
}

# key: terrain index; value: (unit terrain restrictions (manual), nyan object name, filename prefix)
# TODO: Use terrain restrictions from .dat
TERRAIN_GROUP_LOOKUPS = {
    0:  ((0, 1, 4, 7, 8, 9, 10,), "Grass", "grass"),
    1:  ((0, 3, 6,), "Water", "water"),
    2:  ((0, 2, 6, 7, 10,), "Beach", "beach"),
    3:  (((),), "ThinRiver", "thin_river"),
    4:  ((0, 1, 3, 6, 7,), "Shallows", "shallows"),
    5:  ((0, 1, 4, 7, 9, 10,), "JungleEdge", "jungle_edge"),
    6:  ((0, 1, 4, 7, 8, 9, 10,), "Desert", "desert"),
    7:  (((),), "Crop", "crop"),
    8:  (((),), "Rows", "rows"),
    9:  (((),), "Wheat", "wheat"),
    10: ((0, 1, 4, 7, 9, 10,), "Forest", "forest"),
    11: ((0, 1, 4, 10,), "Dirt", "dirt"),
    12: (((),), "Grass2", "grass2"),
    13: ((0, 1, 4, 7, 9, 10,), "DesertPalm", "desert_palm"),
    14: (((),), "DesertImpassable", "desert_impassable"),
    15: (((),), "WaterImpassable", "water_impassable"),
    16: (((),), "GrassImpassable", "grass_impassable"),
    17: (((),), "Fog", "fog"),
    18: ((0, 1, 4, 7, 9, 10,), "ForestEdge", "forest_edge"),
    19: ((0, 1, 4, 7, 9, 10,), "PineForest", "pine_forest"),
    20: ((0, 1, 4, 7, 9, 10,), "Jungle", "jungle"),
    21: ((0, 1, 4, 7, 9, 10,), "PineForestEdge", "pine_forest_edge"),
    22: ((0, 3, 6,), "WaterDark", "water_dark"),
}

# key: not relevant; value: (terrain indices, unit terrain restrictions (manual), nyan object name)
# TODO: Use terrain restrictions from .dat
TERRAIN_TYPE_LOOKUPS = {
    0: ((0, 4, 5, 6, 10, 11, 13, 18, 19, 20, 21,),
        (1, 4, 7, 8, 9, 10),
        "Land",),
    1: ((1, 22,),
        (3, 6,),
        "Water",),
    2: ((2, 4,),
        (2, 6, ),
        "Shallow",),
}

CLASS_ID_LOOKUPS = {
    0: "Archer",
    1: "Artifact",
    2: "TradeBoat",
    3: "BuildingMisc",
    4: "Villager",
    5: "OceanFish",
    6: "Infantry",
    7: "BerryBush",
    8: "StoneMine",
    9: "AnimalPrey",
    10: "AnimalPredator",
    11: "DeadOrProjectileOrBird",     # do not use this as GameEntityType
    12: "Cavalry",
    13: "SiegeWeapon",
    14: "Ambient",
    15: "Tree",
    18: "Monk",
    19: "TradecCart",
    20: "TransportShip",
    21: "FishingShip",
    22: "Warship",
    23: "ChariotArcher",
    24: "WarElephant",
    25: "Hero",
    26: "ElephantArcher",
    27: "Wall",
    28: "Phalanx",
    29: "DomesticAnimal",
    30: "AmbientFlag",
    31: "DeepSeaFish",
    32: "GoldMine",
    33: "ShoreFish",
    34: "Cliff",
    35: "Chariot",
    36: "CavalryArcher",
    37: "Doppelgaenger",
    38: "Bird",
    39: "Slinger",
}

# key: genie unit id; value: Gather ability name
GATHER_TASK_LOOKUPS = {
    13: ("Fish", "fish"),  # fishing boat
    119: ("Fish", "fish"),
    120: ("CollectBerries", "collect_berries"),
    122: ("HarvestGame", "harvest_game"),
    123: ("ChopWood", "chop_wood"),
    124: ("MineStone", "mine_stone"),
    251: ("MineGold", "mine_gold"),
    259: ("FarmCrops", "farm_crops"),
}

# key: armor class; value: Gather ability name
ARMOR_CLASS_LOOKUPS = {
    0: "FireGalley",
    1: "Archer",
    3: "Pierce",
    4: "Melee",
    6: "Building",
    7: "Priest",
    8: "Cavalry",
    9: "Infantry",
    10: "StoneDefense",
    12: "Civilian",
}

# key: command type; value: Apply*Effect ability name
COMMAND_TYPE_LOOKUPS = {
    7: ("Attack", "attack"),
    101: ("Construct", "construct"),
    104: ("Convert", "convert"),
    105: ("Heal", "heal"),
    106: ("Repair", "repair"),
    110: ("Hunt", "hunt"),
}
