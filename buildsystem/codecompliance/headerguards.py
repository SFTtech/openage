# Copyright 2014-2020 the openage authors. See copying.md for legal info.

"""
Verifies the guard macros of all C++ header files.
"""

import re

from .util import findfiles, readfile


class HeaderIssue(Exception):
    """ Some issue was detected with the Header guard. """


GUARD_RE = re.compile((
    # allow any number of comments or empty lines
    "^(\\n|(#|//).*\\n)*"

    # the header guard
    "#pragma once\n"
))

NO_GUARD_REQUIRED_RE = re.compile((
    # allow any number of comments or empty lines
    "^(\\n|(#|//).*\\n)*"

    # require comment "has no header guard"
    "(#|//) has no header guard:"
))


def find_issues(dirname):
    """
    checks all headerguards in header files in the cpp folders.
    """
    for fname in findfiles((dirname,), ('.h',)):
        try:
            data = readfile(fname)

            if NO_GUARD_REQUIRED_RE.match(data):
                # this file needs no header guard, for the reason
                # detailed in the comment.
                continue

            match = GUARD_RE.match(data)
            if not match:
                raise HeaderIssue("No valid header guard found")

        except HeaderIssue as exc:
            yield (f"header guard issue in {fname}", exc.args[0], None)
