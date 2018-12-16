# Copyright 2015-2018 the openage authors. See copying.md for legal info.

"""
Installs the Python extension modules that were created in the configuration
specific subdirectory of the build directory to the places where they would
be expected.

Attemts to use OS facilities such as hardlinking, and falls back to copying.
"""

import argparse
import os
from pathlib import Path
import shutil


def main():
    """ CLI entry point """
    cli = argparse.ArgumentParser()
    cli.add_argument("module_list_file", help=(
        "semicolon-separated list of all modules that shall be installed "
        "in-place."
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

    for module in modules:
        sourcefile = Path(module)

        if args.configuration in sourcefile.parts:
            # If `sourcefile` has a configuration component, remove it.
            file_parts = list(sourcefile.parts)
            file_parts.remove(args.configuration)
            targetfile = Path(*file_parts)
        else:
            continue

        if targetfile.exists():
            if args.clean:
                print(targetfile)
                targetfile.unlink()
                continue

            if targetfile.stat().st_mtime >= sourcefile.stat().st_mtime:
                continue

            targetfile.unlink()

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
        shutil.copy(sourcefile, targetfile)


if __name__ == '__main__':
    main()
