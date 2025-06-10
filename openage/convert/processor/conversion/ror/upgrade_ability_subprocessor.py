# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for abilities.
"""
from .upgrade_ability.shoot_projectile import shoot_projectile_ability


class RoRUpgradeAbilitySubprocessor:
    """
    Creates raw API objects for ability upgrade effects in RoR.
    """

    shoot_projectile_ability = staticmethod(shoot_projectile_ability)
