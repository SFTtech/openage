# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-branches
"""
Mount asset dirs of a game version into the conversion folder.
"""

from ....util.fslike.union import Union
from ...value_object.read.media.drs import DRS


def mount_asset_dirs(srcdir, game_version):
    """
    Returns a Union path where srcdir is mounted at /,
    and all the asset files are mounted in subfolders.
    """

    result = Union().root
    result.mount(srcdir)

    def mount_drs(filename, target):
        """
        Mounts the DRS file from srcdir's filename at result's target.
        """

        drspath = srcdir[filename]
        result[target].mount(DRS(drspath.open('rb'), game_version).root)

    # Mount the media sources of the game edition
    for media_type, media_paths in game_version[0].media_paths.items():
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
                raise Exception("Media at path %s could not be found"
                                % (path_to_media))

    # Mount the media sources of the game edition
    for expansion in game_version[1]:
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
                    raise Exception("Media at path %s could not be found"
                                    % (path_to_media))

    return result
