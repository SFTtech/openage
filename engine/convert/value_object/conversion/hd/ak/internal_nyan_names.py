# Copyright 2020-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=line-too-long

"""
Age of Empires games do not necessarily come with an english
translation. Therefore, we use the strings in this file to
figure out the names for a nyan object.
"""

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new units of AK
UNIT_LINE_LOOKUPS = {
    583: ("Genitour", "genitour"),
    885: ("SiegeTower", "siege_tower"),             # old version of ID 1105 with combat
    936: ("Elephant", "elephant"),
    1001: ("OrganGun", "organ_gun"),
    1004: ("Caravel", "caravel"),
    1007: ("CamelArcher", "camel_archer"),
    1010: ("Genitour", "genitour"),
    1013: ("Gbeto", "gbeto"),
    1016: ("ShotelWarrior", "shotel_warrior"),
    1019: ("Zebra", "zebra"),
    1026: ("Ostrich", "ostrich"),
    1029: ("Lion", "lion"),
    1031: ("Crocodile", "crocodile"),
    1060: ("Goat", "goat"),
    1103: ("FireGalley", "fire_galley"),
    1104: ("DemolitionRaft", "demolition_raft"),
    1105: ("SiegeTower", "siege_tower"),
}

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new buildings of AK
BUILDING_LINE_LOOKUPS = {
    1021: ("Feitoria", "feitoria"),
}

# key: (head) unit id; value: (nyan object name, filename prefix)
# contains only new/changed ambience of AK
AMBIENT_GROUP_LOOKUPS = {
}

# key: index; value: (nyan object name, filename prefix, units belonging to group, variant type)
# contains only new/changed variants of AK
VARIANT_GROUP_LOOKUPS = {
}

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new techs of AK
TECH_GROUP_LOOKUPS = {
    563: ("EliteOrganGun", "elite_organ_gun"),
    565: ("EliteCamelArcher", "elite_camel_archer"),
    567: ("EliteGbeto", "elite_gbeto"),
    569: ("EliteShotelWarrior", "elite_shotel_warrior"),
    572: ("Carrack", "carrack"),
    573: ("Arquebus", "arquebus"),
    574: ("RoyalHeirs", "royal_heirs"),
    575: ("TorsonEngines", "torson_engines"),
    576: ("Tigui", "tigui"),
    577: ("Farimba", "farimba"),
    578: ("Kasbah", "kasbah"),
    579: ("MaghrebiCamels", "maghrebi_camels"),
    597: ("EliteCaravel", "elite_caravel"),
    599: ("EliteGenitour", "elite_genitour"),
    602: ("Arson", "arson"),
    608: ("Arrowslits", "arrowslits"),
}

# key: civ index; value: (nyan object name, filename prefix)
# contains only new civs of AK
CIV_GROUP_LOOKUPS = {
    24: ("Portugese", "portugese"),
    25: ("Ethiopians", "ethiopians"),
    26: ("Malians", "malians"),
    27: ("Berbers", "berbers"),
}

# key: civ index; value: (civ ids, nyan object name, filename prefix)
# contains only new/changed graphic sets of AK
GRAPHICS_SET_LOOKUPS = {
    9: ((25, 26, 27), "African", "african"),
}

# key: terrain index; value: (unit terrain restrictions (manual), nyan object name, filename prefix)
# TODO: Use terrain restrictions from .dat
# contains only new/changed terrains of DE2
TERRAIN_GROUP_LOOKUPS = {
    41: ((0,), "Savannah", "savannah"),
    42: ((0,), "Dirt4", "dirt4"),
    43: ((0,), "Road3", "road3"),
    44: ((0,), "Moorland", "moorland"),
    45: ((0,), "DesertCracked", "desert_cracked"),
    46: ((0,), "DesertQuicksand", "desert_quicksand"),
    47: ((0,), "Black", "black"),
    48: ((0,), "ForestDragon", "forest_dragon"),
    49: ((0,), "ForestBaobab", "forest_baobab"),
    50: ((0,), "ForestAcacia", "forest_acacia"),
}

# key: not relevant; value: (terrain indices, unit terrain restrictions (manual), nyan object name)
# TODO: Use terrain restrictions from .dat
# contains only new/changed terrain types of DE2
TERRAIN_TYPE_LOOKUPS = {
}

# key: armor class; value: Gather ability name
# contains only new armors of AK
ARMOR_CLASS_LOOKUPS = {
    30: "Camel",
    33: "AntiGunpowder",    # TODO: Should not be used as it is a hacky workaround for minimum damage
}
