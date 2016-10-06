# Copyright 2014-2016 the openage authors. See copying.md for legal info.

terrain = {
    "GRASS":        0,
    "WATER":        1,
    "BEACH":        2,
    "SHALLOW":      4,
    "FORREST":      9,
    "DIRT":        12,
    "DEEP_WATER":  15,
    "ROAD":        16,
    "ROAD_RUINED": 17,
    "ICE":         18,
}

# x and y are the placement size

# units
units = {
    "VILLAGER": {"id": 222, "x": 1, "y": 1},
    "SHEEP":    {"id": 594, "x": 1, "y": 1},
}

# buildings
buildings = {
    "TOWNCENTER": {"id": 109, "x": 4, "y": 4},
}

# gaia
gaias = {
    "FORAGE": {"id":  59, "x": 1, "y": 1},
    "GOLD":   {"id":  66, "x": 1, "y": 1},
    "SHEEP":  {"id": 594, "x": 1, "y": 1},
    "STONE":  {"id": 102, "x": 1, "y": 1},
    "TREE1":  {"id": 349, "x": 1, "y": 1},
    "TREE2":  {"id": 351, "x": 1, "y": 1},
}

for unit in units:
    units[unit]["is_building"] = False
    units[unit]["gaia"] = False
    units[unit]["cost"] = 100000

for building in buildings:
    buildings[building]["is_building"] = True
    buildings[building]["gaia"] = False
    buildings[building]["cost"] = 100000

for gaia in gaias:
    gaias[gaia]["is_building"] = False
    gaias[gaia]["gaia"] = True
    gaias[gaia]["cost"] = 50

objects = {}
objects.update(units)
objects.update(buildings)
objects.update(gaias)
