# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for rotation speed in AoC.
"""
from __future__ import annotations
import typing

from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from ......nyan.nyan_structs import MemberOperator
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def rotation_speed_upgrade(
    converter_group: ConverterObjectGroup,
    line: GenieGameEntityGroup,
    value: typing.Union[int, float],
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the move speed modify effect (ID: 6).

    :param converter_group: Tech/Civ that gets the patch.
    :param line: Unit/Building line that has the ability.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # Unused in AoC

    return patches
