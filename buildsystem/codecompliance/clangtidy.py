# Copyright 2015-2018 the openage authors. See copying.md for legal info.

import subprocess

from .cppstyle import filter_file_list
from .util import findfiles


def find_issues(check_files, dirnames):
    """ Invokes the external clang-tidy tool. """

    invocation = ['clang-tidy', '-checks=-*,readability-*']

    if check_files is not None:
        filenames = filter_file_list(check_files, dirnames)
    else:
        filenames = filter_file_list(findfiles(dirnames), dirnames)

    invocation.extend(filenames)

    try:
        retcode = subprocess.check_call(invocation)
    except subprocess.CalledProcessError as exc:
        retcode = exc.returncode

    if retcode:
        yield ("clang-tidy issue", f"clang-tidy exited with return code {retcode}", None)
