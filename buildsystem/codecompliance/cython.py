# Copyright 2021-2021 the openage authors. See copying.md for legal info.

"""
Verifies that Cython directives for profiling are deactivated.
"""

import re

from buildsystem.codecompliance.util import issue_str_line

from .util import readfile, select_files

GLOBAL_PROFILE_DIREC = re.compile(
    (
        # global profiling directive for a file
        r"^(# cython: .*(profile=True|linetrace=True).*\n)"
    )
)

FUNC_PROFILE_DIREC = re.compile(
    (
        # profiling for single functions
        r"@cython\.profile\(True\)"
    )
)


def find_issues(check_files, dirnames):
    """
    Finds all issues in the given directories (filtered by check_files).
    """
    for filename in select_files(check_files, dirnames, (".pyx",)):
        data = readfile(filename)

        for num, line in enumerate(data.splitlines(True), start=1):
            match = GLOBAL_PROFILE_DIREC.match(line)
            if match:
                yield issue_str_line(
                    "cython profiling activated in header",
                    filename,
                    line,
                    num,
                    (match.start(1), match.end(1)),
                )

            match = FUNC_PROFILE_DIREC.search(line)
            if match:
                yield issue_str_line(
                    "cython function profiling activated in file",
                    filename,
                    line,
                    num,
                    (match.start(0), match.end(0)),
                )
