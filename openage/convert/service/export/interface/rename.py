# Copyright 2016-2022 the openage authors. See copying.md for legal info.

"""
Renaming interface assets and splitting into directories.
"""
from __future__ import annotations
import typing


from ....value_object.read.media.hardcoded.interface import ASSETS
from .cutter import ingame_hud_background_index

if typing.TYPE_CHECKING:
    from openage.util.fslike.path import Path


def hud_rename(filepath: Path) -> Path:
    """
    Returns a human-usable name according to the original
    and hardcoded metadata.
    """
    try:
        return filepath.parent[
            f"hud{str(ingame_hud_background_index(int(filepath.stem))).zfill(4)}{filepath.suffix}"
        ]

    except ValueError:
        return asset_rename(filepath)


def asset_rename(filepath: Path) -> Path:
    """
    Rename a slp asset path by the lookup map above.
    """
    try:
        return filepath.parent[ASSETS[filepath.stem] + filepath.suffix]

    except (KeyError, AttributeError):
        return filepath
