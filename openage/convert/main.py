# Copyright 2015-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-branches
"""
Entry point for all of the asset conversion.
"""
from __future__ import annotations

from datetime import datetime
import typing

from ..log import info, warn

from ..util.fslike.directory import CaseIgnoringDirectory
from ..util.fslike.wrapper import (DirectoryCreator,
                                   Synchronizer as AccessSynchronizer)
from .service.debug_info import debug_cli_args, debug_game_version, debug_mounts
from .service.init.changelog import check_updates
from .service.init.modpack_search import enumerate_modpacks
from .service.init.mount_asset_dirs import mount_asset_dirs
from .service.init.version_detect import create_version_objects
from .tool.interactive import interactive_browser
from .tool.subtool.acquire_sourcedir import acquire_conversion_source_dir, wanna_convert, \
    wanna_check_updates
from .tool.subtool.version_select import get_game_version

if typing.TYPE_CHECKING:
    from argparse import ArgumentParser, Namespace
    from openage.util.fslike.directory import Directory
    from openage.util.fslike.union import UnionPath
    from openage.util.fslike.path import Path


def convert_assets(
    assets: UnionPath,
    args: Namespace,
    srcdir: Directory = None
) -> None:
    """
    Perform asset conversion.

    Requires original assets and stores them in usable and free formats.

    assets must be a filesystem-like object pointing at the game's asset dir.
    srcdir must be None, or point at some source directory.

    This method prepares srcdir and targetdir to allow a pleasant, unified
    conversion experience, then passes them to .driver.convert().
    """
    converted_path = assets / "converted"
    converted_path.mkdirs()
    targetdir = DirectoryCreator(converted_path).root

    # Set compression level for media output if it was not set
    if "compression_level" not in vars(args):
        args.compression_level = 1

    # Set worker count for multi-threading if it was not set
    if "jobs" not in vars(args):
        args.jobs = None

    # Set verbosity for debug output
    if "debug_info" not in vars(args) or not args.debug_info:
        if args.devmode:
            args.debug_info = 3

        else:
            args.debug_info = 0

    # add a dir for debug info
    debug_log_path = converted_path / "debug" / datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
    debugdir = DirectoryCreator(debug_log_path).root
    args.debugdir = debugdir

    # Create CLI args info
    debug_cli_args(args.debugdir, args.debug_info, args)

    # Initialize game versions data
    auxiliary_files_dir = args.cfg_dir / "converter" / "games"
    args.avail_game_eds, args.avail_game_exps = create_version_objects(auxiliary_files_dir)

    # try to get previously used source dir
    asset_locations_path = assets / "converted" / "asset_locations.cache"
    prev_srcdirs = get_prev_srcdir_paths(asset_locations_path)

    # acquire conversion source directory
    if srcdir is None:
        srcdir = acquire_conversion_source_dir(args.avail_game_eds, prev_srcdirs)

    # Acquire game version info
    args.game_version = get_game_version(srcdir, args.avail_game_eds, args.avail_game_exps)
    debug_game_version(args.debugdir, args.debug_info, args)

    if not args.game_version.edition:
        return None

    # Mount assets into conversion folder
    data_dir = mount_asset_dirs(srcdir, args.game_version)
    if not data_dir:
        return None

    args.srcdir = AccessSynchronizer(data_dir).root
    args.targetdir = targetdir

    # Create mountpoint info
    debug_mounts(args.debugdir, args.debug_info, args)

    def flag(name):
        """
        Convenience function for accessing boolean flags in args.
        Flags default to False if they don't exist.
        """
        return getattr(args, name, False)

    args.flag = flag

    # import here so codegen.py doesn't depend on it.
    from .tool.driver import convert

    # Run the conversion process
    convert(args)

    # clean args
    del args.srcdir
    del args.targetdir

    # Remember the asset location if it is not already in the cache
    if prev_srcdirs is None:
        asset_locations_path.touch()
        prev_srcdirs = set()

    used_asset_path = data_dir.resolve_native_path().decode('utf-8')
    if used_asset_path not in prev_srcdirs:
        try:
            with asset_locations_path.open("a") as file_obj:
                if len(prev_srcdirs) > 0:
                    file_obj.write("\n")

                file_obj.write(used_asset_path)

        except IOError:
            # cache file cannot be accessed, skip writing
            warn(f"Cannot access asset location cache file {asset_locations_path}")
            info("Skipped saving asset location")


