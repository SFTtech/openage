# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create effects for attacking units and buildings.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def get_attack_effects(
    line: GenieGameEntityGroup,
    location_ref: str,
    projectile: int = -1
) -> list[ForwardRef]:
    """
    Creates effects that are used for attacking (unit command: 7)

    :param line: Unit/Building line that gets the ability.
    :param location_ref: Reference to API object the effects are added to.
    :returns: The forward references for the effects.
    """
    dataset = line.data

    if projectile != 1:
        current_unit = line.get_head_unit()

    else:
        projectile_id = line.get_head_unit()["projectile_id1"].value
        current_unit = dataset.genie_units[projectile_id]

    effects = []

    armor_lookup_dict = internal_name_lookups.get_armor_class_lookups(dataset.game_version)

    # FlatAttributeChangeDecrease
    effect_parent = "engine.effect.discrete.flat_attribute_change.FlatAttributeChange"
    attack_parent = "engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"

    attacks = current_unit["attacks"].value

    for attack in attacks.values():
        armor_class = attack["type_id"].value
        attack_amount = attack["amount"].value
        class_name = armor_lookup_dict[armor_class]

        attack_ref = f"{location_ref}.{class_name}"
        attack_raw_api_object = RawAPIObject(attack_ref,
                                             class_name,
                                             dataset.nyan_api_objects)
        attack_raw_api_object.add_raw_parent(attack_parent)
        attack_location = ForwardRef(line, location_ref)
        attack_raw_api_object.set_location(attack_location)

        # Type
        type_ref = f"util.attribute_change_type.types.{class_name}"
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        attack_raw_api_object.add_raw_member("type",
                                             change_type,
                                             effect_parent)

        # Min value (optional)
        min_value = dataset.pregen_nyan_objects[("effect.discrete.flat_attribute_change."
                                                 "min_damage.AoE2MinChangeAmount")].get_nyan_object()
        attack_raw_api_object.add_raw_member("min_change_value",
                                             min_value,
                                             effect_parent)

        # Max value (optional; not added because there is none in AoE2)

        # Change value
        # =================================================================================
        amount_name = f"{location_ref}.{class_name}.ChangeAmount"
        amount_raw_api_object = RawAPIObject(
            amount_name, "ChangeAmount", dataset.nyan_api_objects)
        amount_raw_api_object.add_raw_parent("engine.util.attribute.AttributeAmount")
        amount_location = ForwardRef(line, attack_ref)
        amount_raw_api_object.set_location(amount_location)

        attribute = dataset.pregen_nyan_objects["util.attribute.types.Health"].get_nyan_object()
        amount_raw_api_object.add_raw_member("type",
                                             attribute,
                                             "engine.util.attribute.AttributeAmount")
        amount_raw_api_object.add_raw_member("amount",
                                             attack_amount,
                                             "engine.util.attribute.AttributeAmount")

        line.add_raw_api_object(amount_raw_api_object)
        # =================================================================================
        amount_forward_ref = ForwardRef(line, amount_name)
        attack_raw_api_object.add_raw_member("change_value",
                                             amount_forward_ref,
                                             effect_parent)

        # Ignore protection
        attack_raw_api_object.add_raw_member("ignore_protection",
                                             [],
                                             effect_parent)

        line.add_raw_api_object(attack_raw_api_object)
        attack_forward_ref = ForwardRef(line, attack_ref)
        effects.append(attack_forward_ref)

    # Fallback effect
    fallback_effect = dataset.pregen_nyan_objects[("effect.discrete.flat_attribute_change."
                                                   "fallback.AoE2AttackFallback")].get_nyan_object()
    effects.append(fallback_effect)

    return effects
