# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Checks some general whitespace rules and the encoding for text files.
"""

import re

from .util import findfiles, readfile, has_ext, BADUTF8FILES

TRAIL_WHITESPACE_RE = re.compile((
    # trailing whitespace
    "( |\\t)\\n"
))

CPP_MISSING_SPACES_RE = re.compile((
    # on of the folowing, the first group is used for the column where
    # the pointer is going to show
    "(?:"
    # a ) folowed by a { without a space between
    "(?:\\)(\\{))|"
    # a if/for/while folowed by a ( without a space between
    "(?: (?:if|for|while)(\\())"
    ")"
))

CPP_EXTRA_SPACES_RE = re.compile((
    # on of the folowing, the first group is used for the column where
    # the pointer is going to show
    "(?:"
    # a space before a ";"
    "(?:( );)"
    ")"
))


def find_issues(dirnames, exts):
    """
    checks all files ending in exts in dirnames.
    """
    for filename in findfiles(dirnames, exts):
        data = readfile(filename)
        analyse_each_line = False

        if filename.endswith('.gen.h') or filename.endswith('.gen.cpp'):
            # TODO all this for now, until someone fixes the codegen.
            continue

        if filename.startswith('openage/'  ) and filename.endswith('.cpp'):
            # allow issues for Cython-generated files.
            continue

        if '\r' in data:
            yield "Windows EOL format", filename

        if data.endswith('\n\n'):
            yield "Trailing newline", filename

        if len(data) > 0 and not data.endswith('\n'):
            yield "File does not end in '\\n'", filename

        if has_ext(filename, ('.py', '.pyx', '.pxd')):
            if '\t' in data:
                yield "File contains tabs", filename

        if (TRAIL_WHITESPACE_RE.search(data) or
            CPP_MISSING_SPACES_RE.search(data) or
            CPP_EXTRA_SPACES_RE.search(data)):
            analyse_each_line = True

        # if there are possible issues perform a in deepth analysis
        if analyse_each_line:
            yield from find_issues_with_lines(filename)

    for filename in BADUTF8FILES:
        yield "Not valid UTF-8", filename


def find_issues_with_lines(filename):
    """
    checks a file for issues per line
    """
    data = readfile(filename)

    for num, line in enumerate(data.splitlines(True), start=1):

        match = TRAIL_WHITESPACE_RE.search(line);
        if match:
            yield "Trailing whitespace", issue_with_line_to_str(filename, line, num, match.start(1))

        match = CPP_MISSING_SPACES_RE.search(line);
        if match:
            yield "Missing space", issue_with_line_to_str(filename, line, num, match.start(1) + match.start(2))

        match = CPP_EXTRA_SPACES_RE.search(line);
        if match:
            yield "Extra space", issue_with_line_to_str(filename, line, num, match.start(1))


def issue_with_line_to_str(filename, line, line_number, index):
    return (
        filename + ":" + str(line_number) + "\n" +
        "\tLine: " + line.replace('\t', ' ') +
        "\t      " + (' ' * index) + "\x1b[32;1m^\x1b[m")
