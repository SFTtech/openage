# Copyright 2021-2021 the openage authors. See copying.md for legal info.

"""
Verifies that Cython directives for profiling are deactivated.
"""

import re

from buildsystem.codecompliance.util import issue_str_line

from .util import findfiles, readfile


GLOBAL_PROFILE_DIREC = re.compile((
    # global profiling directive for a file
    r"^(# cython: .*(profile=True|linetrace=True).*\n)"
))

FUNC_PROFILE_DIREC = re.compile((
    # profiling for single functions
    r"@cython\.profile\(True\)"
))


def filter_file_list(check_files, dirnames):
    """
    Yields all those files in check_files that are in one of the directories
    and end in '.py'x.
    """
    for filename in check_files:
        if not filename.endswith('.pyx'):
            continue

        if any(filename.startswith(dirname) for dirname in dirnames):
            yield filename


def find_issues(check_files, dirnames):
    """
    Finds all issues in the given directories (filtered by check_files).
    """
    if check_files:
        filenames = filter_file_list(check_files, dirnames)

    else:
        filenames = findfiles(dirnames, ('.pyx',))

    for filename in filenames:
        data = readfile(filename)

        for num, line in enumerate(data.splitlines(True), start=1):
            match = GLOBAL_PROFILE_DIREC.match(line)
            if match:
                yield issue_str_line("cython profiling activated in header",
                                     filename, line, num,
                                     (match.start(1), match.end(1)))

            match = FUNC_PROFILE_DIREC.search(line)
            if match:
                yield issue_str_line("cython function profiling activated in file",
                                     filename, line, num,
                                     (match.start(0), match.end(0)))
