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

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new techs of Forgotten
TECH_GROUP_LOOKUPS = {
    65: ("Gillnets", "gillnets"),
    384: ("EagleWarrior", "eagle_warrior"),
    460: ("Atlatl", "atlatl"),
    461: ("Warwolf", "warwolf"),
    462: ("GreatWall", "great_wall"),
    463: ("Chieftains", "chieftains"),
    464: ("GreekFire", "greek_fire"),
    468: ("EliteGenoeseCrossbowman", "elite_genoese_crossbowman"),
    472: ("EliteMagyarHuszar", "elite_magyar_huszar"),
    481: ("EliteElephantArcher", "elite_elephant_archer"),
    482: ("Stronghold", "stronghold"),
    483: ("Marauders", "marauders"),
    484: ("Yasama", "yasama"),
    485: ("ObsidanArrows", "obsidian_arrows"),
    486: ("Panokseon", "panokseon"),
    487: ("Nomads", "nomads"),
    # TODO: Boiling oil
    489: ("Ironclad", "ironclad"),
    490: ("Madrasah", "madrasah"),
    491: ("Sipahi", "sipahi"),
    492: ("Inquisition", "inquisition"),
    493: ("Chivalry", "chivalry"),
    494: ("Pavise", "pavise"),
    499: ("SilkRoad", "silk_road"),
    504: ("EliteBoyar", "elite_boyar"),
    506: ("Sultans", "sultans"),
    507: ("Shatagni", "shatagni"),
    509: ("EliteKamayuk", "elite_kamayuk"),
    512: ("Orthodoxy", "orthodoxy"),
    513: ("Druzhina", "druzhina"),
    514: ("CorvinianArmy", "corvinian_army"),
    515: ("RecurveBow", "recurve_bow"),
    516: ("AndeanSling", "andean_sling"),
    517: ("FabricShields", "fabric_shields"),           # previously called Couriers
    521: ("ImperialCamelRider", "imperial_camel_rider"),
}

# key: civ index; value: (nyan object name, filename prefix)
# contains only new civs of Forgotten
CIV_GROUP_LOOKUPS = {
    19: ("Italians", "italians"),
    20: ("Indians", "indians"),
    21: ("Incas", "incas"),
    22: ("Magyars", "magyars"),
    23: ("Slavs", "slavs"),
}

# key: civ index; value: (civ ids, nyan object name, filename prefix)
# contains only new/changed graphic sets of Forgotten
GRAPHICS_SET_LOOKUPS = {
    6: ((19, 24), "Mediterranean", "mediterranean"),
    7: ((20,), "Indian", "indian"),
    8: ((22, 23), "Slavic", "slavic"),
}

# key: armor class; value: Gather ability name
# contains only new armors of Forgotten
ARMOR_CLASS_LOOKUPS = {
    14: "AnimalPredator",
    23: "Gunpowder",
}
