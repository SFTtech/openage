# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments
"""
Detects the base version of the game and installed expansions.
"""

from ...value_object.init.game_version import GameEdition, Support


def iterate_game_versions(srcdir):
    """
    Determine what editions and expansions of a game are installed in srcdir
    by iterating through all versions the converter knows about.
    """
    edition = None
    expansions = []

    for game_edition in GameEdition:
        for detection_hints in game_edition.game_file_versions:
            required_path = detection_hints.get_path()
            required_file = srcdir.joinpath(required_path)

            if not required_file.is_file():
                break

        else:
            edition = game_edition

            if edition.support == Support.nope:
                continue

            break

    else:
        raise Exception("no valid game version found.")

    for game_expansion in edition.expansions:
        for detection_hints in game_expansion.game_file_versions:
            required_path = detection_hints.get_path()
            required_file = srcdir.joinpath(required_path)

            if not required_file.is_file():
                break

        else:
            expansions.append(game_expansion)

    return edition, expansions
