# Copyright 2015-2023 the openage authors. See copying.md for legal info.
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
        "--modpacks", nargs="+", required=True,
        help="list of modpacks to load")
    
    cli.add_argument(
        "--window-width", type=int, default=800,
        help="Window width")

    cli.add_argument(
        "--window-height", type=int, default=600,
        help="Window height")

    cli.add_argument(
        "--vsync", action='store_true',
        help="Enable V-Sync")

    cli.add_argument(
        "--fullscreen", action='store_true',
        help="Run in fullscreen mode")

    cli.add_argument(
        "--borderless", action='store_true',
        help="Run in borderless mode")

    cli.add_argument(
        "--windowed", action='store_true',
        help="Run in windowed mode")

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
    from ..convert.main import conversion_required, convert_assets
    from ..convert.tool.api_export import export_api
    from ..convert.service.init.api_export_required import api_export_required
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

    # ensure that the assets have been converted
    if conversion_required(asset_path):
        convert_assets(asset_path, args)

    # modpacks
    mods = []
    for modpack in args.modpacks:
        mods.append(modpack.encode("utf-8"))

    args.modpacks = mods

    window_args = {
        "width": args.window_width,
        "height": args.window_height,
        "vsync": args.vsync,
        "fullscreen": args.fullscreen,
        "borderless": args.borderless,
        "windowed": args.windowed
    }
    # start the game, continue in main_cpp.pyx!
    return run_game(args, root,window_args)
