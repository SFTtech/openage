# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create effects for healing units.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def get_heal_effects(
    line: GenieGameEntityGroup,
    location_ref: str
) -> list[ForwardRef]:
    """
    Creates effects that are used for healing (unit command: 105)

    :param line: Unit/Building line that gets the ability.
    :param location_ref: Reference to API object the effects are added to.
    :returns: The forward references for the effects.
    """
    current_unit = line.get_head_unit()
    dataset = line.data

    effects = []

    effect_parent = "engine.effect.continuous.flat_attribute_change.FlatAttributeChange"
    heal_parent = "engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease"

    unit_commands = current_unit["unit_commands"].value
    heal_command = None

    for command in unit_commands:
        # Find the Heal command.
        type_id = command["type"].value

        if type_id == 105:
            heal_command = command
            break

    else:
        # Return the empty set
        return effects

    heal_rate = heal_command["work_value1"].value

    heal_ref = f"{location_ref}.HealEffect"
    heal_raw_api_object = RawAPIObject(heal_ref,
                                       "HealEffect",
                                       dataset.nyan_api_objects)
    heal_raw_api_object.add_raw_parent(heal_parent)
    heal_location = ForwardRef(line, location_ref)
    heal_raw_api_object.set_location(heal_location)

    # Type
    type_ref = "util.attribute_change_type.types.Heal"
    change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
    heal_raw_api_object.add_raw_member("type",
                                       change_type,
                                       effect_parent)

    # Min value (optional)
    min_value = dataset.pregen_nyan_objects[("effect.discrete.flat_attribute_change."
                                             "min_heal.AoE2MinChangeAmount")].get_nyan_object()
    heal_raw_api_object.add_raw_member("min_change_rate",
                                       min_value,
                                       effect_parent)

    # Max value (optional; not added because there is none in AoE2)

    # Change rate
    # =================================================================================
    rate_name = f"{location_ref}.HealEffect.ChangeRate"
    rate_raw_api_object = RawAPIObject(rate_name, "ChangeRate", dataset.nyan_api_objects)
    rate_raw_api_object.add_raw_parent("engine.util.attribute.AttributeRate")
    rate_location = ForwardRef(line, heal_ref)
    rate_raw_api_object.set_location(rate_location)

    attribute = dataset.pregen_nyan_objects["util.attribute.types.Health"].get_nyan_object()
    rate_raw_api_object.add_raw_member("type",
                                       attribute,
                                       "engine.util.attribute.AttributeRate")
    rate_raw_api_object.add_raw_member("rate",
                                       heal_rate,
                                       "engine.util.attribute.AttributeRate")

    line.add_raw_api_object(rate_raw_api_object)
    # =================================================================================
    rate_forward_ref = ForwardRef(line, rate_name)
    heal_raw_api_object.add_raw_member("change_rate",
                                       rate_forward_ref,
                                       effect_parent)

    # Ignore protection
    heal_raw_api_object.add_raw_member("ignore_protection",
                                       [],
                                       effect_parent)

    line.add_raw_api_object(heal_raw_api_object)
    heal_forward_ref = ForwardRef(line, heal_ref)
    effects.append(heal_forward_ref)

    return effects
