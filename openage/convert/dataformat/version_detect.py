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

    AFRI_KING = ("African Kingdoms (HD)",
                 Support.nope,
                 {GameFileVersion("resources/_common/dat/empires2_x2_p1.dat",
                                  {"f2bf8b128b4bdac36ee36fafe139baf1": "1.0c"})},
                 {
                     MediaType.GRAPHICS: ["resources/_common/slp/"],
                     MediaType.SOUNDS: ["resources/_common/sound/"],
                     MediaType.INTERFACE: ["resources/_common/drs/interface/"],
                     MediaType.TERRAIN: ["resources/_common/terrain/"]
                 },
                 ["aoe2-ak", "aoe2-ak-graphics"])

    SWGB_CC = ("Clone Campaigns",
               Support.yes,
               {GameFileVersion('Game/battlegrounds_x1.exe',
                                {"974f4d4404bb94451a9c27ae5c673243": "GOG"})},
               {
                   MediaType.DATFILE: ["Game/Data/genie_x1.dat"],
                   MediaType.GAMEDATA: ["Game/Data/genie_x1.dat"],
                   MediaType.GRAPHICS: ["Game/Data/graphics_x1.drs"],
                   MediaType.LANGUAGE: ["Game/language_x1.dll"],
                   MediaType.PALETTES: ["Game/Data/interfac_x1.drs"],
                   MediaType.SOUNDS: ["Game/Data/sounds_x1.drs"],
                   MediaType.INTERFACE: ["Game/Data/interfac_x1.drs"],
                   MediaType.TERRAIN: ["Game/Data/terrain_x1.drs"],
               },
               ["swgb-cc", "swgb-cc-graphics"])

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

    ROR = (
        "Age of Empires 1: Rise of Rome",
        Support.yes,
        {
            GameFileVersion('EMPIRESX.EXE',
                            {"140bda90145182966acf582b28a4c8ef": "1.0B"}),
            GameFileVersion('data2/empires.dat',
                            {"3e567b2746653107cf80bae18c6962a7": "1.0B"}),
        },
        {
            MediaType.DATFILE: ["data2/empires.dat"],
            MediaType.GRAPHICS: ["data/graphics.drs", "data2/graphics.drs"],
            MediaType.PALETTES: ["data/Interfac.drs", "data2/Interfac.drs"],
            MediaType.SOUNDS: ["data/sounds.drs", "data2/sounds.drs"],
            MediaType.INTERFACE: ["data/Interfac.drs", "data2/Interfac.drs"],
            MediaType.LANGUAGE: ["language.dll", "languagex.dll"],
            MediaType.TERRAIN: ["data/Terrain.drs"],
            MediaType.BORDER: ["data/Border.drs"],
        },
        ["aoe1-base", "aoe1-base-graphics"],
        []
    )

    AOE1DE = (
        "Age of Empires 1: Definitive Edition (Steam)",
        Support.yes,
        {
            GameFileVersion('AoEDE_s.exe',
                            {"0b652f0821cc0796a6a104bffc69875e": "steam"}),
            GameFileVersion('Data/empires.dat',
                            {"0b652f0821cc0796a6a104bffc69875e": "steam"})},
        {
            MediaType.DATFILE: ["Data/empires.dat"],
            MediaType.GRAPHICS: ["Assets/SLP/"],
            MediaType.PALETTES: ["Assets/Palettes/"],
            MediaType.SOUNDS: ["Assets/Sounds/"],
            MediaType.INTERFACE: ["Data/DRS/interfac.drs", "Data/DRS/interfac_x1.drs"],
        },
        ["aoe1-base", "aoe1-base-graphics"],
        []
    )

    AOK = (
        "Age of Empires 2: Age of Kings",
        Support.nope,
        {
            GameFileVersion('empires2.exe',
                            {"5f7ca6c7edeba075c7982714619bc66b": "2.0a"}),
            GameFileVersion('data/empires2.dat',
                            {"89ff818894b69040ebd1657d8029b068": "2.0a"})},
        {
            MediaType.DATFILE: ["data/empires2.dat"],
            MediaType.GAMEDATA: ["data/gamedata.drs"],
            MediaType.GRAPHICS: ["data/graphics.drs"],
            MediaType.PALETTES: ["data/interfac.drs"],
            MediaType.SOUNDS: ["data/sounds.drs"],
            MediaType.INTERFACE: ["data/interfac.drs"],
            MediaType.TERRAIN: ["data/terrain.drs"],
        },
        [],
        []
    )

    AOC = (
        "Age of Empires 2: The Conqueror's",
        Support.yes,
        {
            GameFileVersion('age2_x1/age2_x1.exe',
                            {"f2bf8b128b4bdac36ee36fafe139baf1": "1.0c"}),
            GameFileVersion('data/empires2_x1_p1.dat',
                            {"8358c9e64ec0e70e7b13bd34d5a46296": "1.0c"})},
        {
            MediaType.DATFILE: ["data/empires2_x1_p1.dat"],
            MediaType.GAMEDATA: ["data/gamedata_x1_p1.drs"],
            MediaType.GRAPHICS: ["data/graphics.drs"],
            MediaType.LANGUAGE: ["language.dll", "language_x1.dll", "language_x1_p1.dll"],
            MediaType.PALETTES: ["data/interfac.drs"],
            MediaType.SOUNDS: ["data/sounds.drs", "data/sounds_x1.drs"],
            MediaType.INTERFACE: ["data/interfac.drs"],
            MediaType.TERRAIN: ["data/terrain.drs"],
            MediaType.BLEND: ["data/blendomatic.dat"],
        },
        ["aoe2-base", "aoe2-base-graphics"],
        []
    )

    HDEDITION = (
        "Age of Empires 2: HD Edition",
        Support.yes,
        {
            GameFileVersion('AoK HD.exe',
                            {"ca2d6c1e26e8900a9a3140ba2e12e4c9": "5.8"}),
            GameFileVersion('resources/_common/dat/empires2_x1_p1.dat',
                            {"6f5a83789ec3dc0fd92986294d03031f": "5.8"})},
        {
            MediaType.DATFILE: ["resources/_common/dat/empires2_x1_p1.dat"],
            MediaType.GAMEDATA: ["resources/_common/drs/gamedata_x1/"],
            MediaType.GRAPHICS: ["resources/_common/drs/graphics/"],
            MediaType.PALETTES: ["resources/_common/drs/interface/"],
            MediaType.SOUNDS: ["resources/_common/drs/sounds/"],
            MediaType.INTERFACE: ["resources/_common/drs/interface/"],
            MediaType.TERRAIN: ["resources/_common/drs/terrain/"],
        },
        ["aoe2-base", "aoe2-base-graphics"],
        []
    )

    AOE2DE = (
        "Age of Empires 2: Definitive Edition",
        Support.yes,
        {
            GameFileVersion('AoE2DE_s.exe',
                            {"8771d2380f8637efb407d09198167c12": "release"}),
            GameFileVersion('resources/_common/dat/empires2_x2_p1.dat',
                            {"1dd581c6b06e2615c1a0ed8069f5eb13": "release"})},
        {
            MediaType.DATFILE: ["resources/_common/dat/empires2_x2_p1.dat"],
            MediaType.GAMEDATA: ["resources/_common/drs/gamedata_x1/"],
            MediaType.GRAPHICS: ["resources/_common/drs/graphics/"],
            MediaType.LANGUAGE: [
                "resources/br/strings/key-value/key-value-strings-utf8.txt",
                "resources/de/strings/key-value/key-value-strings-utf8.txt",
                "resources/en/strings/key-value/key-value-strings-utf8.txt",
                "resources/es/strings/key-value/key-value-strings-utf8.txt",
                "resources/fr/strings/key-value/key-value-strings-utf8.txt",
                "resources/hi/strings/key-value/key-value-strings-utf8.txt",
                "resources/it/strings/key-value/key-value-strings-utf8.txt",
                "resources/jp/strings/key-value/key-value-strings-utf8.txt",
                "resources/ko/strings/key-value/key-value-strings-utf8.txt",
                "resources/ms/strings/key-value/key-value-strings-utf8.txt",
                "resources/mx/strings/key-value/key-value-strings-utf8.txt",
                "resources/ru/strings/key-value/key-value-strings-utf8.txt",
                "resources/tr/strings/key-value/key-value-strings-utf8.txt",
                "resources/tw/strings/key-value/key-value-strings-utf8.txt",
                "resources/vi/strings/key-value/key-value-strings-utf8.txt",
                "resources/zh/strings/key-value/key-value-strings-utf8.txt",
            ],
            MediaType.PALETTES: ["resources/_common/palettes/"],
            MediaType.SOUNDS: ["wwise/"],
            MediaType.INTERFACE: ["resources/_common/drs/interface/"],
            MediaType.TERRAIN: ["resources/_common/terrain/textures/"],
        },
        ["aoe2-base", "aoe2-base-graphics"],
        []
    )

    SWGB = (
        "Star Wars: Galactic Battlegrounds",
        Support.yes,
        {
            GameFileVersion('Game/Battlegrounds.exe',
                            {"6ad181133823a72f046c75a649cf8124": "GOG"}),
            GameFileVersion('Game/Data/GENIE.DAT',
                            {"2e2704e8fcf9e9fd0ee2147209ad6617": "GOG"})},
        {
            MediaType.DATFILE: ["Game/Data/GENIE.DAT"],
            MediaType.GAMEDATA: ["Game/Data/GAMEDATA.DRS"],
            MediaType.GRAPHICS: ["Game/Data/GRAPHICS.DRS"],
            MediaType.LANGUAGE: ["Game/language.dll"],
            MediaType.PALETTES: ["Game/Data/INTERFAC.DRS"],
            MediaType.SOUNDS: ["Game/Data/SOUNDS.DRS"],
            MediaType.INTERFACE: ["Game/Data/INTERFAC.DRS"],
            MediaType.TERRAIN: ["Game/Data/TERRAIN.DRS"],
            MediaType.BLEND: ["Game/Data/blendomatic.dat"],
        },
        ["swgb-base", "swgb-base-graphics"],
        [GameExpansion.SWGB_CC]
    )

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
