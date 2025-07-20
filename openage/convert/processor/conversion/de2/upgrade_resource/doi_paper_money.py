# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for the paper money tech effect in DE2.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......nyan.nyan_structs import MemberOperator
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def doi_paper_money_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Union[int, float],
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the Paper Money effect in Dynasties of India (ID: 266).

    :param converter_group: Tech/Civ that gets the patch.
    :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
    :param value: Value used for patching the member.
    :type value: MemberOperator
    :param operator: Operator used for patching the member.
    :type operator: MemberOperator
    :returns: The forward references for the generated patches.
    :rtype: list
    """
    patches = []

    # TODO: Implement

    return patches
