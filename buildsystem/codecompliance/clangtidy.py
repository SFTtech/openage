# Copyright 2015-2019 the openage authors. See copying.md for legal info.

"""
Checks Cpp files with clang-tidy
"""

import subprocess

from .cppstyle import filter_file_list
from .util import findfiles


def find_issues(check_files, dirnames):
    """ Invokes the external clang-tidy tool. """
    invocation = ['clang-tidy', '-checks=clang-diagnostic-*,clang-analyzer-*,-clang-analyzer-alpha*,' +
                  '-clang-analyzer-optin.performance.Padding,cppcoreguidelines-*,performance-*,llvm-*,' +
                  'bugprone-*,cert-*, -extra-arg=-std=c++17 -fix']

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
