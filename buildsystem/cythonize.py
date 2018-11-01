#!/usr/bin/env python3
#
# Copyright 2015-2018 the openage authors. See copying.md for legal info.

"""
Runs Cython on all modules that were listed via add_cython_module.
"""

import argparse
import os
import sys
from contextlib import redirect_stdout
from multiprocessing import cpu_count

from Cython.Build import cythonize


class LineFilter:
    """ Proxy for a stream (default stdout) to filter out whole unwanted lines """
    # pylint: disable=too-few-public-methods
    def __init__(self, stream=None, filters=None):
        self.stream = stream or sys.stdout
        self.filters = filters or []
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
        Writes to output stream, buffered line-wise,
        omitting lines given in to constructor in filter
        """
        self.buf += data
        lines = self.buf.split('\n')
        for line in lines[:-1]:
            if not any(f(line) for f in self.filters):
                self.stream.write(line + '\n')
        self.buf = lines[-1]


class CythonFilter(LineFilter):
    """ Filters output of cythonize for useless warnings """
    # pylint: disable=too-few-public-methods
    def __init__(self):
        filters = [
            lambda x: x == 'Please put "# distutils: language=c++" in your .pyx or .pxd file(s)',
            lambda x: x.startswith('Compiling ') and x.endswith(' because it changed.')
        ]
        super().__init__(filters=filters)


def read_list_from_file(filename):
    """ Reads a semicolon-separated list of file entires """
    with open(filename) as fileobj:
        data = fileobj.read().strip()

    data = [os.path.realpath(os.path.normpath(filename)) for filename in data.split(';')]
    if data == ['']:
        return []

    return data


def remove_if_exists(filename):
    """ Deletes the file (if it exists) """
    if os.path.exists(filename):
        print(os.path.relpath(filename, os.getcwd()))
        os.remove(filename)


def cythonize_cpp_wrapper(modules, nthreads):
    """ Calls cythonize, filtering useless warnings """
    if not modules:
        return

    with CythonFilter() as cython_filter:
        with redirect_stdout(cython_filter):
            kwargs = {
                'nthreads': nthreads,
                'compiler_directives': {'language_level': 3},
            }

            # this is deprecated, but still better than
            # writing funny lines at the head of each file.
            kwargs['language'] = 'c++'

            cythonize(modules, **kwargs)


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
    cli.add_argument("--memcleanup", type=int, default=0, help=(
        "Generate memory cleanup code to make valgrind happy:\n"
        "0: nothing, 1+: interned objects,\n"
        "2+: cdef globals, 3+: types objects"
    ))
    cli.add_argument("--threads", type=int, default=cpu_count(),
                     help="number of compilation threads to use")
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
    Options.annotate = True
    Options.fast_fail = True
    Options.generate_cleanup_code = args.memcleanup
    Options.cplus = 1

    # build cython modules (emits shared libraries)
    cythonize_cpp_wrapper(modules, args.threads)

    # build standalone executables that embed the py interpreter
    Options.embed = "main"
    cythonize_cpp_wrapper(embedded_modules, args.threads)

    # verify depends
    from Cython.Build.Dependencies import _dep_tree

    depend_failed = False
    # TODO figure out a less hacky way of getting the depends out of Cython
    # pylint: disable=no-member, protected-access
    for module, files in _dep_tree.__cimported_files_cache.items():
        for filename in files:
            if not filename.startswith('.'):
                # system include starts with /
                continue

            if os.path.realpath(os.path.abspath(filename)) not in depends:
                print("\x1b[31mERR\x1b[m unlisted dependency: " + filename)
                depend_failed = True

    if depend_failed:
        sys.exit(1)


if __name__ == '__main__':
    main()
