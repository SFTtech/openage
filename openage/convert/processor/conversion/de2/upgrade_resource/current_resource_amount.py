# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for current resource amounts in DE2.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......nyan.nyan_structs import MemberOperator
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def current_food_amount_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Union[int, float],
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the current food amount modify effect (ID: 0).

    TODO: Move into AoC processor

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def current_wood_amount_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Union[int, float],
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the current wood amount modify effect (ID: 1).

    TODO: Move into AoC processor

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def current_stone_amount_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Union[int, float],
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the current stone amount modify effect (ID: 2).

    TODO: Move into AoC processor

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def current_gold_amount_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Union[int, float],
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the current gold amount modify effect (ID: 3).

    TODO: Move into AoC processor

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches
