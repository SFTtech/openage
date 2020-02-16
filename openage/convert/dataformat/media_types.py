# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Media types used in games. Media types refer to a group
of file types used in the game.
"""

from enum import Enum


class MediaType(Enum):
    """
    A type of media. Stores the mount point as the value.
    """

    DATFILE   = "data"
    GAMEDATA  = "gamedata"
    GRAPHICS  = "graphics"
    PALETTES  = "palettes"
    TERRAIN   = "terrain"
    SOUNDS    = "sounds"
    INTERFACE = "interface"
