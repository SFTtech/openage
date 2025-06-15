# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Turn ability.
"""
from __future__ import annotations
import typing

from math import degrees

from ......nyan.nyan_structs import MemberSpecialValue
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef

FLOAT32_MAX = 3.4028234663852886e+38


def turn_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Turn ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Turn"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "Turn",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Turn")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Speed
    turn_speed_unmodified = current_unit["turn_speed"].value

    # Default case: Instant turning
    turn_speed = MemberSpecialValue.NYAN_INF

    # Ships/Trebuchets turn slower
    if turn_speed_unmodified > 0:
        turn_yaw = current_unit["max_yaw_per_sec_moving"].value

        if not turn_yaw == FLOAT32_MAX:
            turn_speed = degrees(turn_yaw)

    ability_raw_api_object.add_raw_member("turn_speed",
                                          turn_speed,
                                          "engine.ability.type.Turn")

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

    # Ability properties
    properties = {
        api_objects["engine.ability.property.type.Diplomatic"]: property_forward_ref
    }
    ability_raw_api_object.add_raw_member("properties",
                                          properties,
                                          "engine.ability.Ability")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
