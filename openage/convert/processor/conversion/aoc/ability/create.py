# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Create ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def create_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Create ability to a line.

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :returns: The forward reference for the ability.
    :rtype: ...dataformat.forward_ref.ForwardRef
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]
    ability_ref = f"{game_entity_name}.Create"
    ability_raw_api_object = RawAPIObject(ability_ref, "Create", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Create")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

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

    # Creatables
    creatables_set = []
    for creatable in line.creates:
        if creatable.is_unique():
            # Skip this because unique units are handled by civs
            continue

        # CreatableGameEntity objects are created for each unit/building
        # line individually to avoid duplicates. We just point to the
        # raw API objects here.
        creatable_id = creatable.get_head_unit_id()
        creatable_name = name_lookup_dict[creatable_id][0]

        raw_api_object_ref = f"{creatable_name}.CreatableGameEntity"
        creatable_forward_ref = ForwardRef(creatable,
                                           raw_api_object_ref)
        creatables_set.append(creatable_forward_ref)

    ability_raw_api_object.add_raw_member("creatables", creatables_set,
                                          "engine.ability.type.Create")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
