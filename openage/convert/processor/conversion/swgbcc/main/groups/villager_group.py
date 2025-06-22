# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create villager groups from genie units.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_unit import GenieUnitTaskGroup, GenieVillagerGroup

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_villager_groups(full_data_set: GenieObjectContainer) -> None:
    """
    Create task groups and assign the relevant worker group to a
    villager group.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    """
    units = full_data_set.genie_units
    task_group_ids = set()
    unit_ids = set()

    # Find task groups in the dataset
    for unit in units.values():
        if unit.has_member("task_group"):
            task_group_id = unit["task_group"].value

        else:
            task_group_id = 0

        if task_group_id == 0:
            # no task group
            continue

        if task_group_id in task_group_ids:
            task_group = full_data_set.task_groups[task_group_id]
            task_group.add_unit(unit)

        else:
            if task_group_id in GenieUnitTaskGroup.line_id_assignments:
                # SWGB uses the same IDs as AoC for lines
                line_id = GenieUnitTaskGroup.line_id_assignments[task_group_id]

            else:
                raise ValueError(
                    f"Unknown task group ID {task_group_id} for unit {unit['id0'].value}"
                )

            task_group = GenieUnitTaskGroup(line_id, task_group_id, full_data_set)
            task_group.add_unit(unit)
            full_data_set.task_groups.update({task_group_id: task_group})

        task_group_ids.add(task_group_id)
        unit_ids.add(unit["id0"].value)

    # Create the villager task group
    villager = GenieVillagerGroup(118, (1,), full_data_set)
    full_data_set.unit_lines.update({villager.get_id(): villager})
    # TODO: Find the line id elsewhere
    full_data_set.unit_lines_vertical_ref.update({36: villager})
    full_data_set.villager_groups.update({villager.get_id(): villager})
    for unit_id in unit_ids:
        full_data_set.unit_ref.update({unit_id: villager})
