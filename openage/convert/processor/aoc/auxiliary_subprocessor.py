# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Derives complex auxiliary objects from unit lines, techs
or other objects.
"""
from openage.convert.dataformat.aoc.genie_unit import GenieVillagerGroup,\
    GenieBuildingLineGroup, GenieUnitLineGroup
from openage.convert.dataformat.aoc.internal_nyan_names import UNIT_LINE_LOOKUPS,\
    BUILDING_LINE_LOOKUPS, TECH_GROUP_LOOKUPS, CIV_GROUP_LOOKUPS
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.aoc.combined_sound import CombinedSound
from openage.nyan.nyan_structs import MemberSpecialValue


class AoCAuxiliarySubprocessor:

    @staticmethod
    def get_creatable_game_entity(line):
        """
        Creates the CreatableGameEntity object for a unit/building line.

        :param line: Unit/Building line.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        """
        if isinstance(line, GenieVillagerGroup):
            current_unit = line.variants[0].line[0]

        else:
            current_unit = line.line[0]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            game_entity_name = BUILDING_LINE_LOOKUPS[current_unit_id][0]

        else:
            game_entity_name = UNIT_LINE_LOOKUPS[current_unit_id][0]

        obj_ref = "%s.CreatableGameEntity" % (game_entity_name)
        obj_name = "%sCreatable" % (game_entity_name)
        creatable_raw_api_object = RawAPIObject(obj_ref, obj_name, dataset.nyan_api_objects)
        creatable_raw_api_object.add_raw_parent("engine.aux.create.CreatableGameEntity")

        # Get train location of line
        train_location_id = line.get_train_location_id()
        if isinstance(line, GenieBuildingLineGroup):
            train_location = dataset.unit_lines[train_location_id]
            train_location_name = UNIT_LINE_LOOKUPS[train_location_id][0]

        else:
            train_location = dataset.building_lines[train_location_id]
            train_location_name = BUILDING_LINE_LOOKUPS[train_location_id][0]

        # Location of the object depends on whether it'a a unique unit or a normal unit
        if line.is_unique():
            # Add object to the Civ object
            if isinstance(line, GenieUnitLineGroup):
                head_unit_connection = dataset.unit_connections[current_unit_id]

            elif isinstance(line, GenieBuildingLineGroup):
                head_unit_connection = dataset.building_connections[current_unit_id]

            enabling_research_id = head_unit_connection.get_member("enabling_research").get_value()
            enabling_research = dataset.genie_techs[enabling_research_id]
            enabling_civ_id = enabling_research.get_member("civilization_id").get_value()

            civ = dataset.civ_groups[enabling_civ_id]
            civ_name = CIV_GROUP_LOOKUPS[enabling_civ_id][0]

            creatable_location = ExpectedPointer(civ, civ_name)

        else:
            # Add object to the train location's Create ability
            creatable_location = ExpectedPointer(train_location,
                                                 "%s.Create" % (train_location_name))

        creatable_raw_api_object.set_location(creatable_location)

        # Game Entity
        game_entity_expected_pointer = ExpectedPointer(line, game_entity_name)
        creatable_raw_api_object.add_raw_member("game_entity",
                                                game_entity_expected_pointer,
                                                "engine.aux.create.CreatableGameEntity")

        # Cost
        cost_name = "%s.CreatableGameEntity.%sCost" % (game_entity_name, game_entity_name)
        cost_raw_api_object = RawAPIObject(cost_name,
                                           "%sCost" % (game_entity_name),
                                           dataset.nyan_api_objects)
        cost_raw_api_object.add_raw_parent("engine.aux.cost.type.ResourceCost")
        creatable_expected_pointer = ExpectedPointer(line, obj_ref)
        cost_raw_api_object.set_location(creatable_expected_pointer)

        payment_mode = dataset.nyan_api_objects["engine.aux.payment_mode.type.Advance"]
        cost_raw_api_object.add_raw_member("payment_mode",
                                           payment_mode,
                                           "engine.aux.cost.Cost")

        cost_amounts = []
        for resource_amount in current_unit.get_member("resource_cost").get_value():
            resource_id = resource_amount.get_value()["type_id"].get_value()

            resource = None
            resource_name = ""
            if resource_id == -1:
                # Not a valid resource
                continue

            elif resource_id == 0:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Food"].get_nyan_object()
                resource_name = "Food"

            elif resource_id == 1:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Wood"].get_nyan_object()
                resource_name = "Wood"

            elif resource_id == 2:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Stone"].get_nyan_object()
                resource_name = "Stone"

            elif resource_id == 3:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Gold"].get_nyan_object()
                resource_name = "Gold"

            else:
                # Other resource ids are handled differently
                continue

            # Skip resources that are only expected to be there
            if not resource_amount.get_value()["enabled"].get_value():
                continue

            amount = resource_amount.get_value()["amount"].get_value()

            cost_amount_name = "%s.%sAmount" % (cost_name, resource_name)
            cost_amount = RawAPIObject(cost_amount_name,
                                       "%sAmount" % resource_name,
                                       dataset.nyan_api_objects)
            cost_amount.add_raw_parent("engine.aux.resource.ResourceAmount")
            cost_expected_pointer = ExpectedPointer(line, cost_name)
            cost_amount.set_location(cost_expected_pointer)

            cost_amount.add_raw_member("type",
                                       resource,
                                       "engine.aux.resource.ResourceAmount")
            cost_amount.add_raw_member("amount",
                                       amount,
                                       "engine.aux.resource.ResourceAmount")

            cost_amount_expected_pointer = ExpectedPointer(line, cost_amount_name)
            cost_amounts.append(cost_amount_expected_pointer)
            line.add_raw_api_object(cost_amount)

        cost_raw_api_object.add_raw_member("amount",
                                           cost_amounts,
                                           "engine.aux.cost.type.ResourceCost")

        cost_expected_pointer = ExpectedPointer(line, cost_name)
        creatable_raw_api_object.add_raw_member("cost",
                                                cost_expected_pointer,
                                                "engine.aux.create.CreatableGameEntity")
        # Creation time
        if isinstance(line, GenieUnitLineGroup):
            creation_time = current_unit.get_member("creation_time").get_value()

        else:
            # Buildings are created immediately
            creation_time = 0

        creatable_raw_api_object.add_raw_member("creation_time",
                                                creation_time,
                                                "engine.aux.create.CreatableGameEntity")

        # Creation sound
        creation_sound_id = current_unit.get_member("train_sound_id").get_value()

        # Create sound object
        obj_name = "%s.CreatableGameEntity.Sound" % (game_entity_name)
        sound_raw_api_object = RawAPIObject(obj_name, "CreationSound",
                                            dataset.nyan_api_objects)
        sound_raw_api_object.add_raw_parent("engine.aux.sound.Sound")
        sound_location = ExpectedPointer(line,
                                         "%s.CreatableGameEntity" % (game_entity_name))
        sound_raw_api_object.set_location(sound_location)

        # Search for the sound if it exists
        creation_sounds = []
        if creation_sound_id > -1:
            # Creation sound should be civ agnostic
            genie_sound = dataset.genie_sounds[creation_sound_id]
            file_id = genie_sound.get_sounds(civ_id=-1)[0]

            if file_id in dataset.combined_sounds:
                creation_sound = dataset.combined_sounds[file_id]
                creation_sound.add_reference(sound_raw_api_object)

            else:
                creation_sound = CombinedSound(creation_sound_id,
                                               file_id,
                                               "creation_sound_%s" % (creation_sound_id),
                                               dataset)
                dataset.combined_sounds.update({file_id: creation_sound})
                creation_sound.add_reference(sound_raw_api_object)

            creation_sounds.append(creation_sound)

        sound_raw_api_object.add_raw_member("play_delay",
                                            0,
                                            "engine.aux.sound.Sound")
        sound_raw_api_object.add_raw_member("sounds",
                                            creation_sounds,
                                            "engine.aux.sound.Sound")

        sound_expected_pointer = ExpectedPointer(line, obj_name)
        creatable_raw_api_object.add_raw_member("creation_sounds",
                                                [sound_expected_pointer],
                                                "engine.aux.create.CreatableGameEntity")

        line.add_raw_api_object(sound_raw_api_object)

        # TODO: Condition
        unlock_condition = []
        creatable_raw_api_object.add_raw_member("condition",
                                                unlock_condition,
                                                "engine.aux.create.CreatableGameEntity")

        # Placement modes
        placement_modes = []
        if isinstance(line, GenieBuildingLineGroup):
            # Buildings are placed on the map
            # Place mode
            obj_name = "%s.CreatableGameEntity.Place" % (game_entity_name)
            place_raw_api_object = RawAPIObject(obj_name,
                                                "Place",
                                                dataset.nyan_api_objects)
            place_raw_api_object.add_raw_parent("engine.aux.placement_mode.type.Place")
            place_location = ExpectedPointer(line,
                                             "%s.CreatableGameEntity" % (game_entity_name))
            place_raw_api_object.set_location(place_location)

            # Tile snap distance (uses 1.0 for grid placement)
            place_raw_api_object.add_raw_member("tile_snap_distance",
                                                1.0,
                                                "engine.aux.placement_mode.type.Place")
            # Clearance size
            clearance_size_x = current_unit.get_member("clearance_size_x").get_value()
            clearance_size_y = current_unit.get_member("clearance_size_y").get_value()
            place_raw_api_object.add_raw_member("clearance_size_x",
                                                clearance_size_x,
                                                "engine.aux.placement_mode.type.Place")
            place_raw_api_object.add_raw_member("clearance_size_y",
                                                clearance_size_y,
                                                "engine.aux.placement_mode.type.Place")

            # Max elevation difference
            elevation_mode = current_unit.get_member("elevation_mode").get_value()
            if elevation_mode == 2:
                max_elevation_difference = 0

            elif elevation_mode == 3:
                max_elevation_difference = 1

            else:
                max_elevation_difference = MemberSpecialValue.NYAN_INF

            place_raw_api_object.add_raw_member("max_elevation_difference",
                                                max_elevation_difference,
                                                "engine.aux.placement_mode.type.Place")

            line.add_raw_api_object(place_raw_api_object)

            place_expected_pointer = ExpectedPointer(line, obj_name)
            placement_modes.append(place_expected_pointer)

            if line.get_class_id() == 39:
                # Gates
                obj_name = "%s.CreatableGameEntity.Replace" % (game_entity_name)
                replace_raw_api_object = RawAPIObject(obj_name,
                                                      "Replace",
                                                      dataset.nyan_api_objects)
                replace_raw_api_object.add_raw_parent("engine.aux.placement_mode.type.Replace")
                replace_location = ExpectedPointer(line,
                                                   "%s.CreatableGameEntity" % (game_entity_name))
                replace_raw_api_object.set_location(replace_location)

                # Game entities (only stone wall)
                wall_line_id = 117
                wall_line = dataset.building_lines[wall_line_id]
                wall_name = BUILDING_LINE_LOOKUPS[117][0]
                game_entities = [ExpectedPointer(wall_line, wall_name)]
                replace_raw_api_object.add_raw_member("game_entities",
                                                      game_entities,
                                                      "engine.aux.placement_mode.type.Replace")

                line.add_raw_api_object(replace_raw_api_object)

                replace_expected_pointer = ExpectedPointer(line, obj_name)
                placement_modes.append(replace_expected_pointer)

        else:
            placement_modes.append(dataset.nyan_api_objects["engine.aux.placement_mode.type.Eject"])

            # OwnStorage mode
            obj_name = "%s.CreatableGameEntity.OwnStorage" % (game_entity_name)
            own_storage_raw_api_object = RawAPIObject(obj_name, "OwnStorage",
                                                      dataset.nyan_api_objects)
            own_storage_raw_api_object.add_raw_parent("engine.aux.placement_mode.type.OwnStorage")
            own_storage_location = ExpectedPointer(line,
                                                   "%s.CreatableGameEntity" % (game_entity_name))
            own_storage_raw_api_object.set_location(own_storage_location)

            # Container
            container_expected_pointer = ExpectedPointer(train_location,
                                                         "%s.Storage.%sContainer"
                                                         % (train_location_name, train_location_name))
            own_storage_raw_api_object.add_raw_member("container",
                                                      container_expected_pointer,
                                                      "engine.aux.placement_mode.type.OwnStorage")

            line.add_raw_api_object(own_storage_raw_api_object)

            own_storage_expected_pointer = ExpectedPointer(line, obj_name)
            placement_modes.append(own_storage_expected_pointer)

        creatable_raw_api_object.add_raw_member("placement_modes",
                                                placement_modes,
                                                "engine.aux.create.CreatableGameEntity")

        line.add_raw_api_object(creatable_raw_api_object)
        line.add_raw_api_object(cost_raw_api_object)

    @staticmethod
    def get_researchable_tech(tech_group):
        """
        Creates the ResearchableTech object for a Tech.

        :param tech_group: Tech group that is a technology.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        """
        dataset = tech_group.data
        research_location_id = tech_group.get_research_location_id()
        research_location = dataset.building_lines[research_location_id]

        tech_name = TECH_GROUP_LOOKUPS[tech_group.get_id()][0]
        research_location_name = BUILDING_LINE_LOOKUPS[research_location_id][0]

        obj_ref = "%s.ResearchableTech" % (tech_name)
        obj_name = "%sResearchable" % (tech_name)
        researchable_raw_api_object = RawAPIObject(obj_ref, obj_name, dataset.nyan_api_objects)
        researchable_raw_api_object.add_raw_parent("engine.aux.research.ResearchableTech")

        # Location of the object depends on whether it'a a unique tech or a normal tech
        if tech_group.is_unique():
            # Add object to the Civ object
            civ_id = tech_group.get_civilization()
            civ = dataset.civ_groups[civ_id]
            civ_name = CIV_GROUP_LOOKUPS[civ_id][0]

            researchable_location = ExpectedPointer(civ, civ_name)

        else:
            # Add object to the research location's Research ability
            researchable_location = ExpectedPointer(research_location,
                                                    "%s.Research" % (research_location_name))

        researchable_raw_api_object.set_location(researchable_location)

        # Tech
        tech_expected_pointer = ExpectedPointer(tech_group, tech_name)
        researchable_raw_api_object.add_raw_member("tech",
                                                   tech_expected_pointer,
                                                   "engine.aux.research.ResearchableTech")

        # Cost
        cost_name = "%s.ResearchableTech.%sCost" % (tech_name, tech_name)
        cost_raw_api_object = RawAPIObject(cost_name,
                                           "%sCost" % (tech_name),
                                           dataset.nyan_api_objects)
        cost_raw_api_object.add_raw_parent("engine.aux.cost.type.ResourceCost")
        tech_expected_pointer = ExpectedPointer(tech_group, obj_ref)
        cost_raw_api_object.set_location(tech_expected_pointer)

        payment_mode = dataset.nyan_api_objects["engine.aux.payment_mode.type.Advance"]
        cost_raw_api_object.add_raw_member("payment_mode",
                                           payment_mode,
                                           "engine.aux.cost.Cost")

        cost_amounts = []
        for resource_amount in tech_group.tech.get_member("research_resource_costs").get_value():
            resource_id = resource_amount.get_value()["type_id"].get_value()

            resource = None
            resource_name = ""
            if resource_id == -1:
                # Not a valid resource
                continue

            elif resource_id == 0:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Food"].get_nyan_object()
                resource_name = "Food"

            elif resource_id == 1:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Wood"].get_nyan_object()
                resource_name = "Wood"

            elif resource_id == 2:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Stone"].get_nyan_object()
                resource_name = "Stone"

            elif resource_id == 3:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Gold"].get_nyan_object()
                resource_name = "Gold"

            else:
                # Other resource ids are handled differently
                continue

            # Skip resources that are only expected to be there
            if not resource_amount.get_value()["enabled"].get_value():
                continue

            amount = resource_amount.get_value()["amount"].get_value()

            cost_amount_name = "%s.%sAmount" % (cost_name, resource_name)
            cost_amount = RawAPIObject(cost_amount_name,
                                       "%sAmount" % resource_name,
                                       dataset.nyan_api_objects)
            cost_amount.add_raw_parent("engine.aux.resource.ResourceAmount")
            cost_expected_pointer = ExpectedPointer(tech_group, cost_name)
            cost_amount.set_location(cost_expected_pointer)

            cost_amount.add_raw_member("type",
                                       resource,
                                       "engine.aux.resource.ResourceAmount")
            cost_amount.add_raw_member("amount",
                                       amount,
                                       "engine.aux.resource.ResourceAmount")

            cost_amount_expected_pointer = ExpectedPointer(tech_group, cost_amount_name)
            cost_amounts.append(cost_amount_expected_pointer)
            tech_group.add_raw_api_object(cost_amount)

        cost_raw_api_object.add_raw_member("amount",
                                           cost_amounts,
                                           "engine.aux.cost.type.ResourceCost")

        cost_expected_pointer = ExpectedPointer(tech_group, cost_name)
        researchable_raw_api_object.add_raw_member("cost",
                                                   cost_expected_pointer,
                                                   "engine.aux.research.ResearchableTech")

        research_time = tech_group.tech.get_member("research_time").get_value()

        researchable_raw_api_object.add_raw_member("research_time",
                                                   research_time,
                                                   "engine.aux.research.ResearchableTech")

        # Create sound object
        obj_name = "%s.ResearchableTech.Sound" % (research_location_name)
        sound_raw_api_object = RawAPIObject(obj_name, "ResearchSound",
                                            dataset.nyan_api_objects)
        sound_raw_api_object.add_raw_parent("engine.aux.sound.Sound")
        sound_location = ExpectedPointer(tech_group,
                                         "%s.ResearchableTech" % (tech_name))
        sound_raw_api_object.set_location(sound_location)

        # AoE doesn't support sounds here, so this is empty
        sound_raw_api_object.add_raw_member("play_delay",
                                            0,
                                            "engine.aux.sound.Sound")
        sound_raw_api_object.add_raw_member("sounds",
                                            [],
                                            "engine.aux.sound.Sound")

        sound_expected_pointer = ExpectedPointer(tech_group, obj_name)
        researchable_raw_api_object.add_raw_member("research_sounds",
                                                   [sound_expected_pointer],
                                                   "engine.aux.research.ResearchableTech")

        tech_group.add_raw_api_object(sound_raw_api_object)

        # TODO: Condition
        unlock_condition = []
        researchable_raw_api_object.add_raw_member("condition",
                                                   unlock_condition,
                                                   "engine.aux.research.ResearchableTech")

        tech_group.add_raw_api_object(researchable_raw_api_object)
        tech_group.add_raw_api_object(cost_raw_api_object)
