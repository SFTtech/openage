# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create resistances for healing units.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def get_heal_resistances(
    line: GenieGameEntityGroup,
    ability_ref: str
) -> list[ForwardRef]:
    """
    Creates resistances that are used for healing (unit command: 105)

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :param ability_ref: Reference of the ability raw API object the effects are added to.
    :type ability_ref: str
    :returns: The forward references for the effects.
    :rtype: list
    """
    dataset = line.data

    resistances = []

    resistance_parent = "engine.resistance.continuous.flat_attribute_change.FlatAttributeChange"
    heal_parent = "engine.resistance.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease"

    resistance_ref = f"{ability_ref}.Heal"
    resistance_raw_api_object = RawAPIObject(resistance_ref,
                                             "Heal",
                                             dataset.nyan_api_objects)
    resistance_raw_api_object.add_raw_parent(heal_parent)
    resistance_location = ForwardRef(line, ability_ref)
    resistance_raw_api_object.set_location(resistance_location)

    # Type
    type_ref = "util.attribute_change_type.types.Heal"
    change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
    resistance_raw_api_object.add_raw_member("type",
                                             change_type,
                                             resistance_parent)

    # Block rate
    # =================================================================================
    rate_name = f"{ability_ref}.Heal.BlockRate"
    rate_raw_api_object = RawAPIObject(rate_name, "BlockRate", dataset.nyan_api_objects)
    rate_raw_api_object.add_raw_parent("engine.util.attribute.AttributeRate")
    rate_location = ForwardRef(line, resistance_ref)
    rate_raw_api_object.set_location(rate_location)

    attribute = dataset.pregen_nyan_objects["util.attribute.types.Health"].get_nyan_object()
    rate_raw_api_object.add_raw_member("type",
                                       attribute,
                                       "engine.util.attribute.AttributeRate")
    rate_raw_api_object.add_raw_member("rate",
                                       0.0,
                                       "engine.util.attribute.AttributeRate")

    line.add_raw_api_object(rate_raw_api_object)
    # =================================================================================
    rate_forward_ref = ForwardRef(line, rate_name)
    resistance_raw_api_object.add_raw_member("block_rate",
                                             rate_forward_ref,
                                             resistance_parent)

    line.add_raw_api_object(resistance_raw_api_object)
    resistance_forward_ref = ForwardRef(line, resistance_ref)
    resistances.append(resistance_forward_ref)

    return resistances
