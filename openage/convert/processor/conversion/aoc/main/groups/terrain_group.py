# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create terrain groups from genie terrains.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_terrain import GenieTerrainGroup

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_terrain_groups(full_data_set: GenieObjectContainer) -> None:
    """
    Create terrain groups.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    terrains = full_data_set.genie_terrains.values()

    for terrain in terrains:
        slp_id = terrain["slp_id"].value
        replacement_id = terrain["terrain_replacement_id"].value

        if slp_id == -1 and replacement_id == -1:
            # No graphics and no graphics replacement means this terrain is unused
            continue

        enabled = terrain["enabled"].value

        if enabled:
            terrain_group = GenieTerrainGroup(terrain.get_id(), full_data_set)
            full_data_set.terrain_groups.update({terrain.get_id(): terrain_group})
