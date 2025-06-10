# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Link garrisonable lines to their garrison locations and vice versa.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def link_garrison(full_data_set: GenieObjectContainer) -> None:
    """
    Link a garrison unit to the lines that are stored and vice versa. This is done
    to provide quick access during conversion.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    """
    unit_lines = full_data_set.unit_lines

    garrison_class_assignments = {}

    for unit_line in unit_lines.values():
        head_unit = unit_line.get_head_unit()

        unit_commands = head_unit["unit_commands"].value
        for command in unit_commands:
            command_type = command["type"].value

            if not command_type == 3:
                continue

            class_id = command["class_id"].value

            if class_id in garrison_class_assignments:
                garrison_class_assignments[class_id].append(unit_line)

            else:
                garrison_class_assignments[class_id] = [unit_line]

            break

    for garrison in unit_lines.values():
        class_id = garrison.get_class_id()

        if class_id in garrison_class_assignments:
            for line in garrison_class_assignments[class_id]:
                garrison.garrison_entities.append(line)
                line.garrison_locations.append(garrison)
