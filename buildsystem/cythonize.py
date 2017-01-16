#!/usr/bin/env python3
#
# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Runs Cython on all modules that were listed via add_cython_module.
"""

import argparse
import os


def read_list_from_file(filename):
    """ Reads a semicolon-separated list of file entires """
    with open(filename) as fileobj:
        data = fileobj.read().strip()

    data = data.split(';')
    if data == ['']:
        return []

    return data


def remove_if_exists(filename):
    """ Deletes the file (if it exists) """
    if os.path.exists(filename):
        print(os.path.relpath(filename, os.getcwd()))
        os.remove(filename)


def main():
    """ CLI entry point """
    cli = argparse.ArgumentParser()
    cli.add_argument("module_list", help=(
        "Module list file (semicolon-separated)."
    ))
    cli.add_argument("embedded_module_list", help=(
        "Embedded module list file (semicolon-separated).\n"
        "Modules in this list are compiled with the --embed option."
    ))
    cli.add_argument("depends_list", help=(
        "Dependency list file (semicolon-separated).\n"
        "Contains all .pxd and other files that may get included.\n"
        "Used to verify that all dependencies are properly listed "
        "in the CMake build configuration."
    ))
    cli.add_argument("--clean", action="store_true", help=(
        "Clean compilation results and exit."
    ))
    args = cli.parse_args()

    modules = read_list_from_file(args.module_list)
    embedded_modules = read_list_from_file(args.embedded_module_list)
    depends = set(read_list_from_file(args.depends_list))

    if args.clean:
        for module in modules + embedded_modules:
            module = os.path.splitext(module)[0]
            remove_if_exists(module + '.cpp')
            remove_if_exists(module + '.html')
        exit(0)

    from Cython.Compiler import Options
    Options.annotate = True
    Options.fast_fail = True
    # TODO https://github.com/cython/cython/pull/415
    #      Until then, this has no effect.
    Options.short_cfilenm = '"cpp"'

    from Cython.Build import cythonize

    if modules:
        cythonize(modules, language='c++')

    Options.embed = "main"

    if embedded_modules:
        cythonize(embedded_modules, language='c++')

    # verify depends
    from Cython.Build.Dependencies import _dep_tree
    # TODO figure out a less hacky way of getting the depends out of Cython
    # pylint: disable=no-member, protected-access
    for module, files in _dep_tree.__cimported_files_cache.items():
        for filename in files:
            if not filename.startswith('.'):
                # system include
                continue

            if os.path.abspath(filename) not in depends:
                print("\x1b[31mERR\x1b[m unlisted dependency: " + filename)
                exit(1)


if __name__ == '__main__':
    main()
