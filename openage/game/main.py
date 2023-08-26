# Copyright 2015-2023 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals

"""
Holds the game entry point for openage.
"""
from __future__ import annotations
import typing

from ..convert.tool.api_export import export_api
from ..convert.service.init.api_export_required import api_export_required
from ..convert.tool.subtool.acquire_sourcedir import wanna_convert
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
        "--modpacks", nargs="+", type=bytes,
        help="list of modpacks to load")


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
    root["assets"].mount(get_asset_path(args.asset_dir))

    # mount the config folder at "cfg/"
    root["cfg"].mount(get_config_path(args.cfg_dir))
    args.cfg_dir = root["cfg"]

    # ensure that the openage API is present
    if api_export_required(root["assets"]):
        # export to assets folder
        converted_path = root["assets"] / "converted"
        converted_path.mkdirs()
        export_api(converted_path)

    # ensure that the assets have been converted
    if wanna_convert() or conversion_required(root["assets"]):
        convert_assets(root["assets"], args)

    # modpacks
    if args.modpacks:
        mods = []
        for modpack in args.modpacks:
            mods.append(modpack.encode("utf-8"))

        args.modpacks = mods

    # start the game, continue in main_cpp.pyx!
    return run_game(args, root)
