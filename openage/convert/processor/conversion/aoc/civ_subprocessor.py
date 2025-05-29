# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates patches and modifiers for civs.
"""
from __future__ import annotations
import typing

from ....value_object.conversion.forward_ref import ForwardRef
from .tech_subprocessor import AoCTechSubprocessor
from .civ.civ_bonus import setup_civ_bonus
from .civ.modifiers import get_modifiers
from .civ.starting_resources import get_starting_resources
from .civ.tech_tree import setup_tech_tree
from .civ.unique_techs import setup_unique_techs
from .civ.unique_units import setup_unique_units
from .civ.util import create_animation

if typing.TYPE_CHECKING:
    from ....entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup


class AoCCivSubprocessor:
    """
    Creates raw API objects for civs in AoC.
    """

    @classmethod
    def get_civ_setup(cls, civ_group: GenieCivilizationGroup) -> list[ForwardRef]:
        """
        Returns the patches for the civ setup which configures architecture sets
        unique units, unique techs, team boni and unique stat upgrades.
        """
        patches = []

        patches.extend(AoCCivSubprocessor.setup_unique_units(civ_group))
        patches.extend(AoCCivSubprocessor.setup_unique_techs(civ_group))
        patches.extend(AoCCivSubprocessor.setup_tech_tree(civ_group))
        patches.extend(AoCCivSubprocessor.setup_civ_bonus(civ_group))

        if len(civ_group.get_team_bonus_effects()) > 0:
            patches.extend(AoCTechSubprocessor.get_patches(civ_group.team_bonus))

        return patches

    setup_civ_bonus = staticmethod(setup_civ_bonus)
    get_modifiers = staticmethod(get_modifiers)
    get_starting_resources = staticmethod(get_starting_resources)
    setup_tech_tree = staticmethod(setup_tech_tree)
    setup_unique_techs = staticmethod(setup_unique_techs)
    setup_unique_units = staticmethod(setup_unique_units)

    create_animation = staticmethod(create_animation)
