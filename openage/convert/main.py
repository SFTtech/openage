# Copyright 2015-2016 the openage authors. See copying.md for legal info.

""" Entry point for all of the asset conversion. """

import os
# importing readline enables the raw_input calls to have history etc.
import readline  # pylint: disable=unused-import
import subprocess
from configparser import ConfigParser
from pathlib import Path
from tempfile import NamedTemporaryFile

from . import changelog
from .game_versions import GameVersion, get_game_versions

from ..log import warn, info, dbg
from ..util.files import which
from ..util.fslike.wrapper import (Wrapper as FSLikeObjWrapper,
                                   Synchronizer as FSLikeObjSynchronizer)
from ..util.fslike.directory import CaseIgnoringDirectory
from ..util.strings import format_progress

STANDARD_PATH_IN_32BIT_WINEPREFIX =\
    "drive_c/Program Files/Microsoft Games/Age of Empires II/"
STANDARD_PATH_IN_64BIT_WINEPREFIX =\
    "drive_c/Program Files (x86)/Microsoft Games/Age of Empires II/"
REGISTRY_KEY = \
    "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Microsoft Games\\"
REGISTRY_SUFFIX_AOK = "Age of Empires\\2.0"
REGISTRY_SUFFIX_TC = "Age of Empires II: The Conquerors Expansion\\1.0"


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
#        mount_drs("data/gamedata_x1_p1.drs", "gamedata")
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

    # import here so codegen.py doesn't depend on it.
    from .driver import convert

    # acquire conversion source directory
    if srcdir is None:
        srcdir = acquire_conversion_source_dir()

    args.game_versions = set(get_game_versions(srcdir))
    if not args.game_versions:
        warn("Game version(s) could not be detected in {}".format(srcdir))

    versions = "; ".join(str(version) for version in args.game_versions)
    if not any(version.openage_supported for version in args.game_versions):
        warn("None supported of the Game version(s) {}".format(versions))
        warn("You need \x1b[34mAge of Empires 2: The Conquerors\x1b[m")
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


def expand_relative_path(path):
    """Expand relative path to an absolute one, including abbreviations like
    ~ and environment variables"""
    return os.path.realpath(os.path.expandvars(os.path.expanduser(path)))


def wanna_use_wine():
    """
    Ask the user if wine should be used.
    """
    print("Should we call wine to determine an AOE installation? "
          "[Y/n]")
    while True:
        user_selection = input("> ")
        if user_selection.lower() in {"yes", "y", ""}:
            return True
        elif user_selection.lower() in {"no", "n"}:
            return False
        else:
            print("Don't know what you want. Use wine? [Y/n]")


def set_custom_wineprefix():
    """
    Allow the customization of the WINEPREFIX environment variable.
    """

    print("The WINEPREFIX is a separate 'container' for windows "
          "software installations.")

    current_wineprefix = os.environ.get("WINEPREFIX")
    if current_wineprefix:
        print("Currently: WINEPREFIX='%s'" % current_wineprefix)

    print("Enter a custom value or leave empty to keep it as-is:")
    while True:
        new_wineprefix = input("WINEPREFIX=")

        if not new_wineprefix:
            break

        new_wineprefix = expand_relative_path(new_wineprefix)

        # test if it probably is a wineprefix
        if (Path(new_wineprefix) / "drive_c").is_dir():
            break
        else:
            print("This does not appear to be a valid WINEPREFIX.")
            print("Enter a valid one, or leave it empty to skip.")

    # store the updated env variable for the wine subprocess
    if new_wineprefix:
        os.environ["WINEPREFIX"] = new_wineprefix


def query_source_dir(proposals):
    """
    Query interactively for a conversion source directory.
    Lists proposals and allows selection if some were found.
    """

    if proposals:
        print("\nPlease select an Age of Kings installation directory.")
        print("Insert the index of one of the proposals, or any path:")

        proposals = sorted(proposals)
        for index, proposal in enumerate(proposals):
            print("({}) {}".format(index, proposal))

    else:
        print("Could not find any installation directory "
              "automatically.")
        print("Please enter an AOE2 install path manually.")

    while True:
        user_selection = input("> ")
        if user_selection.isdecimal() and int(user_selection) < len(proposals):
            sourcedir = proposals[int(user_selection)]
        else:
            sourcedir = user_selection
        sourcedir = expand_relative_path(sourcedir)
        if Path(sourcedir).is_dir():
            return sourcedir
        else:
            warn("No valid existing directory: {}".format(sourcedir))


