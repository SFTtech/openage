# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Acquire the sourcedir for the game that is supposed to be converted.
"""
from configparser import ConfigParser
import os
from pathlib import Path
import subprocess
import sys
from tempfile import NamedTemporaryFile

from ....log import warn, info, dbg
from ....util.files import which
from ....util.fslike.directory import CaseIgnoringDirectory

STANDARD_PATH_IN_32BIT_WINEPREFIX =\
    "drive_c/Program Files/Microsoft Games/Age of Empires II/"
STANDARD_PATH_IN_64BIT_WINEPREFIX =\
    "drive_c/Program Files (x86)/Microsoft Games/Age of Empires II/"
STANDARD_PATH_IN_WINEPREFIX_STEAM = \
    "drive_c/Program Files (x86)/Steam/steamapps/common/Age2HD/"
REGISTRY_KEY = \
    "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Microsoft Games\\"
REGISTRY_SUFFIX_AOK = "Age of Empires\\2.0"
REGISTRY_SUFFIX_TC = "Age of Empires II: The Conquerors Expansion\\1.0"


def expand_relative_path(path):
    """Expand relative path to an absolute one, including abbreviations like
    ~ and environment variables"""
    return os.path.realpath(os.path.expandvars(os.path.expanduser(path)))


def wanna_convert():
    """
    Ask the user if assets should be converted.
    """
    answer = None
    while answer is None:
        print("  Do you want to convert assets? [Y/n]")

        user_selection = input("> ")
        if user_selection.lower() in {"yes", "y", ""}:
            answer = True

        elif user_selection.lower() in {"no", "n"}:
            answer = False

    return answer


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
        print(f"Currently: WINEPREFIX='{current_wineprefix}'")

    print("Enter a custom value or leave empty to keep it as-is:")
    while True:
        new_wineprefix = input("WINEPREFIX=")

        if not new_wineprefix:
            break

        new_wineprefix = expand_relative_path(new_wineprefix)

        # test if it probably is a wineprefix
        if (Path(new_wineprefix) / "drive_c").is_dir():  # pylint: disable=no-member
            break

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
            print(f"({index}) {proposal}")

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
        warn("No valid existing directory: %s", sourcedir)

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

            proposals = set()

            if prev_source_dir_path and Path(prev_source_dir_path).is_dir():
                prev_source_dir = CaseIgnoringDirectory(prev_source_dir_path).root
                proposals.add(
                    prev_source_dir.resolve_native_path().decode('utf-8', errors='replace')
                )

            call_wine = wanna_use_wine()

            if call_wine:
                set_custom_wineprefix()

            for proposal in source_dir_proposals(call_wine):
                if Path(expand_relative_path(proposal)).is_dir():
                    proposals.add(proposal)

            sourcedir = query_source_dir(proposals)

        except KeyboardInterrupt:
            print("\nInterrupted, aborting")
            sys.exit(0)
        except EOFError:
            print("\nEOF, aborting")
            sys.exit(0)

    print(f"converting from '{sourcedir}'")

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
        yield "$WINEPREFIX/" + STANDARD_PATH_IN_WINEPREFIX_STEAM
    yield "~/.wine/" + STANDARD_PATH_IN_32BIT_WINEPREFIX
    yield "~/.wine/" + STANDARD_PATH_IN_64BIT_WINEPREFIX
    yield "~/.wine/" + STANDARD_PATH_IN_WINEPREFIX_STEAM
    yield "~/.steam/steam/steamapps/common/Age2HD"

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
        dbg("wine registry extraction failed: %s", error)
