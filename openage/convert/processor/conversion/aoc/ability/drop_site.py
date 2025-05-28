# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the DropResources ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def drop_site_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the DropSite ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    gather_lookup_dict = internal_name_lookups.get_gather_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.DropSite"
    ability_raw_api_object = RawAPIObject(ability_ref, "DropSite", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.DropSite")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Resource containers
    gatherer_ids = line.get_gatherer_ids()

    containers = []
    for gatherer_id in gatherer_ids:
        if gatherer_id not in gather_lookup_dict:
            # Skips hunting wolves
            continue

        gatherer_line = dataset.unit_ref[gatherer_id]
        gatherer_head_unit_id = gatherer_line.get_head_unit_id()
        gatherer_name = name_lookup_dict[gatherer_head_unit_id][0]

        container_ref = (f"{gatherer_name}.ResourceStorage."
                         f"{gather_lookup_dict[gatherer_id][0]}Container")
        container_forward_ref = ForwardRef(gatherer_line, container_ref)
        containers.append(container_forward_ref)

    ability_raw_api_object.add_raw_member("accepts_from",
                                          containers,
                                          "engine.ability.type.DropSite")

    # Diplomacy settings
    property_ref = f"{ability_ref}.Diplomatic"
    property_raw_api_object = RawAPIObject(property_ref,
                                           "Diplomatic",
                                           dataset.nyan_api_objects)
    property_raw_api_object.add_raw_parent("engine.ability.property.type.Diplomatic")
    property_location = ForwardRef(line, ability_ref)
    property_raw_api_object.set_location(property_location)

    line.add_raw_api_object(property_raw_api_object)

    diplomatic_stances = [dataset.nyan_api_objects["engine.util.diplomatic_stance.type.Self"]]
    property_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                           "engine.ability.property.type.Diplomatic")

    property_forward_ref = ForwardRef(line, property_ref)
    properties = {
        api_objects["engine.ability.property.type.Diplomatic"]: property_forward_ref
    }

    ability_raw_api_object.add_raw_member("properties",
                                          properties,
                                          "engine.ability.Ability")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
