# Copyright 2020-2023 the openage authors. See copying.md for legal info.

"""
Converter objects for SWGB. Reimplements the ConverterObjectGroup
instances from AoC.
"""
from __future__ import annotations
import typing

from ..aoc.genie_unit import GenieUnitLineGroup, GenieUnitTransformGroup, \
    GenieMonkGroup, GenieStackBuildingGroup

if typing.TYPE_CHECKING:
    from engine.convert.entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer


class SWGBUnitLineGroup(GenieUnitLineGroup):
    """
    A collection of GenieUnitObject types that form an "upgrade line"
    in SWGB. In comparison to AoE, there is one almost identical line
    for every civ (civ line).

    Example: Trooper Recruit->Trooper->Heavy Trooper->Repeater Trooper

    Only the civ lines will get converted to a game entity. All others
    with have their differences patched in by the civ.
    """

    __slots__ = ('civ_lines',)

    def __init__(
        self,
        line_id: int,
        full_data_set: GenieObjectContainer
    ):
        """
        Creates a new SWGBUnitLineGroup.

        :param line_id: Internal line obj_id in the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(line_id, full_data_set)

        # References to alternative lines from other civs
        self.civ_lines: dict[int, SWGBUnitLineGroup] = {}

    def add_civ_line(self, other_line: SWGBUnitLineGroup) -> None:
        """
        Adds a reference to an alternative line from another civ
        to this line.
        """
        other_civ_id = other_line.get_civ_id()
        self.civ_lines[other_civ_id] = other_line

    def get_civ_id(self) -> int:
        """
        Returns the ID of the civ that the line belongs to.
        """
        head_unit = self.get_head_unit()
        return head_unit["civilization_id"].value

    def is_civ_unique(self) -> bool:
        """
        Groups are civ unique if there are alternative lines for this unit line..

        :returns: True if alternative lines for this unit line exist.
        """
        return len(self.civ_lines) > 0

    def is_unique(self) -> bool:
        """
        Groups are unique if they belong to a specific civ.

        :returns: True if the civ id is not Gaia's and no alternative lines
                  for this unit line exist.
        """
        return (self.get_civ_id() != 0 and
                len(self.civ_lines) == 0 and
                self.get_enabling_research_id() > -1)

    def __repr__(self):
        return f"SWGBUnitLineGroup<{self.get_id()}>"


class SWGBStackBuildingGroup(GenieStackBuildingGroup):
    """
    Buildings that stack with other units and have annexes. These buildings
    are replaced by their stack unit once built.

    Examples: Gate, Command Center
    """

    def get_enabling_research_id(self) -> int:
        """
        Returns the enabling tech id of the unit
        """
        stack_unit = self.get_stack_unit()
        stack_unit_id = stack_unit["id0"].value
        stack_unit_connection = self.data.building_connections[stack_unit_id]
        enabling_research_id = stack_unit_connection["enabling_research"].value

        return enabling_research_id

    def __repr__(self):
        return f"SWGBStackBuildingGroup<{self.get_id()}>"


class SWGBUnitTransformGroup(GenieUnitTransformGroup):
    """
    Collection of genie units that reference each other with their
    transform_id.

    Example: Cannon

    Only the civ lines will get converted to a game entity. All others
    with have their differences patched in by the civ.
    """

    __slots__ = ('civ_lines',)

    def __init__(
        self,
        line_id: int,
        head_unit_id: int,
        full_data_set: GenieObjectContainer
    ):
        """
        Creates a new SWGB transform group.

        :param head_unit_id: Internal unit obj_id of the unit that should be
                             the initial state.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(line_id, head_unit_id, full_data_set)

        # References to alternative lines from other civs
        self.civ_lines: dict[int, SWGBUnitTransformGroup] = {}

    def add_civ_line(self, other_line: SWGBUnitLineGroup) -> None:
        """
        Adds a reference to an alternative line from another civ
        to this line.
        """
        other_civ_id = other_line.get_civ_id()
        self.civ_lines[other_civ_id] = other_line

    def get_civ_id(self) -> int:
        """
        Returns the ID of the civ that the line belongs to.
        """
        head_unit = self.get_head_unit()
        return head_unit["civilization_id"].value

    def is_civ_unique(self) -> bool:
        """
        Groups are civ unique if there are alternative lines for this unit line..

        :returns: True if alternative lines for this unit line exist.
        """
        return len(self.civ_lines) > 0

    def is_unique(self) -> bool:
        """
        Groups are unique if they belong to a specific civ.

        :returns: True if the civ id is not Gaia's and no alternative lines
                  for this unit line exist.
        """
        return False

    def get_enabling_research_id(self) -> int:
        """
        Returns the enabling tech id of the unit
        """
        head_unit_connection = self.data.unit_connections[self.get_transform_unit_id()]
        enabling_research_id = head_unit_connection["enabling_research"].value

        return enabling_research_id

    def __repr__(self):
        return f"SWGBUnitTransformGroup<{self.get_id()}>"


class SWGBMonkGroup(GenieMonkGroup):
    """
    Collection of jedi/sith units and jedi/sith with holocron. The switch
    between these is hardcoded like in AoE2.

    Only the civ lines will get converted to a game entity. All others
    with have their differences patched in by the civ.
    """

    __slots__ = ('civ_lines',)

    def __init__(
        self,
        line_id: int,
        head_unit_id: int,
        switch_unit_id: int,
        full_data_set: GenieObjectContainer,
    ):
        """
        Creates a new Genie monk group.

        :param head_unit_id: The unit with this task will become the actual
                             GameEntity.
        :param switch_unit_id: This unit will be used to determine the
                               CarryProgress objects.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(line_id, head_unit_id, switch_unit_id, full_data_set)

        # References to alternative lines from other civs
        self.civ_lines: dict[int, SWGBMonkGroup] = {}

    def add_civ_line(self, other_line: SWGBMonkGroup) -> None:
        """
        Adds a reference to an alternative line from another civ
        to this line.
        """
        other_civ_id = other_line.get_civ_id()
        self.civ_lines[other_civ_id] = other_line

    def get_civ_id(self) -> int:
        """
        Returns the ID of the civ that the line belongs to.
        """
        head_unit = self.get_head_unit()
        return head_unit["civilization_id"].value

    def is_civ_unique(self) -> bool:
        """
        Groups are civ unique if there are alternative lines for this unit line..

        :returns: True if alternative lines for this unit line exist.
        """
        return len(self.civ_lines) > 0

    def is_unique(self) -> bool:
        """
        Groups are unique if they belong to a specific civ.

        :returns: True if the civ id is not Gaia's and no alternative lines
                  for this unit line exist.
        """
        return False

    def __repr__(self):
        return f"SWGBMonkGroup<{self.get_id()}>"
