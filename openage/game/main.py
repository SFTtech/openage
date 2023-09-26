# Copyright 2015-2023 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals

"""
Holds the game entry point for openage.
"""
from __future__ import annotations
import typing


from ..convert.tool.subtool.acquire_sourcedir import wanna_convert
from ..log import err, info

if typing.TYPE_CHECKING:
    from argparse import ArgumentParser


def init_subparser(cli: ArgumentParser) -> None:
    """ Initializes the parser for game-specific args. """
    cli.set_defaults(entrypoint=main)

    cli.add_argument(
        "--fps", type=int,
        help="upper limit for fps. this limit is imposed on top of vsync")

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

    # ensure that the assets have been converted
    if wanna_convert() or conversion_required(root["assets"], args):
        # try to get previously used source dir
        asset_location_path = root["cfg"] / "asset_location"
        try:
            with asset_location_path.open("r") as file_obj:
                prev_source_dir_path = file_obj.read().strip()
        except FileNotFoundError:
            prev_source_dir_path = None
        used_asset_path = convert_assets(
            root["assets"],
            args,
            prev_source_dir_path=prev_source_dir_path
        )

        if used_asset_path:
            # Remember the asset location
            with asset_location_path.open("wb") as file_obj:
                file_obj.write(used_asset_path)
        else:
            err("game asset conversion failed")
            return 1

    # modpacks
    if args.modpacks:
        mods = []
        for modpack in args.modpacks:
            mods.append(modpack.encode("utf-8"))

        args.modpacks = mods

    # start the game, continue in main_cpp.pyx!
    return run_game(args, root)
