# Copyright 2024-2024 the openage authors. See copying.md for legal info.

"""
Check for updates in the openage converter modpacks.
"""
from __future__ import annotations
import typing

from itertools import chain

from openage.util.version import SemanticVersion

from ....log import info

from .modpack_search import enumerate_modpacks
from ..init.version_detect import create_version_objects


if typing.TYPE_CHECKING:
    from openage.util.fslike.union import UnionPath


def check_updates(modpack_dir: UnionPath, game_info_dir: UnionPath):
    """
    Check if there are updates available for the openage converter modpacks.

    :param modpack_dir: The directory containing the modpacks.
    :param game_info_dir: The directory containing the game information.
    """
    available_modpacks = enumerate_modpacks(modpack_dir, exclude={"engine"})

    game_editions, game_expansions = create_version_objects(game_info_dir)
    for game_def in chain(game_editions, game_expansions):
        for targetmod_name, targetmod_def in game_def.target_modpacks.items():
            if targetmod_name in available_modpacks:
                converter_version = SemanticVersion(targetmod_def["version"])
                modpack_version = SemanticVersion(available_modpacks[targetmod_name])
                if converter_version > modpack_version:
                    info(("Modpack %s v%s is outdated: "
                          "newer version v%s is available"),
                         targetmod_name, modpack_version, converter_version)

                else:
                    info("Modpack %s v%s is up-to-date", targetmod_name, modpack_version)
