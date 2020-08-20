# Copyright 2016-2017 the openage authors. See copying.md for legal info.

""" Renaming interface assets and splitting into directories """

from .hardcoded import (ingame_hud_background_index, ASSETS)
# REFA: Whole file -> service


def hud_rename(filepath):
    """
    Returns a human-usable name according to the original
    and hardcoded metadata.
    """
    try:
        return filepath.parent["hud{}{}".format(
            str(ingame_hud_background_index(int(filepath.stem))).zfill(4),
            filepath.suffix
        )]

    except ValueError:
        return asset_rename(filepath)


def asset_rename(filepath):
    """
    Rename a slp asset path by the lookup map above.
    """
    try:
        return filepath.parent[ASSETS[filepath.stem] + filepath.suffix]

    except (KeyError, AttributeError):
        return filepath
