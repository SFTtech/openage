# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
nyan object names and filenames for Age of Empires I.
"""

# key: head unit id; value: (nyan object name, filename prefix)
UNIT_LINE_LOOKUPS = {
    4: ("Bowman", "bowman"),
    5: ("ImprovedBowman", "improved_bowman"),
    11: ("Ballista", "balista"),
    13: ("FishingShip", "fishing_ship"),
    15: ("TradeBoat", "trade_boat"),
    17: ("Transport", "transport"),
    19: ("Galley", "galley"),
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
    # TODO
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
    0: ((1, 4, 8), "Egyptian", "egyptian"),
    1: ((2, 5, 1), "Greek", "greek"),
    2: ((3, 6, 9), "Persian", "persian"),
    3: ((0, 10, 11, 12), "Asian", "asian"),
    4: ((13, 14, 15, 16), "Roman", "roman"),
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
