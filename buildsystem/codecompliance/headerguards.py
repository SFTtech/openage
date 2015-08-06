# Copyright 2014-2015 the openage authors. See copying.md for legal info.

"""
Verifies the guard macros of all C++ header files.
"""

import re

from .util import findfiles, readfile


class HeaderIssue(Exception):
    """ Some issue was detected with the Header guard. """
    pass


GUARD_RE = re.compile((
    # allow any number of comments or empty lines
    "^(\\n|(#|//).*\\n)*"

    # the header guard
    "#ifndef (?P<ifndef_guard>.*)\n"
    "#define (?P<define_guard>.*)\n"

    # any number of lines of code
    "(.*\\n)*"

    # the #endif part of the header guard
    "#endif"

    # any number of empty lines
    "(\\n)*$"
))

NO_GUARD_REQUIRED_RE = re.compile((
    # allow any number of comments or empty lines
    "^(\\n|(#|//).*\\n)*"

    # require comment "has no header guard"
    "(#|//) has no header guard:"
))


def find_issues(dirname, guardprefix):
    """
    checks all headerguards in header files in the cpp folders.
    """
    for fname in findfiles((dirname,), ('.h',)):
        try:
            expected_guard = fname[len(dirname) + 1:].upper()
            expected_guard = expected_guard.replace('.', '_').replace('/', '_')

            expected_guard = "{}{}_".format(guardprefix, expected_guard)

            data = readfile(fname)

            if NO_GUARD_REQUIRED_RE.match(data):
                # this file needs no header guard, for the reason
                # detailed in the comment.
                continue

            match = GUARD_RE.match(data)
            if not match:
                raise HeaderIssue("No valid header guard found")

            ifndef_guard = match.group("ifndef_guard")
            define_guard = match.group("define_guard")

            if ifndef_guard != expected_guard:
                raise HeaderIssue((
                    "Wrong header guard:\n"
                    "\texpected {}\n"
                    "\tfound    {}").format(
                        repr(expected_guard),
                        repr(ifndef_guard)))

            if define_guard != ifndef_guard:
                raise HeaderIssue((
                    "Inconsistent header gaurd:\n"
                    "\t#ifndef {}\n"
                    "\t#define {}").format(
                        repr(ifndef_guard),
                        repr(define_guard)))

        except HeaderIssue as exc:
            yield ("header guard issue in {}".format(fname), exc.args[0])
