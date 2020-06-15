# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Age of Empires games do not necessarily come with an english
translation. Therefore, we use the strings in this file to
figure out the names for a nyan object.
"""

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new units of AK
UNIT_LINE_LOOKUPS = {
    527: ("DemolitionRaft", "demolition_raft"),
    583: ("Genitour", "genitour"),
    885: ("SiegeTower", "siege_tower"),             # old version of ID 1105 with combat
    936: ("Elephant", "elephant"),
    1001: ("OrganGun", "organ_gun"),
    1004: ("Caravel", "caravel"),
    1007: ("CamelArcher", "camel_archer"),
    1013: ("Gbeto", "gbeto"),
    1016: ("ShotelWarrior", "shotel_warrior"),
    1019: ("Zebra", "zebra"),
    1026: ("Ostrich", "ostrich"),
    1029: ("Lion", "lion"),
    1031: ("Crocodile", "crocodile"),
    1060: ("Goat", "goat"),
    1103: ("FireGalley", "fire_galley"),
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
