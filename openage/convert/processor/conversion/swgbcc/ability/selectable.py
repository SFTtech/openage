# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Selectable ability.
"""
from __future__ import annotations
import typing

from ...aoc.ability_subprocessor import AoCAbilitySubprocessor

if typing.TYPE_CHECKING:
    from .....value_object.conversion.forward_ref import ForwardRef
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def selectable_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds Selectable abilities to a line. Units will get two of these,
    one Rectangle box for the Self stance and one MatchToSprite box
    for other stances.

    :param line: Unit/Building line that gets the abilities.
    :returns: The forward reference for the abilities.
    """
    ability_forward_ref = AoCAbilitySubprocessor.selectable_ability(line)

    # TODO: Implement diffing of civ lines

    return ability_forward_ref
