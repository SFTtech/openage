# Copyright 2014-2023 the openage authors. See copying.md for legal info.

"""
Code for locating the game assets.
"""
from __future__ import annotations


import typing
import os
from pathlib import Path

from .util.fslike.directory import Directory
from .util.fslike.union import Union
from .util.fslike.wrapper import WriteBlocker

from . import config
from . import default_dirs

if typing.TYPE_CHECKING:
    from openage.util.fslike.union import UnionPath


def get_asset_path(custom_asset_dir: str = None) -> UnionPath:
    """
    Returns a Path object for the game assets.

    `custom_asset_dir` can a custom asset directory, which is mounted at the
    top of the union filesystem (i.e. has highest priority).

    This function is used by the both the conversion process
    and the game startup. The conversion uses it for its output,
    the game as its data source(s).
    """

    # mount the possible locations in an union:
    result = Union().root

    # if we're in devmode, use only the in-repo asset folder
    if not custom_asset_dir and config.DEVMODE:
        result.mount(Directory(os.path.join(config.BUILD_SRC_DIR, "assets")).root)
        return result

    # else overlay the global dir and the user dir.

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

    assert_value(get_asset_path(args.asset_dir)['test']['textures']['missing.png'].filesize, 580)
