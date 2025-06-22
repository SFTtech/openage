# Copyright 2019-2025 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-lines,too-many-public-methods,too-many-instance-attributes,consider-iterating-dictionary

"""
Contains structures and API-like objects for game entities from AoC.
"""
from __future__ import annotations
import typing

from enum import Enum

from ..converter_object import ConverterObject, ConverterObjectGroup

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer
    from openage.convert.entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup
    from openage.convert.value_object.read.value_members import ValueMember


class GenieUnitObject(ConverterObject):
    """
    Ingame object in AoE2.
    """

    __slots__ = ('data',)

    def __init__(
        self,
        unit_id: int,
        full_data_set: GenieObjectContainer,
        members: dict[str, ValueMember] = None
    ):
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
        return f"GenieUnitObject<{self.get_id()}>"


class GenieGameEntityGroup(ConverterObjectGroup):
    """
    A collection of GenieUnitObject types that form an "upgrade line"
    in Age of Empires. This acts as the common super class for
    units and buildings.

    The first unit in the line will become the GameEntity, the rest will
    be patches to that GameEntity applied by Techs.
    """

    __slots__ = (
        'data',
        'line',
        'line_positions',
        'creates',
        'researches',
        'garrison_entities',
        'garrison_locations',
        'repairable'
    )

    def __init__(
        self,
        line_id: int,
        full_data_set: GenieObjectContainer,
    ):
        """
        Creates a new Genie game entity line.

        :param line_id: Internal line obj_id in the .dat file.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(line_id)

        # Reference to everything else in the gamedata
        self.data = full_data_set

        # The line is stored as an ordered list of GenieUnitObjects.
        self.line: list[GenieUnitObject] = []

        # This dict stores unit ids and their repective position in the
        # unit line for quick referencing and searching
        self.line_positions: dict[int, int] = {}

        # List of units/buildings that the line can create
        self.creates: list[GenieGameEntityGroup] = []

        # List of GenieTechEffectBundleGroup objects
        self.researches: list[GenieTechEffectBundleGroup] = []

        # List of units that the line can garrison
        self.garrison_entities: list[GenieGameEntityGroup] = []

        # List of units/buildings where the line can garrison
        self.garrison_locations: list[GenieGameEntityGroup] = []

        # Can be repaired by villagers
        self.repairable = False

    def add_creatable(self, line: GenieGameEntityGroup) -> None:
        """
        Adds another line to the list of creatables.

        :param line: The GenieBuildingLineGroup the villager produces.
        """
        if not self.contains_creatable(line.get_head_unit_id()):
            self.creates.append(line)

    def add_researchable(self, tech_group: GenieTechEffectBundleGroup) -> None:
        """
        Adds a tech group to the list of researchables.

        :param tech_group: The GenieTechLineGroup the building researches.
        """
        if not self.contains_researchable(tech_group.get_id()):
            self.researches.append(tech_group)

    def add_unit(
        self,
        genie_unit: GenieUnitObject,
        position: int = -1,
        after: GenieUnitObject = None
    ) -> None:
        """
        Adds a unit/building to the line.

        :param genie_unit: A GenieUnit object that is part of this
                           line.
        :param position: Puts the unit at an specific position in the line.
                         If this is -1, the unit is placed at the end.
        :param after: ID of a unit after which the new unit is
                      placed in the line. If a unit with this obj_id
                      is not present, the unit is appended at the end
                      of the line.
        """
        unit_id = genie_unit["id0"].value

        # Only add unit if it is not already in the list
        if not self.contains_entity(unit_id):
            insert_index = len(self.line)

            if position > -1:
                insert_index = position

                for unit, line_pos in self.line_positions.items():
                    if line_pos >= insert_index:
                        self.line_positions[unit] += 1

            elif after:
                if self.contains_entity(after):
                    insert_index = self.line_positions[after] + 1

                    for unit, line_pos in self.line_positions.items():
                        if line_pos >= insert_index:
                            self.line_positions[unit] += 1

            self.line_positions.update({genie_unit.get_id(): insert_index})
            self.line.insert(insert_index, genie_unit)

    def contains_creatable(self, line_id: int) -> bool:
        """
        Returns True if a line with line_id is a creatable of
        this unit.
        """
        if isinstance(self, GenieUnitLineGroup):
            line = self.data.building_lines[line_id]

        elif isinstance(self, GenieBuildingLineGroup):
            line = self.data.unit_lines[line_id]

        else:
            raise ValueError(f"Unknown creatable line ID {line_id} for {repr(self)}")

        return line in self.creates

    def contains_entity(self, unit_id: int) -> bool:
        """
        Returns True if a entity with unit_id is part of the line.
        """
        return unit_id in self.line_positions.keys()

    def contains_researchable(self, line_id: int) -> bool:
        """
        Returns True if a tech line with line_id is researchable
        in this building.
        """
        tech_line = self.data.tech_groups[line_id]

        return tech_line in self.researches

    def has_armor(self, armor_class: int) -> bool:
        """
        Checks if units in the line have a specific armor class.

        :param armor_class: The type of armor class searched for.
        :type armor_class: int
        :returns: True if the train location obj_id is greater than zero.
        """
        head_unit = self.get_head_unit()
        armors = head_unit["armors"].value
        for armor in armors.values():
            type_id = armor["type_id"].value

            if type_id == armor_class:
                return True

        return False

    def has_attack(self, armor_class: int) -> bool:
        """
        Checks if units in the line can execute a specific command.

        :param armor_class: The type of attack class searched for.
        :type armor_class: int
        :returns: True if the train location obj_id is greater than zero.
        """
        head_unit = self.get_head_unit()
        attacks = head_unit["attacks"].value
        for attack in attacks.values():
            type_id = attack["type_id"].value

            if type_id == armor_class:
                return True

        return False

    def has_command(self, command_id: int, civ_id: int = -1) -> bool:
        """
        Checks if units in the line can execute a specific command.

        :param command_id: The type of command searched for.
        :type command_id: int
        :param civ_id: Test if the property is true for unit lines
                       of the civ with this ID:
        :type civ_id: int
        :returns: True if the train location obj_id is greater than zero.
        """
        head_unit = self.get_head_unit()
        if civ_id != -1:
            head_unit = self.data.civ_groups[civ_id]["units"][self.get_head_unit_id()]

        commands = head_unit["unit_commands"].value
        for command in commands:
            type_id = command["type"].value

            if type_id == command_id:
                return True

        return False

    def has_projectile(self, projectile_id: int, civ_id: int = -1) -> bool:
        """
        Checks if units shoot a projectile with this ID.

        :param projectile_id: The ID of the projectile unit.
        :type projectile_id: int
        :param civ_id: Test if the property is true for unit lines
                       of the civ with this ID:
        :type civ_id: int
        :returns: True if the train location obj_id is greater than zero.
        """
        head_unit = self.get_head_unit()
        if civ_id != -1:
            head_unit = self.data.civ_groups[civ_id]["units"][self.get_head_unit_id()]

        projectile_id_0 = head_unit["projectile_id0"].value
        projectile_id_1 = -2
        if head_unit.has_member("projectile_id1"):
            projectile_id_1 = head_unit["projectile_id1"].value

        return projectile_id in (projectile_id_0, projectile_id_1)

    def is_creatable(self, civ_id: int = -1) -> bool:
        """
        Units/Buildings are creatable if they have a valid train location.

        :param civ_id: Test if the property is true for unit lines
                       of the civ with this ID:
        :type civ_id: int
        :returns: True if the train location obj_id is greater than zero.
        """
        # Get the train location obj_id for the first unit in the line
        head_unit = self.get_head_unit()
        if civ_id != -1:
            head_unit = self.data.civ_groups[civ_id]["units"][self.get_head_unit_id()]

        train_location_id = head_unit["train_location_id"].value

        # -1 = no train location
        return train_location_id > -1

    def is_harvestable(self, civ_id: int = -1) -> bool:
        """
        Checks whether the group holds any of the 4 main resources Food,
        Wood, Gold and Stone.

        :param civ_id: Test if the property is true for unit lines
                       of the civ with this ID:
        :type civ_id: int
        :returns: True if the group contains at least one resource storage.
        """
        head_unit = self.get_head_unit()
        if civ_id != -1:
            head_unit = self.data.civ_groups[civ_id]["units"][self.get_head_unit_id()]

        for resource_storage in head_unit["resource_storage"].value:
            type_id = resource_storage["type"].value

            if type_id in (0, 1, 2, 3, 15, 16, 17):
                return True

        return False

    def is_garrison(self, civ_id: int = -1) -> bool:
        """
        Checks whether the group can garrison other entities. This covers
        all of these garrisons:

        - Natural garrisons (castle, town center, tower)
        - Production garrisons (barracks, archery range, stable, etc.)
        - Unit garrisons (ram, siege tower)
        - Transport ships

        This does not include kidnapping units!

        :param civ_id: Test if the property is true for unit lines
                       of the civ with this ID:
        :type civ_id: int
        :returns: True if the group falls into the above categories.
        """
        head_unit = self.get_head_unit()
        if civ_id != -1:
            head_unit = self.data.civ_groups[civ_id]["units"][self.get_head_unit_id()]

        trait = head_unit["trait"].value

        # Transport ship/ram
        if trait & 0x01:
            return True

        # Production garrison
        type_id = head_unit["unit_type"].value
        if len(self.creates) > 0 and type_id == 80:
            return True

        # Natural garrison
        if head_unit.has_member("garrison_type"):
            garrison_type = head_unit["garrison_type"].value

            if garrison_type > 0:
                return True

        return False

    def is_gatherer(self, civ_id: int = -1) -> bool:
        """
        Checks whether the group has any gather abilities.

        :param civ_id: Test if the property is true for unit lines
                       of the civ with this ID:
        :type civ_id: int
        :returns: True if the group contains at least one resource storage.
        """
        return self.has_command(5, civ_id=civ_id)

    def is_passable(self, civ_id: int = -1) -> bool:
        """
        Checks whether the group has a passable hitbox.

        :param civ_id: Test if the property is true for unit lines
                       of the civ with this ID:
        :type civ_id: int
        :returns: True if the group has obstruction type 0.
        """
        head_unit = self.get_head_unit()
        if civ_id != -1:
            head_unit = self.data.civ_groups[civ_id]["units"][self.get_head_unit_id()]

        return head_unit["obstruction_type"].value == 0

    def is_projectile_shooter(self, civ_id: int = -1) -> bool:
        """
        Units/Buildings are projectile shooters if they have assigned a projectile ID.

        :param civ_id: Test if the property is true for unit lines
                       of the civ with this ID:
        :type civ_id: int
        :returns: True if one of the projectile IDs is greater than zero.
        """
        head_unit = self.get_head_unit()
        if civ_id != -1:
            head_unit = self.data.civ_groups[civ_id]["units"][self.get_head_unit_id()]

        if not head_unit.has_member("projectile_id0"):
            return False

        # Get the projectiles' obj_id for the first unit in the line. AoE's
        # units stay ranged with upgrades, so this should be fine.
        projectile_id_0 = head_unit["projectile_id0"].value

        projectile_id_1 = -1
        if head_unit.has_member("projectile_id1"):
            projectile_id_1 = head_unit["projectile_id1"].value

        # -1 -> no projectile
        return projectile_id_0 > -1 or projectile_id_1 > -1

    def is_ranged(self, civ_id: int = -1) -> bool:
        """
        Groups are ranged if their maximum range is greater than 0.

        :param civ_id: Test if the property is true for unit lines
                       of the civ with this ID:
        :type civ_id: int
        :returns: True if the group's max range is greater than 0.
        """
        head_unit = self.get_head_unit()
        if civ_id != -1:
            head_unit = self.data.civ_groups[civ_id]["units"][self.get_head_unit_id()]

        return head_unit["weapon_range_max"].value > 0

    def is_melee(self, civ_id: int = -1) -> bool:
        """
        Groups are melee if they have a Combat ability and are not ranged units.

        :param civ_id: Test if the property is true for unit lines
                       of the civ with this ID:
        :type civ_id: int
        :returns: True if the group is not ranged and has a combat ability.
        """
        return self.has_command(7, civ_id=civ_id)

    def is_repairable(self) -> bool:
        """
        Only certain lines and classes are repairable.

        :returns: True if the group is repairable.
        """
        return self.repairable

    def is_unique(self) -> bool:
        """
        Groups are unique if they belong to a specific civ.

        :returns: True if the group is tied to one specific civ.
        """
        # Get the enabling research obj_id for the first unit in the line
        head_unit = self.get_head_unit()
        head_unit_id = head_unit["id0"].value

        if isinstance(self, GenieUnitLineGroup):
            if head_unit_id in self.data.unit_connections.keys():
                head_unit_connection = self.data.unit_connections[head_unit_id]

            else:
                # Animals or AoE1
                return False

        elif isinstance(self, GenieBuildingLineGroup):
            if head_unit_id in self.data.building_connections.keys():
                head_unit_connection = self.data.building_connections[head_unit_id]

            else:
                # AoE1
                return False
        else:
            raise ValueError(f"Unknown group type for {repr(self)}")

        enabling_research_id = head_unit_connection["enabling_research"].value

        # does not need to be enabled -> not unique
        if enabling_research_id == -1:
            return False

        # Get enabling civ
        enabling_research = self.data.genie_techs[enabling_research_id]
        enabling_civ_id = enabling_research["civilization_id"].value

        # Enabling tech has no specific civ -> not unique
        return enabling_civ_id > -1

    def get_class_id(self) -> int:
        """
        Return the class ID for units in the group.
        """
        return self.get_head_unit()["unit_class"].value

    def get_garrison_mode(self, civ_id: int = -1) -> GenieGarrisonMode:
        """
        Returns the mode the garrison operates in. This is used by the
        converter to determine which storage abilities the line will get.

        :param civ_id: Get the garrison mode for unit lines of the civ
                       with this ID.
        :type civ_id: int
        :returns: The garrison mode of the line.
        :rtype: GenieGarrisonMode
        """
        head_unit = self.get_head_unit()
        if civ_id != -1:
            head_unit = self.data.civ_groups[civ_id]["units"][self.get_head_unit_id()]

        trait = head_unit["trait"].value

        # Ram
        if trait == 1:
            return GenieGarrisonMode.UNIT_GARRISON

        # Transport ship
        if trait == 3:
            return GenieGarrisonMode.TRANSPORT

        # Natural garrison
        if head_unit.has_member("garrison_type"):
            garrison_type = head_unit["garrison_type"].value

            if garrison_type > 0:
                return GenieGarrisonMode.NATURAL

        # Production garrison
        type_id = head_unit["unit_type"].value
        if len(self.creates) > 0 and type_id == 80:
            return GenieGarrisonMode.SELF_PRODUCED

        return None

    def get_head_unit_id(self) -> int:
        """
        Return the obj_id of the first unit in the line.
        """
        head_unit = self.get_head_unit()
        return head_unit["id0"].value

    def get_head_unit(self) -> GenieUnitObject:
        """
        Return the first unit in the line.
        """
        return self.line[0]

    def get_unit_position(self, unit_id: int) -> int:
        """
        Return the position of a unit in the line.
        """
        return self.line_positions[unit_id]

    def get_train_location_id(self) -> typing.Union[int, None]:
        """
        Returns the group_id for building line if the unit is
        creatable, otherwise return None.
        """
        if self.is_creatable():
            head_unit = self.get_head_unit()
            return head_unit["train_location_id"].value

        return None

    def __repr__(self):
        return f"GenieGameEntityGroup<{self.get_id()}>"


class GenieUnitLineGroup(GenieGameEntityGroup):
    """
    A collection of GenieUnitObject types that form an "upgrade line"
    in Age of Empires.

    Example: Spearman->Pikeman->Helbardier

    The first unit in the line will become the GameEntity, the rest will
    be patches to that GameEntity applied by Techs.
    """

    def contains_unit(self, unit_id: int) -> bool:
        """
        Returns True if a unit with unit_id is part of the line.
        """
        return self.contains_entity(unit_id)

    def get_civ_id(self) -> typing.Union[int, None]:
        """
        Returns the enabling civ obj_id if the unit is unique,
        otherwise return None.
        """
        if self.is_unique():
            head_unit = self.get_head_unit()
            head_unit_id = head_unit["id0"].value
            head_unit_connection = self.data.unit_connections[head_unit_id]
            enabling_research_id = head_unit_connection["enabling_research"].value

            enabling_research = self.data.genie_techs[enabling_research_id]
            return enabling_research["civilization_id"].value

        return None

    def get_enabling_research_id(self) -> int:
        """
        Returns the enabling tech id of the unit

        TODO: Find enabling research ID in pre-processor and save it in the group
              like we do for RoR. Not all creatable units must be present in the
              unit connections.
        TODO: Move function into GeneGameEntityGroup after doing the above.
        """
        head_unit = self.get_head_unit()
        head_unit_id = head_unit["id0"].value

        if head_unit_id not in self.data.unit_connections.keys():
            # TODO: Remove this check, see TODOs above
            return -1

        head_unit_connection = self.data.unit_connections[head_unit_id]
        enabling_research_id = head_unit_connection["enabling_research"].value

        return enabling_research_id

    def __repr__(self):
        return f"GenieUnitLineGroup<{self.get_id()}>"


class GenieBuildingLineGroup(GenieGameEntityGroup):
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

    __slots__ = ('gatherer_ids', 'trades_with')

    def __init__(
        self,
        line_id: int,
        full_data_set: GenieObjectContainer,
    ):
        super().__init__(line_id, full_data_set)

        # IDs of gatherers that drop off resources here
        self.gatherer_ids: set[int] = set()

        # Unit lines that this building trades with
        self.trades_with: set[GenieGameEntityGroup] = []

    def add_gatherer_id(self, unit_id: int) -> None:
        """
        Adds Id of gatherers that drop off resources at this building.

        :param unit_id: ID of the gatherer.
        """
        self.gatherer_ids.add(unit_id)

    def add_trading_line(self, unit_line: GenieGameEntityGroup) -> None:
        """
        Adds a reference to a line that trades with this building.

        :param unit_line: Line that trades with this building.
        """
        self.trades_with.append(unit_line)

    def contains_unit(self, building_id: int) -> bool:
        """
        Returns True if a building with building_id is part of the line.
        """
        return self.contains_entity(building_id)

    def has_foundation(self) -> bool:
        """
        Returns True if the building has a foundation terrain.
        """
        head_unit = self.get_head_unit()
        return head_unit["foundation_terrain_id"].value > -1

    def is_dropsite(self) -> bool:
        """
        Returns True if the building accepts resources.
        """
        return len(self.gatherer_ids) > 0

    def is_repairable(self) -> bool:
        return True

    def is_trade_post(self) -> bool:
        """
        Returns True if the building is traded with.
        """
        return len(self.trades_with) > 0

    def get_gatherer_ids(self) -> set[int]:
        """
        Returns gatherer unit IDs that drop off resources at this building.
        """
        return self.gatherer_ids

    def get_enabling_research_id(self) -> int:
        """
        Returns the enabling tech id of the unit
        """
        head_unit = self.get_head_unit()
        head_unit_id = head_unit["id0"].value
        if head_unit_id in self.data.building_connections.keys():
            head_unit_connection = self.data.building_connections[head_unit_id]
            enabling_research_id = head_unit_connection["enabling_research"].value

        else:
            # Assume it is avialable from the start
            enabling_research_id = -1

        return enabling_research_id

    def __repr__(self):
        return f"GenieBuildingLineGroup<{self.get_id()}>"


class GenieStackBuildingGroup(GenieBuildingLineGroup):
    """
    Buildings that stack with other units and have annexes. These buildings
    are replaced by their stack unit once built.

    Examples: Gate, Town Center

    The 'stack unit' becomes the GameEntity, the 'head unit' will be a state
    during construction.
    """

    __slots__ = ('head', 'stack')

    def __init__(
        self,
        stack_unit_id: int,
        head_building_id: int,
        full_data_set: GenieObjectContainer,
    ):
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

    def is_creatable(self, civ_id: int = -1) -> bool:
        """
        Stack buildings are created through their head building. We have to
        lookup its values.

        :returns: True if the train location obj_id is greater than zero.
        """
        train_location_id = self.head["train_location_id"].value

        # -1 = no train location
        if train_location_id == -1:
            return False

        return True

    def is_gate(self) -> bool:
        """
        Checks if the building has gate properties.

        :returns: True if the building has obstruction class 4.
        """
        head_unit = self.get_head_unit()
        return head_unit["obstruction_class"].value == 4

    def get_head_unit(self) -> GenieUnitObject:
        """
        Return the first unit in the line.
        """
        return self.head

    def get_stack_unit(self) -> GenieUnitObject:
        """
        Returns the unit that is stacked on this building after construction.
        """
        return self.stack

    def get_head_unit_id(self) -> int:
        """
        Returns the stack unit ID because that is the unit that is referenced by other entities.
        """
        return self.get_stack_unit_id()

    def get_stack_unit_id(self) -> int:
        """
        Returns the stack unit ID.
        """
        return self.stack["id0"].value

    def get_train_location_id(self) -> int:
        """
        Stack buildings are creatable when their head building is creatable.

        Returns the group_id for a villager group if the head building is
        creatable, otherwise return None.
        """
        if self.is_creatable():
            return self.head["train_location_id"].value

        return None

    def __repr__(self):
        return f"GenieStackBuildingGroup<{self.get_id()}>"


class GenieUnitTransformGroup(GenieUnitLineGroup):
    """
    Collection of genie units that reference each other with their
    transform_id.

    Example: Trebuchet
    """

    __slots__ = ('head_unit', 'transform_unit')

    def __init__(
        self,
        line_id: int,
        head_unit_id: int,
        full_data_set: GenieObjectContainer,
    ):
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

        transform_id = self.head_unit["transform_unit_id"].value
        self.transform_unit = self.data.genie_units[transform_id]

    def is_projectile_shooter(self, civ_id: int = -1) -> int:
        """
        Transform groups are projectile shooters if their head or transform units
        have assigned a projectile ID.

        :returns: True if one of the projectile IDs is greater than zero.
        """
        projectile_id_0 = self.head_unit["projectile_id0"].value
        projectile_id_1 = self.head_unit["projectile_id1"].value
        projectile_id_2 = self.transform_unit["projectile_id0"].value
        projectile_id_3 = self.transform_unit["projectile_id1"].value

        # -1 -> no projectile
        return (projectile_id_0 > -1 or projectile_id_1 > -1
                or projectile_id_2 > -1 or projectile_id_3 > -1)

    def get_head_unit_id(self) -> int:
        """
        Returns the ID of the head unit.
        """
        return self.head_unit["id0"].value

    def get_head_unit(self) -> GenieUnitObject:
        """
        Returns the head unit.
        """
        return self.head_unit

    def get_transform_unit_id(self) -> int:
        """
        Returns the ID of the transform unit.
        """
        return self.transform_unit["id0"].value

    def get_transform_unit(self) -> GenieUnitObject:
        """
        Returns the transform unit.
        """
        return self.transform_unit

    def __repr__(self):
        return f"GenieUnitTransformGroup<{self.get_id()}>"


class GenieMonkGroup(GenieUnitLineGroup):
    """
    Collection of monk and monk with relic. The switch
    is hardcoded in AoE2. (Missionaries are handled as normal lines
    because they cannot pick up relics).

    The 'head unit' will become the GameEntity, the 'switch unit'
    will become a Container ability with CarryProgress.
    """

    __slots__ = ('head_unit', 'switch_unit')

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
        super().__init__(line_id, full_data_set)

        self.head_unit = self.data.genie_units[head_unit_id]
        self.switch_unit = self.data.genie_units[switch_unit_id]

    def is_garrison(self, civ_id: int = -1) -> bool:
        return True

    def get_garrison_mode(self, civ_id=-1) -> GenieGarrisonMode:
        return GenieGarrisonMode.MONK

    def get_switch_unit(self) -> GenieUnitObject:
        """
        Returns the unit that is switched to when picking up something.
        """
        return self.switch_unit

    def __repr__(self):
        return f"GenieMonkGroup<{self.get_id()}>"


class GenieAmbientGroup(GenieGameEntityGroup):
    """
    One Genie unit that is an ambient scenery object.
    Mostly for resources, specifically trees. For these objects
    every frame in their graphics file is a variant.

    Example: Trees, Gold mines, Sign
    """

    def contains_unit(self, ambient_id: int) -> bool:
        """
        Returns True if the ambient object with ambient_id is in this group.
        """
        return self.contains_entity(ambient_id)

    def is_creatable(self, civ_id: int = -1) -> bool:
        return False

    def is_gatherer(self, civ_id: int = -1) -> bool:
        return False

    def is_melee(self, civ_id: int = -1) -> bool:
        return False

    def is_ranged(self, civ_id: int = -1) -> bool:
        return False

    def is_projectile_shooter(self, civ_id: int = -1) -> bool:
        return False

    def is_unique(self) -> bool:
        return False

    def __repr__(self):
        return f"GenieAmbientGroup<{self.get_id()}>"


class GenieVariantGroup(GenieGameEntityGroup):
    """
    Collection of multiple Genie units that are variants of the same game entity.
    Mostly for cliffs and ambient terrain objects.

    Example: Cliffs, flowers, mountains
    """

    def contains_unit(self, object_id: int) -> bool:
        """
        Returns True if a unit with unit_id is part of the group.
        """
        return self.contains_entity(object_id)

    def is_creatable(self, civ_id: int = -1) -> bool:
        return False

    def is_gatherer(self, civ_id: int = -1) -> bool:
        return False

    def is_melee(self, civ_id: int = -1) -> bool:
        return False

    def is_ranged(self, civ_id: int = -1) -> bool:
        return False

    def is_unique(self) -> bool:
        return False

    def __repr__(self):
        return f"GenieVariantGroup<{self.get_id()}>"


class GenieUnitTaskGroup(GenieUnitLineGroup):
    """
    Collection of genie units that have the same task group.

    Example: Male Villager, Female Villager

    The 'head unit' of a task group becomes the GameEntity, all
    the other are used to create more abilities with AnimationOverride.
    """

    __slots__ = ('task_group_id',)

    # Maps task group ID to the line ID of the first unit in the group.
    line_id_assignments = {
        1: 83,   # male villager (with combat task)
        2: 293,  # female villager (with combat task)
        3: 13,   # fishing ship (in DE2)
    }

    def __init__(
        self,
        line_id: int,
        task_group_id: int,
        full_data_set: GenieObjectContainer,
    ):
        """
        Creates a new Genie task group.

        :param line_id: Internal task group obj_id in the .dat file.
        :param task_group_id: ID of the task group.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(line_id, full_data_set)

        self.task_group_id = task_group_id

    def add_unit(
        self,
        genie_unit: GenieUnitObject,
        position: int = -1,
        after: GenieUnitObject = None
    ) -> None:
        # Force the idle/combat units at the beginning of the line
        if genie_unit["id0"].value in self.line_id_assignments.values():
            super().add_unit(genie_unit, 0, after)

        else:
            super().add_unit(genie_unit, position, after)

    def is_creatable(self, civ_id: int = -1) -> bool:
        """
        Task groups are creatable if any unit in the group is creatable.

        :returns: True if any train location obj_id is greater than zero.
        """
        for unit in self.line:
            train_location_id = unit["train_location_id"].value
            # -1 = no train location
            if train_location_id > -1:
                return True

        return False

    def get_train_location_id(self) -> int:
        """
        Returns the group_id for building line if the task group is
        creatable, otherwise return None.
        """
        for unit in self.line:
            train_location_id = unit["train_location_id"].value
            # -1 = no train location
            if train_location_id > -1:
                return train_location_id

        return None

    def __repr__(self):
        return f"GenieUnitTaskGroup<{self.get_id()}>"


