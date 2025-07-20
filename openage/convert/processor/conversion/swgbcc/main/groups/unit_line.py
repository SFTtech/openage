# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create unit lines from genie units.
"""

from __future__ import annotations
import typing

from ......entity_object.conversion.swgbcc.genie_unit import SWGBUnitTransformGroup, \
    SWGBMonkGroup, SWGBUnitLineGroup
from ......value_object.conversion.swgb.internal_nyan_names import MONK_GROUP_ASSOCS, \
    CIV_LINE_ASSOCS

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_unit_lines(full_data_set: GenieObjectContainer) -> None:
    """
    Sort units into lines, based on information in the unit connections.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    """
    unit_connections = full_data_set.unit_connections
    unit_lines = {}
    unit_ref = {}

    # First only handle the line heads (first units in a line)
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
            # Cannon
            # SWGB stores the deployed cannon in the connections, but we
            # want the undeployed cannon
            transform_id = unit["transform_unit_id"].value
            unit_line = SWGBUnitTransformGroup(transform_id, transform_id, full_data_set)

        elif unit_id in MONK_GROUP_ASSOCS:
            # Jedi/Sith
            # Switch to monk with relic is hardcoded :(
            # for every civ (WTF LucasArts)
            switch_unit_id = MONK_GROUP_ASSOCS[unit_id]
            unit_line = SWGBMonkGroup(unit_id, unit_id, switch_unit_id, full_data_set)

        elif unit.has_member("task_group")\
                and unit["task_group"].value > 0:
            # Villager
            # done somewhere else because they are special^TM
            continue

        else:
            # Normal units
            unit_line = SWGBUnitLineGroup(unit_id, full_data_set)

        unit_line.add_unit(unit)
        unit_lines.update({unit_line.get_id(): unit_line})
        unit_ref.update({unit_id: unit_line})

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
            raise ValueError(f"Unit {unit_id} is not first in line, but no previous "
                             "unit can be found in other_connections")

        connected_ids = connection["other_connected_ids"].value
        previous_unit_id = connected_ids[connected_index].value

        # Search for the first unit ID in the line recursively
        previous_id = previous_unit_id
        previous_connection = unit_connections[previous_unit_id]
        while previous_connection["line_mode"] != 2:
            if previous_id in unit_ref:
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

        unit_line = unit_ref[previous_id]
        unit_line.add_unit(unit, after=previous_unit_id)

    # Search for civ lines and attach them to their main line
    final_unit_lines = {}
    final_unit_lines.update(unit_lines)
    for line in unit_lines.values():
        if line.get_head_unit_id() not in CIV_LINE_ASSOCS:
            for main_head_unit_id, civ_head_unit_ids in CIV_LINE_ASSOCS.items():
                if line.get_head_unit_id() in civ_head_unit_ids:
                    # The line is an alternative civ line and should be stored
                    # with the main line only.
                    main_line = unit_lines[main_head_unit_id]
                    main_line.add_civ_line(line)

                    # Remove the line from the main reference dict, so that
                    # it doesn't get converted to a game entity
                    final_unit_lines.pop(line.get_id())

                    # Store a reference to the main line in the unit ID refs
                    for unit in line.line:
                        full_data_set.unit_ref[unit.get_id()] = main_line

                    break

            else:
                # Store a reference to the line in the unit ID refs
                for unit in line.line:
                    full_data_set.unit_ref[unit.get_id()] = line

        else:
            # Store a reference to the line in the unit ID refs
            for unit in line.line:
                full_data_set.unit_ref[unit.get_id()] = line

    # Store the remaining lines in the main reference dict
    full_data_set.unit_lines.update(final_unit_lines)


def create_extra_unit_lines(full_data_set: GenieObjectContainer) -> None:
    """
    Create additional units that are not in the unit connections.

    :param full_data_set: GenieObjectContainer instance that
                          contains all relevant data for the conversion
                          process.
    """
    # Wildlife
    extra_units = (48, 594, 822, 833, 1203, 1249, 1363, 1364,
                   1365, 1366, 1367, 1469, 1471, 1473, 1475)

    for unit_id in extra_units:
        unit_line = SWGBUnitLineGroup(unit_id, full_data_set)
        unit_line.add_unit(full_data_set.genie_units[unit_id])
        full_data_set.unit_lines.update({unit_line.get_id(): unit_line})
        full_data_set.unit_ref.update({unit_id: unit_line})
