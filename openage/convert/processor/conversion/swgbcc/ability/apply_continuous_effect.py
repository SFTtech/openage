# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the ApplyContinuousEffect ability.
"""
from __future__ import annotations
import typing

from ...aoc.ability_subprocessor import AoCAbilitySubprocessor

if typing.TYPE_CHECKING:
    from .....value_object.conversion.forward_ref import ForwardRef
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def apply_continuous_effect_ability(
    line: GenieGameEntityGroup,
    command_id: int,
    ranged: bool = False
) -> ForwardRef:
    """
    Adds the ApplyContinuousEffect ability to a line that is used to make entities die.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    ability_forward_ref = AoCAbilitySubprocessor.apply_continuous_effect_ability(
        line, command_id, ranged)

    # TODO: Implement diffing of civ lines

    return ability_forward_ref
