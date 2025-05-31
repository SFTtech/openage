# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Link creatable unit lines to building lines they can be created in.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def link_creatables(full_data_set: GenieObjectContainer) -> None:
    """
    Link creatable units and buildings to their creating entity. This is done
    to provide quick access during conversion.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    # Link units to buildings
    unit_lines = full_data_set.unit_lines

    for unit_line in unit_lines.values():
        if unit_line.is_creatable():
            train_location_id = unit_line.get_train_location_id()
            full_data_set.building_lines[train_location_id].add_creatable(unit_line)

    # Link buildings to villagers and fishing ships
    building_lines = full_data_set.building_lines

    for building_line in building_lines.values():
        if building_line.is_creatable():
            train_location_id = building_line.get_train_location_id()

            if train_location_id in full_data_set.villager_groups.keys():
                full_data_set.villager_groups[train_location_id].add_creatable(building_line)

            else:
                # try normal units
                full_data_set.unit_lines[train_location_id].add_creatable(building_line)
