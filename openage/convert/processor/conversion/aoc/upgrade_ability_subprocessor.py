# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for abilities.
"""
from .upgrade_ability.apply_continuous_effect import apply_continuous_effect_ability
from .upgrade_ability.apply_discrete_effect import apply_discrete_effect_ability
from .upgrade_ability.attribute_change_tracker import attribute_change_tracker_ability
from .upgrade_ability.death import death_ability
from .upgrade_ability.despawn import despawn_ability
from .upgrade_ability.idle import idle_ability
from .upgrade_ability.line_of_sight import line_of_sight_ability
from .upgrade_ability.live import live_ability
from .upgrade_ability.move import move_ability
from .upgrade_ability.named import named_ability
from .upgrade_ability.resistance import resistance_ability
from .upgrade_ability.selectable import selectable_ability
from .upgrade_ability.shoot_projectile import shoot_projectile_ability
from .upgrade_ability.turn import turn_ability

from .upgrade_ability.util import create_animation, create_sound, create_language_strings, \
    create_animation_patch, create_command_sound_patch


class AoCUpgradeAbilitySubprocessor:
    """
    Creates raw API objects for ability upgrade effects in AoC.
    """

    apply_continuous_effect_ability = staticmethod(apply_continuous_effect_ability)
    apply_discrete_effect_ability = staticmethod(apply_discrete_effect_ability)
    attribute_change_tracker_ability = staticmethod(attribute_change_tracker_ability)
    death_ability = staticmethod(death_ability)
    despawn_ability = staticmethod(despawn_ability)
    idle_ability = staticmethod(idle_ability)
    los_ability = staticmethod(line_of_sight_ability)
    live_ability = staticmethod(live_ability)
    move_ability = staticmethod(move_ability)
    named_ability = staticmethod(named_ability)
    resistance_ability = staticmethod(resistance_ability)
    selectable_ability = staticmethod(selectable_ability)
    shoot_projectile_ability = staticmethod(shoot_projectile_ability)
    turn_ability = staticmethod(turn_ability)

    create_animation = staticmethod(create_animation)
    create_sound = staticmethod(create_sound)
    create_language_strings = staticmethod(create_language_strings)
    create_animation_patch = staticmethod(create_animation_patch)
    create_command_sound_patch = staticmethod(create_command_sound_patch)
