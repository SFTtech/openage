# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Link researchable techs to building lines they can be researched in.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def link_researchables(full_data_set: GenieObjectContainer) -> None:
    """
    Link techs to their buildings. This is done
    to provide quick access during conversion.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    tech_groups = full_data_set.tech_groups

    for tech in tech_groups.values():
        if tech.is_researchable():
            research_location_id = tech.get_research_location_id()
            full_data_set.building_lines[research_location_id].add_researchable(tech)
