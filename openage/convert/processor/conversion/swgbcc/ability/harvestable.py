# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Harvestable ability.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberSpecialValue
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def harvestable_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Harvestable ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Harvestable"
    ability_raw_api_object = RawAPIObject(ability_ref, "Harvestable", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Harvestable")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Resource spot
    resource_storage = current_unit["resource_storage"].value

    for storage in resource_storage:
        resource_id = storage["type"].value

        # IDs 15, 16, 17 are other types of food (meat, berries, fish)
        if resource_id in (0, 15, 16, 17):
            resource = dataset.pregen_nyan_objects["util.resource.types.Food"].get_nyan_object()

        elif resource_id == 1:
            resource = dataset.pregen_nyan_objects["util.resource.types.Carbon"].get_nyan_object(
            )

        elif resource_id == 2:
            resource = dataset.pregen_nyan_objects["util.resource.types.Ore"].get_nyan_object()

        elif resource_id == 3:
            resource = dataset.pregen_nyan_objects["util.resource.types.Nova"].get_nyan_object()

        else:
            continue

        spot_name = f"{game_entity_name}.Harvestable.{game_entity_name}ResourceSpot"
        spot_raw_api_object = RawAPIObject(spot_name,
                                           f"{game_entity_name}ResourceSpot",
                                           dataset.nyan_api_objects)
        spot_raw_api_object.add_raw_parent("engine.util.resource_spot.ResourceSpot")
        spot_location = ForwardRef(line, ability_ref)
        spot_raw_api_object.set_location(spot_location)

        # Type
        spot_raw_api_object.add_raw_member("resource",
                                           resource,
                                           "engine.util.resource_spot.ResourceSpot")

        # Start amount (equals max amount)
        if line.get_id() == 50:
            # Farm food amount (hardcoded in civ)
            starting_amount = dataset.genie_civs[1]["resources"][36].value

        elif line.get_id() == 199:
            # Aqua harvester food amount (hardcoded in civ)
            starting_amount = storage["amount"].value
            starting_amount += dataset.genie_civs[1]["resources"][88].value

        else:
            starting_amount = storage["amount"].value

        spot_raw_api_object.add_raw_member("starting_amount",
                                           starting_amount,
                                           "engine.util.resource_spot.ResourceSpot")

        # Max amount
        spot_raw_api_object.add_raw_member("max_amount",
                                           starting_amount,
                                           "engine.util.resource_spot.ResourceSpot")

        # Decay rate
        decay_rate = current_unit["resource_decay"].value
        spot_raw_api_object.add_raw_member("decay_rate",
                                           decay_rate,
                                           "engine.util.resource_spot.ResourceSpot")

        spot_forward_ref = ForwardRef(line, spot_name)
        ability_raw_api_object.add_raw_member("resources",
                                              spot_forward_ref,
                                              "engine.ability.type.Harvestable")
        line.add_raw_api_object(spot_raw_api_object)

        # Only one resource spot per ability
        break

    # Harvest Progress (we don't use this for SWGB)
    ability_raw_api_object.add_raw_member("harvest_progress",
                                          [],
                                          "engine.ability.type.Harvestable")

    # Restock Progress
    progress_forward_refs = []
    if line.get_class_id() == 7:
        # Farms
        # =====================================================================================
        progress_ref = f"{ability_ref}.RestockProgress33"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "RestockProgress33",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member(
            "type",
            api_objects["engine.util.progress_type.type.Restock"],
            "engine.util.progress.Progress"
        )

        # Interval = (0.0, 33.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               0.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               33.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =====================================================================================
        # Terrain overlay property
        # =====================================================================================
        property_ref = f"{progress_ref}.TerrainOverlay"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "TerrainOverlay",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent(
            "engine.util.progress.property.type.TerrainOverlay")
        property_location = ForwardRef(line, progress_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # Terrain overlay
        terrain_ref = "FarmConstruction1"
        terrain_group = dataset.terrain_groups[29]
        terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
        property_raw_api_object.add_raw_member("terrain_overlay",
                                               terrain_forward_ref,
                                               "engine.util.progress.property.type.TerrainOverlay")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.TerrainOverlay"]: property_forward_ref
        })
        # =====================================================================================
        # State change property
        # =====================================================================================
        property_ref = f"{progress_ref}.StateChange"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "StateChange",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.util.progress.property.type.StateChange")
        property_location = ForwardRef(line, progress_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # State change
        init_state_ref = f"{game_entity_name}.Constructable.InitState"
        init_state_forward_ref = ForwardRef(line, init_state_ref)
        property_raw_api_object.add_raw_member("state_change",
                                               init_state_forward_ref,
                                               "engine.util.progress.property.type.StateChange")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.StateChange"]: property_forward_ref
        })
        # =====================================================================================
        progress_raw_api_object.add_raw_member("properties",
                                               properties,
                                               "engine.util.progress.Progress")

        progress_forward_refs.append(ForwardRef(line, progress_ref))
        # =====================================================================================
        progress_ref = f"{ability_ref}.RestockProgress66"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "RestockProgress66",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member(
            "type",
            api_objects["engine.util.progress_type.type.Restock"],
            "engine.util.progress.Progress"
        )

        # Interval = (33.0, 66.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               33.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               66.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =====================================================================================
        # Terrain overlay property
        # =====================================================================================
        property_ref = f"{progress_ref}.TerrainOverlay"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "TerrainOverlay",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent(
            "engine.util.progress.property.type.TerrainOverlay")
        property_location = ForwardRef(line, progress_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # Terrain overlay
        terrain_ref = "FarmConstruction2"
        terrain_group = dataset.terrain_groups[30]
        terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
        property_raw_api_object.add_raw_member("terrain_overlay",
                                               terrain_forward_ref,
                                               "engine.util.progress.property.type.TerrainOverlay")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.TerrainOverlay"]: property_forward_ref
        })
        # =====================================================================================
        # State change property
        # =====================================================================================
        property_ref = f"{progress_ref}.StateChange"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "StateChange",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.util.progress.property.type.StateChange")
        property_location = ForwardRef(line, progress_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # State change
        construct_state_ref = f"{game_entity_name}.Constructable.ConstructState"
        construct_state_forward_ref = ForwardRef(line, construct_state_ref)
        property_raw_api_object.add_raw_member("state_change",
                                               construct_state_forward_ref,
                                               "engine.util.progress.property.type.StateChange")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.StateChange"]: property_forward_ref
        })
        # =====================================================================================
        progress_raw_api_object.add_raw_member("properties",
                                               properties,
                                               "engine.util.progress.Progress")

        progress_forward_refs.append(ForwardRef(line, progress_ref))
        # =====================================================================================
        progress_ref = f"{ability_ref}.RestockProgress100"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "RestockProgress100",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member(
            "type",
            api_objects["engine.util.progress_type.type.Restock"],
            "engine.util.progress.Progress"
        )

        progress_raw_api_object.add_raw_member("left_boundary",
                                               66.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               100.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =====================================================================================
        # Terrain overlay property
        # =====================================================================================
        property_ref = f"{progress_ref}.TerrainOverlay"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "TerrainOverlay",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent(
            "engine.util.progress.property.type.TerrainOverlay")
        property_location = ForwardRef(line, progress_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # Terrain overlay
        terrain_ref = "FarmConstruction3"
        terrain_group = dataset.terrain_groups[31]
        terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
        property_raw_api_object.add_raw_member("terrain_overlay",
                                               terrain_forward_ref,
                                               "engine.util.progress.property.type.TerrainOverlay")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.TerrainOverlay"]: property_forward_ref
        })
        # =====================================================================================
        # State change property
        # =====================================================================================
        property_ref = f"{progress_ref}.StateChange"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "StateChange",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.util.progress.property.type.StateChange")
        property_location = ForwardRef(line, progress_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # State change
        construct_state_ref = f"{game_entity_name}.Constructable.ConstructState"
        construct_state_forward_ref = ForwardRef(line, construct_state_ref)
        property_raw_api_object.add_raw_member("state_change",
                                               construct_state_forward_ref,
                                               "engine.util.progress.property.type.StateChange")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.StateChange"]: property_forward_ref
        })
        # =======================================================================
        progress_raw_api_object.add_raw_member("properties",
                                               properties,
                                               "engine.util.progress.Progress")

        progress_forward_refs.append(ForwardRef(line, progress_ref))

    ability_raw_api_object.add_raw_member("restock_progress",
                                          progress_forward_refs,
                                          "engine.ability.type.Harvestable")

    # Gatherer limit (infinite in SWGB except for farms)
    gatherer_limit = MemberSpecialValue.NYAN_INF
    if line.get_class_id() == 7:
        gatherer_limit = 1

    ability_raw_api_object.add_raw_member("gatherer_limit",
                                          gatherer_limit,
                                          "engine.ability.type.Harvestable")

    # Unit have to die before they are harvestable (except for farms)
    harvestable_by_default = current_unit["hit_points"].value == 0
    if line.get_class_id() == 7:
        harvestable_by_default = True

    ability_raw_api_object.add_raw_member("harvestable_by_default",
                                          harvestable_by_default,
                                          "engine.ability.type.Harvestable")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    # TODO: Implement diffing of civ lines

    return ability_forward_ref
