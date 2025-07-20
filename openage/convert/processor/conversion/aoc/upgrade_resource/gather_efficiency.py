# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for gather efficiency modifiers in AoC.
"""
from __future__ import annotations
import typing


if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....value_object.conversion.forward_ref import ForwardRef
    from ......nyan.nyan_structs import MemberOperator


def gather_food_efficiency_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the food gathering efficiency modify effect (ID: 190).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def gather_wood_efficiency_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the wood gathering efficiency modify effect (ID: 189).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def gather_gold_efficiency_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the gold gathering efficiency modify effect (ID: 47).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def gather_stone_efficiency_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the stone gathering efficiency modify effect (ID: 79).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches
