# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import os

from . import util


class HeaderIssue(Exception):
    pass


def find_issues(dirname, guardprefix):
    """
    checks all headerguards in header files in the cpp folders.
    """
    fail = False
    for f in os.listdir(dirname):
        if f.startswith('.'):
            continue

        name = os.path.join(dirname, f)
        guard = "{}{}_".format(guardprefix, f.replace('.', '_').upper())

        if os.path.isdir(name):
            for x in find_issues(name, guard):
                yield x
            continue

        if name.endswith('.h'):
            # yay, we found an actual header file!
            try:
                with open(name) as f:
                    test_headerguard(f, guard)
            except HeaderIssue as e:
                yield util.Issue("header guard issue in {}".format(name),
                                 e.args[0])

    return not fail


def test_headerguard(it, guard):
    expectedifndef = "#ifndef {}".format(guard)
    expecteddefine = "#define {}".format(guard)

    guardstart_found = guardend_found = False

    lines = [s.strip() for s in it]

    # look for header guard at beginning of file
    for index, line in enumerate(lines):
        if not line:
            # allow any number of empty lines
            continue

        if line.startswith('// has no header guard:'):
            # this file needs no header guard.
            # the reason is detailed in the comment.
            return True

        if line.startswith('//'):
            # allow any number of comments
            continue

        if not line.startswith("#ifndef"):
            raise HeaderIssue((
                "in line {}:\n"
                "\texpected header guard\n"
                "\tfound    {}").format(
                    index + 1,
                    repr(line)))

        # we've found the header guard; check it
        if line != expectedifndef:
            raise HeaderIssue((
                "in line {}:\n"
                "\texpected {}\n"
                "\tfound    {}").format(
                    index + 1,
                    repr(expectedifndef),
                    repr(line)))

        # the immediately-following line must
        try:
            nextline = lines[index + 1]
        except IndexError:
            nextline = "EOF"

        if nextline != expecteddefine:
            raise HeaderIssue((
                "in line {}:\n"
                "\texpected {}\n"
                "\tfound    {}").format(
                    index + 2,
                    repr(expecteddefine),
                    repr(nextline)))

        # the header guard start is OK
        break

    # look for header guard end
    for line in reversed(lines):
        if not line:
            continue

        # the last non-empty line _must_ contain #endif
        if line == "#endif":
            # the header guard end is OK
            return True

        raise HeaderIssue((
            "at end of file:\n"
            "\texpected '#endif'\n"
            "\tfound    {}").format(repr(line)))

    # wat
    return False
