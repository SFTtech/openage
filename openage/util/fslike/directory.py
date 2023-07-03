# Copyright 2015-2023 the openage authors. See copying.md for legal info.

"""
FSLikeObjects that represent actual file system paths:

 - Directory: enforces case
 - CaseIgnoringReadOnlyDirectory
"""

from __future__ import annotations
import typing

import os
import pathlib

from typing import Union

from .abstract import FSLikeObject

if typing.TYPE_CHECKING:
    from io import BufferedReader


class Directory(FSLikeObject):
    """
    Provides an actual file system directory's contents as-they-are.

    Initialized from some real path that is mounted already by your system.
    """

    def __init__(self, path_, create_if_missing=False):
        if isinstance(path_, pathlib.Path):
            path = bytes(path_)
        elif isinstance(path_, str):
            path = path_.encode()
        elif isinstance(path_, bytes):
            path = path_
        else:
            raise TypeError(f"incompatible type for path: {type(path_)}")

        if not os.path.isdir(path):
            if create_if_missing:
                os.makedirs(path)
            else:
                raise FileNotFoundError(path)

        self.path = path

    def __repr__(self):
        return f"Directory({self.path.decode(errors='replace')})"

    def resolve(self, parts) -> Union[str, bytes]:
        """ resolves parts to an actual path name. """
        return os.path.join(self.path, *parts)

    def open_r(self, parts) -> BufferedReader:
        return open(self.resolve(parts), 'rb')

    def open_w(self, parts) -> BufferedReader:
        return open(self.resolve(parts), 'wb')

    def open_rw(self, parts) -> BufferedReader:
        return open(self.resolve(parts), 'r+b')

    def open_a(self, parts) -> BufferedReader:
        return open(self.resolve(parts), 'ab')

    def open_ar(self, parts) -> BufferedReader:
        return open(self.resolve(parts), 'a+b')

    def get_native_path(self, parts) -> Union[str, bytes]:
        return self.resolve(parts)

    def list(self, parts) -> typing.Generator[str | bytes, None, None]:
        # TODO migrate to scandir, once we're on py 3.5.
        yield from os.listdir(self.resolve(parts))

    def filesize(self, parts) -> int:
        return os.path.getsize(self.resolve(parts))

    def mtime(self, parts) -> float:
        return os.path.getmtime(self.resolve(parts))

    def mkdirs(self, parts) -> None:
        return os.makedirs(self.resolve(parts), exist_ok=True)

    def rmdir(self, parts) -> None:
        return os.rmdir(self.resolve(parts))

    def unlink(self, parts) -> None:
        return os.unlink(self.resolve(parts))

    def touch(self, parts) -> None:
        try:
            os.utime(self.resolve(parts))
        except FileNotFoundError:
            with open(self.resolve(parts), 'ab') as directory:
                directory.close()

    def rename(self, srcparts, tgtparts) -> None:
        return os.rename(self.resolve(srcparts), self.resolve(tgtparts))

    def is_file(self, parts) -> bool:
        return os.path.isfile(self.resolve(parts))

    def is_dir(self, parts) -> bool:
        return os.path.isdir(self.resolve(parts))

    def writable(self, parts) -> bool:
        parts = list(parts)
        path = self.resolve(parts)

        while not os.path.exists(path):
            if not parts:
                raise FileNotFoundError(self.path)

            parts.pop()
            path = self.resolve(parts)

        return os.access(path, os.W_OK)

    def watch(self, parts, callback) -> None:
        # TODO
        pass

    def poll_watches(self) -> None:
        # TODO
        pass


class CaseIgnoringDirectory(Directory):
    """
    Like directory, but all given paths must be lower-case,
    and will be resolved to the actual correct case.

    The one exception is the constructor argument:
    It _must_ be in the correct case.
    """

    def __init__(self, path, create_if_missing=False):
        super().__init__(path, create_if_missing)
        self.cache = {(): ()}
        self.listings = {}

    def __repr__(self):
        return f"Directory({self.path.decode(errors='replace')})"

    def actual_name(self, stem: list, name: str) -> str:
        """
        If the (lower-case) path that's given in stem exists,
        fetches the actual name for the given lower-case name.
        """
        try:
            listing = self.listings[tuple(stem)]
        except KeyError:
            # the directory has not been listed yet.
            try:
                filelist = os.listdir(os.path.join(self.path, *stem))
            except FileNotFoundError:
                filelist = []

            listing = {}
            for filename in filelist:
                if filename.lower() != filename:
                    listing[filename.lower()] = filename
            self.listings[tuple(stem)] = listing

        try:
            return listing[name]
        except KeyError:
            return name

    def resolve(self, parts) -> Union[str, bytes]:
        parts = [part.lower() for part in parts]

        i = 0
        for i in range(len(parts), -1, -1):
            try:
                result = list(self.cache[tuple(parts[:i])])
                break
            except KeyError:
                pass
        else:
            raise RuntimeError("code flow error")

        # result now contains the case-corrected path for parts[:i].
        # we need to append the path for parts[i:].
        for part in parts[i:]:
            result.append(self.actual_name(result, part))
            self.cache[tuple(parts[:len(result)])] = tuple(result)

        return os.path.join(self.path, *result)

    def list(self, parts) -> typing.Generator[str | bytes, None, None]:
        for name in super().list(parts):
            yield name.lower()


# TODO add CaseEnforcingDirectory, with resolve() similar to that of
#      CaseIgnoringDirectory.
#      CaseEnforcingDirectory would prevent modders from getting sloppy with
#      file name case, which would lead to mods stopping to work on Linux.
