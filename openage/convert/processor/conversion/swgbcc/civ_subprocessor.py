# Copyright 2020-2022 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-statements,too-many-branches

"""
Creates patches and modifiers for civs.
"""
from __future__ import annotations
import typing

from ..aoc.civ_subprocessor import AoCCivSubprocessor
from .tech_subprocessor import SWGBCCTechSubprocessor

from .civ.modifiers import get_modifiers
from .civ.starting_resources import get_starting_resources

if typing.TYPE_CHECKING:
    from ....value_object.conversion.forward_ref import ForwardRef
    from ....entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup


class SWGBCCCivSubprocessor:
    """
    Creates raw API objects for civs in SWGB.
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
            patches.extend(SWGBCCTechSubprocessor.get_patches(civ_group.team_bonus))

        return patches

    get_modifiers = staticmethod(get_modifiers)
    get_starting_resources = staticmethod(get_starting_resources)
