# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create unit lines from genie units.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_unit import GenieUnitLineGroup, \
    GenieUnitTransformGroup, GenieMonkGroup

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_unit_lines(full_data_set: GenieObjectContainer) -> None:
    """
    Sort units into lines, based on information in the unit connections.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    unit_connections = full_data_set.unit_connections

    # First only handle the line heads (firstunits in a line)
    for connection in unit_connections.values():
        unit_id = connection["id"].value
        unit = full_data_set.genie_units[unit_id]
        line_mode = connection["line_mode"].value

        if line_mode != 2:
            # It's an upgrade. Skip and handle later
            continue

        # Check for special cases first
        if unit.has_member("transform_unit_id")\
                and unit["transform_unit_id"].value > -1:
            # Trebuchet
            unit_line = GenieUnitTransformGroup(unit_id, unit_id, full_data_set)
            full_data_set.transform_groups.update({unit_line.get_id(): unit_line})

        elif unit_id == 125:
            # Monks
            # Switch to monk with relic is hardcoded :(
            unit_line = GenieMonkGroup(unit_id, unit_id, 286, full_data_set)
            full_data_set.monk_groups.update({unit_line.get_id(): unit_line})

        elif unit.has_member("task_group")\
                and unit["task_group"].value > 0:
            # Villager
            # done somewhere else because they are special^TM
            continue

        else:
            # Normal units
            unit_line = GenieUnitLineGroup(unit_id, full_data_set)

        unit_line.add_unit(unit)
        full_data_set.unit_lines.update({unit_line.get_id(): unit_line})
        full_data_set.unit_ref.update({unit_id: unit_line})

    # Second, handle all upgraded units
    for connection in unit_connections.values():
        unit_id = connection["id"].value
        unit = full_data_set.genie_units[unit_id]
        line_mode = connection["line_mode"].value

        if line_mode != 3:
            # This unit is not an upgrade and was handled in the last for-loop
            continue

        # Search other_connections for the previous unit in line
        connected_types = connection["other_connections"].value
        for index, _ in enumerate(connected_types):
            connected_type = connected_types[index]["other_connection"].value
            if connected_type == 2:
                # 2 == Unit
                connected_index = index
                break

        else:
            raise RuntimeError(f"Unit {unit_id} is not first in line, but no previous "
                               "unit can be found in other_connections")

        connected_ids = connection["other_connected_ids"].value
        previous_unit_id = connected_ids[connected_index].value

        # Search for the first unit ID in the line recursively
        previous_id = previous_unit_id
        previous_connection = unit_connections[previous_unit_id]
        while previous_connection["line_mode"] != 2:
            if previous_id in full_data_set.unit_ref.keys():
                # Short-circuit here, if we the previous unit was already handled
                break

            connected_types = previous_connection["other_connections"].value
            connected_index = -1
            for index, _ in enumerate(connected_types):
                connected_type = connected_types[index]["other_connection"].value
                if connected_type == 2:
                    # 2 == Unit
                    connected_index = index
                    break

            connected_ids = previous_connection["other_connected_ids"].value
            previous_id = connected_ids[connected_index].value
            previous_connection = unit_connections[previous_id]

        unit_line = full_data_set.unit_ref[previous_id]
        unit_line.add_unit(unit, after=previous_unit_id)
        full_data_set.unit_ref.update({unit_id: unit_line})


def create_extra_unit_lines(full_data_set: GenieObjectContainer) -> None:
    """
    Create additional units that are not in the unit connections.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    extra_units = (48, 65, 594, 833)  # Wildlife

    for unit_id in extra_units:
        unit_line = GenieUnitLineGroup(unit_id, full_data_set)
        unit_line.add_unit(full_data_set.genie_units[unit_id])
        full_data_set.unit_lines.update({unit_line.get_id(): unit_line})
        full_data_set.unit_ref.update({unit_id: unit_line})
