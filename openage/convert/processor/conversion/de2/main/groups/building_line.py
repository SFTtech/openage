# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create building lines from genie buildings.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_extra_building_lines(full_data_set: GenieObjectContainer) -> None:
    """
    Create additional units that are not in the building connections.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    """
    extra_units = (
        1734,  # Folwark
        1808,  # Mule Cart
    )

    for unit_id in extra_units:
        building_line = GenieBuildingLineGroup(unit_id, full_data_set)
        building_line.add_unit(full_data_set.genie_units[unit_id])
        full_data_set.building_lines.update({building_line.get_id(): building_line})
        full_data_set.unit_ref.update({unit_id: building_line})
