# Copyright 2019-2020 the openage authors. See copying.md for legal info.


from ...dataformat.converter_object import ConverterObject,\
    ConverterObjectGroup


class GenieUnitObject(ConverterObject):
    """
    Ingame object in AoE2.
    """

    def __init__(self, unit_id, full_data_set, members=None):
        """
        Creates a new Genie unit object.

        :param unit_id: The internal unit_id of the unit.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(unit_id, members=members)

        self.data = full_data_set

    def __repr__(self):
        return "GenieUnitObject<%s>" % (self.get_id())


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

        :param line_id: Internal line obj_id in the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(line_id)

        # The line is stored as an ordered list of GenieUnitObjects.
        self.line = []

        # This dict stores unit ids and their repective position in the
        # unit line for quick referencing and searching
        self.unit_line_positions = {}

        # Reference to everything else in the gamedata
        self.data = full_data_set

        # List of buildings that units can create
        self.creates = []

    def add_creatable(self, building_line):
        """
        Adds a building line to the list of creatables.

        :param building_line: The GenieBuildingLineGroup the villager produces.
        """
        if not self.contains_creatable(building_line.get_id()):
            self.creates.append(building_line)

    def add_unit(self, genie_unit, after=None):
        """
        Adds a unit to the line.

        :param genie_unit: A GenieUnit object that is part of this
                           unit line.
        :param after: ID of a unit after which the new unit is
                      placed in the line. If a unit with this obj_id
                      is not present, the unit is appended at the end
                      of the line.
        """
        unit_id = genie_unit.get_member("id0").get_value()

        # Only add unit if it is not already in the list
        if not self.contains_unit(unit_id):
            insert_index = len(self.line)

            if after:
                if self.contains_unit(after):
                    insert_index = self.unit_line_positions[after] + 1

                    for unit in self.unit_line_positions.keys():
                        if self.unit_line_positions[unit] >= insert_index:
                            self.unit_line_positions[unit] += 1

            self.unit_line_positions.update({genie_unit.get_id(): insert_index})
            self.line.insert(insert_index, genie_unit)

    def contains_creatable(self, line_id):
        """
        Returns True if a building line with line_id is a creatable of
        this unit.
        """
        building_line = self.data.building_lines[line_id]

        return building_line in self.creates

    def contains_unit(self, unit_id):
        """
        Returns True if a unit with unit_id is part of the line.
        """
        return unit_id in self.unit_line_positions.keys()

    def is_unique(self):
        """
        Units are unique if they belong to a specific civ.
        Eagles and battle elephants do not count as unique units.

        :returns: True if the unit is tied to one specific civ.
        """
        # Get the enabling research obj_id for the first unit in the line
        head_unit = self.line[0]
        head_unit_id = head_unit.get_member("id0").get_value()
        head_unit_connection = self.data.unit_connections[head_unit_id]
        enabling_research_id = head_unit_connection.get_member("enabling_research").get_value()

        # Unit does not need to be enabled -> not unique
        if enabling_research_id == -1:
            return False

        # Get enabling civ
        enabling_research = self.data.genie_techs[enabling_research_id]
        enabling_civ_id = enabling_research.get_member("civilization_id").get_value()

        # Enabling tech has no specific civ -> mot unique
        if enabling_civ_id == -1:
            return False

        # Values other than -1 are civ ids -> unit must be unique
        return True

    def is_creatable(self):
        """
        Units are creatable if they have a valid train location.

        :returns: True if the train location obj_id is greater than zero.
        """
        # Get the train location obj_id for the first unit in the line
        head_unit = self.line[0]
        train_location_id = head_unit.get_member("train_location_id").get_value()

        # -1 = no train location
        if train_location_id == -1:
            return False

        return True

    def get_civ_id(self):
        """
        Returns the enabling civ obj_id if the unit is unique,
        otherwise return None.
        """
        if self.is_unique():
            head_unit = self.line[0]
            head_unit_id = head_unit.get_member("id0").get_value()
            head_unit_connection = self.data.unit_connections[head_unit_id]
            enabling_research_id = head_unit_connection.get_member("enabling_research").get_value()

            enabling_research = self.data.genie_techs[enabling_research_id]
            return enabling_research.get_member("civilization_id").get_value()

        return None

    def get_head_unit_id(self):
        """
        Return the obj_id of the first unit in the line.
        """
        head_unit = self.line[0]
        return head_unit.get_member("id0").get_value()

    def get_train_location(self):
        """
        Returns the group_id for building line if the unit is
        creatable, otherwise return None.
        """
        if self.is_creatable():
            head_unit = self.line[0]
            return head_unit.get_member("train_location_id").get_value()

        return None

    def __repr__(self):
        return "GenieUnitLineGroup<%s>" % (self.get_id())


class GenieBuildingLineGroup(ConverterObjectGroup):
    """
    A collection of GenieUnitObject types that represent a building
    in Age of Empires. Buildings actually have no line obj_id, so we take
    the obj_id of the first occurence of the building's obj_id as the line obj_id.

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

        # List of GenieTechEffectBundleGroup objects
        self.researches = []

        # Reference to everything else in the gamedata
        self.data = full_data_set

    def add_unit(self, genie_unit, after=None):
        """
        Adds a unit to the line.

        :param genie_unit: A GenieUnit object that is part of this
                           building line.
        :param after: ID of a unit after which the new unit is
                      placed in the line. If a unit with this obj_id
                      is not present, the unit is appended at the end
                      of the line.
        """
        unit_id = genie_unit.get_member("id0").get_value()

        # Only add building if it is not already in the list
        if not self.contains_unit(unit_id):
            if after:
                for unit in self.line:
                    if after == unit.get_id():
                        self.line.insert(self.line.index(unit) + 1, genie_unit)
                        break

                else:
                    self.line.append(genie_unit)

            else:
                self.line.append(genie_unit)

    def add_creatable(self, unit_line):
        """
        Adds a unit line to the list of creatables.

        :param unit_line: The GenieUnitLine the building produces.
        """
        if not self.contains_creatable(unit_line.get_head_unit_id()):
            self.creates.append(unit_line)

    def add_researchable(self, tech_group):
        """
        Adds a tech group to the list of researchables.

        :param tech_group: The GenieTechLineGroup the building researches.
        """
        if not self.contains_researchable(tech_group.get_id()):
            self.researches.append(tech_group)

    def contains_unit(self, building_id):
        """
        Returns True if a building with building_id is part of the line.
        """
        building = self.data.genie_units[building_id]

        return building in self.line

    def contains_creatable(self, head_unit_id):
        """
        Returns True if a unit line with head_unit_id is a creatable of
        this building.
        """
        unit_line = self.data.unit_lines[head_unit_id]

        return unit_line in self.creates

    def contains_researchable(self, line_id):
        """
        Returns True if a tech line with line_id is researchable
        in this building.
        """
        tech_line = self.data.tech_groups[line_id]

        return tech_line in self.researches

    def is_creatable(self):
        """
        Buildings are creatable if they have a valid train location.

        :returns: True if the train location obj_id is greater than zero.
        """
        # Get the train location obj_id for the first building in the line
        head_building = self.line[0]
        train_location_id = head_building.get_member("train_location_id").get_value()

        # -1 = no train location
        if train_location_id == -1:
            return False

        return True

    def get_train_location(self):
        """
        Returns the group_id for a villager group if the building is
        creatable, otherwise return None.
        """
        if self.is_creatable():
            head_building = self.line[0]
            return head_building.get_member("train_location_id").get_value()

        return None

    def __repr__(self):
        return "GenieBuildingLineGroup<%s>" % (self.get_id())


