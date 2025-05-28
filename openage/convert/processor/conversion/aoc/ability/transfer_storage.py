# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the TransferStorage ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def transfer_storage_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the TransferStorage ability to a line.

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :returns: The forward reference for the ability.
    :rtype: ...dataformat.forward_ref.ForwardRef, None
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.TransferStorage"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "TransferStorage",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.TransferStorage")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # storage element
    storage_entity = None
    garrisoned_forward_ref = None
    for garrisoned in line.garrison_entities:
        creatable_type = garrisoned.get_head_unit()["creatable_type"].value

        if creatable_type == 4:
            storage_name = name_lookup_dict[garrisoned.get_id()][0]
            storage_entity = garrisoned
            garrisoned_forward_ref = ForwardRef(storage_entity, storage_name)

            break

    else:
        garrisoned = line.garrison_entities[0]
        storage_name = name_lookup_dict[garrisoned.get_id()][0]
        storage_entity = garrisoned
        garrisoned_forward_ref = ForwardRef(storage_entity, storage_name)

    ability_raw_api_object.add_raw_member("storage_element",
                                          garrisoned_forward_ref,
                                          "engine.ability.type.TransferStorage")

    # Source container
    source_ref = f"{game_entity_name}.Storage.{game_entity_name}Container"
    source_forward_ref = ForwardRef(line, source_ref)
    ability_raw_api_object.add_raw_member("source_container",
                                          source_forward_ref,
                                          "engine.ability.type.TransferStorage")

    # Target container
    target = None
    unit_commands = line.get_switch_unit()["unit_commands"].value
    for command in unit_commands:
        type_id = command["type"].value

        # Deposit
        if type_id == 136:
            target_id = command["unit_id"].value
            target = dataset.building_lines[target_id]

    target_name = name_lookup_dict[target.get_id()][0]
    target_ref = f"{target_name}.Storage.{target_name}Container"
    target_forward_ref = ForwardRef(target, target_ref)
    ability_raw_api_object.add_raw_member("target_container",
                                          target_forward_ref,
                                          "engine.ability.type.TransferStorage")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
