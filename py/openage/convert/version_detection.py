#!/usr/bin/env python3
#
#version detection routines

from util import dbg
import os

class GameVersion:
    """
    Has lists of files belonging to a version of the game.
    Other versions can be given as prerequisites.
    """

    def __init__(self, name, drs_files=None, dat_files=None,
                 langdll_files=None, prereqs=None):
        """
        Creates a new GameVersion object.

        Arguments:
        name          -- Name of the version (for debug messages)
        drs_files     -- Set of search paths to DRS archives
        dat_files     -- Set of search paths to gamedata DAT files
        langdll_files -- Set of search paths to language DLL files
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
        Checks recursively if the required files exist for this version
        and all prerequisite versions.
        """

        if self.checked:
            dbg("Check for %s has already been done, omitting." %
                self.name, 2)
            return self.available

        self.checked = True

        for prereq in self.prereqs:
            dbg("'%s' requires '%s'. Checking '%s' now..." % (
                self.name, prereq.name, prereq.name
            ), 2)

            if not prereq.exists(basedir):
                dbg("Check for '%s' failed because of prerequisite '%s'!" % (
                    self.name, prereq.name
                ), 1)
                return False

        for filename in (self.files["drs"] |
                         self.files["dat"] |
                         self.files["langdll"]):

            if not os.path.isfile(os.path.join(basedir, filename)):
                dbg("Check for '%s' failed: File '%s' not found." % (
                    self.name, filename
                ), 1)
                return False

            dbg("File '%s' OK." % filename, 3)

        dbg("Check for '%s' successful." % self.name, 2)
        self.available = True
        return True

    def get_files(self, filetype):
        """
        Returns a set of all files defined in the version and its dependencies.
        Valid arguments for filetype are 'drs', 'dat', 'langdll' and 'all'.
        """

        if filetype == "all":
            return (
                self.get_files("drs") |
                self.get_files("dat") |
                self.get_files("langdll")
            )

        else:
            result = set(self.files[filetype])
            for prereq in self.prereqs:
                result |= prereq.get_files(filetype)

            return result


class VersionDetector:
    def __init__(self, basedir):
        self.basedir = basedir

    def get_version(self):
        from hardcoded.version_detection import known_versions
        for version in known_versions:
            if version.exists(self.basedir):
                dbg("Found version '%s'." % version.name, 0)
                return version

        dbg("No valid version found!", 0)
        return None
