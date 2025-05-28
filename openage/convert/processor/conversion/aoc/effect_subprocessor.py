# Copyright 2020-2025 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods

"""
Creates effects and resistances for the Apply*Effect and Resistance
abilities.
"""
from .effect.attack import get_attack_effects
from .effect.construct import get_construct_effects
from .effect.convert import get_convert_effects
from .effect.heal import get_heal_effects
from .effect.repair import get_repair_effects
from .resistance.attack import get_attack_resistances
from .resistance.convert import get_convert_resistances
from .resistance.heal import get_heal_resistances
from .resistance.repair import get_repair_resistances
from .resistance.construct import get_construct_resistances


class AoCEffectSubprocessor:
    """
    Creates raw API objects for attacks/resistances in AoC.
    """

    get_attack_effects = staticmethod(get_attack_effects)
    get_construct_effects = staticmethod(get_construct_effects)
    get_convert_effects = staticmethod(get_convert_effects)
    get_heal_effects = staticmethod(get_heal_effects)
    get_repair_effects = staticmethod(get_repair_effects)

    get_attack_resistances = staticmethod(get_attack_resistances)
    get_construct_resistances = staticmethod(get_construct_resistances)
    get_convert_resistances = staticmethod(get_convert_resistances)
    get_heal_resistances = staticmethod(get_heal_resistances)
    get_repair_resistances = staticmethod(get_repair_resistances)
