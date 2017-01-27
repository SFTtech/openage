# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Behold: The central entry point for all of openage.

This module mostly does argparsing.
Subparsers are initialized by their respective modules.
"""

import argparse
# TODO remove this once all multiprocessing has been eliminated:
import multiprocessing
import os

from .log.logging import set_loglevel, verbosity_to_level, ENV_VERBOSITY


class PrintVersion(argparse.Action):
    """
    The default version printer, unfortunately, inserts newlines.
    This is the easiest way around.
    """
    # pylint: disable=too-few-public-methods
    def __call__(self, parser, namespace, values, option_string=None):
        del parser, namespace, values, option_string  # unused

        from . import LONGVERSION
        print(LONGVERSION)
        exit(0)


def main(argv=None):
    """ Top-level argparsing; invokes subparser for all submodules. """
    cli = argparse.ArgumentParser(
        "openage",
        description=("free age of empires II engine clone")
    )

    cli.add_argument("--version", "-V", nargs=0, action=PrintVersion,
                     help="print version info and exit")

    # shared arguments for all subcommands
    global_cli = argparse.ArgumentParser(add_help=False)
    global_cli.add_argument("--verbose", "-v", action='count',
                            default=ENV_VERBOSITY,
                            help="increase verbosity")
    global_cli.add_argument("--quiet", "-q", action='count', default=0,
                            help="decrease verbosity")

    # shared data-directory argument for most subcommands
    datadir_cli = argparse.ArgumentParser(add_help=False)
    datadir_cli.add_argument("--asset-dir", default='./assets',
                             help="Use this as the asset directory.")

    subparsers = cli.add_subparsers(dest="subcommand")

    # enable reimports for "init_subparser"
    # pylint: disable=reimported

    from .game.main import init_subparser
    game_cli = subparsers.add_parser(
        "game",
        parents=[global_cli, datadir_cli])
    init_subparser(game_cli)

    from .testing.main import init_subparser
    init_subparser(subparsers.add_parser(
        "test",
        parents=[global_cli, datadir_cli]))

    from .convert.main import init_subparser
    init_subparser(subparsers.add_parser(
        "convert",
        parents=[global_cli, datadir_cli]))

    from .convert.singlefile import init_subparser
    init_subparser(subparsers.add_parser(
        "convert-file",
        parents=[global_cli, datadir_cli]))

    from .codegen.main import init_subparser
    init_subparser(subparsers.add_parser(
        "codegen",
        parents=[global_cli]))

    args = cli.parse_args(argv)

    if not args.subcommand:
        # the user didn't specify a subcommand. default to 'game'.
        args = game_cli.parse_args(argv)

    # process the shared args
    set_loglevel(verbosity_to_level(args.verbose - args.quiet))

    if "asset_dir" in args:
        if not os.path.exists(args.asset_dir):
            cli.error("directory does not exist: " + args.asset_dir)

    # call the entry point for the subcommand.
    return args.entrypoint(args, cli.error)


if __name__ == '__main__':
    # openage is complicated and multithreaded; better not use fork.
    # TODO fix pending pylint 1.5: pylint: disable=no-member
    multiprocessing.set_start_method('spawn')

    exit(main())
