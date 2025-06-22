# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Derives and adds abilities to lines. Reimplements only
abilities that are different from AoC.
"""
from .ability.apply_discrete_effect import apply_discrete_effect_ability
from .ability.game_entity_stance import game_entity_stance_ability
from .ability.production_queue import production_queue_ability
from .ability.projectile import projectile_ability
from .ability.resistance import resistance_ability
from .ability.shoot_projectile import shoot_projectile_ability


class RoRAbilitySubprocessor:
    """
    Creates raw API objects for abilities in RoR.
    """

    apply_discrete_effect_ability = staticmethod(apply_discrete_effect_ability)
    game_entity_stance_ability = staticmethod(game_entity_stance_ability)
    production_queue_ability = staticmethod(production_queue_ability)
    projectile_ability = staticmethod(projectile_ability)
    resistance_ability = staticmethod(resistance_ability)
    shoot_projectile_ability = staticmethod(shoot_projectile_ability)
