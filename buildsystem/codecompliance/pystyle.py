# Copyright 2014-2015 the openage authors. See copying.md for legal info.

"""
Checks PEP8 compliance, with some exceptions.
"""

from pep8 import StyleGuide

# these errors will be ignored by pep8
IGNORE_ERRORS = "E251,E221,E501,E241"


def filter_file_list(check_files, dirnames):
    """
    Yields all those files in check_files that are in one of the directories
    and end in '.py'.
    """
    for filename in check_files:
        if not filename.endswith('.py'):
            continue

        if any(filename.startswith(dirname) for dirname in dirnames):
            yield filename


def find_issues(check_files, dirnames):
    """
    Finds all issues in the given directories (filtered by check_files).
    """
    checker = StyleGuide()
    checker.options.ignore = tuple(IGNORE_ERRORS.split(","))

    filenames = dirnames
    if check_files is not None:
        filenames = filter_file_list(check_files, dirnames)

    report = checker.check_files(filenames)

    if len(report.messages) > 0:
        yield ("style issue", "python code violates pep8")
