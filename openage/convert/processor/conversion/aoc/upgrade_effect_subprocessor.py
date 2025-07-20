# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Upgrades effects and resistances for the Apply*Effect and Resistance
abilities.
"""
from .upgrade_effect.attack import get_attack_effects
from .upgrade_resistance.attack import get_attack_resistances


class AoCUpgradeEffectSubprocessor:
    """
    Creates raw API objects for attack/resistance upgrades in AoC.
    """

    get_attack_effects = staticmethod(get_attack_effects)
    get_attack_resistances = staticmethod(get_attack_resistances)
