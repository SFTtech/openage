# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Provides Union, a utility class for combining multiple FSLikeObjects to a
single one.
"""

from io import UnsupportedOperation

from .abstract import FSLikeObject
from .path import Path


class Union(FSLikeObject):
    """
    FSLikeObject that provides a structure for mounting several path objects.

    Unlike in POSIX, mounts may overlap.
    If multiple mounts match for a directory, those that have a higher
    priority are preferred.
    In case of equal priorities, later mounts are preferred.
    """

    # we can hardly reduce the method amount...
    # pylint: disable=too-many-public-methods

    def __init__(self):
        super().__init__()

        # (mountpoint, pathobj, priority), sorted by priority.
        self.mounts = []

        # mountpoints and their parent directories, {name: {...}}.
        # these are the virtual empty folders where mounts can be done
        self.dirstructure = {}

    def __str__(self):
        return "Union({})".format(
            ", ".join(["%s @ %s" % (repr(pnt[1]),
                                    repr(pnt[0]))
                       for pnt in self.mounts])
        )

    @property
    def root(self):
        return UnionPath(self, [])

    def add_mount(self, pathobj, mountpoint, priority):
        """
        This method should not be called directly; instead, use the mount
        method of Path objects that were obtained from this.

        Mounts pathobj at mountpoint, with the given priority.
        """

        if not isinstance(pathobj, Path):
            raise Exception("only a fslike.Path can be mounted, "
                            "not {}".format(type(pathobj)))

        # search for the right place to insert the mount.
        idx = len(self.mounts) - 1
        while idx >= 0 and priority >= self.mounts[idx][2]:
            idx -= 1

        self.mounts.insert(idx + 1, (tuple(mountpoint), pathobj, priority))

        # 'create' parent directories as needed.
        dirstructure = self.dirstructure
        for subdir in mountpoint:
            dirstructure = dirstructure.setdefault(subdir, {})

    def remove_mount(self, search_mountpoint, source_pathobj=None):
        """
        Remove a mount from the union by searching for the source
        that provides the given mountpoint.
        Additionally, can check if the source equals the given pathobj.
        """

        unmount = []

        for idx, (mountpoint, pathobj, _) in enumerate(self.mounts):
            # cut the search so prefixes can be matched.
            if mountpoint == tuple(search_mountpoint[:len(mountpoint)]):
                if not source_pathobj or source_pathobj == pathobj:
                    unmount.append(idx)

        if unmount:
            # reverse the order so that the indices never shift.
            for idx in reversed(sorted(unmount)):
                del self.mounts[idx]

        else:
            raise ValueError("could not find mounted source")

    def candidate_paths(self, parts):
        """
        Helper method.

        Yields path objects from all mounts that match parts, in the order of
        their priorities.
        """

        for mountpoint, pathobj, _ in self.mounts:
            cut_parts = tuple(parts[:len(mountpoint)])
            if mountpoint == cut_parts:
                yield pathobj.joinpath(parts[len(mountpoint):])

    def open_r(self, parts):
        for path in self.candidate_paths(parts):
            if path.is_file():
                return path.open_r()
        raise FileNotFoundError(b'/'.join(parts))

    def open_w(self, parts):
        for path in self.candidate_paths(parts):
            if path.writable():
                return path.open_w()

        raise UnsupportedOperation(
            "not writable: " + b'/'.join(parts).decode(errors='replace'))

    def resolve_r(self, parts):
        for path in self.candidate_paths(parts):
            if path.is_file() or path.is_dir():
                # pylint: disable=protected-access
                return path._resolve_r()
        return None

    def resolve_w(self, parts):
        for path in self.candidate_paths(parts):
            if path.writable():
                # pylint: disable=protected-access
                return path._resolve_w()
        return None

    def list(self, parts):
        duplicates = set()

        dir_exists = False

        dirstructure = self.dirstructure
        try:
            # "cd" into the virtual dirstructure
            for subdir in parts:
                dirstructure = dirstructure[subdir]

            dir_exists = True

            # yield the virtual folders in this folder
            yield from dirstructure
            duplicates.update(dirstructure)

        except KeyError:
            dir_exists = False

        for path in self.candidate_paths(parts):
            if path.is_file():
                raise NotADirectoryError(repr(path))
            if not path.is_dir():
                continue

            dir_exists = True

            for name in path.list():
                if name not in duplicates:
                    yield name
                    duplicates.add(name)

        if not dir_exists:
            raise FileNotFoundError(b'/'.join(parts))

    def filesize(self, parts):
        for path in self.candidate_paths(parts):
            if path.is_file():
                return path.filesize

        raise FileNotFoundError(b'/'.join(parts))

    def mtime(self, parts):
        for path in self.candidate_paths(parts):
            if path.exists():
                return path.mtime

        raise FileNotFoundError(b'/'.join(parts))

    def mkdirs(self, parts):
        for path in self.candidate_paths(parts):
            if path.writable():
                return path.mkdirs()

    def rmdir(self, parts):
        found = False

        # remove the directory in all mounts where it exists
        for path in self.candidate_paths(parts):
            if path.is_dir():
                path.rmdir()
                found = True

        if not found:
            raise FileNotFoundError(b'/'.join(parts))

    def unlink(self, parts):
        found = False

        # remove the file in all mounts where it exists
        for path in self.candidate_paths(parts):
            if path.is_file():
                path.unlink()
                found = True

        if not found:
            raise FileNotFoundError(b'/'.join(parts))

    def touch(self, parts):
        for path in self.candidate_paths(parts):
            if path.writable():
                return path.touch()

        raise FileNotFoundError(b'/'.join(parts))

    def rename(self, srcparts, tgtparts):
        found = False

        for srcpath in self.candidate_paths(srcparts):
            if srcpath.exists():
                found = True
                if srcpath.writable():
                    for tgtpath in self.candidate_paths(tgtparts):
                        if tgtpath.writable():
                            return srcpath.rename(tgtpath)

        if found:
            raise UnsupportedOperation(
                "read-only rename: " +
                b'/'.join(srcparts).decode(errors='replace') + ' to ' +
                b'/'.join(tgtparts).decode(errors='replace'))
        else:
            raise FileNotFoundError(b'/'.join(srcparts))

    def is_file(self, parts):
        for path in self.candidate_paths(parts):
            if path.is_file():
                return True

        return False

    def is_dir(self, parts):
        try:
            dirstructure = self.dirstructure
            for part in parts:
                dirstructure = dirstructure[part]
            return True
        except KeyError:
            pass

        for path in self.candidate_paths(parts):
            if path.is_dir():
                return True

        return False

    def writable(self, parts):
        for path in self.candidate_paths(parts):
            if path.writable():
                return True

        return False

    def watch(self, parts, callback):
        watching = False
        for path in self.candidate_paths(parts):
            if path.exists():
                watching = watching or path.watch(callback)

        return watching

    def poll_watches(self):
        for _, pathobj, _ in self.mounts:
            pathobj.poll_fs_watches()


class UnionPath(Path):
    """
    Provides an additional method for mounting an other path at this path.
    """
    def mount(self, pathobj, priority=0):
        """
        Mounts pathobj here. All parent directories are 'created', if needed.
        """
        return self.fsobj.add_mount(pathobj, self.parts, priority)

    def unmount(self, pathobj=None):
        """
        Unmount a path from the union described by this path.
        This is like "unmounting /home", no matter what the source was.
        If you provide `pathobj`, that source is checked, additionally.

        It will error if that path was not mounted.
        """
        self.fsobj.remove_mount(self.parts, pathobj)
