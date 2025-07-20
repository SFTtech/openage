# Copyright 2020-2025 the openage authors. See copying.md for legal info.
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
    2101: ("Immortal", "immortal"),
    2104: ("Strategos", "strategos"),
    2107: ("Hippeus", "hippeus"),
    2110: ("Hoplite", "hoplite"),
    2123: ("Lembos", "lembos"),
    2127: ("Monoreme", "monoreme"),
    2130: ("AntiqueGalley", "antique_galley"),
    2133: ("IncendiaryRaft", "incendiary_raft"),
    2138: ("CatapultShip", "catapult_ship"),
    2148: ("AntiqueTransportShip", "antique_transport_ship"),
    2149: ("MerchantShip", "merchant_ship"),
    2150: ("WarChariot", "war_chariot"),
    2162: ("Polemarch", "polemarch"),

    # TODO: These are upgrades
    2102: ("EliteImmortal", "elite_immortal"),
    2105: ("EliteStrategos", "elite_strategos"),
    2108: ("EliteHippeus", "elite_hippeus"),
    2111: ("EliteHoplite", "elite_hoplite"),
    2124: ("WarLembos", "war_lembos"),
    2125: ("HeavyLembos", "heavy_lembos"),
    2126: ("EliteLembos", "elite_lembos"),
    2128: ("Bireme", "bireme"),
    2129: ("Trieme", "trireme"),
    2131: ("AntiqueWarGalley", "antique_war_galley"),
    2132: ("AntiqueEliteGalley", "antique_elite_galley"),
    2134: ("IncendiaryShip", "incendiary_ship"),
    2135: ("HeavyIncendiaryShip", "heavy_incendiary_ship"),
    2139: ("OnagerShip", "onager_ship"),
    2140: ("Leviathan", "leviathan"),
    2151: ("EliteWarChariot", "elite_war_chariot"),

    # TTK
    1302: ("DragonShip", "dragon_ship"),
    1901: ("FireLancer", "fire_lancer"),
    1904: ("RocketCart", "rocket_cart"),
    1908: ("IronPagoda", "iron_pagoda"),
    1911: ("Grenadier", "grenadier"),
    1920: ("LiaoDao", "liao_dao"),
    1923: ("MountedTrebuchet", "mounted_trebuchet"),
    1942: ("TractionTrebuchet", "traction_trebuchet"),
    1944: ("HeiGuangCavalry", "hei_guang_cavalry"),
    1948: ("LouChuan", "lou_chuan"),
    1949: ("TigerCavalry", "tiger_cavalry"),
    1952: ("XianbeiRaider", "xianbei_raider"),
    1954: ("CaoCao", "cao_cao"),
    1959: ("WhiteFeatherGuard", "white_feather_guard"),
    1962: ("WarChariotFocusFire", "war_chariot_focus_fire"),
    1966: ("LiuBei", "liu_bei"),
    1968: ("FireArcher", "fire_archer"),
    1974: ("JianSwordsman", "jian_swordsman"),
    1978: ("SunJian", "sun_jian"),
    1980: ("WarChariotBarrage", "war_chariot_barrage"),

    # TODO: These are upgrades
    1903: ("EliteFireLancer", "elite_fire_lancer"),
    1907: ("HeavyRocketCart", "heavy_rocket_cart"),
    1910: ("EliteIronPagoda", "elite_iron_pagoda"),
    1922: ("EliteLiaoDao", "elite_liao_dao"),
    1946: ("HeavyHeiGuangCavalry", "heavy_hei_guang_cavalry"),
    1951: ("EliteTigerCavalry", "elite_tiger_cavalry"),
    1961: ("EliteWhiteFeatherGuard", "elite_white_feather_guard"),
    1970: ("EliteFireArcher", "elite_fire_archer"),
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
    2119: ("Shipyard", "shipyard"),
    2172: ("Port", "port"),

    # TTK
    1897: ("Pasture", "pasture"),
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
    1110: ("Sparabaras", "sparabaras"),
    1111: ("Satrapies", "satrapies"),
    1112: ("ScythedChariots", "scythed_chariots"),
    1113: ("Palta", "palta"),
    1120: ("Dekate", "dekate"),
    1121: ("Taxiarchs", "taxiarchs"),
    1122: ("Eisphora", "eisphora"),
    1123: ("MinesOfLaurion", "mines_of_laurion"),
    1130: ("HelotLevies", "helot_levies"),
    1131: ("Xyphos", "xyphos"),
    1132: ("Krypteia", "krypteia"),
    1133: ("PeloponnesianLeague", "peloponnesian_league"),
    1161: ("ScoopNets", "scoop_nets"),
    1162: ("Drums", "drums"),
    1165: ("Hypozomata", "hypozomata"),
    1167: ("Shipwright", "shipwright"),
    1173: ("TwoHandedSwordsman", "two_handed_swordsman"),
    1198: ("SpawnLembosFromPort", "spawn_lembos_from_port"),
    1202: ("Oligarchy", "oligarchy"),
    1203: ("Democracy", "democracy"),
    1204: ("Tyranny", "tyranny"),
    1223: ("Ephorate", "ephorate"),
    1224: ("Morai", "morai"),
    1225: ("Skeuophoroi", "skeuophoroi"),
    1226: ("Hippagretai", "hippagretai"),

    # TTK
    980: ("HeavyRocketCart", "heavy_rocket_cart"),
    982: ("EliteFireLancer", "elite_fire_lancer"),
    991: ("EliteIronPagoda", "elite_iron_pagoda"),
    996: ("FortifiedBastions", "fortified_bastions"),
    997: ("ThunderclapBombs", "thunderclap_bombs"),
    1002: ("EliteLiaoDao", "elite_liao_dao"),
    1006: ("LamellarArmor", "lamellar_armor"),
    1007: ("OrdoCavalry", "ordo_cavalry"),
    1010: ("DragonShip", "dragon_ship"),
    1012: ("Transhumance", "transhumance"),
    1013: ("Pastoralism", "pastoralism"),
    1014: ("Domestication", "domestication"),
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
    46: ("Achaemenids", "achaemenids"),
    47: ("Athenians", "athenians"),
    48: ("Spartans", "spartans"),

    # TTK
    49: ("Shu", "shu"),
    50: ("Wu", "wu"),
    51: ("Wei", "wei"),
    52: ("Jurchens", "jurchens"),
    53: ("Khitans", "khitans"),
}

# key: civ index; value: (civ ids, nyan object name, filename prefix)
# contains only new/changed graphic sets of DE2
GRAPHICS_SET_LOOKUPS = {
    0: ((0, 1, 2, 13, 14, 36), "WesternEuropean", "western_european"),
    2: ((5, 6, 12, 18, 31, 49, 50, 51, 52, 53), "EastAsian", "east_asian"),
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
