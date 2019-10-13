# Copyright 2019-2019 the openage authors. See copying.md for legal info.


from openage.convert.dataformat.converter_object import ConverterObject,\
    ConverterObjectGroup


class GenieUnitObject(ConverterObject):
    """
    Ingame object in AoE2.
    """

    def __init__(self, unit_id, full_data_set):
        """
        Creates a new Genie unit object.

        :param unit_id: The internal unit_id of the unit.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(unit_id)

        self.data = full_data_set
        self.data.genie_units.update({self.get_id(): self})


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
        self.data.unit_lines.update({self.get_id(): self})

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
        if unit_type != 70:
            raise Exception("GenieUnitObject must have type == 70"
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
    in Age of Empires. Buildings actually have no line id, so we take
    the id of the first occurence of the building's id as the line id.

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
        self.data.building_lines.update({self.get_id(): self})


class GenieUnitTransformGroup(GenieUnitLineGroup):
    """
    Collection of genie units that reference each other with their
    transform_id.

    Example: Trebuchet
    """

    def __init__(self, line_id, head_unit_id, full_data_set):
        """
        Creates a new Genie transform group.

        :param head_unit_id: Internal unit id of the unit that should be
                             the initial state.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(line_id, full_data_set)

        # Add a reference to the unit to the dataset
        self.data.transform_groups.update({self.get_id(): self})

        self.head_unit = self.data.genie_units[head_unit_id]

        transform_id = self.head_unit.get_member("transform_id").get_value()
        self.transform_unit = self.data.genie_units[transform_id]


class GenieMonkGroup(GenieUnitLineGroup):
    """
    Collection of monk and monk with relic. The switch
    is hardcoded in AoE2. (Missionaries are handled as normal lines
    because they cannot pick up relics).

    The 'head unit' will become the GameEntity, the 'switch unit'
    will become a Container ability with CarryProgress.
    """

    def __init__(self, line_id, head_unit_id, switch_unit_id, full_data_set):
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
        super().__init__(line_id, full_data_set)

        # Reference to everything else in the gamedata
        self.data.monk_groups.update({self.get_id(): self})

        self.head_unit = self.data.genie_units[head_unit_id]
        self.switch_unit = self.data.genie_units[switch_unit_id]


class GenieUnitTaskGroup(GenieUnitLineGroup):
    """
    Collection of genie units that have the same task group.

    Example: Villager

    The 'head unit' of a task group becomes the GameEntity, all
    the other ones become variants or AnimationOverrides of abilities.
    """

    def __init__(self, line_id, task_group_id, head_task_id, full_data_set):
        """
        Creates a new Genie task group.

        :param task_group_id: Internal task group id in the .dat file.
        :param head_task_id: The unit with this task will become the head unit.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(line_id, full_data_set)

        self.task_group_id = task_group_id
        self.head_task_id = head_task_id

        # The task group line is stored as a dict of GenieUnitObjects.
        # key: task id; value: unit
        self.line = {}

        # Add a reference for the unit to the dataset
        self.data.task_groups.update({self.get_id(): self})


class GenieVillagerGroup(ConverterObjectGroup):
    """
    Special collection of task groups for villagers.

    Villagers come in two task groups (male/female) and will form
    variants of the common villager game entity.
    """

    def __init__(self, group_id, task_group_ids, full_data_set):
        """
        Creates a new Genie villager group.

        :param group_id: Group id for the cases where there is more than one
                         villager group in the game.
        :param task_group_ids: Internal task group ids in the .dat file. 
                               (as a list of integers)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(group_id)

        self.data = full_data_set
        self.data.villager_groups.update({self.get_id(): self})

        # Reference to the variant task groups
        self.variants = []
        for task_group_id in task_group_ids:
            task_group = self.data.task_groups[task_group_id]
            self.variants.append(task_group)

        # List of buildings that villagers can create
        self.creates = []
