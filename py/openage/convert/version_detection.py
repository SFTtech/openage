# Copyright 2014-2015 the openage authors. See copying.md for legal info.

from openage.log import dbg
import os


class GameVersion:
    """
    Has sets of files belonging to a version of the game.
    Other versions can be given as prerequisites.
    Can check if all the needed files exist.
    """

    def __init__(self, name, drs_files=None, dat_files=None,
                 langdll_files=None, prereqs=None):
        """
        Creates a new GameVersion object.

        Arguments:
        name          -- Name of the version (for debug messages)
        drs_files     -- Set of paths to DRS archives
        dat_files     -- Set of paths to gamedata DAT files
        langdll_files -- Set of paths to language DLL files
        prereqs       -- Set of prerequisite GameVersion objects.
        """

        self.name = name
        self.files = {
            "drs": drs_files or set(),
            "dat": dat_files or set(),
            "langdll": langdll_files or set(),
        }
        self.prereqs = prereqs or set()

        self.checked   = False
        self.available = False

    def __str__(self):
        return self.name

    def exists(self, basedir):
        """
        Checks if the prerequisite versions exist. Then checks if all the files
        needed for this version exist.

        Arguments:
        basedir -- Path to an AoE2 installation
        """

        if self.checked:
            dbg("Check for %s has already been done, omitting." %
                self.name, 3)
            return self.available

        self.checked = True

        for prereq in self.prereqs:
            dbg("'%s' requires '%s'. Checking '%s' now..." % (
                self.name, prereq.name, prereq.name
            ), 3)

            if not prereq.exists(basedir):
                dbg("Check for '%s' failed because of prerequisite '%s'!" % (
                    self.name, prereq.name
                ), 2)
                return False

        for filename in self.get_own_files("all"):

            if not os.path.isfile(os.path.join(basedir, filename)):
                dbg("Check for '%s' failed: File '%s' not found." % (
                    self.name, filename
                ), 2)
                return False

            dbg("File '%s' OK." % filename, 4)

        dbg("Check for '%s' successful." % self.name, 3)
        self.available = True
        return True

    def get_files(self, filetype):
        """
        Returns a set of all files defined in this version and its prerequisites.

        Arguments:
        filetype -- One of 'drs', 'dat', 'langdll' or 'all'
        """

        result = self.get_own_files(filetype)
        for prereq in self.prereqs:
            result |= prereq.get_files(filetype)

        return result

    def get_own_files(self, filetype):
        """
        Returns a set of all files defined in this version.

        Arguments:
        filetype -- One of 'drs', 'dat', 'langdll' or 'all'
        """

        if filetype == "all":
            return (
                self.get_own_files("drs") |
                self.get_own_files("dat") |
                self.get_own_files("langdll")
            )

        return set(self.files[filetype])


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
        Returns a set of all relevant files found, filtered by filetype.

        Arguments:
        filetype -- One of 'drs', 'dat', 'langdll' or 'all'
        """

        return self.version.get_files(filetype)
