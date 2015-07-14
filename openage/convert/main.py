# Copyright 2015-2015 the openage authors. See copying.md for legal info.

""" Entry point for all of the asset conversion. """

import os

from ..log import info
from ..util.fslike import (
    CaseIgnoringReadOnlyDirectory,
    FSLikeObjectWrapper,
    SubDirectory
)
from ..util.fslike_synchronized import Synchronizer
from ..util.strings import format_progress


class DirectoryCreator(FSLikeObjectWrapper):
    """
    Wrapper around a filesystem-like object that automatically creates
    directories when attempting to create a file.
    """
    def _open_impl(self, pathcomponents, mode):
        if 'w' in mode or 'x' in mode or '+' in mode:
            self.mkdirs(pathcomponents[:-1])

        return self.wrapped.open(pathcomponents, mode)

    def __repr__(self):
        return "DirectoryCreator({})".format(self.wrapped)


def mount_drs_archives(srcdir):
    """
    Returns a Union where srcdir is mounted at /, and all the DRS files
    are mounted in subfolders.
    """
    from ..util.fslike import Union
    from .drs import DRS

    result = Union()
    result.mount(srcdir, '.')

    def mount_drs(filename, target, ignore_nonexistant=False):
        """ Mounts the DRS file from srcdir's filename at result's target. """
        if ignore_nonexistant and not srcdir.isfile(filename):
            return

        result.mount(DRS(srcdir.open(filename, 'rb')), target)

    mount_drs("data/graphics.drs", "graphics")
    mount_drs("data/interfac.drs", "interface")

    mount_drs("data/sounds.drs", "sounds")
    mount_drs("data/sounds_x1.drs", "sounds")

    # In the HD edition, gamedata.drs has been merged into gamedata_x1.drs
    mount_drs("data/gamedata.drs", "gamedata", ignore_nonexistant=True)
    mount_drs("data/gamedata_x1.drs", "gamedata")
    mount_drs("data/gamedata_x1_p1.drs", "gamedata")

    mount_drs("data/terrain.drs", "terrain")

    return result


def convert_assets(assets, args, srcdir=None):
    """
    Perform asset conversion.

    Requires original assets and stores them in usable and free formats.

    assets must be a filesystem-like object pointing at the game's asset dir.
    sourceidr must be None, or point at some source directory.

    If gen_extra_files is True, some more files, mostly for debugging purposes,
    are created.

    This method prepares srcdir and targetdir to allow a pleasant, unified
    conversion experience, then passes them to .driver.convert().
    """
    # acquire conversion source data
    if srcdir is None:
        srcdir = acquire_conversion_source_dir()

    testfile = 'data/empires2_x1_p1.dat'
    if not srcdir.isfile(testfile):
        print("file not found: " + testfile)
        return False

    srcdir = mount_drs_archives(srcdir)
    targetdir = DirectoryCreator(SubDirectory(assets, "converted", True))

    # make srcdir and targetdir safe for threaded conversion
    args.srcdir = Synchronizer(srcdir)
    args.targetdir = Synchronizer(targetdir)

    def flag(name):
        """
        Convenience function for accessing boolean flags in args.
        Flags default to False if they don't exist.
        """
        return getattr(args, name, False)

    args.flag = flag

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
            info("[%s] %s" % (converted_count, current_item))
        else:
            info("[%s] %s" % (
                format_progress(converted_count, total_count),
                current_item
            ))

        converted_count += 1

    # clean args
    del args.srcdir
    del args.targetdir


def acquire_conversion_source_dir():
    """
    Acquires source dir for the asset conversion.

    Returns a file system-like object that holds all the required files.
    """
    # ask the for conversion source
    print("media conversion is required.")

    if 'AGE2DIR' in os.environ:
        sourcedir = os.environ['AGE2DIR']
        print("using AGE2DIR: " + sourcedir)
    else:
        # TODO use some sort of GTK GUI for this.
        print("please provide your AoE II installation dir:")

        try:
            sourcedir = input("> ")
        except KeyboardInterrupt:
            print("")
            exit(0)
        except EOFError:
            print("")
            exit(0)

    return CaseIgnoringReadOnlyDirectory(sourcedir)


def conversion_required(asset_dir, args):
    """
    Returns true if an asset conversion is required to run the game.

    Sets options in args according to what sorts of conversion are required.
    """
    try:
        converted_by = asset_dir.open("converted/converted_by").read().strip()

        # TODO this is the place where we can do sophisticated checks whether
        #      the assets are outdated.
        if converted_by:
            return False
        else:
            args.no_media = False
            return True

    except FileNotFoundError:
        # assets have not been converted yet
        return True


def init_subparser(cli):
    """ Initializes the parser for convert-specific args. """
    cli.set_defaults(entrypoint=main)

    cli.add_argument(
        "--source-dir", default=None,
        help="source data directory")

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
        "--jobs", "-j", type=int, default=None)


def main(args, error):
    """ CLI entry point """
    del error  # unused

    # initialize libopenage
    from ..cppinterface.setup import setup
    setup()

    # conversion source
    if args.source_dir is not None:
        srcdir = CaseIgnoringReadOnlyDirectory(args.source_dir)
    else:
        srcdir = None

    # conversion target
    from ..assets import get_assets
    assets = get_assets(args)

    if args.force or conversion_required(assets, args):
        if not convert_assets(assets, args, srcdir):
            return 1
    else:
        print("assets are up to date; no conversion is required.")
        print("override with --force.")
