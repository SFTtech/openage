# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Herd ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def herd_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Herd ability to a line.

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :returns: The forward reference for the ability.
    :rtype: ...dataformat.forward_ref.ForwardRef
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Herd"
    ability_raw_api_object = RawAPIObject(ability_ref, "Herd", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Herd")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Strength
    ability_raw_api_object.add_raw_member("strength",
                                          0,
                                          "engine.ability.type.Herd")

    # Allowed types
    allowed_types = [
        dataset.pregen_nyan_objects["util.game_entity_type.types.Herdable"].get_nyan_object()
    ]
    ability_raw_api_object.add_raw_member("allowed_types",
                                          allowed_types,
                                          "engine.ability.type.Herd")

    # Blacklisted entities
    ability_raw_api_object.add_raw_member("blacklisted_entities",
                                          [],
                                          "engine.ability.type.Herd")

    properties = {}

    # Ranged property
    property_ref = f"{ability_ref}.Ranged"
    property_raw_api_object = RawAPIObject(property_ref,
                                           "Ranged",
                                           dataset.nyan_api_objects)
    property_raw_api_object.add_raw_parent("engine.ability.property.type.Ranged")
    property_location = ForwardRef(line, ability_ref)
    property_raw_api_object.set_location(property_location)

    line.add_raw_api_object(property_raw_api_object)

    property_raw_api_object.add_raw_member("min_range",
                                           0.0,
                                           "engine.ability.property.type.Ranged")
    property_raw_api_object.add_raw_member("max_range",
                                           3.0,  # hardcoded
                                           "engine.ability.property.type.Ranged")

    property_forward_ref = ForwardRef(line, property_ref)
    properties.update({
        dataset.nyan_api_objects["engine.ability.property.type.Ranged"]: property_forward_ref
    })

    # TODO: Animated property
    # animation seems to be hardcoded?

    ability_raw_api_object.add_raw_member("properties",
                                          properties,
                                          "engine.ability.Ability")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
