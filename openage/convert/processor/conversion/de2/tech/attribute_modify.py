# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates patches for modifying attributes of entities.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberOperator
from .upgrade_funcs import UPGRADE_ATTRIBUTE_FUNCS

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_effect import GenieEffectObject
    from .....value_object.conversion.forward_ref import ForwardRef


def attribute_modify_effect(
    converter_group: ConverterObjectGroup,
    effect: GenieEffectObject,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates the patches for modifying attributes of entities.
    """
    patches = []
    dataset = converter_group.data

    effect_type = effect.get_type()
    operator = None
    if effect_type in (0, 10):
        operator = MemberOperator.ASSIGN

    elif effect_type in (4, 14):
        operator = MemberOperator.ADD

    elif effect_type in (5, 15):
        operator = MemberOperator.MULTIPLY

    else:
        raise TypeError(f"Effect type {effect_type} is not a valid attribute effect")

    unit_id = effect["attr_a"].value
    class_id = effect["attr_b"].value
    attribute_type = effect["attr_c"].value
    value = effect["attr_d"].value

    if attribute_type == -1:
        return patches

    affected_entities = []
    if unit_id != -1:
        entity_lines = {}
        entity_lines.update(dataset.unit_lines)
        entity_lines.update(dataset.building_lines)
        entity_lines.update(dataset.ambient_groups)

        for line in entity_lines.values():
            if line.contains_entity(unit_id):
                affected_entities.append(line)

            elif attribute_type == 19:
                if line.is_projectile_shooter() and line.has_projectile(unit_id):
                    affected_entities.append(line)

    elif class_id != -1:
        entity_lines = {}
        entity_lines.update(dataset.unit_lines)
        entity_lines.update(dataset.building_lines)
        entity_lines.update(dataset.ambient_groups)

        for line in entity_lines.values():
            if line.get_class_id() == class_id:
                affected_entities.append(line)

    else:
        return patches

    upgrade_func = UPGRADE_ATTRIBUTE_FUNCS[attribute_type]
    for affected_entity in affected_entities:
        patches.extend(upgrade_func(converter_group, affected_entity, value, operator, team))

    return patches
