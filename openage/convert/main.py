# Copyright 2015-2018 the openage authors. See copying.md for legal info.

""" Entry point for all of the asset conversion. """

import os
# importing readline enables the input() calls to have history etc.
import readline  # pylint: disable=unused-import
import subprocess
import sys
from configparser import ConfigParser
from pathlib import Path
from tempfile import NamedTemporaryFile

from . import changelog
from .game_versions import GameVersion, get_game_versions, Support

from ..log import warn, info, dbg
from ..util.files import which
from ..util.fslike.wrapper import (DirectoryCreator,
                                   Synchronizer as AccessSynchronizer)
from ..util.fslike.directory import CaseIgnoringDirectory, Directory
from ..util.strings import format_progress

STANDARD_PATH_IN_32BIT_WINEPREFIX =\
    "drive_c/Program Files/Microsoft Games/Age of Empires II/"
STANDARD_PATH_IN_64BIT_WINEPREFIX =\
    "drive_c/Program Files (x86)/Microsoft Games/Age of Empires II/"
REGISTRY_KEY = \
    "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Microsoft Games\\"
REGISTRY_SUFFIX_AOK = "Age of Empires\\2.0"
REGISTRY_SUFFIX_TC = "Age of Empires II: The Conquerors Expansion\\1.0"


def mount_drs_archives(srcdir, game_versions=None):
    """
    Returns a Union path where srcdir is mounted at /,
    and all the DRS files are mounted in subfolders.
    """
    from ..util.fslike.union import Union
    from .drs import DRS

    result = Union().root
    result.mount(srcdir)

    # hd edition mounting
    if GameVersion.age2_hd_fe in game_versions:
        result['graphics'].mount(srcdir['resources/_common/drs/graphics'])
        result['interface'].mount(srcdir['resources/_common/drs/interface'])
        result['sounds'].mount(srcdir['resources/_common/drs/sounds'])
        result['gamedata'].mount(srcdir['resources/_common/drs/gamedata'])
        if GameVersion.age2_hd_ak in game_versions:
            result['gamedata'].mount(srcdir['resources/_common/drs/gamedata_x1'])
        if GameVersion.age2_hd_rajas in game_versions:
            result['gamedata'].mount(srcdir['resources/_common/drs/gamedata_x2'])
        result['terrain'].mount(srcdir['resources/_common/drs/terrain'])
        result['data'].mount(srcdir['resources/_common/dat'])

        return result

    def mount_drs(filename, target):
        """
        Mounts the DRS file from srcdir's filename at result's target.
        """

        drspath = srcdir[filename]
        result[target].mount(DRS(drspath.open('rb')).root)

    # non-hd file mounting
    mount_drs("data/graphics.drs", "graphics")
    mount_drs("data/interfac.drs", "interface")
    mount_drs("data/sounds.drs", "sounds")
    mount_drs("data/sounds_x1.drs", "sounds")
    mount_drs("data/terrain.drs", "terrain")

    if GameVersion.age2_hd_3x not in game_versions:
        mount_drs("data/gamedata.drs", "gamedata")

    if GameVersion.age2_tc_fe in game_versions:
        mount_drs("games/forgotten empires/data/gamedata_x1.drs",
                  "gamedata")
        mount_drs("games/forgotten empires/data/gamedata_x1_p1.drs",
                  "gamedata")
    else:
        mount_drs("data/gamedata_x1.drs", "gamedata")
        mount_drs("data/gamedata_x1_p1.drs", "gamedata")

    return result


def mount_input(srcdir=None, prev_source_dir_path=None):
    """
    Mount the input folders for conversion.
    """

    # acquire conversion source directory
    if srcdir is None:
        srcdir = acquire_conversion_source_dir(prev_source_dir_path)

    game_versions = set(get_game_versions(srcdir))
    if not game_versions:
        warn("Game version(s) could not be detected in {}".format(srcdir))

    # true if no supported version was found
    no_support = False

    break_vers = []
    for ver in game_versions:
        if ver.support == Support.breaks:
            break_vers.append(ver)

    # a breaking version is installed
    if break_vers:
        warn("You have installed incompatible game version(s):")
        for ver in break_vers:
            warn(" * \x1b[31;1m{}\x1b[m".format(ver))
        no_support = True

    # no supported version was found
    if not any(version.support == Support.yes for version in game_versions):
        warn("No supported game version found:")
        for version in GameVersion:
            warn(" * {}".format(version))
        no_support = True

    # inform about supported versions
    if no_support:
        warn("You need at least one of:")
        for ver in GameVersion:
            if ver.support == Support.yes:
                warn(" * \x1b[34m{}\x1b[m".format(ver))

        return (False, set())

    info("Game version(s) detected:")
    for version in game_versions:
        info(" * {}".format(version))

    output = mount_drs_archives(srcdir, game_versions)

    return (output, game_versions)


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

    data_dir, game_versions = mount_input(srcdir, prev_source_dir_path)

    if not data_dir:
        return None

    # make versions available easily
    args.game_versions = game_versions

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

    return data_dir.resolve_native_path()