class GenieVillagerGroup(GenieUnitLineGroup):
    """
    Special collection of task groups for villagers.

    Villagers come in two task groups (male/female) and will form
    variants of the common villager game entity.
    """

    __slots__ = ('variants', )

    valid_switch_tasks_lookup = {
        5: "GATHER",    # Gather from resource spots
        7: "COMBAT",    # Attack
        101: "BUILD",   # Build buildings
        106: "REPAIR",  # Repair buildings, ships, rams
        110: "HUNT",    # Kill first, then gather
    }

    def __init__(
        self,
        group_id: int,
        task_group_ids: list[int],
        full_data_set: GenieObjectContainer,
    ):
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
        self.variants: list[GenieUnitTaskGroup] = []
        for task_group_id in task_group_ids:
            task_group = self.data.task_groups[task_group_id]
            self.variants.append(task_group)

        # List of buildings that units can create
        self.creates: list[GenieGameEntityGroup] = []

    def contains_entity(self, unit_id: int) -> bool:
        for task_group in self.variants:
            if task_group.contains_entity(unit_id):
                return True

        return False

    def has_command(self, command_id: int, civ_id: int = -1) -> bool:
        for variant in self.variants:
            for genie_unit in variant.line:
                commands = genie_unit["unit_commands"].value

                for command in commands:
                    type_id = command["type"].value

                    if type_id == command_id:
                        return True

        return False

    def is_creatable(self, civ_id: int = -1) -> bool:
        """
        Villagers are creatable if any of their variant task groups are creatable.

        :returns: True if any train location obj_id is greater than zero.
        """
        for variant in self.variants:
            if variant.is_creatable():
                return True

        return False

    def is_garrison(self, civ_id: int = -1) -> bool:
        return False

    def is_gatherer(self, civ_id: int = -1) -> bool:
        return True

    @classmethod
    def is_hunter(cls) -> bool:
        """
        Returns True if the unit hunts animals.
        """
        return True

    def is_unique(self) -> bool:
        return False

    def is_projectile_shooter(self, civ_id: int = -1) -> bool:
        return False

    def get_garrison_mode(self, civ_id: int = -1) -> bool:
        return None

    def get_head_unit_id(self) -> int:
        """
        For villagers, this returns the group obj_id.
        """
        return self.get_id()

    def get_head_unit(self) -> GenieUnitObject:
        """
        For villagers, this returns the group obj_id.
        """
        return self.variants[0].line[0]

    def get_units_with_command(self, command_id: int) -> GenieUnitObject:
        """
        Returns all genie units which have the specified command.
        """
        matching_units = []

        for variant in self.variants:
            for genie_unit in variant.line:
                commands = genie_unit["unit_commands"].value

                for command in commands:
                    type_id = command["type"].value

                    if type_id == command_id:
                        matching_units.append(genie_unit)

        return matching_units

    def get_train_location_id(self) -> int:
        """
        Returns the group_id for building line if the task group is
        creatable, otherwise return None.
        """
        for variant in self.variants:
            if variant.is_creatable():
                return variant.get_train_location_id()

        return None

    def __repr__(self):
        return f"GenieVillagerGroup<{self.get_id()}>"


class GenieGarrisonMode(Enum):
    """
    Garrison mode of a genie group. This should not be confused with
    the "garrison_type" from the .dat file. These garrison modes reflect
    how the garrison will be handled in the openage API.
    """
    # pylint: disable=line-too-long

    # Keys = all possible creatable types; may be specified further by other factors
    # The negative integers at the start of the tupe prevent Python from creating
    # aliases for the enums.
    NATURAL       = (-1, 1, 2, 3, 5, 6)  # enter/exit/remove; rally point
    # enter/exit/remove; no cavalry/monks; speedboost for infantry; no rally point
    UNIT_GARRISON = (-2, 1, 2, 5)
    TRANSPORT     = (-3, 1, 2, 3, 5, 6)  # enter/exit/remove; no rally point
    # enter only with OwnStorage; exit/remove; only produced units; rally point
    SELF_PRODUCED = (-4, 1, 2, 3, 5, 6)
    MONK          = (-5, 4,)             # remove/collect/transfer; only relics; no rally point
