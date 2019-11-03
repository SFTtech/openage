# Copyright 2015-2019 the openage authors. See copying.md for legal info.

"""
Checks some general whitespace rules and the encoding for text files.
"""

import re

from .util import findfiles, readfile, has_ext, issue_str_line, BADUTF8FILES

TRAIL_WHITESPACE_RE = re.compile((
    # trailing whitespace
    r"( |\t)+\n"
))

IMMEDIATE_TODO_RE = re.compile(
    "as" + "df",
    re.IGNORECASE  # pylint: disable=no-member
)


def find_issues(dirnames, exts):
    """
    Checks all files ending in exts in dirnames.
    """
    for filename in findfiles(dirnames, exts):
        data = readfile(filename)
        analyse_each_line = False

        if filename.endswith('.gen.h') or filename.endswith('.gen.cpp'):
            # TODO all this for now, until someone fixes the codegen.
            continue

        if filename.startswith('openage/') and filename.endswith('.cpp'):
            # allow issues for Cython-generated files.
            continue

        if '\r\n' in data:
            yield "Windows EOL format", filename, None

        if data.endswith('\n\n'):
            yield "Trailing newline at file end", filename, None

        if data and not data.endswith('\n'):
            yield "File does not end in '\\n'", filename, None

        if has_ext(filename, ('.py', '.pyx', '.pxd')):
            if '\t' in data:
                yield "File contains tabs", filename, None

        if TRAIL_WHITESPACE_RE.search(data) or IMMEDIATE_TODO_RE.search(data):
            analyse_each_line = True

        # If there are possible issues, perform an in-depth analysis.
        if analyse_each_line:
            yield from find_issues_with_lines(filename)

    for filename in BADUTF8FILES:
        yield "Not valid UTF-8", filename


def find_issues_with_lines(filename):
    """
    Checks a file for issues per line.
    """
    data = readfile(filename)

    for num, line in enumerate(data.splitlines(True), start=1):

        match = TRAIL_WHITESPACE_RE.search(line)
        if match:
            yield issue_str_line("Trailing whitespace", filename, line, num,
                                 (match.start(1), match.end(1)))

        match = IMMEDIATE_TODO_RE.search(line)
        if match:
            yield issue_str_line("Found 'as"
                                 "df', indicating an immediate TODO",
                                 filename, line, num,
                                 (match.start(), match.end()))
