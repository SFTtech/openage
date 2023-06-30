# Copyright 2020-2023 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-branches
"""
Mount asset dirs of a game version into the conversion folder.
"""
from __future__ import annotations
import typing


from ....util.fslike.union import Union
from ...value_object.read.media.drs import DRS

if typing.TYPE_CHECKING:
    from openage.convert.value_object.init.game_version import GameVersion
    from openage.util.fslike.directory import Directory


def mount_asset_dirs(
    srcdir: Directory,
    game_version: GameVersion
) -> Union:
    """
    Returns a Union path where srcdir is mounted at /,
    and all the asset files are mounted in subfolders.
    """

    result = Union().root
    result.mount(srcdir)

    def mount_drs(filename: str, target: str) -> None:
        """
        Mounts the DRS file from srcdir's filename at result's target.
        """
        drspath = srcdir[filename]
        result[target].mount(DRS(drspath.open('rb'), game_version).root)

    # Mount the media sources of the game edition
    for media_type, media_paths in game_version.edition.media_paths.items():
        for media_path in media_paths:
            path_to_media = srcdir[media_path]
            if path_to_media.is_dir():
                # Mount folder
                result[media_type.value].mount(path_to_media)

            elif path_to_media.is_file():
                # Mount archive
                if path_to_media.suffix.lower() == ".drs":
                    mount_drs(media_path, media_type.value)

            else:
                raise FileNotFoundError(f"Media at path {path_to_media} could not be found")

    # Mount the media sources of the game edition
    for expansion in game_version.expansions:
        for media_type, media_paths in expansion.media_paths.items():
            for media_path in media_paths:
                path_to_media = srcdir[media_path]
                if path_to_media.is_dir():
                    # Mount folder
                    result[media_type.value].mount(path_to_media)

                elif path_to_media.is_file():
                    # Mount archive
                    if path_to_media.suffix.lower() == ".drs":
                        mount_drs(media_path, media_type.value)

                else:
                    raise FileNotFoundError(f"Media at path {path_to_media} could not be found")

    return result