def expand_relative_path(path):
    """Expand relative path to an absolute one, including abbreviations like
    ~ and environment variables"""
    return os.path.realpath(os.path.expandvars(os.path.expanduser(path)))


def wanna_use_wine():
    """
    Ask the user if wine should be used.
    Wine is not used if user has no wine installed.
    """

    # TODO: a possibility to call different wine binaries
    # (e.g. wine-devel from wine upstream debian repos)

    if not which("wine"):
        return False

    answer = None
    long_prompt = True
    while answer is None:
        if long_prompt:
            print("  Should we call wine to determine an AOE installation? [Y/n]")
            long_prompt = False
        else:
            # TODO: text-adventure here
            print("  Don't know what you want. Use wine? [Y/n]")

        user_selection = input("> ")
        if user_selection.lower() in {"yes", "y", ""}:
            answer = True

        elif user_selection.lower() in {"no", "n"}:
            answer = False

    return answer


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
        if (Path(new_wineprefix) / "drive_c").is_dir():  # pylint: disable=no-member
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
            break
        else:
            warn("No valid existing directory: {}".format(sourcedir))

    return sourcedir


def acquire_conversion_source_dir(prev_source_dir_path=None):
    """
    Acquires source dir for the asset conversion.

    Returns a file system-like object that holds all the required files.
    """

    if 'AGE2DIR' in os.environ:
        sourcedir = os.environ['AGE2DIR']
        print("found environment variable 'AGE2DIR'")

    else:
        try:
            # TODO: use some sort of GUI for this (GTK, QtQuick, zenity?)
            #       probably best if directly integrated into the main GUI.

            if prev_source_dir_path is not None:
                prev_source_dir = CaseIgnoringDirectory(prev_source_dir_path).root
                proposals = {
                    prev_source_dir.resolve_native_path().decode('utf-8', errors='replace')
                }
            else:
                call_wine = wanna_use_wine()

                if call_wine:
                    set_custom_wineprefix()

                proposals = set(proposal for proposal in
                                source_dir_proposals(call_wine)
                                if Path(expand_relative_path(proposal)).is_dir())

            sourcedir = query_source_dir(proposals)

        except KeyboardInterrupt:
            print("\nInterrupted, aborting")
            sys.exit(0)
        except EOFError:
            print("\nEOF, aborting")
            sys.exit(0)

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

    if not call_wine:
        # user wants wine not to be called
        return

    try:
        info("using the wine registry to query an installation location...")
        # get wine registry key of the age installation
        with NamedTemporaryFile(mode='rb') as reg_file:
            if not subprocess.call(('wine', 'regedit', '/E', reg_file.name,
                                    REGISTRY_KEY)):

                reg_raw_data = reg_file.read()
                try:
                    reg_data = reg_raw_data.decode('utf-16')
                except UnicodeDecodeError:
                    # this is hopefully enough.
                    # if it isn't, feel free to fight more encoding problems.
                    reg_data = reg_raw_data.decode('utf-8', errors='replace')

                # strip the REGEDIT4 header, so it becomes a valid INI
                lines = reg_data.splitlines()
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
        dbg("wine registry extraction failed: %s" % error)


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

    # TODO: datapack parsing
    changes = changelog.changes(asset_version, spec_version)

    if not changes:
        dbg("Converted assets are up to date")
        return False

    else:
        if asset_version >= 0 and asset_version != changelog.ASSET_VERSION:
            info("Found converted assets with version %d, "
                 "but need version %d" % (asset_version,
                                          changelog.ASSET_VERSION))

        info("Converting {}".format(", ".join(sorted(changes))))

        # try to resolve resolve the output path
        target_path = asset_dir.resolve_native_path_w()
        if not target_path:
            raise OSError("could not resolve a writable asset path "
                          "in {}".format(asset_dir))

        info("Will save to '{}'".format(target_path.decode(errors="replace")))

        for component in changelog.COMPONENTS:
            if component not in changes:
                # don't reconvert this component:
                setattr(args, "no_{}".format(component), True)

        if "metadata" in changes:
            args.no_pickle_cache = True

        return True


def interactive_browser(srcdir=None):
    """
    launch an interactive view for browsing the original
    archives.
    """

    info("launching interactive data browser...")

    # the variables are actually used, in the interactive prompt.
    # pylint: disable=unused-variable
    data, game_versions = mount_input(srcdir)

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
        from .slp import SLP
        from .driver import get_palette

        if not palette:
            palette = get_palette(data)

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
