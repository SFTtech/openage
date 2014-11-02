# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import os
import re

from . import util


class HeaderIssue(Exception):
    pass


guard_re = re.compile((
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

no_guard_required_re = re.compile((
    # allow any number of comments or empty lines
    "^(\\n|(#|//).*\\n)*"

    # require comment "has no header guard"
    "(#|//) has no header guard:"
))


def find_issues(dirname, guardprefix):
    """
    checks all headerguards in header files in the cpp folders.
    """
    for fname in util.findfiles((dirname,), ('.h')):
        try:
            expected_guard = fname[len(dirname) + 1:].upper()
            expected_guard = expected_guard.replace('.', '_').replace('/', '_')

            expected_guard = "{}{}_".format(guardprefix, expected_guard)

            data = util.readfile(fname)

            if no_guard_required_re.match(data):
                # this file needs no header guard, for the reason
                # detailed in the comment.
                continue

            m = guard_re.match(data)
            if not m:
                raise HeaderIssue("No valid header guard found")

            ifndef_guard = m.group("ifndef_guard")
            define_guard = m.group("define_guard")

            if ifndef_guard != expected_guard:
                raise HeaderIssue(
                    ("Wrong header guard:\n"
                     "\texpected {}\n"
                     "\tfound    {}").format(
                        repr(expected_guard),
                        repr(ifndef_guard)))

            if define_guard != ifndef_guard:
                raise HeaderIssue(
                    ("Inconsistent header gaurd:\n"
                     "\t#ifndef {}\n"
                     "\t#define {}").format(
                        repr(ifndef_guard),
                        repr(define_guard)))

        except HeaderIssue as e:
            yield util.Issue("header guard issue in {}".format(fname),
                             e.args[0])
