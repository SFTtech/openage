# Copyright 2020-2024 the openage authors. See copying.md for legal info.

"""
Module for reading .dat files.
"""
from __future__ import annotations
import typing

import os
import pickle
from tempfile import gettempdir
from zlib import decompress

from ....log import spam, dbg, info, warn
from ...value_object.read.media.datfile.empiresdat import EmpiresDatWrapper
from ...value_object.read.media_types import MediaType

if typing.TYPE_CHECKING:
    from openage.convert.value_object.init.game_version import GameVersion
    from openage.convert.value_object.read.read_members import ArrayMember
    from openage.util.fslike.directory import Directory
    from openage.util.fslike.wrapper import GuardedFile


def get_gamespec(srcdir: Directory, game_version: GameVersion, pickle_cache: bool) -> ArrayMember:
    """
    Reads empires.dat file.
    """
    if game_version.edition.game_id in ("ROR", "AOE1DE", "AOC", "HDEDITION", "AOE2DE"):
        filepath = srcdir.joinpath(game_version.edition.media_paths[MediaType.DATFILE][0])

    elif game_version.edition.game_id == "SWGB":
        if "SWGB_CC" in [expansion.game_id for expansion in game_version.expansions]:
            filepath = srcdir.joinpath(game_version.expansions[0].media_paths[MediaType.DATFILE][0])

        else:
            filepath = srcdir.joinpath(game_version.edition.media_paths[MediaType.DATFILE][0])

    else:
        raise RuntimeError("No service found for reading data file of "
                           f"version {game_version.edition.game_id}")

    cache_file = os.path.join(
        gettempdir(), f"{game_version.edition.game_id}_{filepath.name}.pickle")

    with filepath.open('rb') as empiresdat_file:
        gamespec = load_gamespec(empiresdat_file,
                                 game_version,
                                 cache_file,
                                 pickle_cache)

    return gamespec


def load_gamespec(
    fileobj: GuardedFile,
    game_version: GameVersion,
    cachefile_name: str = None,
    pickle_cache: bool = False,
    dynamic_load = False
) -> ArrayMember:
    """
    Helper method that loads the contents of a 'empires.dat' gzipped wrapper
    file.

    If cachefile_name is given, this file is consulted before performing the
    load.
    """
    # try to use the cached result from a previous run
    if cachefile_name:
        try:
            with open(cachefile_name, "rb") as cachefile:
                # pickle.load() can fail in many ways, we need to catch all.
                # pylint: disable=broad-except
                try:
                    info("using cached wrapper: %s", cachefile_name)
                    gamespec = pickle.load(cachefile)
                    return gamespec
                except Exception:
                    warn("could not use cached wrapper:")
                    import traceback
                    traceback.print_exc()
                    warn("we will just skip the cache, no worries.")

        except FileNotFoundError:
            pass

    # read the file ourselves

    dbg("reading dat file")
    compressed_data = fileobj.read()
    fileobj.close()

    dbg("decompressing dat file")
    # -15: there's no header, window size is 15.
    file_data = decompress(compressed_data, -15)
    del compressed_data

    spam("length of decompressed data: %d", len(file_data))

    wrapper = EmpiresDatWrapper()
    _, gamespec = wrapper.read(file_data, 0, game_version, dynamic_load=dynamic_load)

    # Remove the list sorrounding the converted data
    gamespec = gamespec[0]
    del wrapper

    if cachefile_name and pickle_cache:
        dbg("dumping dat file contents to cache file: %s", cachefile_name)
        with open(cachefile_name, "wb") as cachefile:
            pickle.dump(gamespec, cachefile)

    return gamespec
