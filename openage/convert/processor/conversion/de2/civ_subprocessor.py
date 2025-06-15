# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates patches and modifiers for civs.
"""
from __future__ import annotations
import typing

from ....value_object.conversion.forward_ref import ForwardRef
from ..aoc.civ_subprocessor import AoCCivSubprocessor
from .civ.civ_bonus import setup_civ_bonus
from .tech_subprocessor import DE2TechSubprocessor

if typing.TYPE_CHECKING:
    from ....entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup


class DE2CivSubprocessor:
    """
    Creates raw API objects for civs in DE2.
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
        patches.extend(setup_civ_bonus(civ_group))

        if len(civ_group.get_team_bonus_effects()) > 0:
            patches.extend(DE2TechSubprocessor.get_patches(civ_group.team_bonus))

        return patches

    setup_civ_bonus = staticmethod(setup_civ_bonus)
