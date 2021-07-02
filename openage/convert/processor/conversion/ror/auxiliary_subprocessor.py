# Copyright 2020-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=line-too-long,too-many-locals,too-many-branches,too-many-statements
# pylint: disable=too-few-public-methods

"""
Derives complex auxiliary objects from unit lines, techs
or other objects.
"""
from .....nyan.nyan_structs import MemberSpecialValue
from ....entity_object.conversion.aoc.genie_unit import GenieVillagerGroup,\
    GenieBuildingLineGroup, GenieUnitLineGroup
from ....entity_object.conversion.combined_sound import CombinedSound
from ....entity_object.conversion.converter_object import RawAPIObject
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef
from ..aoc.auxiliary_subprocessor import AoCAuxiliarySubprocessor


class RoRAuxiliarySubprocessor:
    """
    Creates complexer auxiliary raw API objects for abilities in RoR.
    """

    @staticmethod
    def get_creatable_game_entity(line):
        """
        Creates the CreatableGameEntity object for a unit/building line. In comparison
        to the AoC version, ths replaces some unit class IDs and removes garrison
        placement modes.

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

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_ref = f"{game_entity_name}.CreatableGameEntity"
        obj_name = f"{game_entity_name}Creatable"
        creatable_raw_api_object = RawAPIObject(obj_ref, obj_name, dataset.nyan_api_objects)
        creatable_raw_api_object.add_raw_parent("engine.util.create.CreatableGameEntity")

        # Get train location of line
        train_location_id = line.get_train_location_id()
        if isinstance(line, GenieBuildingLineGroup):
            train_location = dataset.unit_lines[train_location_id]
            train_location_name = name_lookup_dict[train_location_id][0]

        else:
            train_location = dataset.building_lines[train_location_id]
            train_location_name = name_lookup_dict[train_location_id][0]

        # Add object to the train location's Create ability
        creatable_location = ForwardRef(train_location,
                                        f"{train_location_name}.Create")

        creatable_raw_api_object.set_location(creatable_location)

        # Game Entity
        game_entity_forward_ref = ForwardRef(line, game_entity_name)
        creatable_raw_api_object.add_raw_member("game_entity",
                                                game_entity_forward_ref,
                                                "engine.util.create.CreatableGameEntity")

        # TODO: Variants
        variants_set = []

        creatable_raw_api_object.add_raw_member("variants", variants_set,
                                                "engine.util.create.CreatableGameEntity")

        # Cost (construction)
        cost_name = f"{game_entity_name}.CreatableGameEntity.{game_entity_name}Cost"
        cost_raw_api_object = RawAPIObject(cost_name,
                                           f"{game_entity_name}Cost",
                                           dataset.nyan_api_objects)
        cost_raw_api_object.add_raw_parent("engine.util.cost.type.ResourceCost")
        creatable_forward_ref = ForwardRef(line, obj_ref)
        cost_raw_api_object.set_location(creatable_forward_ref)

        payment_mode = dataset.nyan_api_objects["engine.util.payment_mode.type.Advance"]
        cost_raw_api_object.add_raw_member("payment_mode",
                                           payment_mode,
                                           "engine.util.cost.Cost")

        if isinstance(line, GenieBuildingLineGroup) or line.get_class_id() in (2, 13, 20, 21, 22):
            # Cost (repair) for buildings
            cost_repair_name = "%s.CreatableGameEntity.%sRepairCost" % (game_entity_name,
                                                                        game_entity_name)
            cost_repair_raw_api_object = RawAPIObject(cost_repair_name,
                                                      f"{game_entity_name}RepairCost",
                                                      dataset.nyan_api_objects)
            cost_repair_raw_api_object.add_raw_parent("engine.util.cost.type.ResourceCost")
            creatable_forward_ref = ForwardRef(line, obj_ref)
            cost_repair_raw_api_object.set_location(creatable_forward_ref)

            payment_repair_mode = dataset.nyan_api_objects["engine.util.payment_mode.type.Adaptive"]
            cost_repair_raw_api_object.add_raw_member("payment_mode",
                                                      payment_repair_mode,
                                                      "engine.util.cost.Cost")
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
                resource = dataset.pregen_nyan_objects["util.resource.types.Food"].get_nyan_object()
                resource_name = "Food"

            elif resource_id == 1:
                resource = dataset.pregen_nyan_objects["util.resource.types.Wood"].get_nyan_object()
                resource_name = "Wood"

            elif resource_id == 2:
                resource = dataset.pregen_nyan_objects["util.resource.types.Stone"].get_nyan_object()
                resource_name = "Stone"

            elif resource_id == 3:
                resource = dataset.pregen_nyan_objects["util.resource.types.Gold"].get_nyan_object()
                resource_name = "Gold"

            else:
                # Other resource ids are handled differently
                continue

            # Skip resources that are only expected to be there
            if not resource_amount["enabled"].get_value():
                continue

            amount = resource_amount["amount"].get_value()

            cost_amount_name = f"{cost_name}.{resource_name}Amount"
            cost_amount = RawAPIObject(cost_amount_name,
                                       f"{resource_name}Amount",
                                       dataset.nyan_api_objects)
            cost_amount.add_raw_parent("engine.util.resource.ResourceAmount")
            cost_forward_ref = ForwardRef(line, cost_name)
            cost_amount.set_location(cost_forward_ref)

            cost_amount.add_raw_member("type",
                                       resource,
                                       "engine.util.resource.ResourceAmount")
            cost_amount.add_raw_member("amount",
                                       amount,
                                       "engine.util.resource.ResourceAmount")

            cost_amount_forward_ref = ForwardRef(line, cost_amount_name)
            cost_amounts.append(cost_amount_forward_ref)
            line.add_raw_api_object(cost_amount)

            if isinstance(line, GenieBuildingLineGroup) or\
                    line.get_class_id() in (2, 13, 20, 21, 22):
                # Cost for repairing = half of the construction cost
                cost_amount_name = f"{cost_repair_name}.{resource_name}Amount"
                cost_amount = RawAPIObject(cost_amount_name,
                                           f"{resource_name}Amount",
                                           dataset.nyan_api_objects)
                cost_amount.add_raw_parent("engine.util.resource.ResourceAmount")
                cost_forward_ref = ForwardRef(line, cost_repair_name)
                cost_amount.set_location(cost_forward_ref)

                cost_amount.add_raw_member("type",
                                           resource,
                                           "engine.util.resource.ResourceAmount")
                cost_amount.add_raw_member("amount",
                                           amount / 2,
                                           "engine.util.resource.ResourceAmount")

                cost_amount_forward_ref = ForwardRef(line, cost_amount_name)
                cost_repair_amounts.append(cost_amount_forward_ref)
                line.add_raw_api_object(cost_amount)

        cost_raw_api_object.add_raw_member("amount",
                                           cost_amounts,
                                           "engine.util.cost.type.ResourceCost")

        if isinstance(line, GenieBuildingLineGroup) or line.get_class_id() in (2, 13, 20, 21, 22):
            cost_repair_raw_api_object.add_raw_member("amount",
                                                      cost_repair_amounts,
                                                      "engine.util.cost.type.ResourceCost")

        cost_forward_ref = ForwardRef(line, cost_name)
        creatable_raw_api_object.add_raw_member("cost",
                                                cost_forward_ref,
                                                "engine.util.create.CreatableGameEntity")
        # Creation time
        if isinstance(line, GenieUnitLineGroup):
            creation_time = current_unit["creation_time"].get_value()

        else:
            # Buildings are created immediately
            creation_time = 0

        creatable_raw_api_object.add_raw_member("creation_time",
                                                creation_time,
                                                "engine.util.create.CreatableGameEntity")

        # Creation sound
        creation_sound_id = current_unit["train_sound_id"].get_value()

        # Create sound object
        obj_name = f"{game_entity_name}.CreatableGameEntity.Sound"
        sound_raw_api_object = RawAPIObject(obj_name, "CreationSound",
                                            dataset.nyan_api_objects)
        sound_raw_api_object.add_raw_parent("engine.util.sound.Sound")
        sound_location = ForwardRef(line, obj_ref)
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
                                               f"creation_sound_{creation_sound_id}",
                                               dataset)
                dataset.combined_sounds.update({file_id: creation_sound})
                creation_sound.add_reference(sound_raw_api_object)

            creation_sounds.append(creation_sound)

        sound_raw_api_object.add_raw_member("play_delay",
                                            0,
                                            "engine.util.sound.Sound")
        sound_raw_api_object.add_raw_member("sounds",
                                            creation_sounds,
                                            "engine.util.sound.Sound")

        sound_forward_ref = ForwardRef(line, obj_name)
        creatable_raw_api_object.add_raw_member("creation_sounds",
                                                [sound_forward_ref],
                                                "engine.util.create.CreatableGameEntity")

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
                                                "engine.util.create.CreatableGameEntity")

        # Placement modes
        placement_modes = []
        if isinstance(line, GenieBuildingLineGroup):
            # Buildings are placed on the map
            # Place mode
            obj_name = f"{game_entity_name}.CreatableGameEntity.Place"
            place_raw_api_object = RawAPIObject(obj_name,
                                                "Place",
                                                dataset.nyan_api_objects)
            place_raw_api_object.add_raw_parent("engine.util.placement_mode.type.Place")
            place_location = ForwardRef(line,
                                        f"{game_entity_name}.CreatableGameEntity")
            place_raw_api_object.set_location(place_location)

            # Tile snap distance (uses 1.0 for grid placement)
            place_raw_api_object.add_raw_member("tile_snap_distance",
                                                1.0,
                                                "engine.util.placement_mode.type.Place")
            # Clearance size
            clearance_size_x = current_unit["clearance_size_x"].get_value()
            clearance_size_y = current_unit["clearance_size_y"].get_value()
            place_raw_api_object.add_raw_member("clearance_size_x",
                                                clearance_size_x,
                                                "engine.util.placement_mode.type.Place")
            place_raw_api_object.add_raw_member("clearance_size_y",
                                                clearance_size_y,
                                                "engine.util.placement_mode.type.Place")

            # Allow rotation
            place_raw_api_object.add_raw_member("allow_rotation",
                                                True,
                                                "engine.util.placement_mode.type.Place")

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
                                                "engine.util.placement_mode.type.Place")

            line.add_raw_api_object(place_raw_api_object)

            place_forward_ref = ForwardRef(line, obj_name)
            placement_modes.append(place_forward_ref)

        else:
            placement_modes.append(dataset.nyan_api_objects["engine.util.placement_mode.type.Eject"])

        creatable_raw_api_object.add_raw_member("placement_modes",
                                                placement_modes,
                                                "engine.util.create.CreatableGameEntity")

        line.add_raw_api_object(creatable_raw_api_object)
        line.add_raw_api_object(cost_raw_api_object)
