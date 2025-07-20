# Copyright 2020-2023 the openage authors. See copying.md for legal info.

"""
Derives complex auxiliary objects from unit lines, techs
or other objects.
"""
from .auxiliary.creatable_game_entity import get_creatable_game_entity
from .auxiliary.researchable_tech import get_researchable_tech


class SWGBCCAuxiliarySubprocessor:
    """
    Creates complexer auxiliary raw API objects for abilities in SWGB.
    """

    get_creatable_game_entity = staticmethod(get_creatable_game_entity)
    get_researchable_tech = staticmethod(get_researchable_tech)
