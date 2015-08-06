# Copyright 2014-2015 the openage authors. See copying.md for legal info.

"""
Code for locating the game assets.

All access to game assets should happen through objects obtained from get().
"""

import os

from .util.fslike.directory import Directory
from .util.fslike.union import Union
from .util.fslike.wrapper import WriteBlocker

from . import config


def get_user_data_dir(application="openage"):
    """
    Returns the user-specific data directory for an application,
    e.g. for Linux: ~/.openage
    """
    # TODO cross-platform stuff
    # TODO use one of those fancy freedesktop.org folders on GNU/Linux.
    return os.path.join(os.environ['HOME'], "." + application)


def get_assets(args):
    """
    Returns a Path object for the game assets.

    args are the arguments, as provided by the CLI's ArgumentParser.
    """

    if args.asset_dir:
        return Directory(args.asset_dir).root

    if config.DEVMODE:
        return Directory(os.path.join(config.BUILD_SRC_DIR, "assets")).root

    # overlay the global dir and the user dir.
    result = Union().root

    result.mount(WriteBlocker(Directory(config.GLOBAL_ASSET_DIR)).root)
    result.mount(Directory(os.path.join(get_user_data_dir())).root / "assets")

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

    assert_value(get_assets(args)['missing.png'].filesize, 580)
