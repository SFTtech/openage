# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Link gather objects to entity lines.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def link_gatherers_to_dropsites(full_data_set: GenieObjectContainer) -> None:
    """
    Link gatherers to the buildings they drop resources off. This is done
    to provide quick access during conversion.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    villager_groups = full_data_set.villager_groups

    for villager in villager_groups.values():
        for unit in villager.variants[0].line:
            drop_site_members = unit["drop_sites"].value
            unit_id = unit["id0"].value

            for drop_site_member in drop_site_members:
                drop_site_id = drop_site_member.value

                if drop_site_id > -1:
                    drop_site = full_data_set.building_lines[drop_site_id]
                    drop_site.add_gatherer_id(unit_id)
