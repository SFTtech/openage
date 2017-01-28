# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Holds the game entry point for openage.
"""

from ..log import err


def init_subparser(cli):
    """ Initializes the parser for game-specific args. """
    cli.set_defaults(entrypoint=main)

    cli.add_argument(
        "--fps", type=int,
        help="upper limit for fps. this limit is imposed on top of vsync")

    cli.add_argument(
        "--gl-debug", action='store_true',
        help="throw exceptions directly from the OpenGL calls")


def main(args, error):
    """
    Makes sure that the assets have been converted,
    and jumps into the C++ main method.
    """
    del error  # unused

    # we have to import stuff inside the function
    # as it depends on generated/compiled code
    from .main_cpp import run_game
    from ..assets import get_asset_path
    from ..convert.main import conversion_required, convert_assets
    from ..cppinterface.setup import setup as cpp_interface_setup

    # initialize libopenage
    cpp_interface_setup()

    # set up asset load paths
    assets = get_asset_path(args)

    # ensure that the assets have been converted
    if conversion_required(assets, args):
        if not convert_assets(assets, args):
            err("game asset conversion failed")
            return 1

    # start the game!
    return run_game(args, assets)
