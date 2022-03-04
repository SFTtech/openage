# Copyright 2021-2022 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments

"""
Create a media cache file for a game version.
"""
from __future__ import annotations
import typing

import toml

from ..data_definition import DataDefinition

FILE_VERSION = "1.0"

if typing.TYPE_CHECKING:
    from openage.convert.value_object.read.media_types import MediaType
    from openage.convert.value_object.init.game_version import GameVersion


class MediaCacheFile(DataDefinition):
    """
    Used for creating a media cache file.
    """

    def __init__(self, targetdir: str, filename: str, game_version: GameVersion):
        super().__init__(targetdir, filename)

        self.game_version = game_version
        self.hash_func: str = None
        self.cache = {}

    def dump(self) -> str:
        """
        Returns the media cache file content in TOML format.
        """
        output_dict = {}

        output_dict["file_version"] = FILE_VERSION
        output_dict["hash_algo"] = self.hash_func

        for media_type, cachedata in self.cache.items():
            output_dict.update({media_type.value: {}})

            for idx, cache in enumerate(cachedata):
                cache_table = output_dict[media_type.value]
                cache_table[f"file{idx}"] = {
                    "filepath": cache[0],
                    "hash": cache[1],
                    "compression_settings": cache[2],
                    "packer_settings": cache[3]
                }

        output_str = "# openage media cache file\n\n"
        output_str += toml.dumps(output_dict)

        return output_str

    def add_cache_data(
        self,
        media_type: MediaType,
        filepath: str,
        filehash: str,
        compr_settings: tuple,
        packer_settings: tuple
    ) -> None:
        """
        Add cache data for a file.

        :param media_type: Media type of the file (should be a graphics format)
        :type media_type: MediaType
        :param filepath: Path of the source file in the sourcedir
                         mounted by the openage converter.
        :type filepath: str
        :param filehash: Hash value of the source file.
        :type filehash: str
        :param compr_settings: Settings for the PNG compression.
        :type compr_settings: tuple
        :param packer_settings: Settings for the packing algorithm.
        :type packer_settings: tuple
        """
        if media_type not in self.cache:
            self.cache[media_type] = []

        self.cache[media_type].append(
            (filepath, filehash, compr_settings, packer_settings)
        )

    def set_hash_func(self, hash_func: str) -> None:
        """
        Set the hash function used for generating
        hash values for the graphic files.

        :param hash_func: Hash algorithm
        :type hash_func: str
        """
        self.hash_func = hash_func
