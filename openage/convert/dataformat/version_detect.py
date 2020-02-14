# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Detects the base version of the game and installed expansions.
"""

from enum import Enum
from openage.convert.dataformat.game_info import GameEditionInfo,\
    GameExpansionInfo
from openage.convert.dataformat.media_types import MediaType


class Support(Enum):
    """
    Support state of a game version
    """
    nope = "not supported"
    yes = "supported"
    breaks = "presence breaks conversion"


class GameExpansion(Enum):
    """
    An optional expansion to a GameEdition.
    """

    AFRI_KING = GameExpansionInfo("Age of Empires 2: HD - African Kingdoms",
                                  Support.nope,
                                  {("resources/_common/dat/empires2_x2_p1.dat", {})},
                                  {MediaType.GRAPHICS: (False, ["resources/_common/slp/"]),
                                   MediaType.SOUNDS: (False, ["resources/_common/sound/"]),
                                   MediaType.INTERFACE: (False, ["resources/_common/drs/interface/"]),
                                   MediaType.TERRAIN: (False, ["resources/_common/terrain/"])},
                                  ["aoe2-ak", "aoe2-ak-graphics"])


class GameEdition(Enum):
    """
    Standalone/base version of a game. Multiple standalone versions
    may exist, e.g. AoC, HD, DE2 for AoE2.

    Note that we treat AoE1+Rise of Rome and AoE2+The Conquerors as
    standalone versions. AoE1 without Rise of Rome or AoK without
    The Conquerors are considered "downgrade" expansions.
    """

    AOC = GameEditionInfo("Age of Empires 2: The Conqueror's",
                          Support.yes,
                          {('age2_x1/age2_x1.exe', {}),
                           ('data/empires2_x1_p1.dat', {})},
                          {MediaType.GRAPHICS: (True, ["data/graphics.drs"]),
                           MediaType.SOUNDS: (True, ["data/sounds.drs", "data/sounds_x1.drs"]),
                           MediaType.INTERFACE: (True, ["data/interfac.drs"]),
                           MediaType.TERRAIN: (True, ["data/terrain.drs"])},
                          ["aoe2-base", "aoe2-base-graphics"],
                          [])
