# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Asset version change log

used to determine whether assets that were converted by an earlier version of
openage are still up to date.
"""

from ..log import warn
from ..testing.testing import TestError


# filename where to store the versioning information
ASSET_VERSION_FILENAME = "asset_version"

# available components for reconversion
COMPONENTS = {
    "metadata",
    "graphics",
    "sounds",
}

# each line represents changes to the assets.
# the last line is the most recent change.
CHANGES = (
    {"metadata", "graphics", "sounds"},
    {"sounds"},
    {"metadata"},
)

# the current version number equals the number of changes
ASSET_VERSION = len(CHANGES) - 1


def changes(detected_version):
    """
    return all changed components since the passed version number.
    """

    if detected_version >= len(CHANGES):
        warn("asset version from the future: %d" % detected_version)
        warn("current version is: %d" % (ASSET_VERSION))
        warn("leaving assets as they are.")
        return set()

    changed_components = set()

    first_new_version = detected_version + 1

    # fetch all changes since the detected version
    for version_changes in CHANGES[first_new_version:]:
        changed_components |= version_changes

    return changed_components


def test():
    """
    verify only allowed versions are stored in the changes
    """

    for entry in CHANGES:
        if not entry <= COMPONENTS:
            invalid = entry - COMPONENTS
            raise TestError("'{}': invalid changelog entry".format(invalid))
