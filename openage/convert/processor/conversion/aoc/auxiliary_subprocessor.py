# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Derives utility objects from unit lines, techs, or other objects.
"""
from .auxiliary.creatable_game_entity import get_creatable_game_entity
from .auxiliary.researchable_tech import get_researchable_tech
from .auxiliary.util import get_condition


class AoCAuxiliarySubprocessor:
    """
    Creates utility raw API objects for Create and Research abilities.
    """

    get_creatable_game_entity = staticmethod(get_creatable_game_entity)
    get_researchable_tech = staticmethod(get_researchable_tech)
    get_condition = staticmethod(get_condition)
