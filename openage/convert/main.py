# Copyright 2015-2016 the openage authors. See copying.md for legal info.

""" Entry point for all of the asset conversion. """
import os
# importing readline enables the raw_input calls to have history etc.
import readline  # pylint: disable=unused-import
import subprocess
from configparser import ConfigParser

from .game_versions import GameVersion, get_game_versions
from . import changelog

from ..log import warn, info, dbg
from ..util.fslike.wrapper import (
    Wrapper as FSLikeObjWrapper,
    Synchronizer as FSLikeObjSynchronizer
)
from ..util.fslike.directory import CaseIgnoringDirectory
from ..util.strings import format_progress

_STANDARD_PATH_IN_32BIT_WINEPREFIX =\
    "drive_c/Program Files/Microsoft Games/Age of Empires II/"
_STANDARD_PATH_IN_64BIT_WINEPREFIX =\
    "drive_c/Program Files (x86)/Microsoft Games/Age of Empires II/"
_REGISTRY_KEY = \
    "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Microsoft Games\\"
_REGISTRY_SUFFIX_AOK = "Age of Empires\\2.0"
_REGISTRY_SUFFIX_TC = "Age of Empires II: The Conquerors Expansion\\1.0"


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


def mount_drs_archives(srcdir, game_versions=None):
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

    if GameVersion.age2_fe in game_versions:
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
        if GameVersion.age2_hd_3x not in game_versions:
            mount_drs("data/gamedata.drs", "gamedata")
        mount_drs("data/gamedata_x1.drs", "gamedata")
        mount_drs("data/gamedata_x1_p1.drs", "gamedata")
        # TODO mount gamedata_x2.drs and _x2_p1 if they exist?

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

    args.game_versions = set(get_game_versions(srcdir))
    if not args.game_versions:
        warn("Game version(s) could not be detected in {}".format(srcdir))

    versions = "; ".join(str(version) for version in args.game_versions)
    if not any(version.openage_supported for version in args.game_versions):
        warn("None supported of the Game version(s) {}".format(versions))
        return False
    info("Game version(s) detected: {}".format(versions))

    srcdir = mount_drs_archives(srcdir, args.game_versions)

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


def _expand_relative_path(path):
    """Expand relative path to an absolute one, including abbreviations like
    ~ and environment variables"""
    return os.path.realpath(os.path.expandvars(os.path.expanduser(path)))


def acquire_conversion_source_dir():
    """
    Acquires source dir for the asset conversion.

    Returns a file system-like object that holds all the required files.
    """
    # ask for the conversion source
    print("media conversion is required.")

    if 'AGE2DIR' in os.environ:
        sourcedir = os.environ['AGE2DIR']
        print("found environment variable AGE2DIR")
    else:
        # TODO: use some sort of GUI for this (GTK, QtQuick, zenity?)
        proposals = set(proposal for proposal in _get_source_dir_proposals()
                        if os.path.exists(_expand_relative_path(proposal)))
        print("Select an Age of Kings installation directory. "
              "Insert the index of one of the proposals, or any path:")
        proposals = sorted(proposals)
        for index, proposal in enumerate(proposals):
            print("({}) {}".format(index, proposal))

        try:
            user_selection = input("> ")
            if user_selection.isdecimal():
                sourcedir = proposals[int(user_selection)]
            else:
                sourcedir = user_selection
        except KeyboardInterrupt:
            print("Interrupted, aborting")
            exit(0)
        except EOFError:
            print("EOF, aborting")
            exit(0)

    sourcedir = _expand_relative_path(sourcedir)
    print("converting from " + sourcedir)

    return CaseIgnoringDirectory(sourcedir).root


def _get_source_dir_proposals():
    """Yield a list of directory names where an installation might be found"""
    if "WINEPREFIX" in os.environ:
        yield "$WINEPREFIX/" + _STANDARD_PATH_IN_32BIT_WINEPREFIX
        yield "$WINEPREFIX/" + _STANDARD_PATH_IN_64BIT_WINEPREFIX
    yield "~/.wine/" + _STANDARD_PATH_IN_32BIT_WINEPREFIX
    yield "~/.wine/" + _STANDARD_PATH_IN_64BIT_WINEPREFIX
    try:
        # get wine registry key of the age installation
        tmp_reg_file = 'aoe_temp.reg'
        if not subprocess.call(['wine', 'regedit', '/E', tmp_reg_file,
                        _REGISTRY_KEY]) and os.path.exists(tmp_reg_file):
            # strip the REGEDIT4 header, so it becomes a valid INI
            lines = open(tmp_reg_file, 'r').readlines()
            tmp_reg_file_fix = open(tmp_reg_file, 'w')
            for line in lines[2:]:
                tmp_reg_file_fix.write(line)
            tmp_reg_file_fix.close()

            reg_parser = ConfigParser()
            reg_parser.read(tmp_reg_file)
            for suffix in _REGISTRY_SUFFIX_AOK, _REGISTRY_SUFFIX_TC:
                reg_key = _REGISTRY_KEY + suffix
                if reg_key in reg_parser:
                    if '"InstallationDirectory"' in reg_parser[reg_key]:
                        yield reg_parser[reg_key]['"InstallationDirectory"']
                    if '"EXE Path"' in reg_parser[reg_key]:
                        yield reg_parser[reg_key]['"EXE Path"']
            os.remove(tmp_reg_file)
    except OSError as e:
        dbg("wine registry extraction failed: " + str(e))


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
