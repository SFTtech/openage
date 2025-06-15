# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for the relic gold bonus in AoC.
"""
from __future__ import annotations
import typing


if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....value_object.conversion.forward_ref import ForwardRef
    from ......nyan.nyan_structs import MemberOperator


def relic_gold_bonus_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the relic gold bonus modify effect (ID: 191).

    :param converter_group: Tech/Civ that gets the patch.
    :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
    :param value: Value used for patching the member.
    :type value: Any
    :param operator: Operator used for patching the member.
    :type operator: MemberOperator
    :returns: The forward references for the generated patches.
    :rtype: list
    """
    patches = []

    return patches
