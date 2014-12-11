# Copyright 2014-2014 the openage authors. See copying.md for legal info.


# these errors will be ignored by pep8
IGNORE_ERRORS = "E251,E221,E501,E241"


def find_issues(dirnames):
    from . import util
    import pep8

    pep8style = pep8.StyleGuide()
    pep8style.options.ignore = tuple(IGNORE_ERRORS.split(","))

    report = pep8style.check_files(dirnames)

    if len(report.messages) > 0:
        yield util.Issue("style issue", "python code violates pep8")
