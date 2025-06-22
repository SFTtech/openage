# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the RegenerateResourceSpot ability.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def regenerate_resource_spot_ability(line: GenieGameEntityGroup) -> None:
    """
    Adds the RegenerateResourceSpot ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    # Unused in AoC
    return None
