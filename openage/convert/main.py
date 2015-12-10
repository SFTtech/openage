# Copyright 2015-2015 the openage authors. See copying.md for legal info.

""" Entry point for all of the asset conversion. """
import os
# importing readline enables the raw_input calls to have history etc.
import readline  # pylint: disable=unused-import

from .game_versions import GameVersion, get_game_versions
from . import changelog

from ..log import warn, info, dbg
from ..util.fslike.wrapper import (
    Wrapper as FSLikeObjWrapper,
    Synchronizer as FSLikeObjSynchronizer
)
from ..util.fslike.directory import CaseIgnoringDirectory
from ..util.strings import format_progress


class DirectoryCreator(FSLikeObjWrapper):
    """
    Wrapper around a filesystem-like object that automatically creates
    directories when attempting to create a file.
    """
    def open_w(self, parts):
        self.mkdirs(parts[:-1])
        return super().open_w(parts)

    def __repr__(self):
        return "DirectoryCreator({})".format(self.obj)


def mount_drs_archives(srcdir, game_version=None):
    """
    Returns a Union path where srcdir is mounted at /, and all the DRS files
    are mounted in subfolders.
    """
    from ..util.fslike.union import Union
    from .drs import DRS

    result = Union().root
    result.mount(srcdir)

    def mount_drs(filename, target, ignore_nonexistant=False):
        """ Mounts the DRS file from srcdir's filename at result's target. """
        drspath = srcdir.joinpath(filename)
        if ignore_nonexistant and not drspath.is_file():
            return

        result.joinpath(target).mount(DRS(drspath.open('rb')).root)

    if game_version is GameVersion.age2_fe:
        result['graphics'].mount(srcdir['resources/_common/drs/graphics'])
        result['interface'].mount(srcdir['resources/_common/drs/interface'])
        result['sounds'].mount(srcdir['resources/_common/drs/sounds'])
        result['gamedata'].mount(srcdir['resources/_common/drs/gamedata'])
        result['terrain'].mount(srcdir['resources/_common/drs/terrain'])
        result['data'].mount(srcdir['/resources/_common/dat'])
    else:
        mount_drs("data/graphics.drs", "graphics")
        mount_drs("data/interfac.drs", "interface")
        mount_drs("data/sounds.drs", "sounds")
        mount_drs("data/sounds_x1.drs", "sounds")
        mount_drs("data/terrain.drs", "terrain")
        mount_drs("data/gamedata.drs", "gamedata")
        mount_drs("data/gamedata_x1.drs", "gamedata")
        mount_drs("data/gamedata_x1_p1.drs", "gamedata")

    return result


def convert_assets(assets, args, srcdir=None):
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
        srcdir = acquire_conversion_source_dir()

    args.game_version = set(get_game_versions(srcdir))
    if not args.game_version:
        warn("Game version(s) could not be detected in {}".format(srcdir))
    if not any(version.openage_supported for version in args.game_version):
        warn("None supported of the Game version(s) {}"
             .format("; ".join(args.game_version)))
        return False
    info("Game version(s) detected: {}"
         .format("; ".join(str(version) for version in args.game_version)))

    srcdir = mount_drs_archives(srcdir, args.game_version)

    converted_path = assets.joinpath("converted")
    converted_path.mkdirs()
    targetdir = DirectoryCreator(converted_path).root

    # make srcdir and targetdir safe for threaded conversion
    args.srcdir = FSLikeObjSynchronizer(srcdir).root
    args.targetdir = FSLikeObjSynchronizer(targetdir).root

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

    return True


def acquire_conversion_source_dir():
    """
    Acquires source dir for the asset conversion.

    Returns a file system-like object that holds all the required files.
    """
    # ask for the conversion source
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

    return CaseIgnoringDirectory(sourcedir).root


def conversion_required(asset_dir, args):
    """
    Returns true if an asset conversion is required to run the game.

    Sets options in args according to what sorts of conversion are required.
    """
    try:
        version_path = asset_dir['converted', changelog.ASSET_VERSION_FILENAME]
        with version_path.open() as fileobj:
            asset_version = fileobj.read().strip()
    except FileNotFoundError:
        # assets have not been converted yet
        info("No converted assets have been found")
        return True

    # TODO: datapack parsing

    try:
        asset_version = int(asset_version)
    except ValueError:
        info("Converted assets have improper format; expected integer version")
        return True

    changes = changelog.changes(asset_version)

    if not changes:
        dbg("Converted assets are up to date")
        return False

    else:
        info("Converted assets outdated: Version " + str(asset_version))
        info("Reconverting " + ", ".join(sorted(changes)))
        for component in changelog.COMPONENTS:
            if component not in changes:
                # don't reconvert this component:
                setattr(args, "no_{}".format(component), True)

        if "metadata" in changes:
            args.no_pickle_cache = True

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
        "--no-graphics", action='store_true',
        help="do not convert any graphics")

    cli.add_argument(
        "--no-pickle-cache", action='store_true',
        help="don't use a pickle file to skip the dat file reading.")

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
        srcdir = CaseIgnoringDirectory(args.source_dir).root
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
