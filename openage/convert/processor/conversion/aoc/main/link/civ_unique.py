# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Link unique lines, techs, etc. to civ groups.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def link_civ_uniques(full_data_set: GenieObjectContainer) -> None:
    """
    Link civ bonus techs, unique units and unique techs to their civs.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    for bonus in full_data_set.civ_boni.values():
        civ_id = bonus.get_civilization()
        full_data_set.civ_groups[civ_id].add_civ_bonus(bonus)

    for unit_line in full_data_set.unit_lines.values():
        if unit_line.is_unique():
            head_unit_id = unit_line.get_head_unit_id()
            head_unit_connection = full_data_set.unit_connections[head_unit_id]
            enabling_research_id = head_unit_connection["enabling_research"].value
            enabling_research = full_data_set.genie_techs[enabling_research_id]
            enabling_civ_id = enabling_research["civilization_id"].value

            full_data_set.civ_groups[enabling_civ_id].add_unique_entity(unit_line)

    for building_line in full_data_set.building_lines.values():
        if building_line.is_unique():
            head_unit_id = building_line.get_head_unit_id()
            head_building_connection = full_data_set.building_connections[head_unit_id]
            enabling_research_id = head_building_connection["enabling_research"].value
            enabling_research = full_data_set.genie_techs[enabling_research_id]
            enabling_civ_id = enabling_research["civilization_id"].value

            full_data_set.civ_groups[enabling_civ_id].add_unique_entity(building_line)

    for tech_group in full_data_set.tech_groups.values():
        if tech_group.is_unique() and tech_group.is_researchable():
            civ_id = tech_group.get_civilization()
            full_data_set.civ_groups[civ_id].add_unique_tech(tech_group)
