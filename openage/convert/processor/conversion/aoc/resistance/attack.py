# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create resistances for attacking units and buildings.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def get_attack_resistances(
    line: GenieGameEntityGroup,
    ability_ref: str
) -> list[ForwardRef]:
    """
    Creates resistances that are used for attacking (unit command: 7)

    :param line: Unit/Building line that gets the ability.
    :param ability_ref: Reference of the ability raw API object the effects are added to.
    :returns: The forward references for the effects.
    """
    current_unit = line.get_head_unit()
    dataset = line.data

    armor_lookup_dict = internal_name_lookups.get_armor_class_lookups(dataset.game_version)

    resistances = []

    # FlatAttributeChangeDecrease
    resistance_parent = "engine.resistance.discrete.flat_attribute_change.FlatAttributeChange"
    armor_parent = "engine.resistance.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"

    if current_unit.has_member("armors"):
        armors = current_unit["armors"].value

    else:
        # TODO: Trees and blast defense
        armors = {}

    for armor in armors.values():
        armor_class = armor["type_id"].value
        armor_amount = armor["amount"].value
        class_name = armor_lookup_dict[armor_class]

        armor_ref = f"{ability_ref}.{class_name}"
        armor_raw_api_object = RawAPIObject(armor_ref, class_name, dataset.nyan_api_objects)
        armor_raw_api_object.add_raw_parent(armor_parent)
        armor_location = ForwardRef(line, ability_ref)
        armor_raw_api_object.set_location(armor_location)

        # Type
        type_ref = f"util.attribute_change_type.types.{class_name}"
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        armor_raw_api_object.add_raw_member("type",
                                            change_type,
                                            resistance_parent)

        # Block value
        # =================================================================================
        amount_name = f"{ability_ref}.{class_name}.BlockAmount"
        amount_raw_api_object = RawAPIObject(
            amount_name, "BlockAmount", dataset.nyan_api_objects)
        amount_raw_api_object.add_raw_parent("engine.util.attribute.AttributeAmount")
        amount_location = ForwardRef(line, armor_ref)
        amount_raw_api_object.set_location(amount_location)

        attribute = dataset.pregen_nyan_objects["util.attribute.types.Health"].get_nyan_object()
        amount_raw_api_object.add_raw_member("type",
                                             attribute,
                                             "engine.util.attribute.AttributeAmount")
        amount_raw_api_object.add_raw_member("amount",
                                             armor_amount,
                                             "engine.util.attribute.AttributeAmount")

        line.add_raw_api_object(amount_raw_api_object)
        # =================================================================================
        amount_forward_ref = ForwardRef(line, amount_name)
        armor_raw_api_object.add_raw_member("block_value",
                                            amount_forward_ref,
                                            resistance_parent)

        line.add_raw_api_object(armor_raw_api_object)
        armor_forward_ref = ForwardRef(line, armor_ref)
        resistances.append(armor_forward_ref)

    # Fallback effect
    fallback_effect = dataset.pregen_nyan_objects[("resistance.discrete.flat_attribute_change."
                                                   "fallback.AoE2AttackFallback")].get_nyan_object()
    resistances.append(fallback_effect)

    return resistances
