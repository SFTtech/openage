# Copyright 2021-2022 the openage authors. See copying.md for legal info.

"""
Functions for hashing files.
"""
from __future__ import annotations

import typing
import hashlib

if typing.TYPE_CHECKING:
    from openage.util.fslike.path import Path


def hash_file(
    path: Path,
    hash_algo: str = "sha3_256",
    bufsize: int = 32768
) -> str:
    """
    Get the hash value of a given file.

    :param path: Path of the file.
    :type path: .fslike.path.Path
    :param hash_algo: Hashing algorithm identifier.
    :type hash_algo: str
    :param bufsize: Buffer size for reading files.
    :type bufsize: int
    """
    hashfunc = hashlib.new(hash_algo)

    with path.open_r() as f_in:
        while True:
            data = f_in.read(bufsize)
            if not data:
                break
            hashfunc.update(data)

    return hashfunc.hexdigest()
