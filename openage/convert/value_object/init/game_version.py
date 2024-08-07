# Copyright 2020-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments

"""
Stores information about base game editions and expansions.
"""
from __future__ import annotations

from dataclasses import dataclass
import enum

from ..read.media_types import MediaType
from .game_file_version import GameFileVersion


@enum.unique
class Support(enum.Enum):
    """
    Support state of a game version
    """
    NOPE = "not supported"
    YES = "supported"
    BREAKS = "presence breaks conversion"


class GameBase:
    """
    Common base class for GameEdition and GameExpansion
    """

    def __init__(
        self,
        game_id: str,
        support: Support,
        game_version_info: list[tuple[list[str], dict[str, str]]],
        media_paths: list[tuple[str, list[str]]],
        modpacks: dict[str, dict[str, str]],
        **flags
    ):
        """
        :param game_id: Unique id for the given game.
        :type game_id: str
        :param support: Whether the converter can read/convert
                               the game to openage formats.
        :type support: str
        :param modpacks: List of modpacks.
        :type modpacks: list
        :param game_version_info: Versioning information about the game.
        :type game_version_info: list[tuple]
        :param media_paths: Media types and their paths
        :type media_paths: list[tuple]
        :param flags: Anything else specific to this version which is useful
                      for the converter.
        """
        self.game_id = game_id
        self.flags = flags
        self.target_modpacks = modpacks
        self.support = Support[support.upper()]

        self.game_file_versions = []
        self.media_paths = {}

        self.media_cache = None
        if "media_cache" in flags:
            self.media_cache = flags["media_cache"]

        for path, hash_map in game_version_info:
            self.add_game_file_versions(path, hash_map)

        for media_type, paths in media_paths:
            self.add_media_paths(media_type, paths)

    def add_game_file_versions(self, filepaths: list[str], hashes: dict[str, str]) -> None:
        """
        Add a GameFileVersion object for files which are unique
        to this version of the game.

        :param filepaths: Paths to the specified file. Only one of the paths
                          needs to exist. The other paths are interpreted as
                          alternatives, e.g. if the game is released on different
                          platforms with different names for the same file.
        :type filepaths: list
        :param hashes: Hash value mapped to a file version.
        :type hashes: dict
        """
        self.game_file_versions.append(GameFileVersion(filepaths, hashes))

    def add_media_paths(self, media_type: str, paths: list[str]) -> None:
        """
        Add a media type with the associated files.

        :param media_type: The type of media file.
        :type media_type: MediaType
        :param paths: Paths to those media files.
        :type paths: list
        """
        self.media_paths[MediaType[media_type.upper()]] = paths

    def __eq__(self, other: GameBase) -> bool:
        """
        Compare equality by comparing IDs.
        """
        return self.game_id == other.game_id

    def __hash__(self) -> int:
        """
        Reimplement hash to only consider the game ID.
        """
        return hash(self.game_id)


class GameExpansion(GameBase):
    """
    An optional expansion to a GameEdition.
    """

    def __init__(
        self,
        name: str,
        game_id: str,
        support: Support,
        game_version_info: list[tuple[list[str], dict[str, str]]],
        media_paths: list[tuple[str, list[str]]],
        modpacks: list[str],
        **flags
    ):
        """
        Create a new GameExpansion instance.

        :param name: Name of the game.
        :type name: str
        """
        super().__init__(
            game_id,
            support,
            game_version_info,
            media_paths,
            modpacks,
            **flags
        )

        self.expansion_name = name

    def __str__(self):
        return self.expansion_name


class GameEdition(GameBase):
    """
    Standalone/base version of a game. Multiple standalone versions
    may exist, e.g. AoC, HD, DE2 for AoE2.

    Note that we treat AoE1+Rise of Rome and AoE2+The Conquerors as
    standalone versions. AoE1 without Rise of Rome or AoK without
    The Conquerors are considered "downgrade" expansions.
    """

    def __init__(
        self,
        name: str,
        game_id: str,
        support: Support,
        game_version_info: list[tuple[list[str], dict[str, str]]],
        media_paths: list[tuple[str, list[str]]],
        install_paths: dict[str, list[str]],
        modpacks: list[str],
        expansions: list[str],
        **flags
    ):
        """
        Create a new GameEdition instance.

        :param name: Name of the game.
        :type name: str
        :param expansions: A list of expansions.
        :type expansion: list
        """
        super().__init__(
            game_id,
            support,
            game_version_info,
            media_paths,
            modpacks,
            **flags
        )

        self.install_paths = install_paths
        self.edition_name = name
        self.expansions = tuple(expansions)

    def __str__(self):
        return self.edition_name


@dataclass(frozen=True)
class GameVersion:
    """
    Combination of edition and expansions that defines the exact version
    of a detected game in a folder.
    """
    edition: GameEdition
    expansions: tuple[GameExpansion, ...] = tuple()
