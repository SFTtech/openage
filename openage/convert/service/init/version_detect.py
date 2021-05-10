# Copyright 2020-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments,too-many-locals,too-many-branches
"""
Detects the base version of the game and installed expansions.
"""

import toml

from ....log import info
from ...value_object.init.game_version import GameEdition, GameExpansion, Support


def iterate_game_versions(srcdir, avail_game_eds, avail_game_exps):
    """
    Determine what editions and expansions of a game are installed in srcdir
    by iterating through all versions the converter knows about.
    """
    edition = None
    expansions = []

    for game_edition in avail_game_eds:
        for detection_hints in game_edition.game_file_versions:
            required_path = detection_hints.get_path()
            required_file = srcdir.joinpath(required_path)

            if not required_file.is_file():
                break

        else:
            edition = game_edition

            if edition.support == Support.nope:
                info(f"Found unsupported game edition: {edition}")
                continue

            break

    else:
        # This will clear found unsupported game editions
        return None, []

    for game_expansion in edition.expansions:
        for existing_game_expansion in avail_game_exps:
            if game_expansion == existing_game_expansion.game_id:
                game_expansion = existing_game_expansion

        for detection_hints in game_expansion.game_file_versions:
            required_path = detection_hints.get_path()
            required_file = srcdir.joinpath(required_path)

            if not required_file.is_file():
                break

        else:
            if game_expansion.support == Support.nope:
                info(f"Found unsupported game expansion: {game_expansion}")
                continue

            expansions.append(game_expansion)

    return edition, expansions


def create_version_objects(srcdir):
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


def create_game_obj(game_dic, aux_path, expansion=False):
    """
    Create a GameEdition or GameExpansion object with the help of
    game_dic map and its version hash file.
    Use expansion parameter to decide if a GameEdition object
    is needed to be created or GameExpansion.
    """
    # initialize necessary parameters
    game_name = game_dic['name']
    game_id = game_dic['game_edition_id']
    support = game_dic['support']
    modpacks = game_dic['targetmod']
    if not expansion:
        expansions = game_dic['expansions']

    flags = {}

    # add mediapaths for the game
    game_mediapaths_list = []
    for media_type in game_dic['mediapaths']:
        game_mediapaths_list.append((media_type, game_dic['mediapaths'][media_type]))

    # add version hashes from the auxiliary file specific for the game
    game_hash_path = aux_path["version_hashes.toml"]
    with game_hash_path.open() as game_hash_toml:
        game_hash_dic = toml.loads(game_hash_toml.read())

    game_hash_list = []
    for item in game_hash_dic.items():
        if item[0] in ['file_version', 'hash_algo']:
            continue
        game_hash_list.append((item[1]['path'], item[1]['map']))

    # Check if there is a media cache file and save the path if it exists
    if aux_path["media_cache.toml"].is_file():
        flags["media_cache"] = aux_path["media_cache.toml"]

    if expansion:
        return GameExpansion(game_name, game_id, support, game_hash_list,
                             game_mediapaths_list, modpacks, **flags)

    return GameEdition(game_name, game_id, support, game_hash_list,
                       game_mediapaths_list, modpacks, expansions, **flags)
