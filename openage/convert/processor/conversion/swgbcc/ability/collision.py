# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Collision ability.
"""
from __future__ import annotations
import typing

from ...aoc.ability_subprocessor import AoCAbilitySubprocessor

if typing.TYPE_CHECKING:
    from .....value_object.conversion.forward_ref import ForwardRef
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def collision_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Collision ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    ability_forward_ref = AoCAbilitySubprocessor.collision_ability(line)

    # TODO: Implement diffing of civ lines

    return ability_forward_ref
