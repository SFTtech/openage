# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for civ modifiers.
"""
from __future__ import annotations
import typing


if typing.TYPE_CHECKING:
    from .....value_object.conversion.forward_ref import ForwardRef
    from .....entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup


def get_modifiers(civ_group: GenieCivilizationGroup) -> list[ForwardRef]:
    """
    Returns global modifiers of a civ.
    """
    modifiers = []

    for civ_bonus in civ_group.civ_boni.values():
        if civ_bonus.replaces_researchable_tech():
            # TODO: instant tech research modifier
            pass

    return modifiers
