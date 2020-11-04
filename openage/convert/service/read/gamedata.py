# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Module for reading .dat files.
"""
import os
import pickle
from tempfile import gettempdir
from zlib import decompress

from ....log import spam, dbg, info, warn
from ...value_object.read.media.datfile.empiresdat import EmpiresDatWrapper
from ...value_object.read.media_types import MediaType


def get_gamespec(srcdir, game_version, dont_pickle):
    """
    Reads empires.dat file.
    """
    if game_version[0] .game_id in ("ROR", "AOC", "AOE2DE"):
        filepath = srcdir.joinpath(game_version[0].media_paths[MediaType.DATFILE][0])

    elif game_version[0] .game_id == "SWGB":
        if "SWGB_CC" in [expansion.game_id for expansion in game_version[1]]:
            filepath = srcdir.joinpath(game_version[1][0].media_paths[MediaType.DATFILE][0])

        else:
            filepath = srcdir.joinpath(game_version[0].media_paths[MediaType.DATFILE][0])

    cache_file = os.path.join(gettempdir(), "{}.pickle".format(filepath.name))

    with filepath.open('rb') as empiresdat_file:
        gamespec = load_gamespec(empiresdat_file,
                                 game_version,
                                 cache_file,
                                 not dont_pickle)

    return gamespec


def load_gamespec(fileobj, game_version, cachefile_name=None, load_cache=False):
    """
    Helper method that loads the contents of a 'empires.dat' gzipped wrapper
    file.

    If cachefile_name is given, this file is consulted before performing the
    load.
    """
    # try to use the cached result from a previous run
    if cachefile_name and load_cache:
        try:
            with open(cachefile_name, "rb") as cachefile:
                # pickle.load() can fail in many ways, we need to catch all.
                # pylint: disable=broad-except
                try:
                    wrapper = pickle.load(cachefile)
                    info("using cached wrapper: %s", cachefile_name)
                    return wrapper
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
    _, gamespec = wrapper.read(file_data, 0, game_version)

    # Remove the list sorrounding the converted data
    gamespec = gamespec[0]

    if cachefile_name:
        dbg("dumping dat file contents to cache file: %s", cachefile_name)
        with open(cachefile_name, "wb") as cachefile:
            pickle.dump(gamespec, cachefile)

    return gamespec
