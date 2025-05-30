# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the MoveSpeed modifier.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def move_speed_modifier(converter_obj_group: GenieGameEntityGroup, value: float) -> ForwardRef:
    """
    Adds a MoveSpeed modifier to a line or civ group.

    :param converter_obj_group: ConverterObjectGroup that gets the modifier.
    :returns: The forward reference for the modifier.
    """
    dataset = converter_obj_group.data
    if isinstance(converter_obj_group, GenieGameEntityGroup):
        head_unit_id = converter_obj_group.get_head_unit_id()
        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        target_obj_name = name_lookup_dict[head_unit_id][0]

    else:
        # Civs
        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
        target_obj_name = civ_lookup_dict[converter_obj_group.get_id()][0]

    modifier_ref = f"{target_obj_name}.MoveSpeed"
    modifier_raw_api_object = RawAPIObject(modifier_ref, "MoveSpeed", dataset.nyan_api_objects)
    modifier_raw_api_object.add_raw_parent("engine.modifier.multiplier.type.MoveSpeed")
    modifier_location = ForwardRef(converter_obj_group, target_obj_name)
    modifier_raw_api_object.set_location(modifier_location)

    modifier_raw_api_object.add_raw_member("multiplier",
                                           value,
                                           "engine.modifier.multiplier.MultiplierModifier")

    converter_obj_group.add_raw_api_object(modifier_raw_api_object)

    modifier_forward_ref = ForwardRef(converter_obj_group, modifier_raw_api_object.get_id())

    return modifier_forward_ref
