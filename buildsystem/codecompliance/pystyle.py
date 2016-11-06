# Copyright 2014-2016 the openage authors. See copying.md for legal info.

"""
Checks PEP8 compliance, with some exceptions.
"""

try:
    from pep8 import StyleGuide
except ImportError:
    from pycodestyle import StyleGuide


# these errors will be ignored by pep8
IGNORE_ERRORS = (
    "E221",  # multiple spaces before operator
    "E241",  # multiple spaces after ','
    "E251",  # unexpected spaces around keyword / parameter equals
    "E501",  # line too long
)


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
    checker.options.ignore = IGNORE_ERRORS

    filenames = dirnames
    if check_files is not None:
        filenames = filter_file_list(check_files, dirnames)

    report = checker.check_files(filenames)

    if len(report.messages) > 0:
        yield ("style issue", "python code violates pep8")
