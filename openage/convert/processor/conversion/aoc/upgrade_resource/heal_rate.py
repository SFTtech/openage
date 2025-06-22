# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for heal rates in AoC.
"""
from __future__ import annotations
import typing


if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....value_object.conversion.forward_ref import ForwardRef
    from ......nyan.nyan_structs import MemberOperator


def heal_rate_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the heal rate modify effect (ID: 89).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # Unused in AoC

    return patches
