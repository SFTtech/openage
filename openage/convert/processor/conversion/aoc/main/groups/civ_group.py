# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create civ groups from genie civs.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_civ_groups(full_data_set: GenieObjectContainer) -> None:
    """
    Create civilization groups from civ objects.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    civ_objects = full_data_set.genie_civs

    for index in range(len(civ_objects)):
        civ_id = index

        civ_group = GenieCivilizationGroup(civ_id, full_data_set)
        full_data_set.civ_groups.update({civ_group.get_id(): civ_group})

        index += 1
