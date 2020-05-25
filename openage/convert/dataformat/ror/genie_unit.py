# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Converter objects for Rise of Rome. Reimplements the ConverterObjectGroup
instances from AoC because some features are different.
"""
from openage.convert.dataformat.aoc.genie_unit import GenieUnitLineGroup,\
    GenieBuildingLineGroup, GenieAmbientGroup, GenieVariantGroup,\
    GenieGarrisonMode, GenieUnitTaskGroup, GenieVillagerGroup


class RoRUnitLineGroup(GenieUnitLineGroup):
    """
    A collection of GenieUnitObject types that form an "upgrade line"
    in Age of Empires I. Some methods are reimplemented as RoR does not
    have all features from AoE2.

    Example: Clubman-> Axeman
    """

    __slots__ = ('enabling_research_id',)

    def __init__(self, line_id, enabling_research_id, full_data_set):
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

    def is_garrison(self):
        """
        Only transport shis can garrison in RoR.

        :returns: True if the unit has the unload command (ID: 12).
        """
        return self.has_command(12)

    def is_passable(self):
        """
        Checks whether the group has a passable hitbox.

        :returns: True if the unit class is 10.
        """
        head_unit = self.get_head_unit()
        return head_unit.get_member("unit_class").get_value() == 10

    def get_garrison_mode(self):
        """
        Checks only for transport boat commands.

        :returns: The garrison mode of the line.
        :rtype: GenieGarrisonMode
        """
        if self.has_command(12):
            return GenieGarrisonMode.TRANSPORT

        return None

    def get_enabling_research_id(self):
        return self.enabling_research_id

    def __repr__(self):
        return "RoRUnitLineGroup<%s>" % (self.get_id())


class RoRBuildingLineGroup(GenieBuildingLineGroup):
    """
    A collection of GenieUnitObject types that represent a building
    in Age of Empires 1. Some methods are reimplemented as RoR does not
    have all features from AoE2.

    Example2: WatchTower->SentryTower->GuardTower->BallistaTower
    """

    __slots__ = ('enabling_research_id',)

    def __init__(self, line_id, enabling_research_id, full_data_set):
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

    def is_garrison(self):
        return False

    def is_passable(self):
        """
        Checks whether the group has a passable hitbox.

        :returns: True if the unit class is 10.
        """
        head_unit = self.get_head_unit()
        return head_unit.get_member("unit_class").get_value() == 10

    def get_garrison_mode(self):
        return None

    def get_enabling_research_id(self):
        return self.enabling_research_id

    def __repr__(self):
        return "RoRBuildingLineGroup<%s>" % (self.get_id())


class RoRAmbientGroup(GenieAmbientGroup):
    """
    One Genie unit that is an ambient scenery object.
    Mostly for resources, specifically trees. For these objects
    every frame in their graphics file is a variant.

    Example: Trees, Gold mines, Sign
    """

    def is_garrison(self):
        return False

    def is_passable(self):
        """
        Checks whether the group has a passable hitbox.

        :returns: True if the unit class is 10.
        """
        head_unit = self.get_head_unit()
        return head_unit.get_member("unit_class").get_value() == 10

    def get_garrison_mode(self):
        return None

    def __repr__(self):
        return "RoRAmbientGroup<%s>" % (self.get_id())


class RoRVariantGroup(GenieVariantGroup):
    """
    Collection of multiple Genie units that are variants of the same game entity.
    Mostly for cliffs and ambient terrain objects.

    Example: Cliffs, flowers, mountains
    """

    def is_garrison(self):
        return False

    def is_passable(self):
        """
        Checks whether the group has a passable hitbox.

        :returns: True if the unit class is 10.
        """
        head_unit = self.get_head_unit()
        return head_unit.get_member("unit_class").get_value() == 10

    def get_garrison_mode(self):
        return None

    def __repr__(self):
        return "RoRVariantGroup<%s>" % (self.get_id())


class RoRUnitTaskGroup(GenieUnitTaskGroup):
    """
    Collection of genie units that have the same task group. This is only
    the villager unit in RoR.

    Example: Villager
    """

    # Female villagers do not exist in RoR
    female_line_id = -1

    __slots__ = ('enabling_research_id',)

    def __init__(self, line_id, task_group_id, enabling_research_id, full_data_set):
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

    def is_garrison(self):
        """
        Only transport shis can garrison in RoR.

        :returns: True if the unit has the unload command (ID: 12).
        """
        return self.has_command(12)

    def is_passable(self):
        """
        Checks whether the group has a passable hitbox.

        :returns: True if the unit class is 10.
        """
        head_unit = self.get_head_unit()
        return head_unit.get_member("unit_class").get_value() == 10

    def get_garrison_mode(self):
        """
        Checks only for transport boat commands.

        :returns: The garrison mode of the line.
        :rtype: GenieGarrisonMode
        """
        if self.has_command(12):
            return GenieGarrisonMode.TRANSPORT

        return None

    def get_enabling_research_id(self):
        return self.enabling_research_id

    def __repr__(self):
        return "RoRUnitTaskGroup<%s>" % (self.get_id())


class RoRVillagerGroup(GenieVillagerGroup):
    """
    Special collection of task groups for villagers with some special
    configurations for RoR.
    """

    __slots__ = ('enabling_research_id',)

    def __init__(self, group_id, task_group_ids, full_data_set):
        """
        Creates a new RoR villager group.

        :param group_id: Unit obj_id for the villager unit that is referenced by buildings
                         (in AoE2: 118 = male builder).
        :param task_group_ids: Internal task group ids in the .dat file.
                               (as a list of integers)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(group_id, task_group_ids, full_data_set)

    def is_passable(self):
        return False

    def get_enabling_research_id(self):
        return -1

    def __repr__(self):
        return "RoRVillagerGroup<%s>" % (self.get_id())
