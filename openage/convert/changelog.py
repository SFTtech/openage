# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""
Asset version change log

used to determine whether assets that were converted by an earlier version of
openage are still up to date.
"""

from .gamedata.empiresdat import EmpiresDat

from ..log import warn
from ..testing.testing import TestError


# filename where to store the versioning information
ASSET_VERSION_FILENAME = "asset_version"

# filename where to store the gamespec version hash
GAMESPEC_VERSION_FILENAME = "gamespec_version"

# available components for reconversion
COMPONENTS = {
    "graphics",
    "sounds",
    "metadata",
    "interface",
}

# each line represents changes to the assets.
# the last line is the most recent change.
CHANGES = (
    {"graphics", "sounds"},
    {"sounds"},
    {"graphics"},
    {"interface"},
    {"interface"},
)

# the current version number equals the number of changes
ASSET_VERSION = len(CHANGES) - 1


def changes(asset_version, spec_version):
    """
    return all changed components since the passed version number.
    """

    if asset_version >= len(CHANGES):
        warn("asset version from the future: %d" % asset_version)
        warn("current version is: %d" % (ASSET_VERSION))
        warn("leaving assets as they are.")
        return set()

    changed_components = set()

    first_new_version = asset_version + 1

    # fetch all changes since the detected version
    for version_changes in CHANGES[first_new_version:]:
        changed_components |= version_changes

    if "metadata" not in changed_components:
        if EmpiresDat.get_hash() != spec_version:
            changed_components.add("metadata")

    return changed_components


def test():
    """
    verify only allowed versions are stored in the changes
    """

    for entry in CHANGES:
        if entry > COMPONENTS:
            invalid = entry - COMPONENTS
            raise TestError("'{}': invalid changelog entry".format(invalid))
