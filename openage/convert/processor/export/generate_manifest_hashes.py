# Copyright 2020-2022 the openage authors. See copying.md for legal info.
"""
Provides functions for traversing a directory and
generating hash values for all the items inside.
"""

from __future__ import annotations
import typing


import os

from openage.util.hash import hash_file

if typing.TYPE_CHECKING:
    from openage.util.fslike.directory import Directory
    from openage.util.fslike.path import Path
    from openage.convert.entity_object.conversion.modpack import Modpack


def bfs_directory(root: Path) -> typing.Generator[Path, None, None]:
    """
    Traverse the given directory with breadth-first way.

    :param root: The directory to traverse.
    :type root: ...util.fslike.path.Path
    """

    # initiate from the root directory
    dirs = [root]

    while dirs:
        next_level = []

        for directory in dirs:
            for item in directory.iterdir():
                if item.is_dir():
                    # save next subdirectories for next round
                    next_level.append(item)
                else:
                    # return the path if it is a file
                    yield item

        dirs = next_level


def generate_hashes(
    modpack: Modpack,
    exportdir: Directory,
    hash_algo: str = 'sha3_256',
    bufsize: int = 32768
) -> None:
    """
    Generate hashes for all the items in a
    given modpack and adds them to the manifest
    instance.

    :param modpack: The target modpack.
    :type modpack: ..dataformats.modpack.Modpack
    :param exportdir: Directory wheere modpacks are stored.
    :type exportdir: ...util.fslike.path.Path
    :param hash_algo: Hashing algorithm used.
    :type hash_algo: str
    :param bufsize: Buffer size for reading files.
    :type bufsize: int
    """
    # set the hashing algorithm in the manifest instance
    modpack.manifest.set_hashing_func(hash_algo)

    # traverse the directory with breadth-first way and
    # generate hash values for the items encountered
    for file in bfs_directory(exportdir):
        hash_val = hash_file(file, hash_algo=hash_algo, bufsize=bufsize)
        relative_path = os.path.relpath(str(file), str(exportdir))
        modpack.manifest.add_hash_value(hash_val, relative_path)
