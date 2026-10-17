# Copyright 2015-2024 the openage authors. See copying.md for legal info.

"""
Provides FileCollection, a utility class for combining multiple file-like
objects to a FSLikeObject.
"""

from __future__ import annotations

import typing
from collections import OrderedDict
from io import UnsupportedOperation
from typing import NoReturn

from .abstract import FSLikeObject
from .path import Path, Subpath

if typing.TYPE_CHECKING:
    from openage.util.filelike.stream import StreamFragment


class FileCollection(FSLikeObject):
    """
    FSLikeObject that holds several individual files.

    Uses lambdas to access files somewhere else on the fly.
    """

    def __init__(self):
        super().__init__()

        # stores lambdas to access the files
        # {name: open_r, open_w, size, mtime}, {name: subdir}
        self.rootentries = OrderedDict(), OrderedDict()

    @property
    def root(self):
        return FileCollectionPath([], self)

    def get_direntries(
        self, subpath: Subpath | None = None, create: bool = False
    ) -> tuple[OrderedDict, OrderedDict]:
        """
        Fetches the fileentries, subdirentries tuple for the given dir.

        If create == False, raises FileNotFoundError if the directory doesn't
        exist.

        Helper method for internal use.
        """
        if subpath is None:
            subpath = []

        entries = self.rootentries
        for idx, subdir in enumerate(subpath):
            if subdir not in entries[1]:
                if create:
                    if subdir in entries[0]:
                        raise FileExistsError("/".join(subpath[: idx + 1]))
                    entries[1][subdir] = OrderedDict(), OrderedDict()
                else:
                    raise FileNotFoundError("No such directory: " + "/".join(subpath[: idx + 1]))

            entries = entries[1][subdir]

        return entries

    def add_fileentry(self, subpath: Subpath, fileentry: FileEntry):
        """
        Adds a file entry (and parent directory entries, if needed).
        """
        if not subpath:
            raise IsADirectoryError("FileCollection.root is a directory")

        entries = self.get_direntries(subpath[:-1], create=True)

        name = subpath[-1]
        if name in entries[1]:
            raise IsADirectoryError("/".join(subpath))

        entries[0][name] = fileentry

    def _get_fileentry(self, subpath: Subpath) -> FileEntry:
        """
        Gets a file entry.
        """
        if not subpath:
            raise IsADirectoryError("FileCollection.root is a directory")

        entries = self.get_direntries(subpath[:-1])

        name = subpath[-1]

        if name in entries[1]:
            raise IsADirectoryError("/".join(subpath))

        if name not in entries[0]:
            raise FileNotFoundError("/".join(subpath))

        return entries[0][name]

    def open_r(self, subpath: Subpath) -> StreamFragment:
        entry = self._get_fileentry(subpath)

        open_r = entry.open_r()

        if open_r is None:
            raise UnsupportedOperation("not readable: " + "/".join(subpath))

        return open_r

    def open_w(self, subpath: Subpath):
        entry = self._get_fileentry(subpath)

        open_w = entry.open_w()

        if open_w is None:
            raise UnsupportedOperation("not writable: " + "/".join(subpath))

        return open_w

    def open_rw(self, subpath: Subpath) -> NoReturn:
        raise UnsupportedOperation("FileCollection.open_rw")

    def open_a(self, subpath: Subpath) -> NoReturn:
        raise UnsupportedOperation("FileCollection.open_a")

    def open_ar(self, subpath: Subpath) -> NoReturn:
        raise UnsupportedOperation("FileCollection.open_ar")

    def list(self, subpath: Subpath):
        fileentries, subdirs = self.get_direntries(subpath)

        yield from subdirs
        yield from fileentries

    def filesize(self, subpath: Subpath) -> int:
        entry = self._get_fileentry(subpath)

        return entry.size()

    def mtime(self, subpath: Subpath) -> float:
        entry = self._get_fileentry(subpath)

        return entry.mtime()

    def mkdirs(self, subpath: Subpath) -> None:
        self.get_direntries(subpath, create=True)

    def rmdir(self, subpath: Subpath) -> None:
        if not subpath:
            raise UnsupportedOperation("can't rmdir FileCollection.root")

        parent_files, parent_dirs = self.get_direntries(subpath[:-1])
        name = subpath[-1]

        if name in parent_files:
            raise NotADirectoryError("/".join(subpath))

        try:
            files, subdirs = parent_dirs[name]
        except KeyError:
            raise FileNotFoundError("/".join(subpath)) from None

        if files or subdirs:
            raise IOError("Directory not empty: " + "/".join(subpath))

        del parent_dirs[name]

    def unlink(self, subpath: Subpath) -> None:
        if not subpath:
            raise IsADirectoryError("FileCollection.root")

        parent_files, parent_dirs = self.get_direntries(subpath[:-1])
        name = subpath[-1]

        if name in parent_dirs:
            raise IsADirectoryError("/".join(subpath))

        try:
            del parent_files[name]
        except KeyError:
            raise FileNotFoundError("/".join(subpath)) from None

    def touch(self, subpath: Subpath) -> NoReturn:
        raise UnsupportedOperation("FileCollection.touch")

    def rename(self, srcsubpath: Subpath, tgtsubpath: Subpath) -> NoReturn:
        raise UnsupportedOperation("FileCollection.rename")

    def is_file(self, subpath: Subpath) -> bool:
        try:
            self._get_fileentry(subpath)
            return True
        except IOError:
            return False

    def is_dir(self, subpath: Subpath) -> bool:
        try:
            self.get_direntries(subpath)
            return True
        except IOError:
            return False

    def writable(self, subpath: Subpath) -> bool:
        try:
            entry = self._get_fileentry(subpath)
            return type(entry).open_w is not FileEntry.open_w
        except IOError:
            # generally, directories are not writable,
            # though some of the existing files inside might be.
            return False

    def watch(self, subpath: Subpath, callback) -> bool:
        del self, subpath, callback  # unused
        return False

    def poll_watches(self) -> None:
        pass


class FileCollectionPath(Path):
    """
    Path into a FileCollection.
    """


class FileEntry:
    """
    Entry in a file collection archive.
    """

    # pylint: disable=no-self-use

    def open_r(self) -> StreamFragment:
        """
        Returns a file-like object for reading.
        """
        raise UnsupportedOperation("FileEntry.open_r")

    def open_w(self) -> StreamFragment:
        """
        Returns a file-like object for writing.
        """
        raise UnsupportedOperation("FileEntry.open_w")

    def size(self) -> int:
        """
        Returns the size of the entry.
        """
        raise UnsupportedOperation("FileEntry.size")

    def mtime(self) -> float:
        """
        Returns the modification time of the entry.
        """
        raise UnsupportedOperation("FileEntry.mtime")
