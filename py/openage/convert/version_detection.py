# Copyright 2014-2015 the openage authors. See copying.md for legal info.

from openage.log import dbg
import os
import glob
import itertools


def is_glob(txt):
    return txt != glob.escape(txt)


def file_exists(basedir, filename):
    filepath = os.path.join(basedir, filename)
    if is_glob(filename):
        glob_result = glob.glob(filepath)
        if len(glob_result) == 0:
            dbg("FAIL: No match for glob '%s'." % (filename), 2)
            return False
        dbg("Glob '%s' OK." % filename, 4)

    else:
        if not os.path.isfile(filepath):
            dbg("FAIL: File '%s' not found." % (filename), 2)
            return False
        dbg("File '%s' OK." % filename, 4)

    return True


class FU:
    """
    Something like a File Update. Also Fak U.
    A set of file globs with additional information if the set from
    the prerequisite should be updated or overridden.
    """
    def __init__(self, *files, override=False):
        self.files = set(files)
        self.override = override


class GameVersion:
    """
    Has sets of files belonging to a version of the game.
    Other versions can be given as prerequisites.
    Can check if all the needed files exist.
    """

    def __init__(self, name, interfac=None, drs_files=None, blendomatic=None,
                 dat_files=None, langdll_files=None, langhd_files=None,
                 prereqs=None):
        """
        Creates a new GameVersion object.

        Arguments:
        name          -- Name of the version (for debug messages)
        interfac      -- FU of globs of interface DRS archives
        drs_files     -- FU of globs of other DRS archives
        blendomatic   -- FU of globs of blendomatic DAT files
        dat_files     -- FU of globs of gamedata DAT files
        langdll_files -- FU of globs of language DLL files
        langhd_files  -- FU of globs of HD edition language files
        prereqs       -- Set of prerequisite GameVersion objects
        """

        self.name = name
        self.files = {
            "interfac":    interfac or FU(),
            "drs":         drs_files or FU(),
            "blendomatic": blendomatic or FU(),
            "dat":         dat_files or FU(),
            "langdll":     langdll_files or FU(),
            "langhd":      langhd_files or FU(),
        }
        self.prereqs = prereqs or set()

        self.checked   = False
        self.available = False

    def __str__(self):
        return self.name

    def exists(self, basedir):
        for filename in self.get_files('all'):
            if not file_exists(basedir, filename):
                return False

        return True

    def get_files(self, filetype):
        res = set()
        if filetype == "all":
            for ft in self.files.keys():
                res |= self.get_files(ft)
            return res

        fu = self.files[filetype]
        res |= fu.files

        if not fu.override:
            for prereq in self.prereqs:
                res |= prereq.get_files(filetype)

        return res


class VersionDetector:
    """
    Detects the "best" AoE2 version at a path.
    Gives access to the version name and relevant files.
    """

    def __init__(self, basedir):
        """
        Creates a new VersionDetector object.

        Arguments:
        basedir -- Path to an AoE2 installation
        """

        self.basedir = basedir
        self.version = None

        from .hardcoded.version_detection import known_versions
        for version in known_versions:
            if version.exists(self.basedir):
                dbg("Found version '%s'." % version.name, 1)
                self.version = version
                break

        if self.version is None:
            raise Exception("No valid version found!")

    def get_version_name(self):
        """
        Returns the name of the version found.
        """

        return self.version.name

    def get_files(self, filetype):
        """
        Returns a set of absolute paths to files. Globs are resolved.
        """

        globs = self.version.get_files(filetype)
        results = [glob.glob(os.path.join(self.basedir, g)) for g in globs]
        # Flatten the nested lists
        return set(itertools.chain(*results))
