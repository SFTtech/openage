from ..convert import datafile
from ..convert.util import set_verbosity, dbg
import argparse
import sys
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
    - cmake re-builds are triggered if a cache has changed
    """

    # parse arguments

    ap = argparse.ArgumentParser()

    ap.add_argument("--target-cache", required=True)
    ap.add_argument("--depend-cache", required=True)
    ap.add_argument("--cpp-src-dir", required=True)

    ap.add_argument("--write-to-sourcedir", action='store_true')

    ap.add_argument("--touch-file-on-cache-change")
    ap.add_argument("--force-rerun-on-targetcache-change")

    ap.add_argument("--verbose", "-v", action='count', default=0)

    args = ap.parse_arguments()

    # process and validate arguments

    set_verbosity(args.verbose)

    if args.touch_file_on_change:
        file_to_touch = args.touch_file_on_change
        if not os.path.isfile(file_to_touch):
            ap.error("file doesn't exist: %s" % file_to_touch)
    else:
        file_to_touch_on_cache_change = None

    cache_actions_requested = any(
        file_to_touch_on_cache_change,
        args.force_rerun_on_targetcache_change,
    )

    old_target_cache = set()
    try:
        with open(args.target_cache) as f:
            for target in f:
                old_target_cache.add(target)
    except:
        if cache_actions_requested:
            dbg("warning: cache actions were requested, " +
                "but the target cache could not be read!", 0)

    old_depend_cache = set()
    try:
        with open(args.depend_cache) as f:
            for depend in f:
                old_depend_cache.add(depend)
    except:
        if cache_actions_requested:
            dbg("warning: cache actions were requested, " +
                "but the depends cache could not be read!", 0)

    cpp_src_dir = args.cpp_src_dir
    if not os.path.isdir(cpp_src_dir):
        ap.error("not a directory: %s" % cpp_src_dir)

    # arguments are OK.

    # generate sources to internal dict

    generated_files_raw = {}
    from ..convert import datafile
    for filename, content in datafile.generate_gamedata_structs(cpp_src_dir):
        generated_files_raw[absfilename] = filename, content

    # post-process filenames
    generated_files = {}
    for filename, content in generated_files_raw:
        basename = filename.split('/')[-1]

        try:
            basename, marker, suffix = basename.split('.')
            if not all(basename, marker, suffix) or marker != 'gen':
                raise ValueError()
        except ValueError:
            dbg("error in codegen: required filename format is " +
                "[base].gen.[suffix], but filename is %s" % filename, 0)
            exit(1)

        absfilename = "%s%s%s" % (cpp_src_dir, os.path.sep, filename)
        generated_files[absfilename] = filename, content

    # check filenames for generated sources

    # calculate dependencies (all used python modules)
    new_depend_cache = set()
    depend_cache_file = open(args.depend_cache, 'w')
    for m in sys.modules.values():
        filename = getattr(m, '__file__', None)
        if not filename:
            continue

        filename = os.path.abspath(filename)

        depend_cache_file.write(filename)
        new_depend_cache.add(filename)

    # calculate targets
    new_target_cache = set()
    target_cache_file = open(args.target_cache, 'w')
    for filename in generated_files:
        target_cache_file.write(filename)
        new_target_cache.add(filename)

    # check whether the cache has changed
    if (old_depend_cache != new_depend_cache or
        old_target_cache != new_target_cache):

        if file_to_touch_on_cache_change:
            try:
                os.utime(file_to_touch_on_cache_change)
            except:
                dbg("warning: couldn't update the timestamp for %s"
                    % file_to_touch_on_cache_change, 0)

    if old_target_cache != new_target_cache:
        if args.force_rerun_on_targetcache_change:
            print("""\n\n\n\
The list of generated sourcefiles has changed.
A build update has been triggered; you need to re-start the build.
\n\n\n""")
            # fail
            exit(1)

    # write generated files to sourcedir
    if args.write_to_sourcedir:
        for absfilename, (filename, content) in generated_files.items():
            # post-process content
            extension = filename.split('.')[-1].lower().strip()
            if extension in {'h', 'hpp', 'c', 'cpp'}:
                comment_prefix = '//'
            else:
                comment_prefix = '#'

            content = """\
{prefix} Warning: this file was auto-generated; manual changes are futile.
{prefix} For details, see buildsystem/codegen.cmake and py/openage/codegen.

{code}""".format(prefix=comment_prefix, code=content)

            with open(absfilename) as f:
                if f.read() == content:
                    dbg("file unchanged: %s" % filename, 1)
                    continue

            dbg("generating file: %s" % filename, 0)
            with open(absfilename, 'w') as f:
                f.write(content)

if __name__ == '__main__':
    main()
