# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-branches,too-many-statements
#
# TODO:
# pylint: disable=line-too-long

"""
Derives complex auxiliary objects from unit lines, techs
or other objects.
"""
from ....nyan.nyan_structs import MemberSpecialValue
from ...dataformat.aoc.combined_sound import CombinedSound
from ...dataformat.aoc.forward_ref import ForwardRef
from ...dataformat.aoc.genie_unit import GenieVillagerGroup,\
    GenieBuildingLineGroup, GenieUnitLineGroup
from ...dataformat.converter_object import RawAPIObject
from ...service import internal_name_lookups
from ..aoc.auxiliary_subprocessor import AoCAuxiliarySubprocessor


class SWGBCCAuxiliarySubprocessor:
    """
    Creates complexer auxiliary raw API objects for abilities in SWGB.
    """

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

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_ref = "%s.CreatableGameEntity" % (game_entity_name)
        obj_name = "%sCreatable" % (game_entity_name)
        creatable_raw_api_object = RawAPIObject(obj_ref, obj_name, dataset.nyan_api_objects)
        creatable_raw_api_object.add_raw_parent("engine.aux.create.CreatableGameEntity")

        # Get train location of line
        train_location_id = line.get_train_location_id()
        if isinstance(line, GenieBuildingLineGroup):
            train_location = dataset.unit_lines[train_location_id]
            train_location_name = name_lookup_dict[train_location_id][0]

        else:
            train_location = dataset.building_lines[train_location_id]
            train_location_name = name_lookup_dict[train_location_id][0]

        # Location of the object depends on whether it'a a unique unit or a normal unit
        if line.is_unique():
            # Add object to the Civ object
            enabling_research_id = line.get_enabling_research_id()
            enabling_research = dataset.genie_techs[enabling_research_id]
            enabling_civ_id = enabling_research["civilization_id"].get_value()

            civ = dataset.civ_groups[enabling_civ_id]
            civ_name = civ_lookup_dict[enabling_civ_id][0]

            creatable_location = ForwardRef(civ, civ_name)

        else:
            # Add object to the train location's Create ability
            creatable_location = ForwardRef(train_location,
                                            "%s.Create" % (train_location_name))

        creatable_raw_api_object.set_location(creatable_location)

        # Game Entity
        game_entity_forward_ref = ForwardRef(line, game_entity_name)
        creatable_raw_api_object.add_raw_member("game_entity",
                                                game_entity_forward_ref,
                                                "engine.aux.create.CreatableGameEntity")

        # Cost (construction)
        cost_name = "%s.CreatableGameEntity.%sCost" % (game_entity_name, game_entity_name)
        cost_raw_api_object = RawAPIObject(cost_name,
                                           "%sCost" % (game_entity_name),
                                           dataset.nyan_api_objects)
        cost_raw_api_object.add_raw_parent("engine.aux.cost.type.ResourceCost")
        creatable_forward_ref = ForwardRef(line, obj_ref)
        cost_raw_api_object.set_location(creatable_forward_ref)

        payment_mode = dataset.nyan_api_objects["engine.aux.payment_mode.type.Advance"]
        cost_raw_api_object.add_raw_member("payment_mode",
                                           payment_mode,
                                           "engine.aux.cost.Cost")

        if line.is_repairable():
            # Cost (repair) for buildings
            cost_repair_name = "%s.CreatableGameEntity.%sRepairCost" % (game_entity_name,
                                                                        game_entity_name)
            cost_repair_raw_api_object = RawAPIObject(cost_repair_name,
                                                      "%sRepairCost" % (game_entity_name),
                                                      dataset.nyan_api_objects)
            cost_repair_raw_api_object.add_raw_parent("engine.aux.cost.type.ResourceCost")
            creatable_forward_ref = ForwardRef(line, obj_ref)
            cost_repair_raw_api_object.set_location(creatable_forward_ref)

            payment_repair_mode = dataset.nyan_api_objects["engine.aux.payment_mode.type.Adaptive"]
            cost_repair_raw_api_object.add_raw_member("payment_mode",
                                                      payment_repair_mode,
                                                      "engine.aux.cost.Cost")
            line.add_raw_api_object(cost_repair_raw_api_object)

        cost_amounts = []
        cost_repair_amounts = []
        for resource_amount in current_unit["resource_cost"].get_value():
            resource_id = resource_amount["type_id"].get_value()

            resource = None
            resource_name = ""
            if resource_id == -1:
                # Not a valid resource
                continue

            if resource_id == 0:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Food"].get_nyan_object()
                resource_name = "Food"

            elif resource_id == 1:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Carbon"].get_nyan_object()
                resource_name = "Carbon"

            elif resource_id == 2:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Ore"].get_nyan_object()
                resource_name = "Ore"

            elif resource_id == 3:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Nova"].get_nyan_object()
                resource_name = "Nova"

            else:
                # Other resource ids are handled differently
                continue

            # Skip resources that are only expected to be there
            if not resource_amount["enabled"].get_value():
                continue

            amount = resource_amount["amount"].get_value()

            cost_amount_name = "%s.%sAmount" % (cost_name, resource_name)
            cost_amount = RawAPIObject(cost_amount_name,
                                       "%sAmount" % resource_name,
                                       dataset.nyan_api_objects)
            cost_amount.add_raw_parent("engine.aux.resource.ResourceAmount")
            cost_forward_ref = ForwardRef(line, cost_name)
            cost_amount.set_location(cost_forward_ref)

            cost_amount.add_raw_member("type",
                                       resource,
                                       "engine.aux.resource.ResourceAmount")
            cost_amount.add_raw_member("amount",
                                       amount,
                                       "engine.aux.resource.ResourceAmount")

            cost_amount_forward_ref = ForwardRef(line, cost_amount_name)
            cost_amounts.append(cost_amount_forward_ref)
            line.add_raw_api_object(cost_amount)

            if line.is_repairable():
                # Cost for repairing = half of the construction cost
                cost_amount_name = "%s.%sAmount" % (cost_repair_name, resource_name)
                cost_amount = RawAPIObject(cost_amount_name,
                                           "%sAmount" % resource_name,
                                           dataset.nyan_api_objects)
                cost_amount.add_raw_parent("engine.aux.resource.ResourceAmount")
                cost_forward_ref = ForwardRef(line, cost_repair_name)
                cost_amount.set_location(cost_forward_ref)

                cost_amount.add_raw_member("type",
                                           resource,
                                           "engine.aux.resource.ResourceAmount")
                cost_amount.add_raw_member("amount",
                                           amount / 2,
                                           "engine.aux.resource.ResourceAmount")

                cost_amount_forward_ref = ForwardRef(line, cost_amount_name)
                cost_repair_amounts.append(cost_amount_forward_ref)
                line.add_raw_api_object(cost_amount)

        cost_raw_api_object.add_raw_member("amount",
                                           cost_amounts,
                                           "engine.aux.cost.type.ResourceCost")

        if line.is_repairable():
            cost_repair_raw_api_object.add_raw_member("amount",
                                                      cost_repair_amounts,
                                                      "engine.aux.cost.type.ResourceCost")

        cost_forward_ref = ForwardRef(line, cost_name)
        creatable_raw_api_object.add_raw_member("cost",
                                                cost_forward_ref,
                                                "engine.aux.create.CreatableGameEntity")
        # Creation time
        if isinstance(line, GenieUnitLineGroup):
            creation_time = current_unit["creation_time"].get_value()

        else:
            # Buildings are created immediately
            creation_time = 0

        creatable_raw_api_object.add_raw_member("creation_time",
                                                creation_time,
                                                "engine.aux.create.CreatableGameEntity")

        # Creation sound
        creation_sound_id = current_unit["train_sound_id"].get_value()

        # Create sound object
        obj_name = "%s.CreatableGameEntity.Sound" % (game_entity_name)
        sound_raw_api_object = RawAPIObject(obj_name, "CreationSound",
                                            dataset.nyan_api_objects)
        sound_raw_api_object.add_raw_parent("engine.aux.sound.Sound")
        sound_location = ForwardRef(line, obj_ref)
        sound_raw_api_object.set_location(sound_location)

        # Search for the sound if it exists
        creation_sounds = []
        if creation_sound_id > -1:
            genie_sound = dataset.genie_sounds[creation_sound_id]
            file_ids = genie_sound.get_sounds(civ_id=-1)

            if file_ids:
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

        sound_forward_ref = ForwardRef(line, obj_name)
        creatable_raw_api_object.add_raw_member("creation_sounds",
                                                [sound_forward_ref],
                                                "engine.aux.create.CreatableGameEntity")

        line.add_raw_api_object(sound_raw_api_object)

        # Condition
        unlock_conditions = []
        enabling_research_id = line.get_enabling_research_id()
        if enabling_research_id > -1:
            unlock_conditions.extend(AoCAuxiliarySubprocessor.get_condition(line,
                                                                            obj_ref,
                                                                            enabling_research_id))

        creatable_raw_api_object.add_raw_member("condition",
                                                unlock_conditions,
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
            place_location = ForwardRef(line,
                                        "%s.CreatableGameEntity" % (game_entity_name))
            place_raw_api_object.set_location(place_location)

            # Tile snap distance (uses 1.0 for grid placement)
            place_raw_api_object.add_raw_member("tile_snap_distance",
                                                1.0,
                                                "engine.aux.placement_mode.type.Place")
            # Clearance size
            clearance_size_x = current_unit["clearance_size_x"].get_value()
            clearance_size_y = current_unit["clearance_size_y"].get_value()
            place_raw_api_object.add_raw_member("clearance_size_x",
                                                clearance_size_x,
                                                "engine.aux.placement_mode.type.Place")
            place_raw_api_object.add_raw_member("clearance_size_y",
                                                clearance_size_y,
                                                "engine.aux.placement_mode.type.Place")

            # Max elevation difference
            elevation_mode = current_unit["elevation_mode"].get_value()
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

            place_forward_ref = ForwardRef(line, obj_name)
            placement_modes.append(place_forward_ref)

            if line.get_class_id() == 39:
                # Gates
                obj_name = "%s.CreatableGameEntity.Replace" % (game_entity_name)
                replace_raw_api_object = RawAPIObject(obj_name,
                                                      "Replace",
                                                      dataset.nyan_api_objects)
                replace_raw_api_object.add_raw_parent("engine.aux.placement_mode.type.Replace")
                replace_location = ForwardRef(line,
                                              "%s.CreatableGameEntity" % (game_entity_name))
                replace_raw_api_object.set_location(replace_location)

                # Game entities (only stone wall)
                wall_line_id = 117
                wall_line = dataset.building_lines[wall_line_id]
                wall_name = name_lookup_dict[117][0]
                game_entities = [ForwardRef(wall_line, wall_name)]
                replace_raw_api_object.add_raw_member("game_entities",
                                                      game_entities,
                                                      "engine.aux.placement_mode.type.Replace")

                line.add_raw_api_object(replace_raw_api_object)

                replace_forward_ref = ForwardRef(line, obj_name)
                placement_modes.append(replace_forward_ref)

        else:
            placement_modes.append(dataset.nyan_api_objects["engine.aux.placement_mode.type.Eject"])

            # OwnStorage mode
            obj_name = "%s.CreatableGameEntity.OwnStorage" % (game_entity_name)
            own_storage_raw_api_object = RawAPIObject(obj_name, "OwnStorage",
                                                      dataset.nyan_api_objects)
            own_storage_raw_api_object.add_raw_parent("engine.aux.placement_mode.type.OwnStorage")
            own_storage_location = ForwardRef(line,
                                              "%s.CreatableGameEntity" % (game_entity_name))
            own_storage_raw_api_object.set_location(own_storage_location)

            # Container
            container_forward_ref = ForwardRef(train_location,
                                               "%s.Storage.%sContainer"
                                               % (train_location_name, train_location_name))
            own_storage_raw_api_object.add_raw_member("container",
                                                      container_forward_ref,
                                                      "engine.aux.placement_mode.type.OwnStorage")

            line.add_raw_api_object(own_storage_raw_api_object)

            own_storage_forward_ref = ForwardRef(line, obj_name)
            placement_modes.append(own_storage_forward_ref)

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

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

        research_location_name = name_lookup_dict[research_location_id][0]
        tech_name = tech_lookup_dict[tech_group.get_id()][0]

        obj_ref = "%s.ResearchableTech" % (tech_name)
        obj_name = "%sResearchable" % (tech_name)
        researchable_raw_api_object = RawAPIObject(obj_ref, obj_name, dataset.nyan_api_objects)
        researchable_raw_api_object.add_raw_parent("engine.aux.research.ResearchableTech")

        # Location of the object depends on whether it'a a unique tech or a normal tech
        if tech_group.is_unique():
            # Add object to the Civ object
            civ_id = tech_group.get_civilization()
            civ = dataset.civ_groups[civ_id]
            civ_name = civ_lookup_dict[civ_id][0]

            researchable_location = ForwardRef(civ, civ_name)

        else:
            # Add object to the research location's Research ability
            researchable_location = ForwardRef(research_location,
                                               "%s.Research" % (research_location_name))

        researchable_raw_api_object.set_location(researchable_location)

        # Tech
        tech_forward_ref = ForwardRef(tech_group, tech_name)
        researchable_raw_api_object.add_raw_member("tech",
                                                   tech_forward_ref,
                                                   "engine.aux.research.ResearchableTech")

        # Cost
        cost_ref = "%s.ResearchableTech.%sCost" % (tech_name, tech_name)
        cost_raw_api_object = RawAPIObject(cost_ref,
                                           "%sCost" % (tech_name),
                                           dataset.nyan_api_objects)
        cost_raw_api_object.add_raw_parent("engine.aux.cost.type.ResourceCost")
        tech_forward_ref = ForwardRef(tech_group, obj_ref)
        cost_raw_api_object.set_location(tech_forward_ref)

        payment_mode = dataset.nyan_api_objects["engine.aux.payment_mode.type.Advance"]
        cost_raw_api_object.add_raw_member("payment_mode",
                                           payment_mode,
                                           "engine.aux.cost.Cost")

        cost_amounts = []
        for resource_amount in tech_group.tech["research_resource_costs"].get_value():
            resource_id = resource_amount["type_id"].get_value()

            resource = None
            resource_name = ""
            if resource_id == -1:
                # Not a valid resource
                continue

            if resource_id == 0:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Food"].get_nyan_object()
                resource_name = "Food"

            elif resource_id == 1:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Carbon"].get_nyan_object()
                resource_name = "Carbon"

            elif resource_id == 2:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Ore"].get_nyan_object()
                resource_name = "Ore"

            elif resource_id == 3:
                resource = dataset.pregen_nyan_objects["aux.resource.types.Nova"].get_nyan_object()
                resource_name = "Nova"

            else:
                # Other resource ids are handled differently
                continue

            # Skip resources that are only expected to be there
            if not resource_amount["enabled"].get_value():
                continue

            amount = resource_amount["amount"].get_value()

            cost_amount_ref = "%s.%sAmount" % (cost_ref, resource_name)
            cost_amount = RawAPIObject(cost_amount_ref,
                                       "%sAmount" % resource_name,
                                       dataset.nyan_api_objects)
            cost_amount.add_raw_parent("engine.aux.resource.ResourceAmount")
            cost_forward_ref = ForwardRef(tech_group, cost_ref)
            cost_amount.set_location(cost_forward_ref)

            cost_amount.add_raw_member("type",
                                       resource,
                                       "engine.aux.resource.ResourceAmount")
            cost_amount.add_raw_member("amount",
                                       amount,
                                       "engine.aux.resource.ResourceAmount")

            cost_amount_forward_ref = ForwardRef(tech_group, cost_amount_ref)
            cost_amounts.append(cost_amount_forward_ref)
            tech_group.add_raw_api_object(cost_amount)

        cost_raw_api_object.add_raw_member("amount",
                                           cost_amounts,
                                           "engine.aux.cost.type.ResourceCost")

        cost_forward_ref = ForwardRef(tech_group, cost_ref)
        researchable_raw_api_object.add_raw_member("cost",
                                                   cost_forward_ref,
                                                   "engine.aux.research.ResearchableTech")

        research_time = tech_group.tech["research_time"].get_value()

        researchable_raw_api_object.add_raw_member("research_time",
                                                   research_time,
                                                   "engine.aux.research.ResearchableTech")

        # Create sound object
        sound_ref = "%s.ResearchableTech.Sound" % (tech_name)
        sound_raw_api_object = RawAPIObject(sound_ref, "ResearchSound",
                                            dataset.nyan_api_objects)
        sound_raw_api_object.add_raw_parent("engine.aux.sound.Sound")
        sound_location = ForwardRef(tech_group,
                                    "%s.ResearchableTech" % (tech_name))
        sound_raw_api_object.set_location(sound_location)

        # AoE doesn't support sounds here, so this is empty
        sound_raw_api_object.add_raw_member("play_delay",
                                            0,
                                            "engine.aux.sound.Sound")
        sound_raw_api_object.add_raw_member("sounds",
                                            [],
                                            "engine.aux.sound.Sound")

        sound_forward_ref = ForwardRef(tech_group, sound_ref)
        researchable_raw_api_object.add_raw_member("research_sounds",
                                                   [sound_forward_ref],
                                                   "engine.aux.research.ResearchableTech")

        tech_group.add_raw_api_object(sound_raw_api_object)

        # Condition
        unlock_conditions = []
        if tech_group.get_id() > -1:
            unlock_conditions.extend(AoCAuxiliarySubprocessor.get_condition(tech_group,
                                                                            obj_ref,
                                                                            tech_group.get_id(),
                                                                            top_level=True))

        researchable_raw_api_object.add_raw_member("condition",
                                                   unlock_conditions,
                                                   "engine.aux.research.ResearchableTech")

        tech_group.add_raw_api_object(researchable_raw_api_object)
        tech_group.add_raw_api_object(cost_raw_api_object)
