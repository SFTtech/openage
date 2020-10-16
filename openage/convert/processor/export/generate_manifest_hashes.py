# Copyright 2020-2020 the openage authors. See copying.md for legal info.
"""
Provides functions for traversing a directory and
generating hash values for all the items inside.
"""

import hashlib
import os


def bfs_directory(root):
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


def get_file_hash(file_path, hashfunc, bufsize):
    """
    Returns the hash value of a given file.

    :param file_path: Path of the given file.
    :type file_path: ...util.fslike.path.Path
    :param hashfunc: Hashing algorithm used.
    :type hashfunc: str
    :param bufsize: Buffer size for reading files.
    :type bufsize: int
    """
    # set the hashing algorithm
    hashf = hashlib.new(hashfunc)

    with file_path.open_r() as f_in:
        while True:
            data = f_in.read(bufsize)
            if not data:
                break
            hashf.update(data)

    return hashf.hexdigest()


def generate_hashes(modpack, exportdir, hashfunc='sha3_256', bufsize=32768):
    """
    Generate hashes for all the items in a
    given modpack and adds them to the manifest
    instance.

    :param modpack: The target modpack.
    :type modpack: ..dataformats.modpack.Modpack
    :param exportdir: Directory wheere modpacks are stored.
    :type exportdir: ...util.fslike.path.Path
    :param hashfunc: Hashing algorithm used.
    :type hashfunc: str
    :param bufsize: Buffer size for reading files.
    :type bufsize: int
    """

    # set the hashing algorithm in the manifest instance
    modpack.manifest.set_hashing_func(hashfunc)

    # traverse the directory with breadth-first way and
    # generate hash values for the items encountered
    for file in bfs_directory(exportdir):
        hash_val = get_file_hash(file, hashfunc, bufsize)
        relative_path = os.path.relpath(str(file), str(exportdir))
        modpack.manifest.add_hash_value(hash_val, relative_path)
