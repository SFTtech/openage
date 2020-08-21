# Copyright 2015-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-branches
""" Entry point for all of the asset conversion. """
# importing readline enables the input() calls to have history etc.

import os
import readline  # pylint: disable=unused-import

from . import changelog
from ..log import warn, info, dbg
from ..util.fslike.directory import CaseIgnoringDirectory, Directory
from ..util.fslike.wrapper import (DirectoryCreator,
                                   Synchronizer as AccessSynchronizer)
from ..util.strings import format_progress
from .service.mount.mount_asset_dirs import mount_asset_dirs
from .tool.sourcedir.acquire_sourcedir import acquire_conversion_source_dir
from .tool.sourcedir.version_select import get_game_version


def convert_assets(assets, args, srcdir=None, prev_source_dir_path=None):
    """
    Perform asset conversion.

    Requires original assets and stores them in usable and free formats.

    assets must be a filesystem-like object pointing at the game's asset dir.
    srcdir must be None, or point at some source directory.

    If gen_extra_files is True, some more files, mostly for debugging purposes,
    are created.

    This method prepares srcdir and targetdir to allow a pleasant, unified
    conversion experience, then passes them to .driver.convert().
    """
    # acquire conversion source directory
    if srcdir is None:
        srcdir = acquire_conversion_source_dir(prev_source_dir_path)

    # Acquire game version info
    game_version = get_game_version(srcdir)

    # Mount assets into conversion folder
    data_dir = mount_asset_dirs(srcdir, game_version)

    if not data_dir:
        return None

    # make versions available easily
    args.game_version = game_version

    converted_path = assets / "converted"
    converted_path.mkdirs()
    targetdir = DirectoryCreator(converted_path).root

    # make srcdir and targetdir safe for threaded conversion
    args.srcdir = AccessSynchronizer(data_dir).root
    args.targetdir = AccessSynchronizer(targetdir).root

    def flag(name):
        """
        Convenience function for accessing boolean flags in args.
        Flags default to False if they don't exist.
        """
        return getattr(args, name, False)

    args.flag = flag

    # import here so codegen.py doesn't depend on it.
    from .driver import convert

    converted_count = 0
    total_count = None
    for current_item in convert(args):
        if isinstance(current_item, int):
            # convert is informing us about the estimated number of remaining
            # items.
            total_count = current_item + converted_count
            continue

        # TODO a GUI would be nice here.

        if total_count is None:
            info("[%s] %s", converted_count, current_item)
        else:
            info("[%s] %s", format_progress(converted_count, total_count), current_item)

        converted_count += 1

    # clean args
    del args.srcdir
    del args.targetdir

    return data_dir.resolve_native_path()


# REFA: function -> service
def conversion_required(asset_dir, args):
    """
    Returns true if an asset conversion is required to run the game.

    Sets options in args according to what sorts of conversion are required.
    """

    version_path = asset_dir / 'converted' / changelog.ASSET_VERSION_FILENAME
    spec_path = asset_dir / 'converted' / changelog.GAMESPEC_VERSION_FILENAME

    # determine the version of assets
    try:
        with version_path.open() as fileobj:
            asset_version = fileobj.read().strip()

        try:
            asset_version = int(asset_version)
        except ValueError:
            info("Converted asset version has improper format; "
                 "expected integer number")
            asset_version = -1

    except FileNotFoundError:
        # assets have not been converted yet
        info("No converted assets have been found")
        asset_version = -1

    # determine the version of the gamespec format
    try:
        with spec_path.open() as fileobj:
            spec_version = fileobj.read().strip()

    except FileNotFoundError:
        info("Game specification version file not found.")
        spec_version = None

    changes = changelog.changes(asset_version, spec_version)

    if not changes:
        dbg("Converted assets are up to date")
        return False

    if asset_version >= 0 and asset_version != changelog.ASSET_VERSION:
        info("Found converted assets with version %d, "
             "but need version %d", asset_version, changelog.ASSET_VERSION)

    info("Converting %s", ", ".join(sorted(changes)))

    # try to resolve resolve the output path
    target_path = asset_dir.resolve_native_path_w()
    if not target_path:
        raise OSError("could not resolve a writable asset path "
                      "in {}".format(asset_dir))

    info("Will save to '%s'", target_path.decode(errors="replace"))

    for component in changelog.COMPONENTS:
        if component not in changes:
            # don't reconvert this component:
            setattr(args, "no_{}".format(component), True)

    if "metadata" in changes:
        args.no_pickle_cache = True

    return True


# REFA: function -> subtool
def interactive_browser(srcdir=None):
    """
    launch an interactive view for browsing the original
    archives.

    TODO: Enhance functionality and fix SLP conversion.
    """

    info("launching interactive data browser...")

    # the variables are actually used, in the interactive prompt.
    # pylint: disable=possibly-unused-variable
    game_version = get_game_version(srcdir)
    data = mount_asset_dirs(srcdir, game_version)

    if not data:
        warn("cannot launch browser as no valid input assets were found.")
        return

    def save(path, target):
        """
        save a path to a custom target
        """
        with path.open("rb") as infile:
            with open(target, "rb") as outfile:
                outfile.write(infile.read())

    def save_slp(path, target, palette=None):
        """
        save a slp as png.
        """
        from .texture import Texture
        from .value_object.media.slp import SLP
        from .driver import get_palettes

        if not palette:
            palette = get_palettes(data)

        with path.open("rb") as slpfile:
            tex = Texture(SLP(slpfile.read()), palette)

            out_path, filename = os.path.split(target)
            tex.save(Directory(out_path).root, filename)

    import code
    from pprint import pprint

    import rlcompleter

    completer = rlcompleter.Completer(locals())
    readline.parse_and_bind("tab: complete")
    readline.parse_and_bind("set show-all-if-ambiguous on")
    readline.set_completer(completer.complete)

    code.interact(
        banner=("\nuse `pprint` for beautiful output!\n"
                "you can access stuff by the `data` variable!\n"
                "`data` is an openage.util.fslike.path.Path!\n\n"
                "* version detection:   pprint(game_versions)\n"
                "* list contents:       pprint(list(data['graphics'].list()))\n"
                "* dump data:           save(data['file/path'], '/tmp/outputfile')\n"
                "* save a slp as png:   save_slp(data['dir/123.slp'], '/tmp/pic.png')\n"),
        local=locals()
    )


def init_subparser(cli):
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

    if args.interactive:
        interactive_browser(srcdir)
        return 0

    # conversion target
    from ..assets import get_asset_path
    outdir = get_asset_path(args.output_dir)

    if args.force or conversion_required(outdir, args):
        if not convert_assets(outdir, args, srcdir):
            return 1
    else:
        print("assets are up to date; no conversion is required.")
        print("override with --force.")

    return 0
