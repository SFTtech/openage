# Copyright 2016-2016 the openage authors. See copying.md for legal info.

""" Additional hardcoded information about user interface assets """

import re

INTERFACE_INGAME_HUD_BACKGROUNDS = [
    51141,
    51142,
    51143,
    51144,
    51145,
    51146,
    51147,
    51148,
    51149,
    51150,
    51151,
    51152,
    51153,
    51154,
    51157,
    51158,
    51159,
    51160,
]

TOP_STRIP_PATTERN_CORNERS = (400, 0, 464, 32)

KNOWN_SUBTEX_CORNER_COORDS = [
    (0, 806, 339, 1024),
    (850, 806, 1280, 1024),
    (8, 10, 34, 27),
    (85, 10, 111, 27),
    (162, 10, 188, 27),
    (239, 10, 265, 27),
    (316, 10, 342, 27),
]


def ingame_hud_background_index(filename):
    """
    Index + 1 in the hardcoded list of the known ingame hud backgrounds to match the civ.
    """
    basename = re.sub(r"^interface/(\d+)\.slp$", r'\1', filename)
    return INTERFACE_INGAME_HUD_BACKGROUNDS.index(int(basename)) + 1


def is_ingame_hud_background(filename):
    """
    True if in the hardcoded list of the known ingame hud backgrounds.
    """
    try:
        ingame_hud_background_index(filename)
        return True
    except ValueError:
        return False
