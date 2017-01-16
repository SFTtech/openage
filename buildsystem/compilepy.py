# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Compiles python modules with cpython to pyc/pyo files.
"""

import argparse
import importlib.util
import os
import py_compile


def main():
    """ CLI entry point """
    cli = argparse.ArgumentParser()
    cli.add_argument("pymodule_list_file", help=(
        "semicolon-separated list of all modules that shall be compiled"
    ))
    cli.add_argument("input_dir", help=(
        "base directory where files from the above list are in."
    ))
    cli.add_argument("output_dir", help=(
        "base directory where output files will be created."
    ))
    args = cli.parse_args()

    with open(args.pymodule_list_file) as fileobj:
        modules = fileobj.read().strip().split(';')
        if modules == ['']:
            modules = []

    if not os.path.isdir(args.output_dir):
        cli.error("not a directory: '%s'" % args.output_dir)

    to_compile = []

    for module in modules:
        sourcefile = module
        relsource = os.path.relpath(sourcefile, args.input_dir)
        reloutput = importlib.util.cache_from_source(relsource)
        outputfile = os.path.join(args.output_dir, reloutput)

        if os.path.exists(outputfile):
            if os.path.getmtime(outputfile) >= os.path.getmtime(sourcefile):
                continue

            os.remove(outputfile)

        to_compile.append((module, outputfile))

    maxwidth = len(str(len(to_compile)))
    for idx, (module, outputfile) in enumerate(to_compile):
        try:
            print("[%*d/%d] Compiling %s to %s" % (maxwidth, idx + 1,
                                                   len(to_compile), module,
                                                   outputfile))
            py_compile.compile(module, cfile=outputfile, doraise=True)

        except py_compile.PyCompileError as exc:
            print("FAILED to compile '%s':" % exc.file)
            print(exc.msg)
            exit(1)


if __name__ == '__main__':
    main()
