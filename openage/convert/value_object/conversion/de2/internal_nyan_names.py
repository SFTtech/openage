# Copyright 2020-2024 the openage authors. See copying.md for legal info.
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

    # TMR
    1800: ("CompositeBowman", "composite_bowman"),
    1803: ("Monaspa", "monaspa"),
    1811: ("WarriorPriest", "warrior_priest"),
    1813: ("Savar", "savar"),
    1817: ("QizilbashWarrior", "qizilbash_warrior"),

    # TODO: These are upgrades
    1737: ("EliteUrumiSwordsman", "elite_urumi_swordsman"),
    1743: ("EliteChakramThrower", "elite_chakram_thrower"),
    1746: ("SiegeElephant", "siege_elephant"),
    1749: ("EliteGhumlam", "elite_ghumlam"),
    1753: ("EliteShrivamshaRider", "elite_shrivamsha_rider"),
    1761: ("EliteRRatha", "elite_rratha"),
    1792: ("EliteCenturion", "elite_centurion"),
    1793: ("Legionary", "legionary"),

    1802: ("EliteCompositeBowman", "elite_composite_bowman"),
    1805: ("EliteMonaspa", "elite_monaspa"),

    # BfG
    2101: ("BfGUnkown_2101", "bfg_unkown_2101"),
    2102: ("BfGUnkown_2102", "bfg_unkown_2102"),
    2104: ("BfGUnkown_2104", "bfg_unkown_2104"),
    2105: ("BfGUnkown_2105", "bfg_unkown_2105"),
    2107: ("BfGUnkown_2107", "bfg_unkown_2107"),
    2108: ("BfGUnkown_2108", "bfg_unkown_2108"),
    2110: ("BfGUnkown_2110", "bfg_unkown_2110"),
    2111: ("BfGUnkown_2111", "bfg_unkown_2111"),
    2123: ("BfGUnkown_2123", "bfg_unkown_2123"),
    2124: ("BfGUnkown_2124", "bfg_unkown_2124"),
    2125: ("BfGUnkown_2125", "bfg_unkown_2125"),
    2126: ("BfGUnkown_2126", "bfg_unkown_2126"),
    2127: ("BfGUnkown_2127", "bfg_unkown_2127"),
    2128: ("BfGUnkown_2128", "bfg_unkown_2128"),
    2129: ("BfGUnkown_2129", "bfg_unkown_2129"),
    2130: ("BfGUnkown_2130", "bfg_unkown_2130"),
    2131: ("BfGUnkown_2131", "bfg_unkown_2131"),
    2132: ("BfGUnkown_2132", "bfg_unkown_2132"),
    2133: ("BfGUnkown_2133", "bfg_unkown_2133"),
    2134: ("BfGUnkown_2134", "bfg_unkown_2134"),
    2135: ("BfGUnkown_2135", "bfg_unkown_2135"),
    2138: ("BfGUnkown_2138", "bfg_unkown_2138"),
    2139: ("BfGUnkown_2139", "bfg_unkown_2139"),
    2140: ("BfGUnkown_2140", "bfg_unkown_2140"),
    2148: ("BfGUnkown_2148", "bfg_unkown_2148"),
    2149: ("BfGUnkown_2149", "bfg_unkown_2149"),
    2150: ("BfGUnkown_2150", "bfg_unkown_2150"),
    2151: ("BfGUnkown_2151", "bfg_unkown_2151"),
    2162: ("BfGUnkown_2162", "bfg_unkown_2162"),
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

    # TMR
    1808: ("MuleCart", "mule_cart"),

    # BfG
    2119: ("BfGUnkown_2119", "bfg_unkown_2119"),
    2172: ("BfGUnkown_2172", "bfg_unkown_2172"),
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
    46: ("Devotion", "devotion"),

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

    875: ("Gambesons", "gambesons"),

    # ROR
    882: ("EliteCenturion", "elite_centurion"),
    883: ("Ballistas", "ballistas"),
    884: ("Comitatensis", "comitatensis"),
    885: ("Legionary", "legionary"),

    # TMR
    918: ("EliteCompositeBowman", "elite_composite_bowman"),
    920: ("EliteMonaspa", "elite_monaspa"),
    921: ("Fereters", "fereters"),
    922: ("CilicianFleet", "cilician_fleet"),
    923: ("SvanTowers", "svan_towers"),
    924: ("AsnuariCavalry", "asnuari_cavalry"),
    929: ("FortifiedChurch", "fortified_church"),
    967: ("EliteQizilbashWarrior", "elite_qizilbash_warrior"),

    # BfG
    1110: ("BfGUnkown_1110", "bfg_unkown_1110"),
    1111: ("BfGUnkown_1111", "bfg_unkown_1111"),
    1112: ("BfGUnkown_1112", "bfg_unkown_1112"),
    1113: ("BfGUnkown_1113", "bfg_unkown_1113"),
    1120: ("BfGUnkown_1120", "bfg_unkown_1120"),
    1121: ("BfGUnkown_1121", "bfg_unkown_1121"),
    1122: ("BfGUnkown_1122", "bfg_unkown_1122"),
    1123: ("BfGUnkown_1123", "bfg_unkown_1123"),
    1130: ("BfGUnkown_1130", "bfg_unkown_1130"),
    1131: ("BfGUnkown_1131", "bfg_unkown_1131"),
    1132: ("BfGUnkown_1132", "bfg_unkown_1132"),
    1133: ("BfGUnkown_1133", "bfg_unkown_1133"),
    1161: ("BfGUnkown_1161", "bfg_unkown_1161"),
    1162: ("BfGUnkown_1162", "bfg_unkown_1162"),
    1165: ("BfGUnkown_1165", "bfg_unkown_1165"),
    1167: ("BfGUnkown_1167", "bfg_unkown_1167"),
    1173: ("BfGUnkown_1173", "bfg_unkown_1173"),
    1198: ("BfGUnkown_1198", "bfg_unkown_1198"),
    1202: ("BfGUnkown_1202", "bfg_unkown_1202"),
    1203: ("BfGUnkown_1203", "bfg_unkown_1203"),
    1204: ("BfGUnkown_1204", "bfg_unkown_1204"),
    1223: ("BfGUnkown_1223", "bfg_unkown_1223"),
    1224: ("BfGUnkown_1224", "bfg_unkown_1224"),
    1225: ("BfGUnkown_1225", "bfg_unkown_1225"),
    1226: ("BfGUnkown_1226", "bfg_unkown_1226"),
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

    # TMR
    44: ("Armenians", "armenians"),
    45: ("Georgians", "georgians"),

    # BfG
    46: ("BfGUnkown_46", "bfg_unkown_46"),
    47: ("BfGUnkown_47", "bfg_unkown_47"),
    48: ("BfGUnkown_48", "bfg_unkown_48"),
}

# key: civ index; value: (civ ids, nyan object name, filename prefix)
# contains only new/changed graphic sets of DE2
GRAPHICS_SET_LOOKUPS = {
    0: ((0, 1, 2, 13, 14, 36), "WesternEuropean", "western_european"),
    4: ((7, 37), "Byzantine", "byzantine"),
    6: ((19, 24, 43, 44, 45, 46, 47, 48), "Mediterranean", "mediterranean"),
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
    40: "BfGUnkown_40",
    60: "BfGUnkown_60",
}