class GenieStackBuildingGroup(GenieBuildingLineGroup):
    """
    Buildings that stack with other units and have annexes. These buildings
    are replaced by their stack unit once built.

    Examples: Gate, Town Center

    The 'stack unit' becomes the GameEntity, the 'head unit' will be a state
    during construction.
    """

    def __init__(self, stack_unit_id, head_building_id, full_data_set):
        """
        Creates a new Genie building line.

        :param stack_unit_id: "Actual" building that appears when constructed.
        :param head_building_id: The building used during construction.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(stack_unit_id, full_data_set)

        self.head = self.data.genie_units[head_building_id]
        self.stack = self.data.genie_units[stack_unit_id]

    def is_creatable(self):
        """
        Stack buildings are created through their head building. We have to
        lookup its values.

        :returns: True if the train location obj_id is greater than zero.
        """
        train_location_id = self.head.get_member("train_location_id").get_value()

        # -1 = no train location
        if train_location_id == -1:
            return False

        return True

    def get_train_location(self):
        """
        Stack buildings are creatable when their head building is creatable.

        Returns the group_id for a villager group if the head building is
        creatable, otherwise return None.
        """
        if self.is_creatable():
            return self.head.get_member("train_location_id").get_value()

        return None

    def __repr__(self):
        return "GenieStackBuildingGroup<%s>" % (self.get_id())


class GenieUnitTransformGroup(GenieUnitLineGroup):
    """
    Collection of genie units that reference each other with their
    transform_id.

    Example: Trebuchet
    """

    def __init__(self, line_id, head_unit_id, full_data_set):
        """
        Creates a new Genie transform group.

        :param head_unit_id: Internal unit obj_id of the unit that should be
                             the initial state.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(line_id, full_data_set)

        self.head_unit = self.data.genie_units[head_unit_id]

        transform_id = self.head_unit.get_member("transform_unit_id").get_value()
        self.transform_unit = self.data.genie_units[transform_id]

    def __repr__(self):
        return "GenieUnitTransformGroup<%s>" % (self.get_id())


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

        self.head_unit = self.data.genie_units[head_unit_id]
        self.switch_unit = self.data.genie_units[switch_unit_id]

    def __repr__(self):
        return "GenieMonkGroup<%s>" % (self.get_id())


