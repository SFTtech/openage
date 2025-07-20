# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Derives and adds modifiers to lines or civ groups. Subroutine of the
nyan subprocessor.
"""

from .modifier.elevation_attack import elevation_attack_modifiers
from .modifier.flyover_effect import flyover_effect_modifier
from .modifier.gather_rate import gather_rate_modifier
from .modifier.move_speed import move_speed_modifier


class AoCModifierSubprocessor:
    """
    Creates raw API objects for modifiers in AoC.
    """

    elevation_attack_modifiers = staticmethod(elevation_attack_modifiers)
    flyover_effect_modifier = staticmethod(flyover_effect_modifier)
    gather_rate_modifier = staticmethod(gather_rate_modifier)
    move_speed_modifier = staticmethod(move_speed_modifier)
