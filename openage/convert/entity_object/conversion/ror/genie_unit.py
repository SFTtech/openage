# Copyright 2020-2023 the openage authors. See copying.md for legal info.

"""
Contains structures and API-like objects for game entities from RoR.

Based on the classes from the AoC converter.
"""
from __future__ import annotations
import typing

from ..aoc.genie_unit import GenieUnitLineGroup, GenieBuildingLineGroup, \
    GenieAmbientGroup, GenieVariantGroup, GenieUnitTaskGroup, \
    GenieVillagerGroup, GenieGarrisonMode

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer


class RoRUnitLineGroup(GenieUnitLineGroup):
    """
    A collection of GenieUnitObject types that form an "upgrade line"
    in Age of Empires I. Some methods are reimplemented as RoR does not
    have all features from AoE2.

    Example: Clubman-> Axeman
    """

    __slots__ = ('enabling_research_id',)

    def __init__(
        self,
        line_id: int,
        enabling_research_id: int,
        full_data_set: GenieObjectContainer
    ):
        """
        Creates a new RoR game entity line.

        :param line_id: Internal line obj_id in the .dat file.
        :param enabling_research_id: ID of the tech that enables this unit.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(line_id, full_data_set)

        # Saved for RoR because there's no easy way to detect it with a connection
        self.enabling_research_id = enabling_research_id

    def is_garrison(self, civ_id: int = -1) -> bool:
        """
        Only transport ships can garrison in RoR.

        :returns: True if the unit has the unload command (ID: 12).
        """
        return self.has_command(12)

    def is_passable(self, civ_id: int = -1) -> bool:
        """
        Checks whether the group has a passable hitbox.

        :returns: True if the unit class is 10.
        """
        head_unit = self.get_head_unit()
        return head_unit["unit_class"].value == 10

    def get_garrison_mode(self, civ_id: int = -1) -> typing.Union[GenieGarrisonMode, None]:
        """
        Checks only for transport boat commands.

        :returns: The garrison mode of the line.
        :rtype: GenieGarrisonMode
        """
        if self.has_command(12):
            return GenieGarrisonMode.TRANSPORT

        return None

    def get_enabling_research_id(self) -> int:
        return self.enabling_research_id

    def __repr__(self):
        return f"RoRUnitLineGroup<{self.get_id()}>"


class RoRBuildingLineGroup(GenieBuildingLineGroup):
    """
    A collection of GenieUnitObject types that represent a building
    in Age of Empires 1. Some methods are reimplemented as RoR does not
    have all features from AoE2.

    Example2: WatchTower->SentryTower->GuardTower->BallistaTower
    """

    __slots__ = ('enabling_research_id',)

    def __init__(
        self,
        line_id: int,
        enabling_research_id: int,
        full_data_set: GenieObjectContainer
    ):
        """
        Creates a new RoR game entity line.

        :param line_id: Internal line obj_id in the .dat file.
        :param enabling_research_id: ID of the tech that enables this unit.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(line_id, full_data_set)

        # Saved for RoR because there's no easy way to detect it with a connection
        self.enabling_research_id = enabling_research_id

    def is_garrison(self, civ_id: int = -1) -> bool:
        return False

    def is_passable(self, civ_id: int = -1) -> bool:
        """
        Checks whether the group has a passable hitbox.

        :returns: True if the unit class is 10.
        """
        head_unit = self.get_head_unit()
        return head_unit["unit_class"].value == 10

    def get_garrison_mode(self, civ_id: int = -1) -> None:
        return None

    def get_enabling_research_id(self) -> int:
        return self.enabling_research_id

    def __repr__(self):
        return f"RoRBuildingLineGroup<{self.get_id()}>"


class RoRAmbientGroup(GenieAmbientGroup):
    """
    One Genie unit that is an ambient scenery object.
    Mostly for resources, specifically trees. For these objects
    every frame in their graphics file is a variant.

    Example: Trees, Gold mines, Sign
    """

    def is_garrison(self, civ_id: int = -1) -> bool:
        return False

    def is_passable(self, civ_id: int = -1) -> bool:
        """
        Checks whether the group has a passable hitbox.

        :returns: True if the unit class is 10.
        """
        head_unit = self.get_head_unit()
        return head_unit["unit_class"].value == 10

    def get_garrison_mode(self, civ_id: int = -1) -> None:
        return None

    def __repr__(self):
        return f"RoRAmbientGroup<{self.get_id()}>"


class RoRVariantGroup(GenieVariantGroup):
    """
    Collection of multiple Genie units that are variants of the same game entity.
    Mostly for cliffs and ambient terrain objects.

    Example: Cliffs, flowers, mountains
    """

    def is_garrison(self, civ_id: int = -1) -> bool:
        return False

    def is_passable(self, civ_id: int = -1) -> bool:
        """
        Checks whether the group has a passable hitbox.

        :returns: True if the unit class is 10.
        """
        head_unit = self.get_head_unit()
        return head_unit["unit_class"].value == 10

    def get_garrison_mode(self, civ_id: int = -1) -> None:
        return None

    def __repr__(self):
        return f"RoRVariantGroup<{self.get_id()}>"


class RoRUnitTaskGroup(GenieUnitTaskGroup):
    """
    Collection of genie units that have the same task group. This is only
    the villager unit in RoR.

    Example: Villager
    """

    # Female villagers do not exist in RoR
    female_line_id = -1

    __slots__ = ('enabling_research_id',)

    def __init__(
        self,
        line_id: int,
        task_group_id: int,
        enabling_research_id: int,
        full_data_set: GenieObjectContainer
    ):
        """
        Creates a new RoR task group.

        :param line_id: Internal task group obj_id in the .dat file.
        :param task_group_id: ID of the task group.
        :param enabling_research_id: ID of the tech that enables this unit.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(line_id, task_group_id, full_data_set)

        # Saved for RoR because there's no easy way to detect it with a connection
        self.enabling_research_id = enabling_research_id

    def is_garrison(self, civ_id: int = -1) -> bool:
        """
        Only transport ships can garrison in RoR.

        :returns: True if the unit has the unload command (ID: 12).
        """
        return self.has_command(12)

    def is_passable(self, civ_id: int = -1) -> bool:
        """
        Checks whether the group has a passable hitbox.

        :returns: True if the unit class is 10.
        """
        head_unit = self.get_head_unit()
        return head_unit["unit_class"].value == 10

    def get_garrison_mode(self, civ_id: int = -1) -> typing.Union[GenieGarrisonMode, None]:
        """
        Checks only for transport boat commands.

        :returns: The garrison mode of the line.
        :rtype: GenieGarrisonMode
        """
        if self.has_command(12):
            return GenieGarrisonMode.TRANSPORT

        return None

    def get_enabling_research_id(self) -> int:
        return self.enabling_research_id

    def __repr__(self):
        return f"RoRUnitTaskGroup<{self.get_id()}>"


class RoRVillagerGroup(GenieVillagerGroup):
    """
    Special collection of task groups for villagers with some special
    configurations for RoR.
    """

    def is_passable(self, civ_id: int = -1) -> bool:
        return False

    def get_enabling_research_id(self) -> int:
        return -1

    def __repr__(self):
        return f"RoRVillagerGroup<{self.get_id()}>"
