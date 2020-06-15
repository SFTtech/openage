# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Age of Empires games do not necessarily come with an english
translation. Therefore, we use the strings in this file to
figure out the names for a nyan object.
"""

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new units of DE2
UNIT_LINE_LOOKUPS = {
    705: ("Cow", "cow"),
    1228: ("Keshik", "keshik"),
    1231: ("Kipchak", "kipchak"),
    1234: ("Leitis", "leitis"),
    1239: ("Ibex", "ibex"),
    1243: ("Goose", "goose"),
    1245: ("Pig", "pig"),
    1254: ("Konnik", "konnik"),
    1258: ("Ram", "ram"),                       # replacement for ID 35?
    1263: ("FlamingCamel", "flaming_camel"),
    1370: ("SteppeLancer", "steppe_lancer"),
}

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new buildings of DE2
BUILDING_LINE_LOOKUPS = {
    1251: ("Krepost", "krepost"),
}

# key: (head) unit id; value: (nyan object name, filename prefix)
# contains only new/changed ambience of DE2
AMBIENT_GROUP_LOOKUPS = {
}

# key: index; value: (nyan object name, filename prefix, units belonging to group, variant type)
# contains only new/changed variants of DE2
VARIANT_GROUP_LOOKUPS = {
    264: ("Cliff", "cliff", (264, 265, 266, 267, 268, 269, 270, 271, 272), "angle"),
}

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new techs of DE2
TECH_GROUP_LOOKUPS = {
    488: ("Kamandaran", "kamandaran"),
    685: ("Stirrups", "stirrups"),
    686: ("Bagains", "bagains"),
    687: ("SilkArmor", "silk_armor"),
    688: ("TimuridSiegecraft", "timurid_siegecraft"),
    689: ("SteppeHusbandry", "steppe_husbandry"),
    690: ("CumanMercanaries", "cuman_mercinaries"),
    691: ("HillForts", "hill_forts"),
    692: ("TowerShields", "tower_shields"),
    716: ("Supplies", "supplies"),
}

# key: civ index; value: (nyan object name, filename prefix)
# contains only new civs of DE2
CIV_GROUP_LOOKUPS = {
    32: ("Bulgarians", "bulgarians"),
    33: ("Tatars", "tatars"),
    34: ("Cumans", "cumans"),
    35: ("Lithuanians", "lithuanians"),
}

# key: civ index; value: (civ ids, nyan object name, filename prefix)
# contains only new/changed graphic sets of DE2
GRAPHICS_SET_LOOKUPS = {
    8: ((32, 35), "EasternEuropean", "eastern_european"),
    11: ((33, 34), "CentralAsian", "central_asian"),
}

# key: terrain index; value: (unit terrain restrictions (manual), nyan object name, filename prefix)
# TODO: Use terrain restrictions from .dat
# contains only new/changed terrains of DE2
TERRAIN_GROUP_LOOKUPS = {
}

# key: not relevant; value: (terrain indices, unit terrain restrictions (manual), nyan object name)
# TODO: Use terrain restrictions from .dat
# contains only new/changed terrain types of DE2
TERRAIN_TYPE_LOOKUPS = {
}

# key: armor class; value: Gather ability name
# contains only new armors of DE2
ARMOR_CLASS_LOOKUPS = {
    31: "AntiLetis",
}
