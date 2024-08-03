# Copyright 2020-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments,too-many-locals,too-many-branches
"""
Detects the base version of the game and installed expansions.
"""

from __future__ import annotations

import typing

import toml


from ....log import info, warn, dbg
from ....util.hash import hash_file
from ...value_object.init.game_version import GameEdition, GameExpansion, GameVersion, Support

if typing.TYPE_CHECKING:
    from openage.util.fslike.directory import Directory
    from openage.util.fslike.path import Path


def iterate_game_versions(
    srcdir: Directory,
    avail_game_eds: list[GameEdition],
    avail_game_exps: list[GameExpansion]
) -> GameVersion:
    """
    Determine what editions and expansions of a game are installed in srcdir
    by iterating through all versions the converter knows about.
    """
    best_edition = None
    expansions = []

    for game_edition in avail_game_eds:
        # Check for files that we know exist in the game's folder
        for detection_hints in game_edition.game_file_versions:
            check_paths = detection_hints.get_paths()

            # Check if any of the known paths for the file exists
            found_file = False
            for required_path in check_paths:
                required_file = srcdir.joinpath(required_path)

                if required_file.is_file():
                    hash_val = hash_file(required_file,
                                         hash_algo=detection_hints.hash_algo)

                    if hash_val not in detection_hints.get_hashes():
                        dbg(f"Found required file {required_file.resolve_native_path()} "
                            "but could not determine version number")

                    else:
                        version_no = detection_hints.get_hashes()[hash_val]
                        dbg(f"Found required file {required_file.resolve_native_path()} "
                            f"for version {version_no}")

                    found_file = True
                    break

            if not found_file:
                break

        else:
            # All files were found. Now check if the version is supported.
            if game_edition.support == Support.NOPE:
                dbg(f"Found unsupported game edition: {game_edition}")

                if best_edition is None:
                    best_edition = game_edition

                # Continue to look for supported editions
                continue

            if game_edition.support == Support.BREAKS:
                dbg(f"Found broken game edition: {game_edition}")

                if best_edition is None or best_edition.support == Support.NOPE:
                    best_edition = game_edition

                # Continue to look for supported editions
                continue

            # We found a fully supported edition!
            # No need to check for better editions
            best_edition = game_edition
            break

    else:
        # Either no version or an unsupported or broken was found
        # Return the last detected edition
        return GameVersion(edition=best_edition)

    for game_expansion in best_edition.expansions:
        for existing_game_expansion in avail_game_exps:
            if game_expansion == existing_game_expansion.game_id:
                game_expansion = existing_game_expansion

        # Check for files that we know exist in the game expansion's folder
        for detection_hints in game_expansion.game_file_versions:
            check_paths = detection_hints.get_paths()

            # Check if any of the known paths for the file exists
            found_file = False
            for required_path in check_paths:
                required_file = srcdir.joinpath(required_path)

                if required_file.is_file():
                    found_file = True
                    break

            if not found_file:
                break

        else:
            if game_expansion.support == Support.NOPE:
                info(f"Found unsupported game expansion: {game_expansion}")
                # Continue to look for supported expansions
                continue

            if game_expansion.support == Support.BREAKS:
                info(f"Found broken game expansion: {best_edition}")
                # Continue to look for supported expansions
                continue

            expansions.append(game_expansion)

    return GameVersion(edition=best_edition, expansions=tuple(expansions))


def create_version_objects(srcdir: Directory) -> tuple[list[GameEdition], list[GameExpansion]]:
    """
    Create GameEdition and GameExpansion objects from auxiliary
    config files.
    """
    game_expansion_list = []
    game_edition_list = []

    # initiliaze necessary paths
    game_edition_path = srcdir.joinpath("game_editions.toml")
    game_expansion_path = srcdir.joinpath("game_expansions.toml")

    # load toml config files to a dictionary variable
    with game_edition_path.open() as game_edition_toml:
        game_editions = toml.loads(game_edition_toml.read())

    with game_expansion_path.open() as game_expansion_toml:
        game_expansions = toml.loads(game_expansion_toml.read())

    # create and list GameEdition objects
    game_editions.pop("file_version")
    for game in game_editions:
        aux_path = srcdir[game_editions[game]["subfolder"]]
        game_obj = create_game_obj(game_editions[game], aux_path)
        game_edition_list.append(game_obj)

    # create and list GameExpansion objects
    game_expansions.pop("file_version")
    for game in game_expansions:
        aux_path = srcdir[game_expansions[game]["subfolder"]]
        game_obj = create_game_obj(game_expansions[game], aux_path, True)
        game_expansion_list.append(game_obj)

    return game_edition_list, game_expansion_list


def create_game_obj(
    game_info: dict[str, str],
    aux_path: Path,
    expansion: bool = False
) -> typing.Union[GameEdition, GameExpansion]:
    """
    Create a GameEdition or GameExpansion object from the contents
    of the game_info dictionary and its version hash file.
    Use expansion parameter to decide if a GameEdition object
    is needed to be created or GameExpansion.
    """
    # initialize necessary parameters
    game_name = game_info['name']
    game_id = game_info['game_edition_id']
    support = game_info['support']
    modpacks = game_info['targetmods']
    if not expansion:
        expansions = game_info['expansions']

    flags = {}

    # add mediapaths for the game
    game_mediapaths = []
    for media_type in game_info['mediapaths']:
        game_mediapaths.append(
            (media_type, game_info['mediapaths'][media_type]))

    # add install dirs for the game
    game_installpaths = {}
    if 'installpaths' in game_info:
        game_installpaths = game_info['installpaths']

    # add version hashes from the auxiliary file specific for the game
    game_hash_path = aux_path["version_hashes.toml"]
    with game_hash_path.open() as game_hash_toml:
        game_hashes = toml.loads(game_hash_toml.read())

    file_version = game_hashes.pop("file_version")
    hash_algo = game_hashes.pop("hash_algo")
    if hash_algo != "SHA3-256":
        warn(f"{game_hash_path}: Unrecognized hash algorithm: {hash_algo}")

    game_version_info = []
    for item in game_hashes.items():
        if file_version == "1.0":
            game_version_info.append((
                [item[1]['path']],
                item[1]['map']
            ))

        elif file_version == "2.0":
            game_version_info.append((
                item[1]['paths'],
                item[1]['map']
            ))

        else:
            raise SyntaxError(
                f"{game_hash_path}: Unrecognized file version: '{file_version}'")

    # Check if there is a media cache file and save the path if it exists
    if aux_path["media_cache.toml"].is_file():
        flags["media_cache"] = aux_path["media_cache.toml"]

    if expansion:
        return GameExpansion(game_name, game_id, support, game_version_info,
                             game_mediapaths, modpacks, **flags)

    return GameEdition(game_name, game_id, support, game_version_info,
                       game_mediapaths, game_installpaths, modpacks,
                       expansions, **flags)
