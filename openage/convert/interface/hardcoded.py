# Copyright 2016-2017 the openage authors. See copying.md for legal info.

""" Additional hardcoded information about user interface assets """


INGAME_HUD_BACKGROUNDS = [
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

INGAME_HUD_BACKGROUNDS_SET = set(INGAME_HUD_BACKGROUNDS)

ASSETS = {
    '50721': 'hudactions'
}

TOP_STRIP_PATTERN_CORNERS = (400, 0, 464, 32)
TOP_STRIP_PATTERN_SEARCH_AREA_CORNERS = (400, 0, 1024, 32)

MID_STRIP_PATTERN_CORNERS = (339, 806, 403, 818)
MID_STRIP_PATTERN_SEARCH_AREA_CORNERS = (339, 806, 850, 818)

KNOWN_SUBTEX_CORNER_COORDS = [
    (0, 806, 339, 1024),
    (850, 806, 1280, 1024),
    (8, 10, 34, 27),
    (85, 10, 111, 27),
    (162, 10, 188, 27),
    (239, 10, 265, 27),
    (316, 10, 342, 27),
]


def ingame_hud_background_index(idx):
    """
    Index in the hardcoded list of the known ingame hud backgrounds to match the civ.
    """
    return INGAME_HUD_BACKGROUNDS.index(int(idx))


def is_ingame_hud_background(idx):
    """
    True if in the hardcoded list of the known ingame hud backgrounds.
    """
    return int(idx) in INGAME_HUD_BACKGROUNDS_SET
