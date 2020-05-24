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
