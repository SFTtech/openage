# Copyright 2020-2020 the openage authors. See copying.md for legal info.

# pylint: disable=bad-whitespace
# REFA: Whole file -> value object

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
    INTERFACE = "interface"
    LANGUAGE  = "language"
    PALETTES  = "palettes"
    TERRAIN   = "terrain"
    SOUNDS    = "sounds"
    BLEND     = "blend"
    BORDER    = "border"
