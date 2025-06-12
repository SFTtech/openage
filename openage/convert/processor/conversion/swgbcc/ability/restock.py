# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Restock ability.
"""
from __future__ import annotations
import typing

from ...aoc.ability_subprocessor import AoCAbilitySubprocessor

if typing.TYPE_CHECKING:
    from .....value_object.conversion.forward_ref import ForwardRef
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def restock_ability(line: GenieGameEntityGroup, restock_target_id: int) -> ForwardRef:
    """
    Adds the Restock ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    ability_forward_ref = AoCAbilitySubprocessor.restock_ability(line, restock_target_id)

    # TODO: Implement diffing of civ lines

    return ability_forward_ref
