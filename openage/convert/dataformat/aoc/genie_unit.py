# Copyright 2019-2019 the openage authors. See copying.md for legal info.


from openage.convert.dataformat.converter_object import ConverterObject,\
    ConverterObjectGroup


class GenieUnitObject(ConverterObject):
    """
    Ingame object in AoE2.
    """

    def __init__(self, unit_id, full_data_set):

        super().__init__(unit_id, [])

        self.data = full_data_set
        self.data.genie_units.append(self)


class GenieUnitLineGroup(ConverterObjectGroup):
    """
    A collection of GenieUnitObject types that form an "upgrade line"
    in Age of Empires.

    Example: Spearman->Pikeman->Helbardier

    The first unit in the line will become the GameEntity, the rest will
    be patches to that GameEntity applied by Techs.
    """

    def __init__(self, line_id, full_data_set):
        """
        Creates a new Genie unit line.

        :param line_id: Internal line id in the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(line_id)

        # The line is stored as an ordered list of GenieUnitObjects.
        self.line = []

        # Reference to everything else in the gamedata
        self.data = full_data_set
        self.data.add_unit_line(self)

    def add_unit(self, genie_unit, after=None):
        """
        Adds a unit to the line.

        :param genie_unit: A GenieUnit object that is part of this
                           unit line.
        :param after: ID of a unit after which the new unit is
                      placed in the line. If a unit with this id
                      is not present, the unit is appended at the end
                      of the line.
        """

        unit_type = genie_unit.get_member("type").get_value()

        # Valid units have type >= 70
        if unit_type < 70:
            raise Exception("GenieUnitObject must have type >= 70"
                            "to be added to line")

        if after:
            for unit in self.line:
                if after == unit.get_id():
                    self.line.insert(self.line.index(unit), genie_unit)
                    break

            else:
                self.line.append(genie_unit)

        else:
            self.line.append(genie_unit)

    def contains_unit(self, unit_id):
        """
        Returns True if a unit with unit_id is part of the line.
        """
        for unit in self.line:
            if unit.get_member("id") == unit_id:
                return True

        return False


class GenieBuildingLineGroup(ConverterObjectGroup):
    """
    A collection of GenieUnitObject types that represent a building
    in Age of Empires. While buildings have no actual "lines" like units in
    the game data, we will handle them as if they were organized that way.

    Example1: Blacksmith(feudal)->Blacksmith(castle)->Blacksmith(imp)

    Example2: WatchTower->GuardTower->Keep


    Buildings in AoE2 also create units and research techs, so
    this is handled in here.

    The 'head unit' of a building line becomes the GameEntity, the rest will
    be patches to that GameEntity applied by Techs.
    """

    def __init__(self, head_building_id, full_data_set):
        """
        Creates a new Genie building line.

        :param head_building_id: The building that is first in line.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(head_building_id)

        # The line is stored as an ordered list of GenieUnitObjects.
        self.line = []

        # List of GenieUnitLine objects
        self.creates = []

        # List of TODO objects
        self.researches = []

        # Reference to everything else in the gamedata
        self.data = full_data_set
        self.data.add_building_line(self)


class GenieUnitTransformGroup(ConverterObjectGroup):
    """
    Collection of genie units that reference each other with their
    transform_id.

    Example: Trebuchet
    """

    def __init__(self, head_unit_id, full_data_set):
        """
        Creates a new Genie transform group.

        :param head_unit_id: Internal unit id of the unit that should be
                             the initial state.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(head_unit_id)

        # Reference to everything else in the gamedata
        self.data = full_data_set
        self.data.add_transform_group(self)

        self.head_unit = self.data.genie_units[head_unit_id]

        transform_id = self.head_unit.get_member("transform_id").get_value()
        self.transform_unit = self.data.genie_units[transform_id]


class GenieUnitTaskGroup(ConverterObjectGroup):
    """
    Collection of genie units that have the same task group.

    Example: Villager

    The 'head unit' of a task group becomes the GameEntity, all
    the other ones become variants or AnimationOverrides of abilities.
    """

    def __init__(self, task_group_id, head_task_id, full_data_set):
        """
        Creates a new Genie task group.

        :param task_group_id: Internal task group id in the .dat file.
        :param head_task_id: The unit with this task will become the head unit.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(task_group_id)

        self.head_task_id = head_task_id

        # The task group is stored as a dict of GenieUnitObjects.
        # key: task id; value: unit
        self.units = {}

        # Add task group to gamedata
        self.data = full_data_set
        self.data.add_task_group(self)


class GenieVillagerGroup(GenieUnitTaskGroup):
    """
    Special GenieUnitTaskGroup for villagers.

    Villagers come in two task groups, so one task group is a
    variant of the other one.
    """

    def __init__(self, task_group_id, head_task_id,
                 variant_task_group_id, full_data_set):
        """
        Creates a new Genie villager group.

        :param task_group_id: Internal task group id in the .dat file.
        :param head_task_id: The unit with this task will become the head unit.
        :param variant_task_group_id: The task group id of the variant.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(task_group_id, head_task_id, full_data_set)

        # Reference to the other task group
        self.variant = self.data.task_groups[variant_task_group_id]

        # List of buildings that villagers can create
        self.creates = []

        self.data.add_villager_group(self)


class GenieMonkGroup(ConverterObjectGroup):
    """
    Collection of monk and monk with relic. The switch
    is hardcoded in AoE2.

    The 'head unit' will become the GameEntity, the 'switch unit'
    will become a Container ability with CarryProgress.
    """

    def __init__(self, head_unit_id, switch_unit_id, full_data_set):
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
        super().__init__(head_unit_id)

        # Reference to everything else in the gamedata
        self.data = full_data_set
        self.data.add_monk_group(self)

        self.head_unit = self.data.genie_units[head_unit_id]
        self.switch_unit = self.data.genie_units[switch_unit_id]
