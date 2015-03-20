# Copyright 2014-2015 the openage authors. See copying.md for legal info.

"""
Codegen interface to the build system.

See buildsystem/codegen.cmake.

Generates c++ code within the source tree.
Designed to be used by [buildsystem/codegen.cmake].

All file and directory names should be absolute;
otherwise, assumptions made by this script or the cmake script might
not be fulfilled.

Invocation synopsis:

python3 -m openage codegen

    (mandatory)

    --generated-list-file
    --depend-list-file
    --project-dir
    --mode in {clean, dryrun, codegen}

    (optional)

    --touch-file-on-cache-change (CMakeLists.txt)
    --force-rerun-on-generated-list-change (bool)

all file and directory names SHOULD be absolute paths.
this is not enforced, but relative paths may violate assumptions made by
codegen.cmake.

for each invocation, all code generation is performed, and the generated
sourcefiles are stored in an internal dict.

in addition, text data is written to the specified cache files:

- a list of all generated files (targets) to target-cache
- a list of all loaded python module files to depend-cache

depending on the specified invocation commands,

- generated sources are written to the source dir
- generated sources are cleaned
- cmake re-builds are triggered if a cache has changed
"""

import os

from ..util.fslike import Directory

from .codegen import CodegenMode, codegen


def init_subparser(cli):
    """ Codegen-specific CLI. """
    cli.set_defaults(entrypoint=main)

    cli.add_argument(
        "--generated-list-file", required=True,
        help=("filename for target cache. a list of all generated sources "
              "is written there during each invocation. "
              "if the list changes, --touch-file-on-cache-change and "
              "--force-rerun-on-generated-list-change trigger cmake re-runs."))

    cli.add_argument(
        "--depend-list-file", required=True,
        help=("filename for dependency cache. a list of all python files and "
              "other resources that were used during source generation is "
              "stored here. "
              "if the list changes, --touch-file-on-cache-change will trigger "
              "cmake re-runs"))

    cli.add_argument(
        "--project-dir", required=True,
        help=("templates are read from here, and generated code is written "
              "here."))

    cli.add_argument(
        "--mode", required=True,
        help=("operating mode; must be one of {'codegen', 'dryrun', 'clean'}. "
              "in dry run mode, only the caches are updated. "
              "in codegen mode, all source files are created. "
              "in clean mode, the source files are deleted instead."))

    cli.add_argument(
        "--touch-file-on-cache-change", dest="file_to_touch",
        help=("the file passed here is touched if one of the caches changes. "
              "designed for use with a CMakeLists.txt file, to trigger cmake "
              "re-runs."))

    cli.add_argument(
        "--force-rerun-on-generated-list-change",
        action="store_true",
        help=("more drastic than --touch-file-on-cache-change, this causes "
              "codegen to abort with an error message if the target cache has "
              "changed."))


def main(args, error):
    """ Codegen entry point. """

    if args.file_to_touch and not os.path.isfile(args.file_to_touch):
        error("file doesn't exist: " + args.file_to_touch)

    def read_cache_file_lines(filename):
        """
        Yields all lines from the file.
        If the file doesn't exist, a warning is printed.
        """
        try:
            with open(filename) as fileobj:
                for line in fileobj:
                    line = line.strip()
                    if line:
                        yield line
        except FileNotFoundError:
            if args.file_to_touch or args.force_rerun_on_generated_list_change:
                print("warning: cache actions were requested, " +
                      "but the target cache could not be read!")

    old_generated = set(read_cache_file_lines(args.generated_list_file))
    old_depends = set(read_cache_file_lines(args.depend_list_file))

    if not os.path.isdir(args.project_dir):
        error("not a directory: " + args.project_dir)

    try:
        mode = CodegenMode(args.mode)
    except ValueError as exc:
        error(exc.args[0])

    # arguments are OK.

    # generate sources
    generated, depends = codegen(Directory(args.project_dir), mode)

    def print_set_differences(old, new, name):
        """ Prints the difference between old and new. """
        if old - new:
            print("codegen: removed from " + name + ":\n"
                  "\t" + "\n\t".join(old - new) + "\n")

        if new - old and old:
            print("codegen: new " + name + ":\n"
                  "\t" + "\n\t".join(new - old) + "\n")

    print_set_differences(old_generated, generated, "generated files")
    print_set_differences(old_depends, depends, "dependencies")

    with open(args.generated_list_file, 'w') as fileobj:
        fileobj.write('\n'.join(generated))

    with open(args.depend_list_file, 'w') as fileobj:
        fileobj.write('\n'.join(depends))

    if args.file_to_touch:
        if old_generated != generated or old_depends != depends:
            os.utime(args.file_to_touch)

    if args.force_rerun_on_generated_list_change:
        if old_generated != generated:
            print("\n\n\n\n"
                  "The list of generated source files has changed.\n"
                  "A build update has been triggered; you need to build "
                  "again.\n"
                  "\n\n\n")

            # fail
            exit(1)
