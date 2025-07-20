# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for the conversion of buildings in DE2.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......nyan.nyan_structs import MemberOperator
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def convert_min_building_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Union[int, float],
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the conversion min building modify effect (ID: 180).

    TODO: Move into AoC processor

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def convert_max_building_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Union[int, float],
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the conversion max building modify effect (ID: 181).

    TODO: Move into AoC processor

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def convert_building_chance_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Union[int, float],
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the conversion building chance modify effect (ID: 182).

    TODO: Move into AoC processor

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches
