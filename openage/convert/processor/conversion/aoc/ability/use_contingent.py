# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the UseContingent ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def use_contingent_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the UseContingent ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    game_entity_name = name_lookup_dict[current_unit_id][0]
    ability_ref = f"{game_entity_name}.UseContingent"

    # Check if contingents are stored in the unit before creating the ability

    # Stores the pop space
    resource_storage = current_unit["resource_storage"].value
    contingents = []
    for storage in resource_storage:
        type_id = storage["type"].value

        if type_id == 11:
            resource = dataset.pregen_nyan_objects["util.resource.types.PopulationSpace"].get_nyan_object(
            )
            resource_name = "PopSpace"

        else:
            continue

        amount = storage["amount"].value

        contingent_amount_name = f"{game_entity_name}.UseContingent.{resource_name}"
        contingent_amount = RawAPIObject(contingent_amount_name, resource_name,
                                         dataset.nyan_api_objects)
        contingent_amount.add_raw_parent("engine.util.resource.ResourceAmount")
        ability_forward_ref = ForwardRef(line, ability_ref)
        contingent_amount.set_location(ability_forward_ref)

        contingent_amount.add_raw_member("type",
                                         resource,
                                         "engine.util.resource.ResourceAmount")
        contingent_amount.add_raw_member("amount",
                                         amount,
                                         "engine.util.resource.ResourceAmount")

        line.add_raw_api_object(contingent_amount)
        contingent_amount_forward_ref = ForwardRef(line,
                                                   contingent_amount_name)
        contingents.append(contingent_amount_forward_ref)

    if not contingents:
        # Break out of function if no contingents were found
        return None

    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "UseContingent",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.UseContingent")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    ability_raw_api_object.add_raw_member("amount",
                                          contingents,
                                          "engine.ability.type.UseContingent")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
