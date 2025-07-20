# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Link garrisonable lines to their garrison locations and vice versa.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_unit import GenieGarrisonMode

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
    garrisoned_lines = {}
    garrisoned_lines.update(full_data_set.unit_lines)
    garrisoned_lines.update(full_data_set.ambient_groups)

    garrison_lines = {}
    garrison_lines.update(full_data_set.unit_lines)
    garrison_lines.update(full_data_set.building_lines)

    # Search through all units and look at their garrison commands
    for unit_line in garrisoned_lines.values():
        garrison_classes = []
        garrison_units = []

        if unit_line.has_command(3):
            unit_commands = unit_line.get_head_unit()["unit_commands"].value
            for command in unit_commands:
                type_id = command["type"].value

                if type_id != 3:
                    continue

                class_id = command["class_id"].value
                if class_id > -1:
                    garrison_classes.append(class_id)

                    if class_id == 18:
                        # Towers because LucasArts ALSO didn't like consistent rules
                        garrison_classes.append(10)

                unit_id = command["unit_id"].value
                if unit_id > -1:
                    garrison_units.append(unit_id)

        for garrison_line in garrison_lines.values():
            if not garrison_line.is_garrison():
                continue

            # Natural garrison
            garrison_mode = garrison_line.get_garrison_mode()
            if garrison_mode == GenieGarrisonMode.NATURAL:
                if unit_line.get_head_unit().has_member("creatable_type"):
                    creatable_type = unit_line.get_head_unit()["creatable_type"].value

                else:
                    creatable_type = 0

                if garrison_line.get_head_unit().has_member("garrison_type"):
                    garrison_type = garrison_line.get_head_unit()["garrison_type"].value

                else:
                    garrison_type = 0

                if creatable_type == 1 and not garrison_type & 0x01:
                    continue

                if creatable_type == 2 and not garrison_type & 0x02:
                    continue

                if creatable_type == 3 and not garrison_type & 0x04:
                    continue

                if creatable_type == 6 and not garrison_type & 0x08:
                    continue

                if (creatable_type == 0 and unit_line.get_class_id() == 1) and not\
                        garrison_type & 0x10:
                    # Bantha/Nerf
                    continue

                if garrison_line.get_class_id() in garrison_classes:
                    unit_line.garrison_locations.append(garrison_line)
                    garrison_line.garrison_entities.append(unit_line)
                    continue

                if garrison_line.get_head_unit_id() in garrison_units:
                    unit_line.garrison_locations.append(garrison_line)
                    garrison_line.garrison_entities.append(unit_line)
                    continue

            # Transports/ unit garrisons (no conditions)
            elif garrison_mode in (GenieGarrisonMode.TRANSPORT,
                                   GenieGarrisonMode.UNIT_GARRISON):
                if garrison_line.get_class_id() in garrison_classes:
                    unit_line.garrison_locations.append(garrison_line)
                    garrison_line.garrison_entities.append(unit_line)

            # Self produced units (these cannot be determined from commands)
            elif garrison_mode == GenieGarrisonMode.SELF_PRODUCED:
                if unit_line in garrison_line.creates:
                    unit_line.garrison_locations.append(garrison_line)
                    garrison_line.garrison_entities.append(unit_line)

            # Jedi/Sith inventories
            elif garrison_mode == GenieGarrisonMode.MONK:
                # Search for a pickup command
                unit_commands = garrison_line.get_head_unit()["unit_commands"].value
                for command in unit_commands:
                    type_id = command["type"].value

                    if type_id != 132:
                        continue

                    unit_id = command["unit_id"].value
                    if unit_id == unit_line.get_head_unit_id():
                        unit_line.garrison_locations.append(garrison_line)
                        garrison_line.garrison_entities.append(unit_line)
