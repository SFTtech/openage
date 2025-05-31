# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Link building upgrades to building lines.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def link_building_upgrades(full_data_set: GenieObjectContainer) -> None:
    """
    Find building upgrades in the AgeUp techs and append them to the building lines.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    age_ups = full_data_set.age_upgrades

    # Order of age ups should be correct
    for age_up in age_ups.values():
        for effect in age_up.effects.get_effects():
            type_id = effect.get_type()

            if type_id != 3:
                continue

            upgrade_source_id = effect["attr_a"].value
            upgrade_target_id = effect["attr_b"].value

            if upgrade_source_id not in full_data_set.building_lines.keys():
                continue

            upgraded_line = full_data_set.building_lines[upgrade_source_id]
            upgrade_target = full_data_set.genie_units[upgrade_target_id]

            upgraded_line.add_unit(upgrade_target)
            full_data_set.unit_ref.update({upgrade_target_id: upgraded_line})
