# Copyright 2015-2023 the openage authors. See copying.md for legal info.

"""
Checks the Python modules with pylint.
"""

from pylint import lint

from .pystyle import filter_file_list
from .util import findfiles


def find_pyx_modules(dirnames):
    """ Yields the names of all .pyx modules. """
    for pyx_file in findfiles(dirnames, [".pyx"]):
        yield pyx_file.replace('/', '.')[:-len(".pyx")]


def find_issues(check_files, dirnames):
    """ Invokes the external utility. """

    invocation = ['--rcfile=etc/pylintrc', '--reports=n']

    from multiprocessing import cpu_count
    invocation.append(f"--jobs={cpu_count():d}")

    if check_files is None:
        invocation.extend(dirnames)
    else:
        check_files = list(filter_file_list(check_files, dirnames))
        if not check_files:
            return
        invocation.extend(check_files)

    try:
        lint.Run(invocation)
    except SystemExit as exc:
        error_count = exc.args[0]
        if error_count != 0:
            if check_files is None:
                msg = f"python code is noncompliant: {error_count:d}"
            else:
                msg = ("false positives may result from not checking the "
                       "entire codebase")

            yield "linting issue", msg, None
