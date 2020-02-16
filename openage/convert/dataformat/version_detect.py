# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Detects the base version of the game and installed expansions.
"""

import enum
from openage.convert.dataformat.game_info import GameFileVersion
from openage.convert.dataformat.media_types import MediaType


@enum.unique
class Support(enum.Enum):
    """
    Support state of a game version
    """
    nope = "not supported"
    yes = "supported"
    breaks = "presence breaks conversion"


@enum.unique
class GameExpansion(enum.Enum):
    """
    An optional expansion to a GameEdition.
    """

    AFRI_KING = ("Age of Empires 2: HD - African Kingdoms",
                 Support.nope,
                 {GameFileVersion("resources/_common/dat/empires2_x2_p1.dat",
                                  {"f2bf8b128b4bdac36ee36fafe139baf1": "1.0c"})},
                 {MediaType.GRAPHICS: ["resources/_common/slp/"],
                  MediaType.SOUNDS: ["resources/_common/sound/"],
                  MediaType.INTERFACE: ["resources/_common/drs/interface/"],
                  MediaType.TERRAIN: ["resources/_common/terrain/"]},
                 ["aoe2-ak", "aoe2-ak-graphics"])

    def __init__(self, name, support_status, game_file_versions,
                 media_paths, target_modpacks, **flags):
        """
        Create a new GameInfo instance.

        :param name: Name of the game.
        :type name: str
        :param support_status: Whether the converter can read/convert
                               the game to openage formats.
        :type support_status: SupportStatus
        :param game_file_versions: A set of files that is unique to this
                                   version of the game.
        :type game_file_versions: set
        :param media_paths: A dictionary with MediaType as keys and
                            (bool, [str]). bool denotes whether the path
                            is a file that requires extraction. every str is
                            a path to a file or folder.
        :type media_paths: dict
        :param target_modpacks: A list of tuples containing
                                (modpack_name, uid, expected_manifest_hash).
                                These modpacks will be created for this version.
        :type target_modpacks: list
        :param flags: Anything else specific to this version which is useful
                      for the converter. 
        """
        self.expansion_name = name
        self.support = support_status
        self.game_file_versions = game_file_versions
        self.media_paths = media_paths
        self.target_modpacks = target_modpacks
        self.flags = flags


@enum.unique
class GameEdition(enum.Enum):
    """
    Standalone/base version of a game. Multiple standalone versions
    may exist, e.g. AoC, HD, DE2 for AoE2.

    Note that we treat AoE1+Rise of Rome and AoE2+The Conquerors as
    standalone versions. AoE1 without Rise of Rome or AoK without
    The Conquerors are considered "downgrade" expansions.
    """

    AOC = ("Age of Empires 2: The Conqueror's",
           Support.yes,
           {GameFileVersion('age2_x1/age2_x1.exe',
                            {"f2bf8b128b4bdac36ee36fafe139baf1": "1.0c"}),
            GameFileVersion('data/empires2_x1_p1.dat',
                            {"8358c9e64ec0e70e7b13bd34d5a46296": "1.0c"})},
           {MediaType.DATFILE: ["data/empires2_x1_p1.dat"],
            MediaType.GAMEDATA: ["data/gamedata_x1_p1.drs"],
            MediaType.GRAPHICS: ["data/graphics.drs"],
            MediaType.PALETTES: ["data/interfac.drs"],
            MediaType.SOUNDS: ["data/sounds.drs", "data/sounds_x1.drs"],
            MediaType.INTERFACE: ["data/interfac.drs"],
            MediaType.TERRAIN: ["data/terrain.drs"]},
           ["aoe2-base", "aoe2-base-graphics"],
           [])

    def __init__(self, name, support_status, game_file_versions,
                 media_paths, target_modpacks, expansions, **flags):
        """
        Create a new GameEditionInfo instance.

        :param name: Name of the game.
        :type name: str
        :param support_status: Whether the converter can read/convert
                               the game to openage formats.
        :type support_status: SupportStatus
        :param game_file_versions: A set of of files that is
                               unique to this version of the game.
        :type game_file_versions: set
        :param media_paths: A dictionary with MediaType as keys and
                            (bool, [str]). bool denotes whether the path
                            is a file that requires extraction. every str is
                            a path to a file or folder.
        :type media_paths: dict
        :param target_modpacks: A list of tuples containing
                                (modpack_name, uid, expected_manifest_hash).
                                These modpacks will be created for this version.
        :type target_modpacks: list
        :param expansions: A list of expansions available for this edition.
        :type expansion: list
        :param flags: Anything else specific to this version which is useful
                      for the converter. 
        """
        self.edition_name = name
        self.support = support_status
        self.game_file_versions = game_file_versions
        self.media_paths = media_paths
        self.target_modpacks = target_modpacks
        self.expansions = expansions
        self.flags = flags


def get_game_info(srcdir):
    """
    Determine what editions and expansions of a game are installed in srcdir.
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
            break

    for game_expansion in edition.expansions:
        for detection_hints in game_expansion.game_file_versions:
            required_path = detection_hints.get_path()
            required_file = srcdir.joinpath(required_path)

            if not required_file.is_file():
                break

        else:
            expansions.append(game_expansion)

    return edition, expansions
