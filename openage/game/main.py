# Copyright 2015-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals

"""
Holds the game entry point for openage.
"""
from __future__ import annotations
import typing


from ..log import info

if typing.TYPE_CHECKING:
    from argparse import ArgumentParser


def init_subparser(cli: ArgumentParser) -> None:
    """ Initializes the parser for game-specific args. """
    cli.set_defaults(entrypoint=main)

    cli.add_argument(
        "--gl-debug", action='store_true',
        help="throw exceptions directly from the OpenGL calls")

    cli.add_argument(
        "--headless", action='store_true',
        help="run without displaying graphics")

    cli.add_argument(
        "--modpacks", nargs="+", required=True, type=str,
        help="list of modpacks to load")

    cli.add_argument(
        "--check-updates", action='store_true',
        help="Check if the assets are up to date"
    )

    cli.add_argument(
        "--window-size", nargs=2, type=int, default=[1024, 768],
        metavar=('WIDTH', 'HEIGHT'),
        help="Initial window size in pixels")

    cli.add_argument(
        "--vsync", action='store_true',
        help="Enable vertical synchronization")

    cli.add_argument(
        "--window-mode", choices=["fullscreen", "borderless", "windowed"], default="windowed",
        help="Set the window mode")


def main(args, error):
    """
    Makes sure that the assets have been converted,
    and jumps into the C++ main method.
    """
    del error  # unused

    # we have to import stuff inside the function
    # as it depends on generated/compiled code
    from .main_cpp import run_game
    from .. import config
    from ..assets import get_asset_path
    from ..convert.tool.api_export import export_api
    from ..convert.service.init.api_export_required import api_export_required
    from ..convert.service.init.changelog import check_updates
    from ..convert.service.init.modpack_search import enumerate_modpacks
    from ..cppinterface.setup import setup as cpp_interface_setup
    from ..cvar.location import get_config_path
    from ..util.fslike.union import Union

    # initialize libopenage
    cpp_interface_setup(args)

    info("launching openage %s", config.VERSION)
    info("compiled by %s", config.COMPILER)

    if config.DEVMODE:
        info("running in DEVMODE")

    # create virtual file system for data paths
    root = Union().root

    # mount the assets folder union at "assets/"
    asset_path = get_asset_path(args.asset_dir)
    root["assets"].mount(asset_path)

    # mount the config folder at "cfg/"
    root["cfg"].mount(get_config_path(args.cfg_dir))
    args.cfg_dir = root["cfg"]

    # ensure that the openage API is present
    if api_export_required(asset_path):
        # export to assets folder
        converted_path = asset_path / "converted"
        converted_path.mkdirs()
        export_api(converted_path)

    # ensure that modpacks are available
    modpack_dir = asset_path / "converted"
    available_modpacks = enumerate_modpacks(modpack_dir, exclude={"engine"})
    for modpack in args.modpacks:
        if modpack not in available_modpacks:
            raise FileNotFoundError(f"Modpack '{modpack}' not found in {modpack_dir}")

    # check if the converted modpacks are up to date
    if args.check_updates:
        check_updates(available_modpacks, args.cfg_dir / "converter" / "games")

    # encode modpacks as bytes for the C++ interface
    args.modpacks = [modpack.encode('utf-8') for modpack in args.modpacks]

    # Pass window parameters to engine
    args.window_args = {
        "width": args.window_size[0],
        "height": args.window_size[1],
        "vsync": args.vsync,
        "window_mode": args.window_mode,
    }

    # start the game, continue in main_cpp.pyx!
    return run_game(args, root)
