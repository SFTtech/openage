# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Contains structures and API-like objects for techs from RoR.

Based on the classes from the AoC converter.
"""

from ..aoc.genie_tech import StatUpgrade, AgeUpgrade, UnitLineUpgrade,\
    BuildingLineUpgrade, UnitUnlock, BuildingUnlock


class RoRStatUpgrade(StatUpgrade):
    """
    Upgrades attributes of units/buildings or other stats in the game.
    """

    def is_unique(self):
        return False

    def __repr__(self):
        return f"RoRStatUpgrade<{self.get_id()}>"


class RoRAgeUpgrade(AgeUpgrade):
    """
    Researches a new Age.
    """

    def is_unique(self):
        return False

    def __repr__(self):
        return f"RoRAgeUpgrade<{self.get_id()}>"


class RoRUnitLineUpgrade(UnitLineUpgrade):
    """
    Upgrades a unit in a line.
    """

    def is_unique(self):
        return False

    def __repr__(self):
        return f"RoRUnitLineUpgrade<{self.get_id()}>"


class RoRBuildingLineUpgrade(BuildingLineUpgrade):
    """
    Upgrades a building in a line.
    """

    def is_unique(self):
        return False

    def __repr__(self):
        return f"RoRBuildingLineUpgrade<{self.get_id()}>"


class RoRUnitUnlock(UnitUnlock):
    """
    Unlocks units.
    """

    def is_unique(self):
        return False

    def get_unlocked_line(self):
        """
        Returns the line that is unlocked by this tech.
        """
        return self.data.unit_lines[self.line_id]

    def __repr__(self):
        return f"RoRUnitUnlock<{self.get_id()}>"


class RoRBuildingUnlock(BuildingUnlock):
    """
    Unlocks buildings.
    """

    def is_unique(self):
        return False

    def __repr__(self):
        return f"RoRBuildingUnlock<{self.get_id()}>"
