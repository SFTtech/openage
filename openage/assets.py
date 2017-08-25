# Copyright 2014-2017 the openage authors. See copying.md for legal info.

"""
Code for locating the game assets.
"""

import os
from pathlib import Path

from .util.fslike.directory import Directory
from .util.fslike.union import Union
from .util.fslike.wrapper import WriteBlocker

from . import config
from . import default_dirs


def get_asset_path(custom_asset_dir=None):
    """
    Returns a Path object for the game assets.

    `custom_asset_dir` can a custom asset directory, which is mounted at the
    top of the union filesystem (i.e. has highest priority).

    This function is used by the both the conversion process
    and the game startup. The conversion uses it for its output,
    the game as its data source(s).
    """

    # if we're in devmode, use only the in-repo asset folder
    if not custom_asset_dir and config.DEVMODE:
        return Directory(os.path.join(config.BUILD_SRC_DIR, "assets")).root

    # else, mount the possible locations in an union:
    # overlay the global dir and the user dir.
    result = Union().root

    # the cmake-determined folder for storing assets
    global_data = Path(config.GLOBAL_ASSET_DIR)
    if global_data.is_dir():
        result.mount(WriteBlocker(Directory(global_data).root).root)

    # user-data directory as provided by environment variables
    # and platform standards
    # we always create this!
    home_data = default_dirs.get_dir("data_home") / "openage"
    result.mount(
        Directory(
            home_data,
            create_if_missing=True
        ).root / "assets"
    )

    # the program argument overrides it all
    if custom_asset_dir:
        result.mount(Directory(custom_asset_dir).root)

    return result


def test():
    """
    Tests whether a specific asset exists.
    """
    from .testing.testing import assert_value
    import argparse

    fakecli = argparse.ArgumentParser()
    fakecli.add_argument("--asset-dir", default=None)
    args = fakecli.parse_args([])

    assert_value(get_asset_path(args.asset_dir)['missing.png'].filesize, 580)
