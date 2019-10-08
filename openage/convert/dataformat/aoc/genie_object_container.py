# Copyright 2019-2019 the openage authors. See copying.md for legal info.

from openage.convert.dataformat.converter_object import ConverterObjectContainer


class GenieObjectContainer(ConverterObjectContainer):
    """
    Contains everything from the dat file, sorted into several
    categories.
    """

    def __init__(self):

        # ConverterObject types (the data from the game)
        self.genie_units = {}
        self.genie_techs = {}
        self.graphics = {}
        self.age_connections = {}
        self.building_connections = {}
        self.unit_connections = {}
        self.tech_connections = {}
        self.sounds = {}
        self.civs = {}

        # ConverterObjectGroup types (things that will become
        # nyan objects)
        # key: group_id; value: ConverterObjectGroup instance
        self.unit_lines = {}
        self.building_lines = {}
        self.task_groups = {}
        self.transform_groups = {}
        self.villager_groups = {}
        self.monk_groups = {}

    def add_unit_line(self, unit_line):
        """
        Adds a Genie unit line to the data set.
        """
        self.unit_lines.update({unit_line.get_id(): unit_line})

    def add_building_line(self, building_line):
        """
        Adds a Genie building line to the data set.
        """
        self.building_lines.update({building_line.get_id(): building_line})

    def add_monk_group(self, monk_group):
        """
        Adds a Genie villager task group to the data set.
        """
        self.monk_groups.update({monk_group.get_id(): monk_group})

    def add_task_group(self, task_group):
        """
        Adds a Genie task group to the data set.
        """
        self.task_groups.update({task_group.get_id(): task_group})

    def add_transform_group(self, transform_group):
        """
        Adds a Genie transform group to the data set.
        """
        self.transform_groups.update({transform_group.get_id(): transform_group})

    def add_villager_group(self, task_group):
        """
        Adds a Genie villager task group to the data set.
        """
        self.villager_groups.update({task_group.get_id(): task_group})
