# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Provides filesystem-like objects that implement the abstract interfaces from
the fslikebastract module:

 - Directory
    represents an actual directory in the file system.

 - CaseIgnoringReadOnlyDirectory
    like directory, but ignores case, and is read-only.

 - SubDirectory
    sub directory of an other filesystem-like object.

 - FileCollection
    allows mounting file-like objects.

 - Union
    allows mounting filesystem-like objects.

 - Wrapper
    simply wraps an other filesystem-like object, changing nothing.
    designed to be inherited from.
"""

import os
from io import UnsupportedOperation
from collections import OrderedDict

from .fslikeabstract import FileSystemLikeObject, ReadOnlyFileSystemLikeObject
from .fslikeabstract import parse_pathname


class Directory(FileSystemLikeObject):
    """
    Verbatim directory wrapper.

    The constructor takes the actual directory path as an argument.

    Provides a functional implementation of watch(), which requires you to
    call poll_file_watches().
    """

    # TODO on win32, Directory must enforce fspath case
    #      (raise FileNotFoundError if the wrong case is used).
    #      otherwise, mods created on Win32 might not work on other platforms.

    def __init__(self, fspath, create_if_noexist=False):
        if not os.path.isdir(fspath):
            if create_if_noexist:
                os.makedirs(fspath)
            else:
                raise FileNotFoundError(fspath)

        self.fspath = fspath

    def actual_filepath(self, pathcomponents):
        """
        Returns the actual path that is pointed at by pathcomponents.

        Designed for internal usage mostly, but may be used from outside.
        note that this is _not_ available in other filesystem-like objects!
        """
        return os.path.join(self.fspath, os.path.sep.join(pathcomponents))

    def _listfiles_impl(self, pathcomponents):
        fspath = self.actual_filepath(pathcomponents)
        for entry in os.listdir(fspath):
            if os.path.isfile(os.path.join(fspath, entry)):
                yield entry

    def _listdirs_impl(self, pathcomponents):
        fspath = self.actual_filepath(pathcomponents)
        for entry in os.listdir(fspath):
            if os.path.isdir(os.path.join(fspath, entry)):
                yield entry

    def _isfile_impl(self, pathcomponents):
        fspath = self.actual_filepath(pathcomponents)
        return os.path.isfile(fspath)

    def _isdir_impl(self, pathcomponents):
        fspath = self.actual_filepath(pathcomponents)
        return os.path.isdir(fspath)

    def _open_impl(self, pathcomponents, mode):
        fspath = self.actual_filepath(pathcomponents)
        return open(fspath, mode=mode)

    def _mkdirs_impl(self, pathcomponents):
        fspath = self.actual_filepath(pathcomponents)
        os.makedirs(fspath, exist_ok=True)

    def _remove_impl(self, pathcomponents):
        fspath = self.actual_filepath(pathcomponents)
        os.remove(fspath)

    def _rmdir_impl(self, pathcomponents):
        fspath = self.actual_filepath(pathcomponents)
        os.rmdir(fspath)

    def _writable_impl(self, pathcomponents):
        fspath = self.actual_filepath(pathcomponents)
        while not os.path.exists(fspath):
            fspath, _ = os.path.split(fspath)
            if not fspath:
                return False

        return os.access(fspath, os.W_OK)

    def _mtime_impl(self, pathcomponents):
        fspath = self.actual_filepath(pathcomponents)
        return os.path.getmtime(fspath)

    def _filesize_impl(self, pathcomponents):
        fspath = self.actual_filepath(pathcomponents)
        return os.path.getsize(fspath)

    def _watch_impl(self, pathcomponents, callback):
        fspath = self.actual_filepath(pathcomponents)
        # TODO implement (via inotify and other platform-specific libs)
        #      decide whether to use python-inotify, or the cpp interface.
        del self, fspath, callback  # unused for now
        return

    def poll_file_watches(self):
        """
        Checks whether any of the watched files have been updated, and invokes
        all the relevant callbacks.
        """
        # TODO implement
        # pylint: disable=no-self-use
        del self  # unused for now

    def __repr__(self):
        return "Directory({})".format(repr(self.fspath))


class CaseIgnoringReadOnlyDirectory(ReadOnlyFileSystemLikeObject):
    """
    Read-only, case-ignoring wrapper for an actual file system directory.

    The constructor takes the actual directory path as an argument.
    """
    def __init__(self, fspath):
        # sanity check
        if not os.path.isdir(fspath):
            raise FileNotFoundError("path is not a directory: " + fspath)

        self.fspath = fspath

        # caches directory listings for actual file system paths.
        self.listingcache = {}

    def list_directory(self, fspath):
        """
        Lists the files and subdirs in an actual file system path.

        Returns a tuple of conflicts, files, subdirs.

        conflicts is a set that contains all lower-case file and subdir
        names that are conflicting for ignore-case reasons.

        files is a dict that maps lower-case file names to the actual file
        paths.

        subdirs is a dict that maps lower-case subdir names to the actual
        subdir paths.

        If list_directory has been run in the past for a given fspath,
        the cached values are used.
        """
        try:
            # attempt to use cached value
            return self.listingcache[fspath]
        except KeyError:
            pass

        conflicts = set()
        files = dict()
        subdirs = dict()

        for entry in os.listdir(fspath):
            entrypath = os.path.join(fspath, entry)
            entry = entry.lower()

            if entry in files or entry in subdirs:
                # conflict!
                conflicts.add(entry)

            if os.path.isdir(entrypath):
                subdirs[entry] = entrypath
            else:
                files[entry] = entrypath

        result = conflicts, files, subdirs
        self.listingcache[fspath] = result

        return result

    def get_fspath(self, pathcomponents):
        """
        For given path components, returns the actual file system path for the
        (existing) file or directory pointed at by pathcomponents.

        Raises FileNotFoundError on failure.
        """
        result = self.fspath

        for component in pathcomponents:
            component = component.lower()

            conflicts, files, subdirs = self.list_directory(result)

            if component in conflicts:
                raise FileNotFoundError(
                    "There are multiple conflicting file system entries for "
                    "case-ignored " + os.path.join(result, component))

            rightcase = files.get(component)

            if rightcase is None:
                rightcase = subdirs.get(component)
            if rightcase is None:
                raise FileNotFoundError(
                    "No such (case-ignored) file or directory: " +
                    os.path.join(result, component))

            result = os.path.join(result, rightcase)

        return result

    def _listfiles_impl(self, pathcomponents):
        fspath = self.get_fspath(pathcomponents)
        _, files, _ = self.list_directory(fspath)
        return sorted(files)

    def _listdirs_impl(self, pathcomponents):
        fspath = self.get_fspath(pathcomponents)
        _, _, subdirs = self.list_directory(fspath)
        return sorted(subdirs)

    def _isfile_impl(self, pathcomponents):
        try:
            fspath = self.get_fspath(pathcomponents)
        except FileNotFoundError:
            return False

        return os.path.isfile(fspath)

    def _isdir_impl(self, pathcomponents):
        try:
            fspath = self.get_fspath(pathcomponents)
        except FileNotFoundError:
            return False

        return os.path.isdir(fspath)

    def _open_impl(self, pathcomponents, mode):
        fspath = self.get_fspath(pathcomponents)
        return open(fspath, mode)

    def _mtime_impl(self, pathcomponents):
        fspath = self.get_fspath(pathcomponents)
        return os.path.getmtime(fspath)

    def _filesize_impl(self, pathcomponents):
        fspath = self.get_fspath(pathcomponents)
        return os.path.getsize(fspath)

    def _watch_impl(self, pathcomponents, callback):
        # no actual functionality
        del self, pathcomponents, callback  # unused
        return

    def __repr__(self):
        return "CaseIgnoringReadOnlyDirectory({})".format(repr(self.fspath))


class SubDirectory(FileSystemLikeObject):
    """
    Represents a subdirectory of an other filesystem-like object.

    The constructor takes the filesystem-like object, plus the
    subdirectory path.
    """

    # pylint: disable=too-few-public-methods

    def __init__(self, obj, path, create_if_noexist=False):
        self.obj = obj
        self.prefixpathcomponents = parse_pathname(path)
        if not self.obj.isdir(self.prefixpathcomponents):
            if create_if_noexist:
                self.obj.mkdirs(self.prefixpathcomponents)
            else:
                raise FileNotFoundError(
                    "subdirectory " + str(path) + " of " + str(obj))

    def _listfiles_impl(self, pathcomponents):
        pathcomponents = self.prefixpathcomponents + pathcomponents
        return self.obj.listfiles(pathcomponents)

    def _listdirs_impl(self, pathcomponents):
        pathcomponents = self.prefixpathcomponents + pathcomponents
        return self.obj.listdirs(pathcomponents)

    def _isfile_impl(self, pathcomponents):
        pathcomponents = self.prefixpathcomponents + pathcomponents
        return self.obj.isfile(pathcomponents)

    def _isdir_impl(self, pathcomponents):
        pathcomponents = self.prefixpathcomponents + pathcomponents
        return self.obj.isdir(pathcomponents)

    def _open_impl(self, pathcomponents, mode):
        pathcomponents = self.prefixpathcomponents + pathcomponents
        return self.obj.open(pathcomponents, mode)

    def _mkdirs_impl(self, pathcomponents):
        pathcomponents = self.prefixpathcomponents + pathcomponents
        return self.obj.mkdirs(pathcomponents)

    def _remove_impl(self, pathcomponents):
        pathcomponents = self.prefixpathcomponents + pathcomponents
        return self.obj.remove(pathcomponents)

    def _rmdir_impl(self, pathcomponents):
        pathcomponents = self.prefixpathcomponents + pathcomponents
        return self.obj.rmdir(pathcomponents)

    def _writable_impl(self, pathcomponents):
        pathcomponents = self.prefixpathcomponents + pathcomponents
        return self.obj.writable(pathcomponents)

    def _mtime_impl(self, pathcomponents):
        pathcomponents = self.prefixpathcomponents + pathcomponents
        return self.obj.mtime(pathcomponents)

    def _filesize_impl(self, pathcomponents):
        pathcomponents = self.prefixpathcomponents + pathcomponents
        return self.obj.filesize(pathcomponents)

    def _watch_impl(self, pathcomponents, callback):
        # no actual functionality
        del self, pathcomponents, callback  # unused
        return

    def __repr__(self):
        return "SubDirectory({}, {})".format(
            repr(self.obj),
            repr('/'.join(self.prefixpathcomponents)))


class FileCollection(ReadOnlyFileSystemLikeObject):
    """
    A filesystem-like object for one or more single file-like objects.

    The constructor takes no arguments; more objects can be added
    via add_file.
    """
    def __init__(self):
        self.openers = {}
        self.mtimes = {}
        self.filesizes = {}

        self.dirs = {tuple()}

    def add_file(self, pathname, opener, mtime=None, filesize=None):
        """
        Adds file at pathname; replaces a possible existing file of the same
        name.

        Opener must be a callable that accepts a 'mode' parameter, and returns
        an opened file object.

        Adds all parent directories of pathname.
        """
        if not callable(opener):
            raise TypeError("opener must be a callable that opens the file.")

        pathcomponents = parse_pathname(pathname)

        self.openers[pathcomponents] = opener
        self.mtimes[pathcomponents] = mtime
        self.filesizes[pathcomponents] = filesize

        while pathcomponents:
            pathcomponents = pathcomponents[:-1]
            self.dirs.add(pathcomponents)

    def add_dir(self, pathname):
        """
        Adds an empty directory; entirely optional.

        Directories get auto-added when a file is added.
        """
        self.dirs.add(parse_pathname(pathname))

    def _listfiles_impl(self, pathcomponents):
        pathcomponents = tuple(pathcomponents)

        if pathcomponents not in self.dirs:
            raise FileNotFoundError(
                "no such directory: " + '/'.join(pathcomponents))

        for filepath in self.openers:
            if not len(filepath) == len(pathcomponents) + 1:
                continue

            if filepath[:len(pathcomponents)] != pathcomponents:
                continue

            yield filepath[len(pathcomponents)]

    def _listdirs_impl(self, pathcomponents):
        pathcomponents = tuple(pathcomponents)

        if pathcomponents not in self.dirs:
            raise FileNotFoundError(
                "no such directory: " + '/'.join(pathcomponents))

        for dirpath in self.dirs:
            if not len(dirpath) == len(pathcomponents) + 1:
                continue

            if dirpath[:len(pathcomponents)] != pathcomponents:
                continue

            yield dirpath[len(pathcomponents)]

    def _isfile_impl(self, pathcomponents):
        return tuple(pathcomponents) in self.openers

    def _isdir_impl(self, pathcomponents):
        return tuple(pathcomponents) in self.dirs

    def _open_impl(self, pathcomponents, mode):
        try:
            opener = self.openers[tuple(pathcomponents)]
        except KeyError:
            raise FileNotFoundError(
                "no such file " + '/'.join(pathcomponents)) from None

        return opener(mode)

    def _mtime_impl(self, pathcomponents):
        pathcomponents = tuple(pathcomponents)
        if pathcomponents in self.dirs:
            raise UnsupportedOperation("mtime not known for directories")

        try:
            mtime = self.mtimes[pathcomponents]
        except KeyError:
            raise FileNotFoundError(
                "no such file " + '/'.join(pathcomponents)) from None

        if mtime is None:
            raise UnsupportedOperation(
                "mtime unknown for " + '/'.join(pathcomponents))

        return mtime

    def _filesize_impl(self, pathcomponents):
        pathcomponents = tuple(pathcomponents)
        try:
            filesize = self.filesizes[pathcomponents]
        except KeyError:
            raise FileNotFoundError(
                "no such file " + '/'.join(pathcomponents)) from None

        if filesize is None:
            raise UnsupportedOperation(
                "file size unknown for " + '/'.join(pathcomponents))

        return filesize

    def _watch_impl(self, pathcomponents, callback):
        # no actual functionality
        del self, pathcomponents, callback  # unused
        return

    def __repr__(self):
        return "FileCollection(<{} objects>)".format(len(self.openers))


class Union(FileSystemLikeObject):
    """
    Represents a union/overlay of several filesystem-like objects.

    The constructor takes no arguments; more objects can be added
    to the unit via mount().

    Example:

    If you
        - init the Union
        - mount A (containing the files a, b, c) to x
        - mount B (containing the files x/c, d) to /

    listdirs('.') will return ['x']
    listfiles('.') will be ['d']
    listfiles('x') will be ['a', 'b', 'c']
    open('x/c') will return 'x/c' from B
    open('x/e', 'w') will create the file in B (if B.writable('x'))
    """
    def __init__(self):
        # a list of tuples of (mountpoint, fs/file-like obj, priority)
        self.mounts = []

        # a set of directories that are parent dirs of mount points,
        # and thus implicitly exist.
        self.mountparentdirs = {tuple()}

    def mount(self, obj, mountpoint, priority=0):
        """
        Mounts obj at mountpoint.

        Priority (defaults to 0) decides the priority of the mount.
        Mounts with equal priority are treated as if later mounts had higher
        priority.

        Mountpoints do not need to exist.
        Multiple filesystem-like objects can be mounted at the same mount
        point; in case of conflicts, the file system with the higher priority
        counts.
        """
        mountpoint = parse_pathname(mountpoint)

        # search for the right place to insert the mount
        idx = len(self.mounts) - 1
        while idx >= 0 and priority >= self.mounts[idx][2]:
            idx -= 1

        self.mounts.insert(idx, (mountpoint, obj, priority))

        mountpoint = tuple(mountpoint)
        while mountpoint:
            self.mountparentdirs.add(mountpoint)
            mountpoint = mountpoint[:-1]

    def relevant_mounts(self, pathcomponents, must_be_dir=False):
        """
        Yields the relevant mounts for pathcomponents, as tuples of

        remainingpathcomponents, obj.

        Yields the highest-priority mounts first.

        If must_be_dir is True, only objs where remainingpathcomponents is a
        dir are yielded.
        """
        for mountpoint, obj, _ in self.mounts:
            if pathcomponents[:len(mountpoint)] == mountpoint:
                remainingpathcomponents = pathcomponents[len(mountpoint):]
                if must_be_dir and not obj.isdir(remainingpathcomponents):
                    continue

                yield remainingpathcomponents, obj

    def _listfiles_impl(self, pathcomponents):
        files = OrderedDict()

        dir_exists = tuple(pathcomponents) in self.mountparentdirs

        for path, obj in self.relevant_mounts(pathcomponents, True):
            for filename in obj.listfiles(path):
                files[filename] = None

            dir_exists = True

        if not dir_exists:
            raise FileNotFoundError(
                "no such directory: " + '/'.join(pathcomponents))

        return list(files)

    def _listdirs_impl(self, pathcomponents):
        dirs = OrderedDict()

        dir_exists = tuple(pathcomponents) in self.mountparentdirs

        for path, obj in self.relevant_mounts(pathcomponents, True):
            for dirname in obj.listdirs(path):
                dirs[dirname] = None

            dir_exists = True

        # add the implicitly-existing mount point parent directories
        for mountparentdir in self.mountparentdirs:
            if mountparentdir and mountparentdir[:-1] == tuple(pathcomponents):
                dirs[mountparentdir[-1]] = None

        if not dir_exists:
            raise FileNotFoundError(
                "no such directory: " + '/'.join(pathcomponents))

        return list(dirs)

    def _isfile_impl(self, pathcomponents):
        for path, obj in self.relevant_mounts(pathcomponents):
            if obj.isfile(path):
                return True

        return False

    def _isdir_impl(self, pathcomponents):
        if tuple(pathcomponents) in self.mountparentdirs:
            return True

        for path, obj in self.relevant_mounts(pathcomponents):
            if obj.isdir(path):
                return True

        return False

    def _open_impl(self, pathcomponents, mode):
        if '+' in mode:
            def mountfilter(obj, path):
                """ require writable, existing files for modifying """
                return obj.writable(path) and obj.isfile(path)

            errmsg = "no writable file"

        elif 'w' in mode or 'a' in mode or 'x' in mode:
            def mountfilter(obj, path):
                """ require writable paths for creating """
                return obj.writable(path)

            errmsg = "no writable path for file"

        else:
            def mountfilter(obj, path):
                """ require existing files for reading """
                return obj.isfile(path)

            errmsg = "no such file"

        for path, obj in self.relevant_mounts(pathcomponents):
            if mountfilter(obj, path):
                return obj.open(path, mode)

        raise FileNotFoundError(errmsg + " " + '/'.join(pathcomponents))

    def _mkdirs_impl(self, pathcomponents):
        for path, obj in self.relevant_mounts(pathcomponents):
            if obj.writable(path):
                return obj.mkdirs(path)

        raise UnsupportedOperation(
            "no writable mountpoint to create " + '/'.join(pathcomponents))

    def _remove_impl(self, pathcomponents):
        found = False

        # remove the file in all mounts where it exists
        for path, obj in self.relevant_mounts(pathcomponents):
            if obj.isfile(path):
                obj.remove(path)
                found = True

        if not found:
            raise FileNotFoundError('/'.join(pathcomponents))

    def _rmdir_impl(self, pathcomponents):
        found = False

        # remove the directory in all mounts where it exists
        for path, obj in self.relevant_mounts(pathcomponents):
            if obj.isdir(path):
                obj.rmdir(path)
                found = True

        if not found:
            raise FileNotFoundError('/'.join(pathcomponents))

    def _writable_impl(self, pathcomponents):
        for path, obj in self.relevant_mounts(pathcomponents):
            if obj.writable(path):
                return True

        return False

    def _mtime_impl(self, pathcomponents):
        for path, obj in self.relevant_mounts(pathcomponents):
            if obj.isfile(path) or obj.isdir(path):
                return obj.mtime(path)

        raise FileNotFoundError('/'.join(pathcomponents))

    def _filesize_impl(self, pathcomponents):
        for path, obj in self.relevant_mounts(pathcomponents):
            if obj.isfile(path):
                return obj.filesize(path)

        raise FileNotFoundError('/'.join(pathcomponents))

    def _watch_impl(self, pathcomponents, callback):
        # no actual functionality
        del self, pathcomponents, callback  # unused
        return

    def __repr__(self):
        return "Union({})".format(repr(self.mounts))


class FSLikeObjectWrapper(FileSystemLikeObject):
    """
    Simply wraps an other filesystem-like object without changing anything.

    The constructor takes the wrappee filesystem-like object as an argument.

    Designed to be used by child classes that overwrite specific methods.
    """
    # pylint doesn't understand that this simply implements the abstract
    # interface, and doesn't need any more methods.
    # pylint: disable=too-few-public-methods

    def __init__(self, wrapped):
        self.wrapped = wrapped

    def _listdirs_impl(self, pathcomponents):
        return self.wrapped.listdirs(pathcomponents)

    def _listfiles_impl(self, pathcomponents):
        return self.wrapped.listfiles(pathcomponents)

    def _isdir_impl(self, pathcomponents):
        return self.wrapped.isdir(pathcomponents)

    def _isfile_impl(self, pathcomponents):
        return self.wrapped.isfile(pathcomponents)

    def _open_impl(self, pathcomponents, mode):
        return self.wrapped.open(pathcomponents, mode)

    def _mkdirs_impl(self, pathcomponents):
        return self.wrapped.mkdirs(pathcomponents)

    def _remove_impl(self, pathcomponents):
        return self.wrapped.remove(pathcomponents)

    def _rmdir_impl(self, pathcomponents):
        return self.wrapped.rmdir(pathcomponents)

    def _writable_impl(self, pathcomponents):
        return self.wrapped.writable(pathcomponents)

    def _mtime_impl(self, pathcomponents):
        return self.wrapped.mtime(pathcomponents)

    def _filesize_impl(self, pathcomponents):
        return self.wrapped.filesize(pathcomponents)

    def _watch_impl(self, pathcomponents, callback):
        return self.wrapped.watch(pathcomponents, callback)

    def __repr__(self):
        return "FSLikeObjectWrapper({})".format(self.wrapped)


class WriteBlocker(ReadOnlyFileSystemLikeObject, FSLikeObjectWrapper):
    """
    Wraps an other filesystem-like object, blocking all writing
    (and returning False for writable()).
    """
    def __repr__(self):
        return "WriteBlocker({})".format(self.wrapped)
