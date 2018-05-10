# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Installs the Python extension modules that were created in the build directory
to the places where they would be expected in the source tree.

Attemts to use OS facilities such as hardlinking, and falls back to copying.
"""

import argparse
import os
import sys


def main():
    """ CLI entry point """
    cli = argparse.ArgumentParser()
    cli.add_argument("module_list_file", help=(
        "semicolon-separated list of all modules that shall be installed "
        "in-place."
    ))
    cli.add_argument("binary_dir", help=(
        "the build directory where those files will be found."
    ))
    cli.add_argument("configuration", help=(
        "the build configuration like Debug or Release"
    ))
    cli.add_argument("--clean", action="store_true", help=(
        "remove instead of creating"
    ))
    args = cli.parse_args()

    with open(args.module_list_file) as fileobj:
        modules = fileobj.read().strip().split(';')
        if modules == ['']:
            modules = []

    if not os.path.isdir(args.binary_dir):
        print("not a directory: " + args.binary_dir)
        sys.exit(1)

    for module in modules:
        sourcefile = module
        targetfile = os.path.relpath(module, args.binary_dir)

        # If `targetfile` has a configuration component, remove it.
        targetfile = os.path.normpath(targetfile.replace(args.configuration, '.'))

        if os.path.exists(targetfile):
            if args.clean:
                print(targetfile)
                os.remove(targetfile)
                continue

            if os.path.getmtime(targetfile) >= os.path.getmtime(sourcefile):
                continue

            os.remove(targetfile)

        if args.clean:
            continue

        # try to hardlink
        try:
            os.link(sourcefile, targetfile)
            continue
        except OSError:
            pass

        # we don't try to symlink, because then Python may "intelligently"
        # determine the actual location, and refuse to work.

        # fallback to copying the file
        with open(sourcefile, 'rb') as infile:
            with open(targetfile, 'wb') as outfile:
                outfile.write(infile.read())


if __name__ == '__main__':
    main()
