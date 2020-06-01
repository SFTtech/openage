# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Converter objects for SWGB. Reimplements the ConverterObjectGroup
instances from AoC.
"""
from openage.convert.dataformat.aoc.genie_unit import GenieUnitLineGroup


class SWGBUnitLineGroup(GenieUnitLineGroup):
    """
    A collection of GenieUnitObject types that form an "upgrade line"
    in SWGB. In comparison to AoE, there is one almost identical line
    for every civ (civ line). 

    Example: Trooper Recruit->Trooper->Heavy Trooper->Repeater Trooper

    Only the civ lines will get converted to a game entity. All others
    with have their differences patched in by the civ.
    """

    def __init__(self, line_id, civ_id, full_data_set):
        """
        Creates a new SWGBUnitLineGroup.
        """
        super().__init__(line_id, full_data_set)

        # The ID of the civ this line is associated with
        self.civ_id = civ_id

        # References to alternative lines from other civs
        self.civ_lines = {}

    def add_civ_line(self, other_line):
        """
        Adds a reference to an alternative line from another civ
        to this line.
        """
        other_civ_id = other_line.get_civ_id()
        self.civ_lines[other_civ_id] = other_line

    def get_civ_id(self):
        """
        Returns the ID of the civ that the line belongs to.
        """
        return self.civ_id

    def is_unique(self):
        """
        Groups are unique if they belong to a specific civ.

        :returns: True if there is no alternative line for this unit line.
        """
        return len(self.civ_lines) > 0