def get_prev_srcdir_paths(asset_location_path: Path) -> set[str] | None:
    """
    Get previously used source directories from a cache file.

    :param asset_location_path: Path to the cache file.
    :type asset_location_path: Path
    :return: Previously used source directories.
    :rtype: set[str] | None
    """
    prev_source_dirs: set[str] = set()
    try:
        with asset_location_path.open("r") as file_obj:
            prev_source_dirs.update(file_obj.read().split("\n"))

    except FileNotFoundError:
        prev_source_dirs = None

    return prev_source_dirs


def init_subparser(cli: ArgumentParser):
    """ Initializes the parser for convert-specific args. """
    cli.set_defaults(entrypoint=main)

    cli.add_argument(
        "--source-dir", default=None,
        help="source data directory")

    cli.add_argument(
        "--output-dir", default=None,
        help="destination data output directory")

    cli.add_argument(
        "--force", action='store_true',
        help="force conversion, even if up-to-date assets already exist.")

    cli.add_argument(
        "--gen-extra-files", action='store_true',
        help="generate some extra files, useful for debugging the converter.")

    cli.add_argument(
        "--no-media", action='store_true',
        help="do not convert any media files (slp, wav, ...)")

    cli.add_argument(
        "--no-metadata", action='store_true',
        help=("do not store any metadata "
              "(except for those associated with media files)"))

    cli.add_argument(
        "--no-sounds", action='store_true',
        help="do not convert any sound files")

    cli.add_argument(
        "--no-graphics", action='store_true',
        help="do not convert game graphics")

    cli.add_argument(
        "--no-interface", action='store_true',
        help="do not convert interface graphics")

    cli.add_argument(
        "--no-scripts", action='store_true',
        help="do not convert scripts (AI and Random Maps)")

    cli.add_argument(
        "--no-pickle-cache", action='store_true',
        help="don't use a pickle file to skip the dat file reading.")

    cli.add_argument(
        "--jobs", "-j", type=int, default=None)

    cli.add_argument(
        "--interactive", "-i", action='store_true',
        help="browse the files interactively")

    cli.add_argument(
        "--id", type=int, default=None,
        help="only convert files with this id (used for debugging..)")

    cli.add_argument(
        "--compression-level", type=int, default=2, choices=[0, 1, 2, 3, 4],
        help="set PNG compression level")

    cli.add_argument(
        "--debug-info", type=int, choices=[0, 1, 2, 3, 4, 5, 6],
        help="create debug output for the converter run; verbosity levels 0-6")

    cli.add_argument(
        "--low-memory", action='store_true',
        help="Activate low memory mode")

    cli.add_argument(
        "--export-api", action='store_true',
        help="Export the openage nyan API definition as a modpack")

    cli.add_argument(
        "--check-updates", action='store_true',
        help="Check if the assets are up to date"
    )

    cli.add_argument(
        "--no-prompts", action='store_false', dest='show_prompts',
        help="Disable user prompts"
    )


def main(args, error):
    """ CLI entry point """
    del error  # unused

    # initialize libopenage
    from ..cppinterface.setup import setup
    setup(args)

    # conversion source
    if args.source_dir is not None:
        srcdir = CaseIgnoringDirectory(args.source_dir).root
    else:
        srcdir = None

    # mount the config folder at "cfg/"
    from ..cvar.location import get_config_path
    from ..util.fslike.union import Union
    root = Union().root
    root["cfg"].mount(get_config_path())
    args.cfg_dir = root["cfg"]

    if args.interactive:
        interactive_browser(root["cfg"], srcdir)
        return 0

    # conversion target
    from ..assets import get_asset_path
    outdir = get_asset_path(args.output_dir)

    if args.force or (args.show_prompts and wanna_convert()):
        convert_assets(outdir, args, srcdir)

    if args.check_updates or (args.show_prompts and wanna_check_updates()):
        # check if the assets are up to date
        modpack_dir = outdir / "converted"
        available_modpacks = enumerate_modpacks(modpack_dir, exclude={"engine"})

        game_info_dir = args.cfg_dir / "converter" / "games"

        check_updates(available_modpacks, game_info_dir)

    return 0
