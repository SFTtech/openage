# Copyright 2015-2023 the openage authors. See copying.md for legal info.

"""
FSLikeObjects that represent actual file system paths:

 - Directory: enforces case
 - CaseIgnoringReadOnlyDirectory
"""

from __future__ import annotations

import os
import pathlib
from typing import IO, Generator, Sequence

from .abstract import FSLikeObject, Subpath


class Directory(FSLikeObject):
    """
    Provides an actual file system directory's contents as-they-are.

    Initialized from some real path that is mounted already by your system.
    """

    def __init__(self, path_: pathlib.Path | str | bytes, create_if_missing=False):
        if isinstance(path_, pathlib.Path):
            path = str(path_)
        elif isinstance(path_, str):
            path = path_
        elif isinstance(path_, bytes):
            path = path_.decode()
        else:
            raise TypeError(f"incompatible type for path: {type(path_)}")

        if not os.path.isdir(path):
            if create_if_missing:
                os.makedirs(path)
            else:
                raise FileNotFoundError(path)

        self.path = path

    def __repr__(self):
        return f"Directory({self.path})"

    def resolve(self, subpath: Subpath) -> str:
        """resolves subpath to an actual path name."""
        return os.path.join(self.path, *subpath)

    def open_r(self, subpath: Subpath) -> IO[bytes]:
        return open(self.resolve(subpath), "rb")

    def open_w(self, subpath: Subpath) -> IO[bytes]:
        return open(self.resolve(subpath), "wb")

    def open_rw(self, subpath: Subpath) -> IO[bytes]:
        return open(self.resolve(subpath), "r+b")

    def open_a(self, subpath: Subpath) -> IO[bytes]:
        return open(self.resolve(subpath), "ab")

    def open_ar(self, subpath: Subpath) -> IO[bytes]:
        return open(self.resolve(subpath), "a+b")

    def get_native_path(self, subpath: Subpath) -> str:
        return self.resolve(subpath)

    def list(self, subpath: Subpath) -> Generator[str, None, None]:
        yield from (entry.name for entry in os.scandir(self.resolve(subpath)))

    def filesize(self, subpath: Subpath) -> int:
        return os.path.getsize(self.resolve(subpath))

    def mtime(self, subpath: Subpath) -> float:
        return os.path.getmtime(self.resolve(subpath))

    def mkdirs(self, subpath: Subpath) -> None:
        return os.makedirs(self.resolve(subpath), exist_ok=True)

    def rmdir(self, subpath: Subpath) -> None:
        return os.rmdir(self.resolve(subpath))

    def unlink(self, subpath: Subpath) -> None:
        return os.unlink(self.resolve(subpath))

    def touch(self, subpath: Subpath) -> None:
        try:
            os.utime(self.resolve(subpath))
        except FileNotFoundError:
            with open(self.resolve(subpath), "ab") as directory:
                directory.close()

    def rename(self, srcsubpath: Subpath, tgtsubpath: Subpath) -> None:
        return os.rename(self.resolve(srcsubpath), self.resolve(tgtsubpath))

    def is_file(self, subpath: Subpath) -> bool:
        return os.path.isfile(self.resolve(subpath))

    def is_dir(self, subpath: Subpath) -> bool:
        return os.path.isdir(self.resolve(subpath))

    def writable(self, subpath: Subpath) -> bool:
        subpath = list(subpath)
        path = self.resolve(subpath)

        while not os.path.exists(path):
            if not subpath:
                raise FileNotFoundError(self.path)

            subpath.pop()
            path = self.resolve(subpath)

        return os.access(path, os.W_OK)

    def watch(self, subpath: Subpath, callback) -> bool:
        # TODO
        del subpath, callback
        return False

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

    def __init__(self, path: pathlib.Path | str | bytes, create_if_missing=False):
        super().__init__(path, create_if_missing)
        self.cache: dict[tuple[str, ...], tuple[str, ...]] = {(): ()}
        self.listings: dict[tuple[str, ...], dict[str, str]] = {}

    def __repr__(self):
        return f"Directory({self.path})"

    def actual_name(self, stem: Sequence[str], name: str) -> str:
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

    def resolve(self, subpath: Subpath) -> str:
        subpath = [part.lower() for part in subpath]

        i = 0
        for i in range(len(subpath), -1, -1):
            try:
                result = list(self.cache[tuple(subpath[:i])])
                break
            except KeyError:
                pass
        else:
            raise RuntimeError("code flow error")

        # result now contains the case-corrected path for subpath[:i].
        # we need to append the path for subpath[i:].
        for part in subpath[i:]:
            result.append(self.actual_name(result, part))
            self.cache[tuple(subpath[: len(result)])] = tuple(result)

        return os.path.join(self.path, *result)

    def list(self, subpath: Subpath) -> Generator[str, None, None]:
        for name in super().list(subpath):
            yield name.lower()


filesystem_root = Directory("/")


# TODO add CaseEnforcingDirectory, with resolve() similar to that of
#      CaseIgnoringDirectory.
#      CaseEnforcingDirectory would prevent modders from getting sloppy with
#      file name case, which would lead to mods stopping to work on Linux.
