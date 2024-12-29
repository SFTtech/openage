# Copyright 2015-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-statements
"""
Behold: The central entry point for all of openage.

This module mostly does argparsing.
Subparsers are initialized by their respective modules.
"""

import argparse
# TODO remove this once all multiprocessing has been eliminated:
import multiprocessing
import os
import sys

from .log import set_loglevel, verbosity_to_level, ENV_VERBOSITY


def print_version():
    """
    The default version printer, unfortunately, inserts newlines.
    This is the easiest way around.
    """
    from . import LONGVERSION
    print(LONGVERSION)
    from .versions.versions import get_version_numbers
    version_numbers = get_version_numbers()
    for key in version_numbers:
        print(key.decode("utf8") + " " + version_numbers[key].decode("utf8"))
    sys.exit(0)


def add_dll_search_paths(dll_paths: list[str]):
    """
    This function adds DLL search paths.
    """
    from .util.dll import DllDirectoryManager

    manager = DllDirectoryManager(dll_paths)

    return manager


def main(argv=None):
    """ Top-level argparsing; invokes subparser for all submodules. """
    cli = argparse.ArgumentParser(
        "openage",
        description=("free age of empires II engine clone")
    )

    if sys.platform == 'win32':
        from .util.dll import default_paths
        cli.add_argument(
            "--add-dll-search-path", action='append', dest='dll_paths',
            # use path of current openage executable as default
            default=default_paths(),
            help="(Windows only) provide additional DLL search path")

    cli.add_argument("--version", "-V", action='store_true', dest='print_version',
                     help="print version info and exit")

    # shared arguments for all subcommands
    global_cli = argparse.ArgumentParser(add_help=False)
    global_cli.add_argument("--verbose", "-v", action='count',
                            default=ENV_VERBOSITY,
                            help="increase verbosity")
    global_cli.add_argument("--quiet", "-q", action='count', default=0,
                            help="decrease verbosity")
    global_cli.add_argument("--trap-exceptions", action="store_true",
                            help=("upon throwing an exception a debug break is "
                                  "triggered. this will crash openage if no "
                                  "debugger is present"))

    devmodes = global_cli.add_mutually_exclusive_group()
    devmodes.add_argument("--devmode", action="store_true",
                          help="force-enable development mode")
    devmodes.add_argument("--no-devmode", action="store_true",
                          help="force-disable development mode")

    # shared directory arguments for most subcommands
    cfg_cli = argparse.ArgumentParser(add_help=False)

    cfg_cli.add_argument("--asset-dir",
                         help="Use this as an additional asset directory.")
    cfg_cli.add_argument("--cfg-dir",
                         help="Use this as an additional config directory.")

    subparsers = cli.add_subparsers(dest="subcommand")

    # enable reimports for "init_subparser"
    # pylint: disable=reimported

    from .main.main import init_subparser
    main_cli = subparsers.add_parser(
        "main",
        parents=[global_cli, cfg_cli])
    init_subparser(main_cli)

    from .game.main import init_subparser
    init_subparser(subparsers.add_parser(
        "game",
        parents=[global_cli, cfg_cli]))

    from .testing.main import init_subparser
    init_subparser(subparsers.add_parser(
        "test",
        parents=[global_cli, cfg_cli]))

    from .convert.main import init_subparser
    init_subparser(subparsers.add_parser(
        "convert",
        parents=[global_cli]))

    from .convert.tool.singlefile import init_subparser
    init_subparser(subparsers.add_parser(
        "convert-file",
        parents=[global_cli]))

    from .convert.tool.api_export import init_subparser
    init_subparser(subparsers.add_parser(
        "convert-export-api",
        parents=[global_cli]))

    from .codegen.main import init_subparser
    init_subparser(subparsers.add_parser(
        "codegen",
        parents=[global_cli]))

    args, remaining_args = cli.parse_known_args(argv)

    dll_manager = None
    if sys.platform == 'win32':
        dll_manager = add_dll_search_paths(args.dll_paths)
        dll_manager.add_directories()

    if args.print_version:
        print_version()

    if not args.subcommand:
        # the user didn't specify a subcommand. default to 'main'.
        args = main_cli.parse_args(remaining_args)

    args.dll_manager = dll_manager

    # process the shared args
    set_loglevel(verbosity_to_level(args.verbose - args.quiet))

    try:
        from . import config

        if args.no_devmode:
            config.DEVMODE = False
        if args.devmode:
            config.DEVMODE = True
            from . import cython_check
            cython_check.this_is_true()
    except ImportError:
        cli.error("code was not yet generated. "
                  "Did you run the command from the build directory (bin/)?\n"
                  "See doc/building.md for more information.")

    if "asset_dir" in args and args.asset_dir:
        if not os.path.exists(args.asset_dir):
            cli.error("asset directory does not exist: " + args.asset_dir)

    # call the entry point for the subcommand.
    return args.entrypoint(args, cli.error)


if __name__ == '__main__':
    # Required for Windows executables (and apparently macOS too)
    # https://docs.python.org/3/library/multiprocessing.html#multiprocessing.freeze_support
    # https://pyinstaller.org/en/latest/common-issues-and-pitfalls.html#multi-processing
    multiprocessing.freeze_support()

    # openage is complicated and multithreaded; better not use fork.
    multiprocessing.set_start_method('spawn')

    sys.exit(main())
