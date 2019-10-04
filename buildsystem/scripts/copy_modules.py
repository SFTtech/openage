# Copyright 2017-2019 the openage authors. See copying.md for legal info.

"""
Copies the specified modules to the required directory.
Used for packaging the python dependencies.
"""

import argparse
import importlib
import importlib.abc
import importlib.util
import os
import shutil
import sys


def copy_module(name, destination):
    """Copy the importable module 'name' to the 'destination' directory"""
    loader = importlib.util.find_spec(name).loader
    if not isinstance(loader, importlib.abc.FileLoader):
        sys.exit("Loader for module %s is not handled" % (name))

    print('Copying "%s" to "%s"' % (name, destination))
    filename = loader.get_filename(name)
    if loader.is_package(name):
        pkgdir, _ = os.path.split(filename)
        shutil.copytree(pkgdir, os.path.join(destination, name))
    else:
        shutil.copy2(filename, destination)


def main():
    """ CLI entry point """
    cli = argparse.ArgumentParser()
    cli.add_argument("pymodule_name", nargs='+', help=(
        "list of all modules that shall be copied"
    ))
    cli.add_argument("dest_dir", help=(
        "destination directory where modules will be copied"
    ))
    args = cli.parse_args()
    for module in args.pymodule_name:
        copy_module(module, args.dest_dir)


if __name__ == '__main__':
    main()
