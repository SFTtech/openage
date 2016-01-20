# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""
Tests whether the files listed via add_py_module are consistent with the
.py files in the openage directory.
"""

import argparse
import os


def main():
    """ CLI entry point """
    cwd = os.getcwd()

    cli = argparse.ArgumentParser()
    cli.add_argument('py_file_list', help=(
        "semicolon-separated list of listed .py files"
    ))
    args = cli.parse_args()

    with open(args.py_file_list) as fileobj:
        listed = set(fileobj.read().strip().split(';'))
        if listed == {''}:
            listed = set()

    actual = set()
    for dirname, _, files in os.walk('openage'):
        dirname = os.path.abspath(dirname)
        for filename in files:
            if filename.endswith('.py'):
                actual.add(os.path.join(dirname, filename))

    success = True
    for filename in sorted(actual - listed):
        success = False
        print("file was not listed via add_py_module: " +
              os.path.relpath(filename, cwd))

    for filename in sorted(listed - actual):
        success = False
        print("file was listed via add_py_module but does not exist: " +
              os.path.relpath(filename, cwd))

    if success:
        return 0
    else:
        return 1


if __name__ == '__main__':
    exit(main())
