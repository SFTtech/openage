# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create unit/building lines from genie buildings.
"""


from __future__ import annotations
import typing

from ......entity_object.conversion.ror.genie_unit import RoRUnitTaskGroup, \
    RoRUnitLineGroup, RoRBuildingLineGroup, RoRVillagerGroup

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer
    from ......value_object.read.value_members import ArrayMember


def create_entity_lines(gamespec: ArrayMember, full_data_set: GenieObjectContainer) -> None:
    """
    Sort units/buildings into lines, based on information from techs and civs.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    """
    # Search a player civ (egyptians) for the starting units
    player_civ_units = gamespec[0]["civs"][1]["units"].value
    task_group_ids = set()
    villager_unit_ids = set()

    for raw_unit in player_civ_units.values():
        unit_id = raw_unit["id0"].value
        enabled = raw_unit["enabled"].value
        entity = full_data_set.genie_units[unit_id]

        if not enabled:
            # Unlocked by tech
            continue

        unit_type = entity["unit_type"].value

        if unit_type == 70:
            if entity.has_member("task_group") and\
                    entity["task_group"].value > 0:
                task_group_id = entity["task_group"].value
                villager_unit_ids.add(unit_id)

                if task_group_id in task_group_ids:
                    task_group = full_data_set.task_groups[task_group_id]
                    task_group.add_unit(entity)

                else:
                    if task_group_id in RoRUnitTaskGroup.line_id_assignments:
                        line_id = RoRUnitTaskGroup.line_id_assignments[task_group_id]

                    task_group = RoRUnitTaskGroup(line_id, task_group_id, -1, full_data_set)
                    task_group.add_unit(entity)
                    task_group_ids.add(task_group_id)
                    full_data_set.task_groups.update({task_group_id: task_group})

            else:
                unit_line = RoRUnitLineGroup(unit_id, -1, full_data_set)
                unit_line.add_unit(entity)
                full_data_set.unit_lines.update({unit_line.get_id(): unit_line})
                full_data_set.unit_ref.update({unit_id: unit_line})

        elif unit_type == 80:
            building_line = RoRBuildingLineGroup(unit_id, -1, full_data_set)
            building_line.add_unit(entity)
            full_data_set.building_lines.update({building_line.get_id(): building_line})
            full_data_set.unit_ref.update({unit_id: building_line})

    # Create the villager task group
    villager = RoRVillagerGroup(118, (1,), full_data_set)
    full_data_set.unit_lines.update({villager.get_id(): villager})
    full_data_set.villager_groups.update({villager.get_id(): villager})
    for unit_id in villager_unit_ids:
        full_data_set.unit_ref.update({unit_id: villager})

    # Other units unlocks through techs
    unit_unlocks = full_data_set.unit_unlocks
    for unit_unlock in unit_unlocks.values():
        line_id = unit_unlock.get_line_id()
        unit = full_data_set.genie_units[line_id]

        unit_line = RoRUnitLineGroup(line_id, unit_unlock.get_id(), full_data_set)
        unit_line.add_unit(unit)
        full_data_set.unit_lines.update({unit_line.get_id(): unit_line})
        full_data_set.unit_ref.update({line_id: unit_line})

        # Check if the tech unlocks other lines
        # TODO: Make this cleaner
        unlock_effects = unit_unlock.get_effects(effect_type=2)
        for unlock_effect in unlock_effects:
            line_id = unlock_effect["attr_a"].value
            unit = full_data_set.genie_units[line_id]

            if line_id not in full_data_set.unit_lines.keys():
                unit_line = RoRUnitLineGroup(line_id, unit_unlock.get_id(), full_data_set)
                unit_line.add_unit(unit)
                full_data_set.unit_lines.update({unit_line.get_id(): unit_line})
                full_data_set.unit_ref.update({line_id: unit_line})

    # Upgraded units in a line
    unit_upgrades = full_data_set.unit_upgrades
    for unit_upgrade in unit_upgrades.values():
        line_id = unit_upgrade.get_line_id()
        target_id = unit_upgrade.get_upgrade_target_id()
        unit = full_data_set.genie_units[target_id]

        # Find the previous unit in the line
        required_techs = unit_upgrade.tech["required_techs"].value
        for required_tech in required_techs:
            required_tech_id = required_tech.value
            if required_tech_id in full_data_set.unit_unlocks.keys():
                source_id = full_data_set.unit_unlocks[required_tech_id].get_line_id()
                break

            if required_tech_id in full_data_set.unit_upgrades.keys():
                source_id = full_data_set.unit_upgrades[required_tech_id].get_upgrade_target_id(
                )
                break

        unit_line = full_data_set.unit_lines[line_id]
        unit_line.add_unit(unit, after=source_id)
        full_data_set.unit_ref.update({target_id: unit_line})

    # Other buildings unlocks through techs
    building_unlocks = full_data_set.building_unlocks
    for building_unlock in building_unlocks.values():
        line_id = building_unlock.get_line_id()
        building = full_data_set.genie_units[line_id]

        building_line = RoRBuildingLineGroup(line_id, building_unlock.get_id(), full_data_set)
        building_line.add_unit(building)
        full_data_set.building_lines.update({building_line.get_id(): building_line})
        full_data_set.unit_ref.update({line_id: building_line})

    # Upgraded buildings through techs
    building_upgrades = full_data_set.building_upgrades
    for building_upgrade in building_upgrades.values():
        line_id = building_upgrade.get_line_id()
        target_id = building_upgrade.get_upgrade_target_id()
        unit = full_data_set.genie_units[target_id]

        # Find the previous unit in the line
        required_techs = building_upgrade.tech["required_techs"].value
        for required_tech in required_techs:
            required_tech_id = required_tech.value
            if required_tech_id in full_data_set.building_unlocks.keys():
                source_id = full_data_set.building_unlocks[required_tech_id].get_line_id()
                break

            if required_tech_id in full_data_set.building_upgrades.keys():
                source_id = full_data_set.building_upgrades[required_tech_id].get_upgrade_target_id(
                )
                break

        building_line = full_data_set.building_lines[line_id]
        building_line.add_unit(unit, after=source_id)
        full_data_set.unit_ref.update({target_id: building_line})

    # Upgraded units/buildings through age ups
    age_ups = full_data_set.age_upgrades
    for age_up in age_ups.values():
        effects = age_up.get_effects(effect_type=3)
        for effect in effects:
            source_id = effect["attr_a"].value
            target_id = effect["attr_b"].value
            unit = full_data_set.genie_units[target_id]

            if source_id in full_data_set.building_lines.keys():
                building_line = full_data_set.building_lines[source_id]
                building_line.add_unit(unit, after=source_id)
                full_data_set.unit_ref.update({target_id: building_line})

            elif source_id in full_data_set.unit_lines.keys():
                unit_line = full_data_set.unit_lines[source_id]
                unit_line.add_unit(unit, after=source_id)
                full_data_set.unit_ref.update({target_id: unit_line})
