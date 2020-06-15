# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Age of Empires games do not necessarily come with an english
translation. Therefore, we use the strings in this file to
figure out the names for a nyan object.
"""

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new units of Forgotten
UNIT_LINE_LOOKUPS = {
    184: ("Condottiero", "condottiero"),
    185: ("Slinger", "slinger"),
    305: ("Llama", "llama"),
    486: ("Bear", "bear"),
    639: ("Penguin", "penguin"),
    705: ("Cow", "cow"),
    751: ("EagleScout", "eagle_scout"),
    866: ("GenoeseCrossbowman", "genoese_crossbowman"),
    869: ("MagyarHuszar", "magyar_huszar"),
    873: ("ElephantArcher", "elephant_archer"),
    876: ("Boyar", "boyar"),
    879: ("Kamayuk", "kamayuk"),
}

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new buildings of Forgotten
BUILDING_LINE_LOOKUPS = {
    789: ("PalisadeGate", "palisade_gate"),
}

# key: (head) unit id; value: (nyan object name, filename prefix)
# contains only new/changed ambience of Forgotten
AMBIENT_GROUP_LOOKUPS = {
}

# key: index; value: (nyan object name, filename prefix, units belonging to group, variant type)
# contains only new/changed variants of Forgotten
VARIANT_GROUP_LOOKUPS = {
}
