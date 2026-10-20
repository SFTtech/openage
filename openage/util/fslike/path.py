# Copyright 2015-2026 the openage authors. See copying.md for legal info.

"""
Provides Path, which is analogous to pathlib.Path,
and the type of FSLikeObject.root.
"""

from __future__ import annotations

import os
import tempfile
from io import TextIOWrapper, UnsupportedOperation
from typing import IO, TYPE_CHECKING, List, Self, Sequence

from ...util.filelike.abstract import FileLikeObject

if TYPE_CHECKING:
    from .abstract import FSLikeObject, Subpath

type Subpath = Sequence[str]


class Path:
    """
    Similar to pathlib.Path, but on arbitrary (virtual) filesystems, like transparent archive access.

    Represents a specific path in a given filesystem-like (fslike) object; mostly, that
    object's member methods are simply wrapped.

    parts: starting path in the given fsobj,
           e.g. ["folder", "file"],
           or "folder/file"
           or b"folder/file".

    fsobj: filesystem object that is e.g. a real filesystem Directory("/"), some archive,
           or anything that is like some filesystem.
           default is Directory("/") - your real filesystem.
    """

    def __init__(
        self, path: Subpath | str | bytes | Sequence[bytes] | None = None, fsobj: FSLikeObject | None = None
    ):
        if fsobj is None:
            from .directory import filesystem_root

            fsobj = filesystem_root

        if isinstance(path, bytes):
            path = path.decode()

        if isinstance(path, str):
            subpath = path.split("/")
        else:
            subpath = path

        if not isinstance(subpath, (list, tuple)):
            raise ValueError(f"path parts must be str, bytes, list or tuple, but not: {type(path)}")

        if subpath is None:
            subpath = []

        result = []
        for part in subpath:
            if isinstance(part, bytes):
                part = part.decode()

            if part in (".", ""):
                pass
            elif part == "..":
                try:
                    result.pop()
                except IndexError:
                    pass
            else:
                result.append(part)

        self.fsobj = fsobj

        # Set to True by create_temp_file or create_temp_dir
        self.is_temp: bool = False

        # use tuple instead of list to prevent accidential modification
        self.subpath = tuple(result)

    def __str__(self):
        return self.fsobj.pretty(self.subpath)

    def __repr__(self):
        if not self.subpath:
            return repr(self.fsobj) + ".root"

        return f"Path({self.fsobj!r}, {self.subpath!r})"

    def exists(self) -> bool:
        """True if path exists"""
        return self.fsobj.exists(self.subpath)

    def is_dir(self) -> bool:
        """True if path points to dir (or symlink to one)"""
        return self.fsobj.is_dir(self.subpath)

    def is_file(self) -> bool:
        """True if path points to file (or symlink to one)"""
        return self.fsobj.is_file(self.subpath)

    def writable(self) -> bool:
        """True if path is probably writable"""
        return self.fsobj.writable(self.subpath)

    def list(self):
        """Yields path names for all members of this dir"""
        yield from self.fsobj.list(self.subpath)

    def iterdir(self):
        """Yields path objects for all members of this dir"""
        for name in self.fsobj.list(self.subpath):
            yield type(self)(path=(*self.subpath, name), fsobj=self.fsobj)

    def mkdirs(self) -> None:
        """Creates this path (including parents). No-op if path exists."""
        return self.fsobj.mkdirs(self.subpath)

    def open(self, mode="r"):
        """Opens the file at this path; returns a file-like object."""

        dmode = mode.replace("b", "")

        match dmode:
            case "r":
                handle = self.fsobj.open_r(self.subpath)

            case "w":
                handle = self.fsobj.open_w(self.subpath)

            case "r+" | "rw":
                handle = self.fsobj.open_rw(self.subpath)

            case "a":
                handle = self.fsobj.open_a(self.subpath)

            case "a+" | "ar":
                handle = self.fsobj.open_ar(self.subpath)

            case _:
                raise UnsupportedOperation("unsupported open mode: " + mode)

        if "b" in mode:
            return handle

        return TextIOWrapper(handle)

    def open_r(self) -> IO[bytes] | FileLikeObject:
        """open with mode='rb'"""
        return self.fsobj.open_r(self.subpath)

    def open_w(self) -> IO[bytes] | FileLikeObject:
        """open with mode='wb'"""
        return self.fsobj.open_w(self.subpath)

    def open_a(self) -> IO[bytes] | FileLikeObject:
        """open with mode='ab'"""
        return self.fsobj.open_a(self.subpath)

    def _get_native_path(self) -> str | None:
        """
        return the native path (usable by your kernel) of this path,
        or None if the path is not natively usable.

        Don't use this method directly, use the resolve methods below.
        """
        return self.fsobj.get_native_path(self.subpath)

    def _resolve_r(self) -> Path | None:
        """
        Flatten the path recursively for read access.
        Used to cancel out some wrappers in between.
        """
        return self.fsobj.resolve_r(self.subpath)

    def _resolve_w(self) -> Path | None:
        """
        Flatten the path recursively for write access.
        Used to cancel out some wrappers in between.
        """
        return self.fsobj.resolve_w(self.subpath)

    def resolve_native_path(self, mode="r") -> str | None:
        """
        Minimize the path and possibly return a native one.
        Returns None if there was no native path.
        """
        match mode:
            case "r":
                return self.resolve_native_path_r()
            case "w":
                return self.resolve_native_path_w()

        raise UnsupportedOperation(f"unsupported resolve mode: {mode!r}")

    def resolve_native_path_r(self) -> str | None:
        """
        Resolve the path for read access and possibly return
        a native equivalent.
        If no native path was found, return None.
        """
        resolved_path = self._resolve_r()
        if resolved_path:
            return resolved_path._get_native_path()
        return None

    def resolve_native_path_w(self) -> str | None:
        """
        Resolve the path for write access and try to return
        a native equivalent.
        If no native path could be determined, return None.
        """
        resolved_path = self._resolve_w()
        if resolved_path:
            return resolved_path._get_native_path()
        return None

    def rename(self, targetpath: Path) -> None:
        """renames to targetpath"""
        if self.fsobj != targetpath.fsobj:
            raise UnsupportedOperation("can't rename across two FSLikeObjects")
        return self.fsobj.rename(self.subpath, targetpath.subpath)

    def rmdir(self) -> None:
        """Removes the empty directory at this path."""
        return self.fsobj.rmdir(self.subpath)

    def touch(self) -> None:
        """Creates the file at this path, or updates the timestamp."""
        return self.fsobj.touch(self.subpath)

    def unlink(self) -> None:
        """Removes the file at this path."""
        return self.fsobj.unlink(self.subpath)

    def removerecursive(self) -> None:
        """Recursively deletes this file or directory."""
        if self.is_dir():
            for path in self.iterdir():
                path.removerecursive()
            self.rmdir()
        else:
            self.unlink()

    @property
    def mtime(self) -> float | None:
        """Returns the time of last modification of the file or directory."""
        return self.fsobj.mtime(self.subpath)

    @property
    def filesize(self) -> int:
        """Returns the file size."""
        return self.fsobj.filesize(self.subpath)

    def watch(self, callback) -> bool:
        """
        Installs 'callback' as callback that gets invoked whenever the file at
        this path changes.

        Returns True if the callback was installed, and false if not
        (e.g. because the some OS limit was reached, or the underlying
         FSLikeObject doesn't support watches).
        """
        return self.fsobj.watch(self.subpath, callback)

    def poll_fs_watches(self):
        """Polls the installed watches for the entire file-system."""
        self.fsobj.poll_watches()

    @property
    def parent(self):
        """Parent path object. The parent of root is root."""
        return type(self)(self.subpath[:-1], self.fsobj)

    @property
    def name(self) -> str:
        """The name of the topmost component (str)."""
        return self.subpath[-1]

    @property
    def suffix(self) -> str:
        """The last suffix of the name of the topmost component (str)."""
        name = self.name
        pos = name.rfind(".")
        if pos <= 0:
            return ""
        return name[pos:]

    @property
    def suffixes(self) -> List[str]:
        """The suffixes of the name of the topmost component (str list)."""
        name = self.name
        if name.startswith("."):
            name = name[1:]
        return ["." + suffix for suffix in name.split(".")[1:]]

    @property
    def stem(self) -> str:
        """Name without suffix (such that stem + suffix == name)."""
        name = self.name
        pos = name.rfind(".")
        if pos <= 0:
            return name

        return name[:pos]

    def joinpath(self, subpath: Subpath) -> Self:
        """Returns path for the given subpath."""
        match subpath:
            case str():
                raw_subpath = subpath.split("/")
            case bytes():
                raw_subpath = subpath.decode().split("/")
            case list() | tuple():
                raw_subpath = subpath
            case _:
                raise TypeError(f"Subpath must be str or bytes, got '{type(subpath)}'")

        return type(self)(self.subpath + tuple(raw_subpath), self.fsobj)

    def __getitem__(self, subpath) -> Self:
        """Like joinpath."""
        return self.joinpath(subpath)

    def __truediv__(self, subpath) -> Self:
        """Like joinpath."""
        return self.joinpath(subpath)

    def __eq__(self, other):
        """comparison by fslike and parts"""
        return (self.fsobj == other.fsobj) and (self.subpath == other.subpath)

    def __hash__(self):
        """hash consistent with __eq__"""
        return hash((self.fsobj, tuple(self.subpath)))

    def with_name(self, name) -> Self:
        """Returns path for differing name (same parent)."""
        return self.parent.joinpath(name)

    def with_suffix(self, suffix) -> Self:
        """Returns path for different suffix (same parent and stem)."""
        if isinstance(suffix, bytes):
            suffix = suffix.decode()

        return self.parent.joinpath(self.stem + suffix)

    def mount(self, pathobj, priority=0) -> None:
        """This is only valid for UnionPath, don't call here"""
        # pylint: disable=no-self-use,unused-argument
        # TODO: https://github.com/PyCQA/pylint/issues/2329
        raise PermissionError("Do not call mount on Path instances!")

    @classmethod
    def get_temp_file(cls) -> Self:
        """
        Creates a temporary file.
        """
        temp_fd, temp_file = tempfile.mkstemp()

        # Close the file descriptor to release resources
        os.close(temp_fd)

        # Wrap the temporary file path in a Path object and return it
        path = cls(temp_file)
        path.is_temp = True

        return path

    @classmethod
    def get_temp_dir(cls) -> Self:
        """
        Creates a temporary directory.
        """
        # Create a temporary directory using tempfile.mkdtemp
        temp_dir = tempfile.mkdtemp()

        # Wrap the temporary directory path in a Path object and return it
        path = cls(temp_dir)
        path.is_temp = True

        return path
