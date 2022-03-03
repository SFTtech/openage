# Copyright 2019-2022 the openage authors. See copying.md for legal info.

"""
Contains structures and API-like objects for civilization from AoC.
"""

from __future__ import annotations
import typing

from ..converter_object import ConverterObject, ConverterObjectGroup
from .genie_tech import CivTeamBonus, CivTechTree

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_effect import GenieEffectObject
    from openage.convert.entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer
    from openage.convert.entity_object.conversion.aoc.genie_tech import CivBonus, GenieTechObject
    from openage.convert.entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from openage.convert.value_object.read.value_members import ValueMember


class GenieCivilizationObject(ConverterObject):
    """
    Civilization in AoE2.
    """

    __slots__ = ('data',)

    def __init__(
        self,
        civ_id: int,
        full_data_set: GenieObjectContainer,
        members: dict[str, ValueMember] = None
    ):
        """
        Creates a new Genie civilization object.

        :param civ_id: The index of the civilization in the .dat file's civilization
                       block. (the index is referenced as civilization_id by techs)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(civ_id, members=members)

        self.data = full_data_set

    def __repr__(self):
        return f"GenieCivilizationObject<{self.get_id()}>"


class GenieCivilizationGroup(ConverterObjectGroup):
    """
    All necessary civiization data.

    This will become a Civilization API object.
    """

    __slots__ = ('data', 'civ', 'team_bonus', 'tech_tree', 'civ_boni',
                 'unique_entities', 'unique_techs')

    def __init__(
        self,
        civ_id: int,
        full_data_set: GenieObjectContainer
    ):
        """
        Creates a new Genie civ group line.

        :param civ_id: The index of the civilization in the .dat file's civilization
                       block. (the index is referenced as civ_id by techs)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(civ_id)

        # Reference to everything else in the gamedata
        self.data = full_data_set

        self.civ: dict[int, GenieCivilizationObject] = self.data.genie_civs[civ_id]

        self.team_bonus: CivTeamBonus = None
        if self.civ.has_member("team_bonus_id"):
            team_bonus_id = self.civ["team_bonus_id"].value
            if team_bonus_id == -1:
                # Gaia civ has no team bonus
                self.team_bonus = None
            else:
                # Create an object for the team bonus. We use the effect ID + 10000 to avoid
                # conflicts with techs or effects
                self.team_bonus = CivTeamBonus(10000 + team_bonus_id, civ_id,
                                               team_bonus_id, full_data_set)

        # Create an object for the tech tree bonus. We use the effect ID + 10000 to avoid
        # conflicts with techs or effects
        tech_tree_id: int  = self.civ["tech_tree_id"].value
        self.tech_tree = CivTechTree(10000 + tech_tree_id, civ_id,
                                     tech_tree_id, full_data_set)

        # Civ boni (without team bonus)
        self.civ_boni: dict[int, CivBonus] = {}

        # Unique units/buildings
        self.unique_entities: dict[int, GenieGameEntityGroup] = {}

        # Unique techs
        self.unique_techs: dict[int, GenieTechObject] = {}

    def add_civ_bonus(self, civ_bonus: CivBonus):
        """
        Adds a civ bonus tech to the civilization.
        """
        self.civ_boni.update({civ_bonus.get_id(): civ_bonus})

    def add_unique_entity(self, entity_group: GenieGameEntityGroup):
        """
        Adds a unique unit to the civilization.
        """
        self.unique_entities.update({entity_group.get_head_unit_id(): entity_group})

    def add_unique_tech(self, tech_group: GenieTechObject):
        """
        Adds a unique tech to the civilization.
        """
        self.unique_techs.update({tech_group.get_id(): tech_group})

    def get_team_bonus_effects(self) -> list[GenieEffectObject]:
        """
        Returns the effects of the team bonus.
        """
        if self.team_bonus:
            return self.team_bonus.get_effects()

        return []

    def get_tech_tree_effects(self) -> list[GenieEffectObject]:
        """
        Returns the tech tree effects.
        """
        if self.tech_tree:
            return self.tech_tree.get_effects()

        return []

    def __repr__(self):
        return f"GenieCivilizationGroup<{self.get_id()}>"
