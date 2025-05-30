# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the ElevationAttack modifier.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......nyan.nyan_structs import NyanObject
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def elevation_attack_modifiers(converter_obj_group: GenieGameEntityGroup) -> list[NyanObject]:
    """
    Adds the pregenerated elevation damage multipliers to a line or civ group.

    :param converter_obj_group: ConverterObjectGroup that gets the modifier.
    :returns: The forward references for the modifier.
    """
    dataset = converter_obj_group.data
    modifiers = [
        dataset.pregen_nyan_objects[
            "util.modifier.elevation_difference.AttackHigh"
        ].get_nyan_object(),
        dataset.pregen_nyan_objects[
            "util.modifier.elevation_difference.AttackLow"
        ].get_nyan_object()
    ]

    return modifiers
