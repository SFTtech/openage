# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the EnterContainer ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieGarrisonMode
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def enter_container_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the EnterContainer ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability. None if no valid containers were found.
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.EnterContainer"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "EnterContainer",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.EnterContainer")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Containers
    containers = []
    entity_lookups = internal_name_lookups.get_entity_lookups(dataset.game_version)
    for garrison in line.garrison_locations:
        garrison_mode = garrison.get_garrison_mode()

        # Cannot enter production buildings or monk inventories
        if garrison_mode in (GenieGarrisonMode.SELF_PRODUCED, GenieGarrisonMode.MONK):
            continue

        garrison_name = entity_lookups[garrison.get_head_unit_id()][0]

        container_ref = f"{garrison_name}.Storage.{garrison_name}Container"
        container_forward_ref = ForwardRef(garrison, container_ref)
        containers.append(container_forward_ref)

    if not containers:
        return None

    ability_raw_api_object.add_raw_member("allowed_containers",
                                          containers,
                                          "engine.ability.type.EnterContainer")

    # Allowed types (all buildings/units)
    allowed_types = [
        dataset.pregen_nyan_objects["util.game_entity_type.types.Unit"].get_nyan_object(),
        dataset.pregen_nyan_objects["util.game_entity_type.types.Building"].get_nyan_object()
    ]

    ability_raw_api_object.add_raw_member("allowed_types",
                                          allowed_types,
                                          "engine.ability.type.EnterContainer")
    ability_raw_api_object.add_raw_member("blacklisted_entities",
                                          [],
                                          "engine.ability.type.EnterContainer")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
