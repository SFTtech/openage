# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Checks some general whitespace rules and the encoding for text files.
"""

import re

from .util import find_all_files, readfile, has_ext, issue_str, issue_str_line, BADUTF8FILES


MISSING_SPACES_RE = re.compile((
    # on of the folowing, the first group is used for the column where
    # the pointer is going to show
    "(?:"
    # a ) folowed by a { without a space between
    "(?:\\)(\\{))|"
    # a if/for/while folowed by a ( without a space between
    "(?: (?:if|for|while)(\\())"
    ")"
))

EXTRA_SPACES_RE = re.compile((
    # on of the folowing, the first group is used for the column where
    # the pointer is going to show
    "(?:"
    # a space before a ";"
    "(?:( );)"
    ")"
))


def filter_file_list(check_files, dirnames):
    """
    Yields all those files in check_files that are in one of the directories
    and end in '.cpp' or '.h' and some other conditions.
    """
    for filename in check_files:
        if not (filename.endswith('.cpp') or filename.endswith('.h')):
            continue

        if filename.endswith('.gen.h') or filename.endswith('.gen.cpp'):
            # TODO all this for now, until someone fixes the codegen.
            continue

        if filename.startswith('openage/'  ) and filename.endswith('.cpp'):
            # allow issues for Cython-generated files.
            continue

        if any(filename.startswith(dirname) for dirname in dirnames):
            yield filename



def find_issues(check_files, dirnames):
    """
    Finds all issues in the given directories (filtered by check_files).
    """

    if check_files is not None:
        filenames = filter_file_list(check_files, dirnames)
    else:
        filenames = filter_file_list(find_all_files(dirnames), dirnames)

    for filename in filenames:
        data = readfile(filename)
        analyse_each_line = False

        if (MISSING_SPACES_RE.search(data) or
            EXTRA_SPACES_RE.search(data)):
            analyse_each_line = True

        # if there are possible issues perform a per line analysis
        if analyse_each_line:
            yield from find_issues_with_lines(filename)


def find_issues_with_lines(filename):
    """
    Checks a file for issues per line
    """
    data = readfile(filename)

    for num, line in enumerate(data.splitlines(True), start=1):

        match = MISSING_SPACES_RE.search(line);
        if match:
            yield issue_str_line("Missing space", filename, line, num, match.start(1) + match.start(2))

        match = EXTRA_SPACES_RE.search(line);
        if match:
            yield issue_str_line("Extra space", filename, line, num, match.start(1))