def acquire_conversion_source_dir():
    """
    Acquires source dir for the asset conversion.

    Returns a file system-like object that holds all the required files.
    """
    # ask for the conversion source
    print("\x1b[33mmedia conversion is required.\x1b[m")

    if 'AGE2DIR' in os.environ:
        sourcedir = os.environ['AGE2DIR']
        print("found environment variable 'AGE2DIR'")

    else:
        try:
            # TODO: use some sort of GUI for this (GTK, QtQuick, zenity?)
            #       probably best if directly integrated into the main GUI.

            call_wine = wanna_use_wine()

            if call_wine:
                set_custom_wineprefix()

            proposals = set(proposal for proposal in
                            source_dir_proposals(call_wine)
                            if Path(expand_relative_path(proposal)).is_dir())

            sourcedir = query_source_dir(proposals)

        except KeyboardInterrupt:
            print("\nInterrupted, aborting")
            exit(0)
        except EOFError:
            print("\nEOF, aborting")
            exit(0)

    print("converting from '%s'" % sourcedir)

    return CaseIgnoringDirectory(sourcedir).root


def wine_to_real_path(path):
    """
    Turn a Wine file path (C:\\xyz) into a local filesystem path (~/.wine/xyz)
    """
    return subprocess.check_output(('winepath', path)).strip().decode()


def unescape_winereg(value):
    """Remove quotes and escapes from a Wine registry value"""
    return value.strip('"').replace(r'\\\\', '\\')


def source_dir_proposals(call_wine):
    """Yield a list of directory names where an installation might be found"""
    if "WINEPREFIX" in os.environ:
        yield "$WINEPREFIX/" + STANDARD_PATH_IN_32BIT_WINEPREFIX
        yield "$WINEPREFIX/" + STANDARD_PATH_IN_64BIT_WINEPREFIX
    yield "~/.wine/" + STANDARD_PATH_IN_32BIT_WINEPREFIX
    yield "~/.wine/" + STANDARD_PATH_IN_64BIT_WINEPREFIX

    # TODO: a possibility to call different wine binaries
    # (e.g. wine-devel from wine upstream debian repos)
    if not call_wine or not which("wine"):
        # no wine is found in PATH
        return

    try:
        info("using the wine registry to query an installation location...")
        # get wine registry key of the age installation
        with NamedTemporaryFile(mode='r') as reg_file:
            if not subprocess.call(('wine', 'regedit', '/E', reg_file.name,
                                    REGISTRY_KEY)):
                # strip the REGEDIT4 header, so it becomes a valid INI
                lines = reg_file.readlines()
                del lines[0:2]

                reg_parser = ConfigParser()
                reg_parser.read_string(''.join(lines))
                for suffix in REGISTRY_SUFFIX_AOK, REGISTRY_SUFFIX_TC:
                    reg_key = REGISTRY_KEY + suffix
                    if reg_key in reg_parser:
                        if '"InstallationDirectory"' in reg_parser[reg_key]:
                            yield wine_to_real_path(unescape_winereg(
                                reg_parser[reg_key]['"InstallationDirectory"']))
                        if '"EXE Path"' in reg_parser[reg_key]:
                            yield wine_to_real_path(unescape_winereg(
                                reg_parser[reg_key]['"EXE Path"']))

    except OSError as error:
        dbg("wine registry extraction failed: " + str(error))


def conversion_required(asset_dir, args):
    """
    Returns true if an asset conversion is required to run the game.

    Sets options in args according to what sorts of conversion are required.
    """

    version_path = asset_dir['converted',
                             changelog.ASSET_VERSION_FILENAME]

    spec_path = asset_dir['converted',
                          changelog.GAMESPEC_VERSION_FILENAME]

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
        spec_version = "lol"

    # TODO: datapack parsing
    changes = changelog.changes(asset_version, spec_version)

    if not changes:
        dbg("Converted assets are up to date")
        return False

    else:
        if asset_version >= 0:
            info("Converted assets outdated: Version %d" % asset_version)

        info("Converting " + ", ".join(sorted(changes)))
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
        help="do not convert game graphics")

    cli.add_argument(
        "--no-interface", action='store_true',
        help="do not convert interface graphics")

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
