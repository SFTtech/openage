# Copyright 2015-2026 the openage authors. See copying.md for legal info.

"""
Provides Union, a utility class for combining multiple FSLikeObjects to a
single one.
"""

from io import UnsupportedOperation

from .abstract import FSLikeObject, Subpath
from .path import Path


class UnionPath(Path):
    """
    Provides an additional method for mounting an other path at this path.
    """

    def mount(self, pathobj: Path, priority: int = 0) -> None:
        """
        Mounts pathobj here. All parent directories are 'created', if needed.
        """
        assert isinstance(self.fsobj, Union)
        return self.fsobj.add_mount(pathobj, self.subpath, priority)

    def unmount(self, pathobj: Path | None = None) -> None:
        """
        Unmount a path from the union described by this path.
        This is like "unmounting /home", no matter what the source was.
        If you provide `pathobj`, that source is checked, additionally.

        It will error if that path was not mounted.
        """
        assert isinstance(self.fsobj, Union)
        self.fsobj.remove_mount(self.subpath, pathobj)


class Union(FSLikeObject):
    """
    FSLikeObject that provides a structure for mounting several path objects.

    Unlike in POSIX, mounts may overlap.
    If multiple mounts match for a directory, those that have a higher
    priority are preferred.
    In case of equal priorities, later mounts are preferred.
    """

    def __init__(self):
        super().__init__()

        # (mountpoint, pathobj, priority), sorted by priority.
        self.mounts = []

        # mountpoints and their parent directories, {name: {...}}.
        # these are the virtual empty folders where mounts can be done
        self.dirstructure = {}

    def __str__(self):
        content = ", ".join([f"{pnt[1]!s} @ {pnt[0]!s}" for pnt in self.mounts])
        return f"Union({content})"

    @property
    def root(self) -> UnionPath:
        return UnionPath(path=[], fsobj=self)

    def add_mount(self, pathobj: Path, mountpoint: Subpath, priority: int) -> None:
        """
        This method should not be called directly; instead, use the mount
        method of Path objects that were obtained from this.

        Mounts pathobj at mountpoint, with the given priority.
        """

        if not isinstance(pathobj, Path):
            raise PermissionError(f"only a fslike.Path can be mounted, not {type(pathobj)}")

        # search for the right place to insert the mount.
        idx = len(self.mounts) - 1
        while idx >= 0 and priority >= self.mounts[idx][2]:
            idx -= 1

        self.mounts.insert(idx + 1, (tuple(mountpoint), pathobj, priority))

        # 'create' parent directories as needed.
        dirstructure = self.dirstructure
        for subdir in mountpoint:
            dirstructure = dirstructure.setdefault(subdir, {})

    def remove_mount(self, search_mountpoint: Subpath, source_pathobj: Path | None = None) -> None:
        """
        Remove a mount from the union by searching for the source
        that provides the given mountpoint.
        Additionally, can check if the source equals the given pathobj.
        """

        unmount = []

        for idx, (mountpoint, pathobj, _) in enumerate(self.mounts):
            # cut the search so prefixes can be matched.
            if mountpoint == tuple(search_mountpoint[: len(mountpoint)]):
                if not source_pathobj or source_pathobj == pathobj:
                    unmount.append(idx)

        if unmount:
            # reverse the order so that the indices never shift.
            for idx in sorted(unmount, reverse=True):
                del self.mounts[idx]

        else:
            raise ValueError("could not find mounted source")

    def candidate_paths(self, subpath: Subpath):
        """
        Helper method.

        Yields path objects from all mounts that match subpath, in the order of
        their priorities.
        """

        for mountpoint, pathobj, _ in self.mounts:
            cut_subpath = tuple(subpath[: len(mountpoint)])
            if mountpoint == cut_subpath:
                yield pathobj.joinpath(subpath[len(mountpoint) :])

    def open_r(self, subpath: Subpath):
        for path in self.candidate_paths(subpath):
            if path.is_file():
                return path.open_r()
        raise FileNotFoundError("/".join(subpath))

    def open_w(self, subpath: Subpath):
        for path in self.candidate_paths(subpath):
            if path.writable():
                return path.open_w()

        raise UnsupportedOperation("not writable: " + "/".join(subpath))

    def open_a(self, subpath: Subpath):
        for path in self.candidate_paths(subpath):
            if path.writable():
                return path.open_a()

        raise UnsupportedOperation("not appendable: " + "/".join(subpath))

    def open_rw(self, subpath: Subpath):
        for path in self.candidate_paths(subpath):
            if path.writable():
                return path.open_rw()

        raise UnsupportedOperation("not writable: " + "/".join(subpath))

    def open_ar(self, subpath: Subpath):
        for path in self.candidate_paths(subpath):
            if path.writable():
                return path.open_ar()

        raise UnsupportedOperation("not appendable: " + "/".join(subpath))

    def resolve_r(self, subpath: Subpath):
        for path in self.candidate_paths(subpath):
            if path.is_file() or path.is_dir():
                # pylint: disable=protected-access
                return path._resolve_r()
        return None

    def resolve_w(self, subpath: Subpath):
        for path in self.candidate_paths(subpath):
            if path.writable():
                # pylint: disable=protected-access
                return path._resolve_w()
        return None

    def list(self, subpath: Subpath):
        duplicates = set()

        dir_exists = False

        dirstructure = self.dirstructure
        try:
            # "cd" into the virtual dirstructure
            for subdir in subpath:
                dirstructure = dirstructure[subdir]

            dir_exists = True

            # yield the virtual folders in this folder
            yield from dirstructure
            duplicates.update(dirstructure)

        except KeyError:
            dir_exists = False

        for path in self.candidate_paths(subpath):
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
            raise FileNotFoundError("/".join(subpath))

    def filesize(self, subpath: Subpath) -> int:
        for path in self.candidate_paths(subpath):
            if path.is_file():
                return path.filesize

        raise FileNotFoundError("/".join(subpath))

    def mtime(self, subpath: Subpath) -> float:
        for path in self.candidate_paths(subpath):
            if path.exists():
                return path.mtime

        raise FileNotFoundError("/".join(subpath))

    def mkdirs(self, subpath: Subpath) -> None:
        for path in self.candidate_paths(subpath):
            if path.writable():
                return path.mkdirs()
        return None

    def rmdir(self, subpath: Subpath) -> None:
        found = False

        # remove the directory in all mounts where it exists
        for path in self.candidate_paths(subpath):
            if path.is_dir():
                path.rmdir()
                found = True

        if not found:
            raise FileNotFoundError("/".join(subpath))

    def unlink(self, subpath: Subpath) -> None:
        found = False

        # remove the file in all mounts where it exists
        for path in self.candidate_paths(subpath):
            if path.is_file():
                path.unlink()
                found = True

        if not found:
            raise FileNotFoundError("/".join(subpath))

    def touch(self, subpath: Subpath) -> None:
        for path in self.candidate_paths(subpath):
            if path.writable():
                return path.touch()

        raise FileNotFoundError("/".join(subpath))

    def rename(self, srcsubpath: Subpath, tgtsubpath: Subpath) -> None:
        found = False

        for srcpath in self.candidate_paths(srcsubpath):
            if srcpath.exists():
                found = True
                if srcpath.writable():
                    for tgtpath in self.candidate_paths(tgtsubpath):
                        if tgtpath.writable():
                            return srcpath.rename(tgtpath)

        if found:
            raise UnsupportedOperation(
                "read-only rename: " + "/".join(srcsubpath) + " to " + "/".join(tgtsubpath)
            )
        raise FileNotFoundError("/".join(srcsubpath))

    def is_file(self, subpath: Subpath) -> bool:
        for path in self.candidate_paths(subpath):
            if path.is_file():
                return True

        return False

    def is_dir(self, subpath: Subpath) -> bool:
        try:
            dirstructure = self.dirstructure
            for part in subpath:
                dirstructure = dirstructure[part]
            return True
        except KeyError:
            pass

        for path in self.candidate_paths(subpath):
            if path.is_dir():
                return True

        return False

    def writable(self, subpath: Subpath) -> bool:
        for path in self.candidate_paths(subpath):
            if path.writable():
                return True

        return False

    def watch(self, subpath: Subpath, callback) -> bool:
        watching = False
        for path in self.candidate_paths(subpath):
            if path.exists():
                watching = watching or path.watch(callback)

        return watching

    def poll_watches(self):
        for _, pathobj, _ in self.mounts:
            pathobj.poll_fs_watches()
