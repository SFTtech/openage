# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import openage.log as log
import argparse
import os


def main():
    """
    this codegen script auto-generates sourcefiles in the cpp/ subtree,
    and is designed for usage by the build system
    (see buildsystem/codegen.cmake).

    invocation synopsis:

    python3 -m openage.codegen

        (mandatory)

        --target-cache=filea
        --depend-cache=fileb
        --cpp-src-dir=dira

        (commands, optional)

        --write-to-sourcedir
        --touch-file-on-cache-change=CMakeLists.txt
        --force-rerun-on-targetcache-change
        --clean

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

    # parse arguments

    ap = argparse.ArgumentParser(
        description=("generates c++ code within the source tree. "
                     "designed to be used by [buildsystem/codegen.cmake]"),
        epilog=("all file and directory names should be absolute; otherwise, "
                "assumptions made by this script or the cmake script might "
                "not be fulfilled"))

    ap.add_argument("--target-cache", required=True,
                    help=("filename for target cache. a list of all generated "
                          "sources is written there for every invocation. if "
                          "the list changes, --touch-file-on-cache-change and "
                          "--force-rerun-on-targetcache-change trigger cmake "
                          "re-runs"))
    ap.add_argument("--depend-cache", required=True,
                    help=("filename for dependency cache. a list of all "
                          "python files and other resources that were used "
                          "during source generation. if the list changes, "
                          "--touch-file-on-cache-change will trigger cmake "
                          "re-runs"))
    ap.add_argument("--cpp-src-dir", required=True,
                    help=("c++ source directory; used to determine generated "
                          "file names"))
    ap.add_argument("--write-to-sourcedir", action='store_true',
                    help=("causes the sources to be actually written to "
                          "cpp-src-dir. otherwise, a dry run is performed. "
                          "even during a dry run, all code generation is "
                          "performed in order to determine generation targets "
                          "and dependencies."))
    ap.add_argument("--clean", action='store_true',
                    help=("all generated files are deleted from the source "
                          "directory"))
    ap.add_argument("--touch-file-on-cache-change",
                    help=("the file passed here will be touched if one of the"
                          "caches changes. designed for use with a "
                          "CMakeLists.txt file, to trigger cmake re-runs"))
    ap.add_argument("--force-rerun-on-targetcache-change", action='store_true',
                    help=("a bit more drastic than --touch-file-on-change, "
                          "this causes codegen to abort with an error message "
                          "if the target cache has changed."))

    ap.add_argument("--verbose", "-v", action='count', default=0)

    args = ap.parse_args()

    # process and validate arguments
    if not args.verbose and 'VERBOSE' in os.environ:
        try:
            args.verbose = int(os.environ['VERBOSE'])
        except:
            args.verbose = 2

    log.set_verbosity(args.verbose)

    file_to_touch = args.touch_file_on_cache_change
    if file_to_touch and not os.path.isfile(file_to_touch):
            ap.error("file doesn't exist: %s" % file_to_touch)

    cache_actions_requested = (file_to_touch or
                               args.force_rerun_on_targetcache_change)

    old_target_cache = set()
    try:
        with open(args.target_cache) as f:
            for target in f:
                old_target_cache.add(target.strip())
    except:
        if cache_actions_requested:
            log.dbg("warning: cache actions were requested, " +
                    "but the target cache could not be read!", 0)

    old_depend_cache = set()
    try:
        with open(args.depend_cache) as f:
            for depend in f:
                old_depend_cache.add(depend.strip())
    except:
        if cache_actions_requested:
            log.dbg("warning: cache actions were requested, " +
                    "but the depends cache could not be read!", 0)

    cpp_src_dir = args.cpp_src_dir
    if not os.path.isdir(cpp_src_dir):
        ap.error("not a directory: %s" % cpp_src_dir)

    # arguments are OK.

    # generate sources
    generated_files = {}
    from . import codegen
    for absfilename, filename, content in codegen.generate_all(cpp_src_dir):
        generated_files[absfilename] = filename, content

    # calculate dependencies (all used python modules)
    new_depend_cache = set()
    depend_cache_file = open(args.depend_cache, 'w')
    for depend in codegen.get_depends():
        depend_cache_file.write("%s\n" % depend)
        new_depend_cache.add(depend)

    # calculate targets
    new_target_cache = set()
    target_cache_file = open(args.target_cache, 'w')
    for filename in generated_files:
        target_cache_file.write(filename)
        target_cache_file.write('\n')
        new_target_cache.add(filename)

    # check whether the cache has changed
    def print_set_difference(fun, old, new):
        if old:
            if old - new:
                fun("removed:\n\t%s" % "\n\t".join(old - new))
            if new - old:
                fun("added:\n\t%s" % "\n\t".join(new - old))
        else:
            fun("\n\t".join(new))

    depend_cache_changed = False
    if old_depend_cache != new_depend_cache:
        depend_cache_changed = True
        log.dbg("codegen dependencies:", 1)
        print_set_difference(lambda s: log.dbg(s, 1),
                             old_depend_cache, new_depend_cache)

    target_cache_changed = False
    if old_target_cache != new_target_cache:
        target_cache_changed = True
        log.dbg("codegen target sources:", 1)
        print_set_difference(lambda s: log.dbg(s, 1),
                             old_target_cache, new_target_cache)

    if file_to_touch and (depend_cache_changed or target_cache_changed):
        try:
            os.utime(file_to_touch)
        except:
            log.dbg("warning: couldn't update the timestamp for %s"
                    % file_to_touch, 0)

    if target_cache_changed and args.force_rerun_on_targetcache_change:
        print("""\n\n\n\
The list of generated sourcefiles has changed.
A build update has been triggered; you need to build again.
\n\n\n""")
        # fail
        exit(1)

    # clean sourcedir
    if args.clean:
        for absfilename, (filename, content) in generated_files.items():
            if os.path.isfile(absfilename):
                log.dbg("cleaning file: %s" % filename, 0)
                os.unlink(absfilename)

    # write generated files to sourcedir
    if args.write_to_sourcedir:
        for absfilename, (filename, content) in generated_files.items():
            if os.path.isfile(absfilename):
                with open(absfilename) as f:
                    if f.read() == content:
                        log.dbg("file unchanged: %s" % filename, 1)
                        continue

            log.dbg("generating file: %s" % filename, 0)
            with open(absfilename, 'w') as f:
                f.write(content)

if __name__ == '__main__':
    main()
