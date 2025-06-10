# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Link repairable units/buildings to villager groups.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def link_repairables(full_data_set: GenieObjectContainer) -> None:
    """
    Set units/buildings as repairable

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    """
    villager_groups = full_data_set.villager_groups

    repair_lines = {}
    repair_lines.update(full_data_set.unit_lines)
    repair_lines.update(full_data_set.building_lines)

    repair_classes = []
    for villager in villager_groups.values():
        repair_unit = villager.get_units_with_command(106)[0]
        unit_commands = repair_unit["unit_commands"].value
        for command in unit_commands:
            type_id = command["type"].value

            if type_id != 106:
                continue

            class_id = command["class_id"].value
            if class_id == -1:
                # Buildings/Siege
                repair_classes.append(3)
                repair_classes.append(13)

            else:
                repair_classes.append(class_id)

    for repair_line in repair_lines.values():
        if repair_line.get_class_id() in repair_classes:
            repair_line.repairable = True
