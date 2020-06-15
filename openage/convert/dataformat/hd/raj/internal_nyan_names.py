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
