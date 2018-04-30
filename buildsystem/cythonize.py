#!/usr/bin/env python3
#
# Copyright 2015-2018 the openage authors. See copying.md for legal info.

"""
Runs Cython on all modules that were listed via add_cython_module.
"""

import argparse
import os
import sys


class LineFilter:
    """ Proxy for a stream (default stdout) to filter out whole unwanted lines """
    # pylint: disable=too-few-public-methods
    def __init__(self, stream=None, filtered=None):
        self.stream = stream or sys.stdout
        self.filtered = filtered or []
        self.buf = ""

    def __getattr__(self, attr_name):
        return getattr(self.stream, attr_name)

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.stream.write(self.buf)
        self.buf = ""

    def write(self, data):
        """
        Writes to output stream, buffered linewise,
        omitting lines given in to constructor in filter
        """
        self.buf += data
        lines = self.buf.split('\n')
        for line in lines[:-1]:
            if line not in self.filtered:
                self.stream.write(line + '\n')
        self.buf = lines[-1]


class CythonFilter(LineFilter):
    """ Filters output of cythonize for useless warnings """
    # pylint: disable=too-few-public-methods
    def __init__(self):
        filtered = ['Please put "# distutils: language=c++" in your .pyx or .pxd file(s)']
        super().__init__(filtered=filtered)


def read_list_from_file(filename):
    """ Reads a semicolon-separated list of file entires """
    with open(filename) as fileobj:
        data = fileobj.read().strip()

    data = [os.path.normpath(filename) for filename in data.split(';')]
    if data == ['']:
        return []

    return data


def remove_if_exists(filename):
    """ Deletes the file (if it exists) """
    if os.path.exists(filename):
        print(os.path.relpath(filename, os.getcwd()))
        os.remove(filename)


def cythonize_cpp_wrapper(modules):
    """ Calls cythonize, filtering useless warnings """
    from Cython.Build import cythonize
    from contextlib import redirect_stdout

    if not modules:
        return

    with CythonFilter() as cython_filter:
        with redirect_stdout(cython_filter):
            cythonize(modules, language='c++')


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
        sys.exit(0)

    from Cython.Compiler import Options
    from Cython.Compiler.Main import default_options
    Options.annotate = True
    Options.fast_fail = True
    # TODO https://github.com/cython/cython/pull/415
    #      Until then, this has no effect.
    Options.short_cfilenm = '"cpp"'

    default_options['cache'] = True
    cythonize_cpp_wrapper(modules)

    Options.embed = "main"

    default_options['cache'] = False
    cythonize_cpp_wrapper(embedded_modules)

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
                sys.exit(1)


if __name__ == '__main__':
    main()
