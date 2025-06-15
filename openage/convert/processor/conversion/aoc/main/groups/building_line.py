# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create building lines from genie buildings.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup, \
    GenieStackBuildingGroup

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_building_lines(full_data_set: GenieObjectContainer) -> None:
    """
    Establish building lines, based on information in the building connections.
    Because of how Genie building lines work, this will only find the first
    building in the line. Subsequent buildings in the line have to be determined
    by effects in AgeUpTechs.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    building_connections = full_data_set.building_connections

    for connection in building_connections.values():
        building_id = connection["id"].value
        building = full_data_set.genie_units[building_id]
        previous_building_id = None
        stack_building = False

        # Buildings have no actual lines, so we use
        # their unit ID as the line ID.
        line_id = building_id

        # Check if we have to create a GenieStackBuildingGroup
        if building.has_member("stack_unit_id") and \
                building["stack_unit_id"].value > -1:
            stack_building = True

        if building.has_member("head_unit_id") and \
                building["head_unit_id"].value > -1:
            # we don't care about head units because we process
            # them with their stack unit
            continue

        # Check if the building is part of an existing line.
        # To do this, we look for connected techs and
        # check if any tech has an upgrade effect.
        connected_types = connection["other_connections"].value
        connected_tech_indices = []
        for index, _ in enumerate(connected_types):
            connected_type = connected_types[index]["other_connection"].value
            if connected_type == 3:
                # 3 == Tech
                connected_tech_indices.append(index)

        connected_ids = connection["other_connected_ids"].value

        for index in connected_tech_indices:
            connected_tech_id = connected_ids[index].value
            connected_tech = full_data_set.genie_techs[connected_tech_id]
            effect_bundle_id = connected_tech["tech_effect_id"].value
            effect_bundle = full_data_set.genie_effect_bundles[effect_bundle_id]

            upgrade_effects = effect_bundle.get_effects(effect_type=3)

            if len(upgrade_effects) == 0:
                continue

            # Search upgrade effects for the line_id
            for upgrade in upgrade_effects:
                upgrade_source = upgrade["attr_a"].value
                upgrade_target = upgrade["attr_b"].value

                # Check if the upgrade target is correct
                if upgrade_target == building_id:
                    # Line id is the source building id
                    line_id = upgrade_source
                    break

            else:
                # If no upgrade was found, then search remaining techs
                continue

            # Find the previous building
            for c_index, _ in enumerate(connected_types):
                connected_type = connected_types[c_index]["other_connection"].value
                if connected_type == 1:
                    # 1 == Building
                    connected_index = c_index
                    break

            else:
                raise RuntimeError(f"Building {building_id} is not first in line, but no "
                                   "previous building could be found in other_connections")

            previous_building_id = connected_ids[connected_index].value
            break

        if line_id == building_id:
            # First building in line
            if stack_building:
                stack_unit_id = building["stack_unit_id"].value
                building_line = GenieStackBuildingGroup(stack_unit_id, line_id, full_data_set)

            else:
                building_line = GenieBuildingLineGroup(line_id, full_data_set)

            full_data_set.building_lines.update({building_line.get_id(): building_line})
            building_line.add_unit(building, after=previous_building_id)
            full_data_set.unit_ref.update({building_id: building_line})

        else:
            # It's an upgraded building
            building_line = full_data_set.building_lines[line_id]
            building_line.add_unit(building, after=previous_building_id)
            full_data_set.unit_ref.update({building_id: building_line})
