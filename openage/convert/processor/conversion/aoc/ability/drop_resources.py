# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the DropResources ability.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberSpecialValue

from .....entity_object.conversion.aoc.genie_unit import GenieVillagerGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def drop_resources_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the DropResources ability to a line.

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :returns: The forward reference for the ability.
    :rtype: ...dataformat.forward_ref.ForwardRef
    """
    if isinstance(line, GenieVillagerGroup):
        gatherers = line.variants[0].line

    else:
        gatherers = [line.line[0]]

    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    gather_lookup_dict = internal_name_lookups.get_gather_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.DropResources"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "DropResources",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.DropResources")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Resource containers
    containers = []
    for gatherer in gatherers:
        unit_commands = gatherer["unit_commands"].value

        for command in unit_commands:
            # Find a gather ability. It doesn't matter which one because
            # they should all produce the same resource for one genie unit.
            type_id = command["type"].value

            if type_id in (5, 110):
                break

        gatherer_unit_id = gatherer.get_id()
        if gatherer_unit_id not in gather_lookup_dict:
            # Skips hunting wolves
            continue

        container_ref = (f"{game_entity_name}.ResourceStorage."
                         f"{gather_lookup_dict[gatherer_unit_id][0]}Container")
        container_forward_ref = ForwardRef(line, container_ref)
        containers.append(container_forward_ref)

    ability_raw_api_object.add_raw_member("containers",
                                          containers,
                                          "engine.ability.type.DropResources")

    # Search range
    ability_raw_api_object.add_raw_member("search_range",
                                          MemberSpecialValue.NYAN_INF,
                                          "engine.ability.type.DropResources")

    # Allowed types
    allowed_types = [
        dataset.pregen_nyan_objects["util.game_entity_type.types.DropSite"].get_nyan_object()
    ]
    ability_raw_api_object.add_raw_member("allowed_types",
                                          allowed_types,
                                          "engine.ability.type.DropResources")
    # Blacklisted enties
    ability_raw_api_object.add_raw_member("blacklisted_entities",
                                          [],
                                          "engine.ability.type.DropResources")

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
