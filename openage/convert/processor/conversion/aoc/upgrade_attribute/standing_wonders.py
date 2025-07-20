# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for standing wonders effects in AoC.
"""
from __future__ import annotations
import typing

from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from ......nyan.nyan_structs import MemberOperator
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def standing_wonders_upgrade(
    converter_group: ConverterObjectGroup,
    line: GenieGameEntityGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the standing wonders effect (ID: 42).

    :param converter_group: Tech/Civ that gets the patch.
    :param line: Unit/Building line that has the ability.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # Unused in AoC

    return patches
