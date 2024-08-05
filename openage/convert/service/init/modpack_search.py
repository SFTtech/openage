# Copyright 2023-2024 the openage authors. See copying.md for legal info.

"""
Search for and enumerate openage modpacks.
"""
from __future__ import annotations
import typing

import toml

from ....log import info, dbg

if typing.TYPE_CHECKING:
    from openage.util.fslike.union import UnionPath


def enumerate_modpacks(modpacks_dir: UnionPath, exclude: set[str] = None) -> dict[str, str]:
    """
    Enumerate openage modpacks in a directory.

    :param asset_dir: The asset directory to search in.
    :type asset_dir: UnionPath
    :param exclude: Modpack names to exclude from the enumeration.
    :type exclude: set[str]
    :returns: Modpacks that were found. Names are keys, versions are values.
    :rtype: dict[str, str]
    """
    if not modpacks_dir.exists():
        info("openage modpack directory has not been created yet")
        raise FileNotFoundError("openage modpack directory not found")

    modpacks: dict[str] = {}
    for check_dir in modpacks_dir.iterdir():
        if check_dir.is_dir():
            try:
                modpack_info = get_modpack_info(check_dir)
                modpack_name = modpack_info["info"]["name"]
                modpack_version = modpack_info["info"]["version"]
                info("Found modpack %s v%s", modpack_name, modpack_version)

                if exclude is not None and modpack_name in exclude:
                    dbg("Excluding modpack %s from enumeration", modpack_name)
                    continue

                modpacks.update({modpack_name: modpack_version})

            except (FileNotFoundError, TypeError, toml.TomlDecodeError):
                dbg("No modpack found in directory: %s", check_dir)

    return modpacks


def get_modpack_info(modpack_dir: UnionPath) -> dict[str, typing.Any]:
    """
    Get information about an openage modpack from its definition file.

    :param modpack_dir: Modpack root directory.
    :type modpack_dir: UnionPath
    :returns: Modpack information.
    :rtype: dict[str, typing.Any]

    :raises FileNotFoundError: If the modpack definition file could not be found.
    :raises TypeError: If the modpack definition file could not be parsed.
    :raises toml.TomlDecodeError: If the modpack definition file is malformed.
    """
    if not modpack_dir.exists():
        info("Modpack directory %s not found", modpack_dir.name)
        raise FileNotFoundError("Modpack directory not found")

    modpack_def = modpack_dir / "modpack.toml"
    dbg("Checking modpack definition file %s", modpack_def)
    try:
        with modpack_def.open() as fileobj:
            content = toml.loads(fileobj.read())

        return content

    except FileNotFoundError as err:
        dbg("Modpack definition file not found; could not find %s", modpack_def)
        raise err

    except TypeError as err:
        dbg("Cannot parse modpack definition file %s; content is not a string", modpack_def)
        raise err

    except toml.TomlDecodeError as err:
        dbg("Cannot parse modpack definition file %s; content is not TOML or malformed",
            modpack_def)
        raise err


def query_modpack(proposals: list[str]) -> str:
    """
    Query interactively for a modpack from a selection of proposals.
    """
    print("\nPlease select a modpack before starting.")
    print("Enter the index of one of the proposals (Default = 0):")

    proposals = sorted(proposals)
    for index, proposal in enumerate(proposals):
        print(f"({index}) {proposal}")

    user_selection = input("> ")
    if user_selection == "":
        selection = proposals[0]

    else:
        while not (user_selection.isdecimal() and int(user_selection) < len(proposals)):
            print(f"'{user_selection}' is not a valid index. Please try again.")
            user_selection = input("> ")

        selection = proposals[int(user_selection)]

    return selection
