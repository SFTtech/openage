# Copyright 2021-2022 the openage authors. See copying.md for legal info.

"""
Load data of media cache files.
"""
from __future__ import annotations
import typing

import toml

if typing.TYPE_CHECKING:
    from openage.util.fslike.path import Path


def load_media_cache(filepath: Path) -> dict[str, dict[str, tuple]]:
    """
    Parses a media cache file and returns it as a dict.
    """
    output_dict = {}

    with filepath.open() as infile:
        cache_file = toml.loads(infile.read())

    cache_file.pop("file_version")
    cache_file.pop("hash_algo")

    cachedata = list(cache_file["graphics"].values())
    for cache in cachedata:
        output_dict.update(
            {
                cache["filepath"]: {
                    "compr_settings": cache["compression_settings"],
                    "packer_settings": cache["packer_settings"]
                }
            }
        )

    return output_dict
