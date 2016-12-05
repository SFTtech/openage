# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""Detect the version of the original game"""

import enum


@enum.unique
class GameVersion(enum.Enum):
    """
    An installed version of the original game. Multiple may coexist (e.g. AoK
    and TC). We should usually distinguish versions only as far as we're
    concerned (i.e. assets changed).
    """
    age2_aok = (
        "Age of Empires 2: The Age of Kings",
        False,
        {'empires2.exe', 'data/empires2.dat'},
    )
    age2_tc = (
        "Age of Empires 2: The Conquerors",
        True,
        {'age2_x1/age2_x1.exe', 'data/empires2_x1.dat'},
    )
    age2_tc_10c = (
        "Age of Empires 2: The Conquerors, Patch 1.0c",
        True,
        {'age2_x1/age2_x1.exe', 'data/empires2_x1_p1.dat'},
    )
    age2_hd_3x = (
        "Age of Empires 2: HD Edition (Version 3.x)",
        True,
        {'AoK HD.exe', 'data/empires2_x1_p1.dat'}
    )
    # HD edition version 4.0
    age2_fe = (
        "Forgotten Empires",
        True,
        {'AoK HD.exe', 'resources/_common/dat/empires2_x1_p1.dat'},
    )
    # HD Edition v4.8 or african kingdoms?
    # TODO Check if the x2 dat file exists in older HD versions, and name the
    # game version accordingly.
    age2_ak = (
        "African Kingdoms",
        True,
        {'AoK HD.exe', 'resources/_common/dat/empires2_x2_p1.dat'},
    )

    def __init__(self, description, openage_supported, required_files=None):
        self.description = description
        self.openage_supported = openage_supported
        self.required_files = required_files or frozenset()

    def __str__(self):
        return self.description


def get_game_versions(srcdir):
    """
    Determine what versions of the game are installed in srcdir. Yield
    GameVersion values.
    """
    for version in GameVersion:
        if all(srcdir.joinpath(path).is_file()
               for path in version.required_files):
            yield version
