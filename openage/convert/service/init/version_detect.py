# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments
"""
Detects the base version of the game and installed expansions.
"""

import toml

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
                continue

            break

    else:
        raise Exception("no valid game version found.")

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
    game_edition_path = srcdir.joinpath("game_edition.toml")
    game_expansion_path = srcdir.joinpath("game_expansion.toml")
    version_hashes_path = srcdir.joinpath("version_hashes")

    # load toml config files to a dictionary variable
    game_edition_dic = toml.load(game_edition_path)
    game_expansion_dic = toml.load(game_expansion_path)

    # create and list GameEdition objects
    for game in game_edition_dic:
        if game == 'file_version':
            continue

        game_obj = create_game_obj(game_edition_dic[game], version_hashes_path)
        game_edition_list.append(game_obj)

    # create and list GameExpansion objects
    for game in game_expansion_dic:
        if game == 'file_version':
            continue

        game_obj = create_game_obj(game_expansion_dic[game], version_hashes_path, True)
        game_expansion_list.append(game_obj)

    return game_edition_list, game_expansion_list


def create_game_obj(game_dic, version_hashes_path, expansion=False):
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

    # add version-hashes from the auxiliary file specific for the game
    game_hash_dic = toml.load(version_hashes_path.joinpath(game_id + '.toml'))
    game_hash_list = []
    for item in game_hash_dic:
        if item in ['file_version', 'hash_algo']:
            continue
        game_hash_list.append((item['path'], item['map']))

    # add mediapaths for the game
    game_mediapaths_list = []
    for media_type in game_dic['mediapaths']:
        game_mediapaths_list.append((media_type, game_dic['mediapaths'][media_type]))

    if expansion:
        return GameExpansion(game_name, game_id, support, game_hash_list,
                             game_mediapaths_list, modpacks)

    return GameEdition(game_name, game_id, support, game_hash_list,
                       game_mediapaths_list, modpacks, expansions)
