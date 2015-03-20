# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Holds openage's main main method, used for launching the game.
"""

from ..log import dbg


def init_subparser(cli):
    """ Initializes the parser for game-specific args. """
    cli.set_defaults(entrypoint=main)


def main(args, error):
    """
    Makes sure that the assets have been converted,
    and jumps into the C++ main method.
    """
    del error  # unused

    # initialize libopenage
    dbg("initializing libopenage")
    from ..cppinterface.setup import setup
    setup()

    # ensure that the assets have been converted
    dbg("ensuring that the assets have been converted")
    from ..convert.driver import ensure_assets
    if not ensure_assets():
        print("could not convert media")
        return 1

    # jump into the main method
    from .main_cpp import run_game
    return run_game(args)
