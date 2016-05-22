# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""
Runs Cython on all modules that were listed via add_cython_module.
"""

import argparse
import os


def main():
    """ CLI entry point """
    cli = argparse.ArgumentParser()
    cli.add_argument("--build-dir", help=(
        "The root directory where to generate the .cpp/.html files.\n"
        "For our case, this is same as CMAKE_BINARY_DIR."
    ))
    cli.add_argument("--depends", help=(
        "The file with the list of dependencies for this cython module.\n"
        "This file will be generated if\n"
        "(1) It does not exist\n"
        "(2) The set of dependencies have changed"
    ))
    cli.add_argument("--embed", action="store_true", help=(
        "Flag specifying that this is an embedded cython module."
    ))
    cli.add_argument("sources", nargs='+', help=(
        "List of .py/.pyx source files to be cythonized.\n"
        "These source files will be compiled to .cpp by cython."
    ))
    args = cli.parse_args()

    from Cython.Compiler import Options
    Options.annotate = True
    Options.fast_fail = True
    # TODO https://github.com/cython/cython/pull/415
    #      Until then, this has no effect.
    Options.short_cfilenm = '"cpp"'
    if args.embed:
        Options.embed = "main"

    from Cython.Build import cythonize

    options = dict(
        language='c++',
        quiet=True,
        build_dir=args.build_dir,
        include_path=[args.build_dir]
    )
    cythonize(args.sources, **options)

    from Cython.Build.Dependencies import create_dependency_tree

    dep_tree = create_dependency_tree()
    all_dependencies = set()
    for source in args.sources:
        all_dependencies.update(dep_tree.all_dependencies(source))
    new_dependencies = sorted([os.path.abspath(dep) for dep in all_dependencies])

    old_dependencies = []
    first_run = not os.path.exists(args.depends)
    if not first_run:
        with open(args.depends, 'r') as infile:
            old_dependencies = [line.strip() for line in infile.readlines()]

    if first_run or not new_dependencies == old_dependencies:
        # Write the dependencies to file
        with open(args.depends, 'w') as outfile:
            outfile.write("\n".join(new_dependencies))

        # Touch the CMakeLists.txt file, so cmake will rerun and load these dependencies
        os.utime(os.path.abspath("CMakeLists.txt"))

if __name__ == '__main__':
    main()
