# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""
Holds openage's main main method, used for launching the game.
"""

from ..log import err


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
    from ..cppinterface.setup import setup
    setup()

    # load assets
    from ..assets import get_assets
    assets = get_assets(args)

    # ensure that the assets have been converted
    from ..convert.main import conversion_required, convert_assets
    if conversion_required(assets, args):
        if not convert_assets(assets, args):
            err("game asset conversion failed")
            return 1

    # jump into the main method
    from .main_cpp import run_game
    return run_game(args, assets)
