# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Age of Empires games do not necessarily come with an english
translation. Therefore, we use the strings in this file to
figure out the names for a nyan object.
"""

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new units of Rajas
UNIT_LINE_LOOKUPS = {
    1120: ("BallistaElephant", "ballista_elephant"),
    1123: ("KarambitWarrior", "karambit_warrior"),
    1126: ("Arambai", "arambai"),
    1129: ("RattanArcher", "rattan_archer"),
    1132: ("BattleElephant", "battle_elephant"),
    1135: ("KomodoDragon", "komodo_dragon"),
    1137: ("Tiger", "tiger"),
    1139: ("Rhinoceros", "rhinoceros"),
    1142: ("WaterBuffalo", "water_buffallo"),
}

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new buildings of Rajas
BUILDING_LINE_LOOKUPS = {
    1189: ("Harbor", "harbor"),
}

# key: (head) unit id; value: (nyan object name, filename prefix)
# contains only new/changed ambience of Rajas
AMBIENT_GROUP_LOOKUPS = {
}

# key: index; value: (nyan object name, filename prefix, units belonging to group, variant type)
# contains only new/changed variants of Rajas
VARIANT_GROUP_LOOKUPS = {
}

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new techs of Rajas
TECH_GROUP_LOOKUPS = {
    622: ("TuskSwords", "tusk_swords"),
    623: ("DoubleCrossbow", "double_crossbow"),
    624: ("Thalassocracy", "thalassocracy"),
    625: ("ForcedLevy", "forced_levy"),
    626: ("Howdah", "howdah"),
    627: ("ManipurCavalry", "manipur_cavalry"),
    628: ("Chatras", "chatras"),
    629: ("PaperMoney", "paper_money"),
    622: ("TuskSwords", "tusk_swords"),
}

# key: civ index; value: (nyan object name, filename prefix)
# contains only new civs of Rajas
CIV_GROUP_LOOKUPS = {
    28: ("Khmer", "khmer"),
    29: ("Malay", "malay"),
    30: ("Burmese", "burmese"),
    31: ("Vietnamese", "vietnamese"),
}

# key: civ index; value: (civ ids, nyan object name, filename prefix)
# contains only new/changed graphic sets of Rajas
GRAPHICS_SET_LOOKUPS = {
    10: ((28, 29, 30), "SouthEastAsian", "south_east_asian"),
}

# key: armor class; value: Gather ability name
# contains only new armors of Rajas
ARMOR_CLASS_LOOKUPS = {
    32: "Condottiero",
    34: "FishingShip",
    35: "Mameluke",
}
