# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Checks the Python modules with pylint.
"""

from pylint import lint

from .util import findfiles
from .pystyle import filter_file_list


def find_pyx_modules(dirnames):
    """ Yields the names of all .pyx modules. """
    for pyx_file in findfiles(dirnames, [".pyx"]):
        yield pyx_file.replace('/', '.')[:-len(".pyx")]


def find_issues(check_files, dirnames):
    """ Invokes the external utility. """

    invocation = ['--rcfile=etc/pylintrc', '--reports=n']

    # pylint crashes with multiprocessing when cython modules are present
    if any(findfiles(dirnames, [".so"])):
        print("Cython modules found, using single process linting.")
    else:
        from multiprocessing import cpu_count
        invocation.append("--jobs={:d}".format(cpu_count()))

    ignored_modules = list(find_pyx_modules(dirnames))
    ignored_modules.append('numpy')

    invocation.append('--ignored-modules=' + ','.join(ignored_modules))

    if check_files is None:
        invocation.extend(dirnames)
    else:
        invocation.extend(filter_file_list(check_files, dirnames))

    try:
        lint.Run(invocation)
    except SystemExit as exc:
        errorcode = exc.args[0]
        if errorcode != 0:
            if check_files is None:
                msg = "python code is noncompliant: %d" % errorcode
            else:
                msg = ("false positives may result from not checking the "
                       "entire codebase")

            yield "linting issue", msg
