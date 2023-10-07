# Copyright 2020-2023 the openage authors. See copying.md for legal info.

"""
Test whether there already are converted modpacks present.
"""
from __future__ import annotations
import typing

from ....log import info

from .modpack_search import enumerate_modpacks

if typing.TYPE_CHECKING:
    from openage.util.fslike.union import UnionPath


def conversion_required(asset_dir: UnionPath) -> bool:
    """
    Check if an asset conversion is required to run the game.

    Asset conversions are required if:
        - the modpack folder does not exist
        - no modpacks inside the modpack folder exist
        - the converted assets are outdated

    :param asset_dir: The asset directory to check.
    :type asset_dir: UnionPath
    :return: True if an asset conversion is required, else False.
    """
    try:
        # TODO: Do not use hardcoded path to "converted" directory.
        #       The mod directory should be its own configurable path
        modpacks = enumerate_modpacks(asset_dir / "converted")

    except FileNotFoundError:
        # modpack folder not created yet
        modpacks = set()

    if not modpacks or \
            len(modpacks) == 1 and "engine" in modpacks:  # engine is not usable on its own
        info("No converted assets have been found")
        return True

    # TODO: Check if modpack version of converted assets are up to date
    # if not changes:
    #     dbg("Converted assets are up to date")
    #     return False

    # if asset_version >= 0 and asset_version != changelog.ASSET_VERSION:
    #     info("Found converted assets with version %d, "
    #          "but need version %d", asset_version, changelog.ASSET_VERSION)

    return False
