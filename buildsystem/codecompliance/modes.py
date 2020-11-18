# Copyright 2016-2018 the openage authors. See copying.md for legal info.

"""
Checks the mode of all files and prevents executable source files.
"""

import re
import pathlib
import stat

from .util import findfiles, SHEBANG


SHEBANG_RE = re.compile("^" + SHEBANG)

EXTENSIONS_NO_X_BIT = {
    '.h', '.cpp', '.py', '.pyx', '.pxi', '.cmake', '.h.in',
    '.cpp.in', '.py.in', '.h.template', '.cpp.template',
    '.py.template', '.qml'
}

EXTENSIONS_SHEBANG_XBIT = {
    '.sh', '.py'
}


def check_mode(filename):
    """
    Test if the the file has no executable bit set.
    """

    path = pathlib.Path(filename)
    filemode = path.stat().st_mode

    x_ok = False

    if filemode & (stat.S_IXGRP | stat.S_IXOTH | stat.S_IXUSR):

        if path.suffix in EXTENSIONS_SHEBANG_XBIT:
            # if the file is allowed to have a shebang,
            # allow its executable bit if it actually has a shebang
            with path.open() as file:
                firstline = file.readline()

                if SHEBANG_RE.match(firstline):
                    x_ok = True

        if not x_ok:
            raise ValueError(f'file {filename} is executable')


def find_issues(check_files, paths):
    """
    Check all source files for their required filesystem bits.
    """

    for filename in findfiles(paths, EXTENSIONS_NO_X_BIT):
        if check_files and filename not in check_files:
            continue

        try:
            check_mode(filename)
        except ValueError as exc:
            yield ("wrong file access bits", str(exc), None)
            continue
