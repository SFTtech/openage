# Copyright 2020-2022 the openage authors. See copying.md for legal info.
#
# pylint: disable=line-too-long

"""
Age of Empires games do not necessarily come with an english
translation. Therefore, we use the strings in this file to
figure out the names for a nyan object.
"""

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new units of DE2
UNIT_LINE_LOOKUPS = {
    705: ("Cow", "cow"),
    1225: ("KonnikCastle", "konnik"),                 # Castle unit
    1228: ("Keshik", "keshik"),
    1231: ("Kipchak", "kipchak"),
    1234: ("Leitis", "leitis"),
    1239: ("Ibex", "ibex"),
    1243: ("Goose", "goose"),
    1245: ("Pig", "pig"),
    1254: ("KonnikKrepost", "konnik"),                 # Krepost unit
    1258: ("Ram", "ram"),                       # replacement for ID 35?
    1263: ("FlamingCamel", "flaming_camel"),

    # LOTW
    1370: ("SteppeLancer", "steppe_lancer"),
    1655: ("Coustillier", "coustillier"),
    1658: ("Serjeant", "serjeant"),
    1660: ("DSerjeant", "dserjeant"),           # Donjon Serjeant
    # 1663: ("FlemishMilitia", "flemish_militia"),
    1699: ("FlemishMilitia", "flemish_militia"),

    # DOTD
    1701: ("Obuch", "obuch"),
    1704: ("HussiteWagon", "hussite_wagon"),
    1709: ("Houfnice", "houfnice"),

    # DOI
    1735: ("UrumiSwordsman", "urumi_swordsman"),
    1738: ("MRatha", "mratha"),                 # Melee Rhata
    1741: ("ChakramThrower", "chakram_thrower"),
    1744: ("ArmoredElephant", "armored_elephant"),
    1747: ("Ghulam", "ghulam"),
    1750: ("Thirisadai", "thirisadai"),
    1751: ("ShrivamshaRider", "shrivamsha_rider"),
    1755: ("CamelScout", "camel_scout"),        # technically in line with Camel rider
    1759: ("RRatha", "rratha"),                 # Ranged Rhata

    # ROR
    1790: ("Centurion", "centurion"),
    1795: ("Dromon", "dromon"),

    # TODO: These are upgrades
    1737: ("EliteUrumiSwordsman", "elite_urumi_swordsman"),
    1743: ("EliteChakramThrower", "elite_chakram_thrower"),
    1746: ("SiegeElephant", "siege_elephant"),
    1749: ("EliteGhumlam", "elite_ghumlam"),
    1753: ("EliteShrivamshaRider", "elite_shrivamsha_rider"),
    1761: ("EliteRRatha", "elite_rratha"),
    1792: ("EliteCenturion", "elite_centurion"),
    1793: ("Legionary", "legionary"),
}

# key: head unit id; value: (nyan object name, filename prefix)
# contains only new buildings of DE2
BUILDING_LINE_LOOKUPS = {
    1251: ("Krepost", "krepost"),

    # LOTW
    1665: ("Donjon", "donjon"),

    # DOTD
    1734: ("Folwark", "folwark"),

    # DOI
    1754: ("Caravanserai", "caravanserai"),
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
    678: ("EliteKonnik", "elite_konnik"),
    680: ("EliteKeshik", "elite_keshik"),
    682: ("EliteKipchak", "elite_kipchak"),
    684: ("EliteLeitis", "elite_leitis"),
    685: ("Stirrups", "stirrups"),
    686: ("Bagains", "bagains"),
    687: ("SilkArmor", "silk_armor"),
    688: ("TimuridSiegecraft", "timurid_siegecraft"),
    689: ("SteppeHusbandry", "steppe_husbandry"),
    690: ("CumanMercanaries", "cuman_mercinaries"),
    691: ("HillForts", "hill_forts"),
    692: ("TowerShields", "tower_shields"),
    715: ("EliteSteppeLancer", "elite_steppe_lancer"),
    716: ("Supplies", "supplies"),

    # LOTW
    751: ("EliteCoustillier", "elite_coustillier"),
    753: ("EliteSerjeant", "elite_serjeant"),
    754: ("BurgundianVineyatds", "burgundian_vineyards"),
    755: ("FlemishRevolution", "flemish_revolution"),
    756: ("FirstCrusade", "first_crusade"),
    757: ("Scutage", "scutage"),

    # DOTD
    779: ("EliteObuch", "elite_obuch"),
    781: ("EliteHussiteWagon", "elite_hussite_wagon"),
    782: ("SzlachtaPriviledges", "slzachte_priviledges"),
    783: ("LechiticLegacy", "lechitic_legacy"),
    784: ("WagenburgTactics", "wagenburg_tactics"),
    785: ("HussiteReforms", "hussite_reforms"),
    786: ("WingedHussar", "winged_hussar"),
    787: ("Houfnice", "houfnice"),
    793: ("Folwark", "folwark"),

    # DOI
    828: ("EliteRatha", "elite_ratha"),
    830: ("EliteChakramThrower", "elite_chakram_thrower"),
    831: ("MedicalCorps", "medical_corps"),
    832: ("WootzSteel", "wootz_steel"),
    833: ("Paiks", "paiks"),
    834: ("Mahayana", "mahayana"),
    835: ("Kshatriyas", "kshatriyas"),
    836: ("FrontierGuards", "frontier_guards"),
    838: ("SiegeElephant", "siege_elephant"),
    840: ("EliteGhulam", "elite_ghulam"),
    843: ("EliteSHrivamshaRider", "elite_shrivamsha_rider"),

    # ROR
    882: ("EliteCenturion", "elite_centurion"),
    883: ("Ballistas", "ballistas"),
    884: ("Comitatensis", "comitatensis"),
    885: ("Legionary", "legionary"),
}

# key: civ index; value: (nyan object name, filename prefix)
# contains only new civs of DE2
CIV_GROUP_LOOKUPS = {
    32: ("Bulgarians", "bulgarians"),
    33: ("Tatars", "tatars"),
    34: ("Cumans", "cumans"),
    35: ("Lithuanians", "lithuanians"),

    # LOTW
    36: ("Burgundians", "burgundians"),
    37: ("Sicilians", "sicilians"),

    # DOTD
    38: ("Poles", "poles"),
    39: ("Bohemians", "bohemians"),

    # DOI
    20: ("Hindustanis", "hindustanis"),     # Indians in HD
    40: ("Dravidians", "dravidians"),
    41: ("Bengalis", "bengalis"),
    42: ("Gurjaras", "gurjaras"),

    # ROR
    43: ("Romans", "romans"),
}

# key: civ index; value: (civ ids, nyan object name, filename prefix)
# contains only new/changed graphic sets of DE2
GRAPHICS_SET_LOOKUPS = {
    0: ((0, 1, 2, 13, 14, 36), "WesternEuropean", "western_european"),
    4: ((7, 37), "Byzantine", "byzantine"),
    6: ((19, 24, 43), "Mediterranean", "mediterranean"),
    7: ((20, 40, 41, 42), "Indian", "indian"),
    8: ((22, 23, 32, 35, 38, 39), "EasternEuropean", "eastern_european"),
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
    0: "Wonder",
    31: "AntiLetis",
    32: "DE2Condottiero",
    34: "DE2FishingShip",
    35: "DE2Mameluke",
    36: "DE2Hero",
    37: "SiegeBallista",
    38: "DE2Skirmisher",
    39: "DE2CamelRider",
}
