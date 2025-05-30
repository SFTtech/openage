# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the FlyoverEffect modifier.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from ......nyan.nyan_structs import NyanObject
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def flyover_effect_modifier(converter_obj_group: GenieGameEntityGroup) -> NyanObject:
    """
    Adds the pregenerated fly-over-cliff damage multiplier to a line or civ group.

    :param converter_obj_group: ConverterObjectGroup that gets the modifier.
    :returns: The forward reference for the modifier.
    """
    dataset = converter_obj_group.data
    modifier = dataset.pregen_nyan_objects[
        "util.modifier.flyover_cliff.AttackFlyover"
    ].get_nyan_object()

    return modifier
