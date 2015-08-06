# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Provides Path, which is analogous to pathlib.Path,
and the type of FSLikeObject.root.
"""

from io import UnsupportedOperation, TextIOWrapper


class Path:
    """
    Implements an interface somewhat similar to that of pathlib.Path,
    but some methods are missing or have different usage, and some new have
    been added.

    Represents a specific path in a given FS-Like object; mostly, that
    object's member methods are simply wrapped.

    Independent of the OS, slashes must be used as separators.

    Internally, bytes are used instead of str.
    No lists of strings may be passed to member methods.
    """

    # We're re-implementing pathlib.Path's interface, we have no choice about
    # this. Also, some of these are properties, so the effective count is
    # lower.
    # pylint: disable=too-many-public-methods

    def __init__(self, fsobj, parts):
        if isinstance(parts, str):
            parts = parts.encode()

        if isinstance(parts, bytes):
            parts = parts.split(b'/')

        result = []
        for part in parts:
            if isinstance(part, str):
                part = part.encode()

            if part == b'.' or part == b'':
                pass
            elif part == b'..':
                try:
                    result.pop()
                except IndexError:
                    pass
            else:
                result.append(part)

        self.fsobj = fsobj

        # use tuple instead of list to prevent accidential modification
        self.parts = tuple(result)

    def __str__(self):
        return "[%s]:%s" % (
            str(self.fsobj),
            b"/".join(self.parts).decode(errors='replace'))

    def __repr__(self):
        return "Path({}, {})".format(repr(self.fsobj), repr(self.parts))

    def exists(self):
        """ True if path exists """
        return self.is_dir() or self.is_file()

    def is_dir(self):
        """ True if path points to dir (or symlink to one) """
        return self.fsobj.is_dir(self.parts)

    def is_file(self):
        """ True if path points to file (or symlink to one) """
        return self.fsobj.is_file(self.parts)

    def writable(self):
        """ True if path is probably writable """
        return self.fsobj.writable(self.parts)

    def list(self):
        """ Yields path names for all members of this dir """
        yield from self.fsobj.list(self.parts)

    def iterdir(self):
        """ Yields path objects for all members of this dir """
        for name in self.fsobj.list(self.parts):
            yield type(self)(self.fsobj, self.parts + (name,))

    def mkdirs(self):
        """ Creates this path (including parents). No-op if path exists. """
        return self.fsobj.mkdirs(self.parts)

    def open(self, mode="r"):
        """ Opens the file at this path; returns a file-like object. """
        if mode == "rb":
            return self.fsobj.open_r(self.parts)
        elif mode == "r":
            return TextIOWrapper(self.fsobj.open_r(self.parts))
        elif mode == "wb":
            return self.fsobj.open_w(self.parts)
        elif mode == "w":
            return TextIOWrapper(self.fsobj.open_w(self.parts))
        else:
            raise UnsupportedOperation("unsupported open mode: " + mode)

    def open_r(self):
        """ open with mode='rb' """
        return self.fsobj.open_r(self.parts)

    def open_w(self):
        """ open with mode='wb' """
        return self.fsobj.open_w(self.parts)

    def rename(self, targetpath):
        """ renames to targetpath """
        if self.fsobj != targetpath.fsobj:
            raise UnsupportedOperation("can't rename across two FSLikeObjects")
        return self.fsobj.rename(self.parts, targetpath.parts)

    def rmdir(self):
        """ Removes the empty directory at this path. """
        return self.fsobj.rmdir(self.parts)

    def touch(self):
        """ Creates the file at this path, or updates the timestamp. """
        return self.fsobj.touch(self.parts)

    def unlink(self):
        """ Removes the file at this path. """
        return self.fsobj.unlink(self.parts)

    def removerecursive(self):
        """ Recursively deletes this file or directory. """
        if self.is_dir():
            for path in self.iterdir():
                path.removerecursive()
            self.rmdir()
        else:
            self.unlink()

    @property
    def mtime(self):
        """ Returns the time of last modification of the file or directory. """
        return self.fsobj.mtime(self.parts)

    @property
    def filesize(self):
        """ Returns the file size. """
        return self.fsobj.filesize(self.parts)

    def watch(self, callback):
        """
        Installs 'callback' as callback that gets invoked whenever the file at
        this path changes.

        Returns True if the callback was installed, and false if not
        (e.g. because the some OS limit was reached, or the underlying
         FSLikeObject doesn't support watches).
        """
        return self.fsobj.watch(self.parts, callback)

    def poll_fs_watches(self):
        """ Polls the installed watches for the entire file-system. """
        self.fsobj.poll_watches()

    @property
    def parent(self):
        """ Parent path object. The parent of root is root. """
        return type(self)(self.fsobj, self.parts[:-1])

    @property
    def name(self):
        """ The name of the topmost component (str). """
        return self.parts[-1].decode()

    @property
    def suffix(self):
        """ The last suffix of the name of the topmost component (str). """
        name = self.name
        pos = name.rfind('.')
        if pos <= 0:
            return ""
        return name[pos:]

    @property
    def suffixes(self):
        """ The suffixes of the name of the topmost component (str list). """
        name = self.name
        if name.startswith('.'):
            name = name[1:]
        return ['.' + suffix for suffix in name.split('.')[1:]]

    @property
    def stem(self):
        """ Name without suffix (such that stem + suffix == name). """
        name = self.name
        pos = name.rfind('.')
        if pos <= 0:
            return name
        else:
            return name[:pos]

    def joinpath(self, subpath):
        """ Returns path for the given subpath. """
        if isinstance(subpath, str):
            subpath = subpath.encode()

        if isinstance(subpath, bytes):
            subpath = subpath.split(b'/')

        return type(self)(self.fsobj, self.parts + tuple(subpath))

    def __getitem__(self, subpath):
        """ Like joinpath. """
        return self.joinpath(subpath)

    def with_name(self, name):
        """ Returns path for differing name (same parent). """
        return self.parent.joinpath(name)

    def with_suffix(self, suffix):
        """ Returns path for different suffix (same parent and stem). """
        if isinstance(suffix, bytes):
            suffix = suffix.decode()

        return self.parent.joinpath(self.stem + suffix)
