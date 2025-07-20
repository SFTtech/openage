# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for population effects in RoR.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef
    from ......nyan.nyan_structs import MemberOperator


def population_upgrade(
    converter_group: ConverterObjectGroup,
    line: GenieGameEntityGroup,
    value: typing.Union[int, float],
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the population effect (ID: 101).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches
