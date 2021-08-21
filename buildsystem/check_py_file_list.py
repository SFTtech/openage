# Copyright 2015-2021 the openage authors. See copying.md for legal info.

"""
Tests whether the files listed via add_py_module are consistent with the
.py files in the openage directory.
"""

import argparse
import os
import sys


def main():
    """ CLI entry point """
    cli = argparse.ArgumentParser()
    cli.add_argument('py_file_list', help=(
        "semicolon-separated list of listed .py files"
    ))
    cli.add_argument('py_module_dir', help=(
        "directory containing the python module to check"
    ))
    cli.add_argument('-v', '--verbose', action='store_true', help=(
        "produce verbose output"
    ))
    args = cli.parse_args()

    openage_dir = os.path.realpath(args.py_module_dir)

    listed = set()
    with open(args.py_file_list, encoding='utf8') as fileobj:
        for filename in fileobj.read().strip().split(';'):
            filepath = os.path.realpath(os.path.normpath(filename))
            if filepath.startswith(openage_dir):
                listed.add(filepath)
            elif args.verbose:
                print("Ignoring " + filepath + " outside " + openage_dir)

    if args.verbose:
        print("Files listed in " + args.py_file_list + ":",
              *sorted(listed), sep='\n\t')

    actual = set()
    for dirname, _, files in os.walk(openage_dir):
        dirname = os.path.realpath(os.path.abspath(dirname))
        for filename in files:
            if filename.endswith('.py'):
                actual.add(os.path.join(dirname, filename))

    if args.verbose:
        print("Files available:", *sorted(actual), sep='\n\t')

    success = True
    for filename in sorted(actual - listed):
        success = False
        print("file was not listed via add_py_module: " +
              os.path.relpath(filename, openage_dir))

    for filename in sorted(listed - actual):
        success = False
        print("file was listed via add_py_module but does not exist: " +
              os.path.relpath(filename, openage_dir))

    if success:
        return 0

    return 1


if __name__ == '__main__':
    sys.exit(main())
