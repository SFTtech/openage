# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates patches for modifying resources.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberOperator
from .upgrade_funcs import UPGRADE_RESOURCE_FUNCS

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_effect import GenieEffectObject
    from .....value_object.conversion.forward_ref import ForwardRef


def resource_modify_effect(
    converter_group: ConverterObjectGroup,
    effect: GenieEffectObject,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates the patches for modifying resources.
    """
    patches = []

    effect_type = effect.get_type()
    operator = None
    if effect_type == 1:
        mode = effect["attr_b"].value

        if mode == 0:
            operator = MemberOperator.ASSIGN

        else:
            operator = MemberOperator.ADD

    elif effect_type == 6:
        operator = MemberOperator.MULTIPLY

    else:
        raise TypeError(f"Effect type {effect_type} is not a valid resource effect")

    resource_id = effect["attr_a"].value
    value = effect["attr_d"].value

    if resource_id in (-1, 6, 21, 30):
        # -1 = invalid ID
        # 6  = set current age (unused)
        # 21 = tech count (unused)
        # 30 = building limits (unused)
        return patches

    upgrade_func = UPGRADE_RESOURCE_FUNCS[resource_id]
    patches.extend(upgrade_func(converter_group, value, operator, team))

    return patches
