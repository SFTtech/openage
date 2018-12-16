# Copyright 2015-2018 the openage authors. See copying.md for legal info.

"""
Compiles python modules with cpython to pyc/pyo files.
"""

import argparse
import importlib.util
import os
import py_compile
import shutil
import sys


def clone_file_to_dir(sourcefile, input_dir, output_dir):
    """
    Make a copy of sourcefile from input_dir to output_dir
    Try hardlinking first; on failure fallback to copy.
    Caveat: source files already in output_dir are not cloned.

    :return: the path of file created in output_dir
    """
    if os.path.samefile(input_dir, output_dir) or sourcefile.startswith(output_dir):
        return sourcefile

    relsrcpath = os.path.relpath(sourcefile, input_dir)
    targetfile = os.path.join(output_dir, relsrcpath)

    if os.path.exists(targetfile) and os.path.samefile(sourcefile, targetfile):
        return targetfile

    os.makedirs(os.path.dirname(targetfile), exist_ok=True)

    # try to hardlink
    try:
        os.link(sourcefile, targetfile)
        return targetfile
    except OSError:
        pass

    # we don't try to symlink, because then Python may "intelligently"
    # determine the actual location, and refuse to work.

    # fallback to copying the file
    shutil.copy(sourcefile, targetfile)

    return targetfile


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
    cli.add_argument("--print-output-paths-only", action="store_true", help=(
        "print the paths of the compiled output files and exit"
    ))
    args = cli.parse_args()

    with open(args.pymodule_list_file) as fileobj:
        modules = fileobj.read().strip().split(';')
        if modules == ['']:
            modules = []

    if not os.path.isdir(args.output_dir):
        cli.error("not a directory: '%s'" % args.output_dir)

    all_output_files = []
    to_compile = []

    for module in modules:
        # the module is required in the output directory
        sourcefile = clone_file_to_dir(module, args.input_dir, args.output_dir)
        outputfile = importlib.util.cache_from_source(sourcefile)
        all_output_files.append(outputfile)

        if os.path.exists(outputfile):
            if os.path.getmtime(outputfile) >= os.path.getmtime(sourcefile):
                continue

            os.remove(outputfile)

        to_compile.append((sourcefile, outputfile))

    if args.print_output_paths_only:
        print(';'.join(all_output_files))
        exit(0)

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
            sys.exit(1)


if __name__ == '__main__':
    main()
