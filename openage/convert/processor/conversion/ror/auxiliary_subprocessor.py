# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Derives complex auxiliary objects from unit lines, techs
or other objects.
"""
from .auxiliary.creatable_game_entity import get_creatable_game_entity


class RoRAuxiliarySubprocessor:
    """
    Creates complexer auxiliary raw API objects for abilities in RoR.
    """

    get_creatable_game_entity = staticmethod(get_creatable_game_entity)