class GenieVariantGroup(ConverterObjectGroup):
    """
    Collection of Genie units that are variants of the same game entity.
    Mostly for resource spots.

    Example: Trees, fish, gold mines, stone mines
    """

    def __init__(self, class_id, full_data_set):
        """
        TODO: Implement
        """

        super().__init__(class_id)

        # The variants of the units
        self.variants = []

        # Reference to everything else in the gamedata
        self.data = full_data_set

    def add_unit(self, genie_unit, after=None):
        """
        Adds a unit to the list of variants.

        :param genie_unit: A GenieUnit object that is in the same class.
        :param after: ID of a unit after which the new unit is
                      placed in the list. If a unit with this obj_id
                      is not present, the unit is appended at the end
                      of the list.
        """
        unit_id = genie_unit.get_member("id0").get_value()
        class_id = genie_unit.get_member("unit_class").get_value()

        if class_id != self.get_id():
            raise Exception("Classes do not match: unit %s with class %s cannot be added to"
                            " %s with class %s" % (genie_unit, class_id, self, self.get_id()))

        # Only add unit if it is not already in the list
        if not self.contains_unit(unit_id):
            if after:
                for unit in self.variants:
                    if after == unit.get_id():
                        self.variants.insert(self.variants.index(unit) + 1, genie_unit)
                        break

                else:
                    self.variants.append(genie_unit)

            else:
                self.variants.append(genie_unit)

    def contains_unit(self, object_id):
        """
        Returns True if a unit with unit_id is part of the group.
        """
        obj = self.data.genie_units[object_id]

        return obj in self.variants

    def __repr__(self):
        return "GenieVariantGroup<%s>" % (self.get_id())


class GenieUnitTaskGroup(GenieUnitLineGroup):
    """
    Collection of genie units that have the same task group.

    Example: Male Villager, Female Villager

    The 'head unit' of a task group becomes the GameEntity, all
    the other are used to create more abilities with AnimationOverride.
    """

    # From unit connection
    male_line_id = 83   # male villager (with combat task)

    # Female villagers have no line obj_id, so we use the combat unit
    female_line_id = 293  # female villager (with combat task)

    def __init__(self, line_id, task_group_id, full_data_set):
        """
        Creates a new Genie task group.

        :param task_group_id: Internal task group obj_id in the .dat file.
        :param head_task_id: The unit with this task will become the head unit.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(line_id, full_data_set)

        self.task_group_id = task_group_id

    def is_creatable(self):
        """
        Task groups are creatable if any unit in the group is creatable.

        :returns: True if any train location obj_id is greater than zero.
        """
        for unit in self.line:
            train_location_id = unit.get_member("train_location_id").get_value()
            # -1 = no train location
            if train_location_id > -1:
                return True

        return False

    def get_train_location(self):
        """
        Returns the group_id for building line if the task group is
        creatable, otherwise return None.
        """
        for unit in self.line:
            train_location_id = unit.get_member("train_location_id").get_value()
            # -1 = no train location
            if train_location_id > -1:
                return train_location_id

        return None

    def __repr__(self):
        return "GenieUnitTaskGroup<%s>" % (self.get_id())


class GenieVillagerGroup(GenieUnitLineGroup):
    """
    Special collection of task groups for villagers.

    Villagers come in two task groups (male/female) and will form
    variants of the common villager game entity.
    """

    valid_switch_tasks_lookup = {
        5: "GATHER",    # Gather from resource spots
        7: "COMBAT",    # Attack
        101: "BUILD",   # Build buildings
        106: "REPAIR",  # Repair buildings, ships, rams
        110: "HUNT",    # Hunt animals, Chop trees
    }

    def __init__(self, group_id, task_group_ids, full_data_set):
        """
        Creates a new Genie villager group.

        :param group_id: Unit obj_id for the villager unit that is referenced by buildings
                         (in AoE2: 118 = male builder).
        :param task_group_ids: Internal task group ids in the .dat file.
                               (as a list of integers)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """
        super().__init__(group_id, full_data_set)

        self.data = full_data_set

        # Reference to the variant task groups
        self.variants = []
        for task_group_id in task_group_ids:
            task_group = self.data.task_groups[task_group_id]
            self.variants.append(task_group)

        # List of buildings that units can create
        self.creates = []

    def is_creatable(self):
        """
        Villagers are creatable if any of their variant task groups are creatable.

        :returns: True if any train location obj_id is greater than zero.
        """
        for variant in self.variants:
            if variant.is_creatable():
                return True

        return False

    def get_head_unit_id(self):
        """
        For villagers, this returns the group obj_id.
        """
        return self.get_id()

    def get_train_location(self):
        """
        Returns the group_id for building line if the task group is
        creatable, otherwise return None.
        """
        for variant in self.variants:
            if variant.is_creatable():
                return variant.get_train_location()

        return None

    def __repr__(self):
        return "GenieVillagerGroup<%s>" % (self.get_id())
