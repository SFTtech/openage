# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create building lines from genie buildings.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_tech import BuildingUnlock
from ......entity_object.conversion.aoc.genie_tech import BuildingLineUpgrade
from ......entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from ......entity_object.conversion.swgbcc.genie_unit import SWGBStackBuildingGroup

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
    """
    building_connections = full_data_set.building_connections
    genie_techs = full_data_set.genie_techs

    # Unlocked = first in line
    unlocked_by_tech = set()

    # Upgraded = later in line
    upgraded_by_tech = {}

    # Search all techs for building upgrades. This is necessary because they are
    # not stored in tech connections in SWGB
    for tech_id, tech in genie_techs.items():
        tech_effect_id = tech["tech_effect_id"].value
        if tech_effect_id < 0:
            continue

        tech_effects = full_data_set.genie_effect_bundles[tech_effect_id].get_effects()

        # Search for upgrade or unlock effects
        age_up = False
        for effect in tech_effects:
            effect_type = effect["type_id"].value
            unit_id_a = effect["attr_a"].value
            unit_id_b = effect["attr_b"].value

            if effect_type == 1 and effect["attr_a"].value == 6:
                # if this is an age up tech, we do not need to create any additional
                # unlock techs
                age_up = True

            if effect_type == 2 and unit_id_a in building_connections.keys():
                # Unlocks
                unlocked_by_tech.add(unit_id_a)

                if not age_up:
                    # Add an unlock tech group to the data set
                    building_unlock = BuildingUnlock(tech_id, unit_id_a, full_data_set)
                    full_data_set.building_unlocks.update(
                        {building_unlock.get_id(): building_unlock})

            elif effect_type == 2 and unit_id_a in full_data_set.genie_units.keys():
                # Check if this is a stacked unit (gate or command center)
                # for these units, we needs the stack_unit_id
                building = full_data_set.genie_units[unit_id_a]

                if building.has_member("stack_unit_id") and \
                        building["stack_unit_id"].value > -1:
                    unit_id_a = building["stack_unit_id"].value
                    unlocked_by_tech.add(unit_id_a)

                    if not age_up:
                        building_unlock = BuildingUnlock(tech_id, unit_id_a, full_data_set)
                        full_data_set.building_unlocks.update(
                            {building_unlock.get_id(): building_unlock})

            if effect_type == 3 and unit_id_b in building_connections.keys():
                # Upgrades
                upgraded_by_tech[unit_id_b] = tech_id

    # First only handle the line heads (first buildings in a line)
    for connection in building_connections.values():
        building_id = connection["id"].value

        if building_id not in unlocked_by_tech:
            continue

        building = full_data_set.genie_units[building_id]

        # Check if we have to create a GenieStackBuildingGroup
        if building.has_member("stack_unit_id") and \
                building["stack_unit_id"].value > -1:
            # we don't care about head units because we process
            # them with their stack unit
            continue

        if building.has_member("head_unit_id") and \
                building["head_unit_id"].value > -1:
            head_unit_id = building["head_unit_id"].value
            building_line = SWGBStackBuildingGroup(building_id, head_unit_id, full_data_set)
            full_data_set.stack_building_groups.update({building_id: building_line})

        else:
            building_line = GenieBuildingLineGroup(building_id, full_data_set)

        building_line.add_unit(building)
        full_data_set.building_lines.update({building_line.get_id(): building_line})
        full_data_set.unit_ref.update({building_id: building_line})

    # Second, handle all upgraded buildings
    for connection in building_connections.values():
        building_id = connection["id"].value

        if building_id not in upgraded_by_tech:
            continue

        building = full_data_set.genie_units[building_id]

        # Search other_connections for the previous unit in line
        connected_types = connection["other_connections"].value
        for index, _ in enumerate(connected_types):
            connected_type = connected_types[index]["other_connection"].value
            if connected_type == 1:
                # 1 == Building
                connected_index = index
                break

        else:
            raise ValueError(f"Building {building_id} is not first in line, but no previous "
                             "building can be found in other_connections")

        connected_ids = connection["other_connected_ids"].value
        previous_unit_id = connected_ids[connected_index].value

        # Search for the first unit ID in the line recursively
        previous_id = previous_unit_id
        previous_connection = building_connections[previous_unit_id]
        while previous_connection["line_mode"] != 2:
            if previous_id in full_data_set.unit_ref.keys():
                # Short-circuit here, if we the previous unit was already handled
                break

            connected_types = previous_connection["other_connections"].value
            connected_index = -1
            for index, _ in enumerate(connected_types):
                connected_type = connected_types[index]["other_connection"].value
                if connected_type == 1:
                    # 1 == Building
                    connected_index = index
                    break

            connected_ids = previous_connection["other_connected_ids"].value
            previous_id = connected_ids[connected_index].value
            previous_connection = building_connections[previous_id]

        building_line = full_data_set.unit_ref[previous_id]
        building_line.add_unit(building, after=previous_unit_id)
        full_data_set.unit_ref.update({building_id: building_line})

        # Also add the building upgrade tech here
        building_upgrade = BuildingLineUpgrade(
            upgraded_by_tech[building_id],
            building_line.get_id(),
            building_id,
            full_data_set
        )
        full_data_set.tech_groups.update({building_upgrade.get_id(): building_upgrade})
        full_data_set.building_upgrades.update({building_upgrade.get_id(): building_upgrade})
