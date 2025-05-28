# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the ResourceStorage ability.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberSpecialValue
from .....entity_object.conversion.aoc.genie_unit import GenieVillagerGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .util import create_animation

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def resource_storage_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the ResourceStorage ability to a line.

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :returns: The forward reference for the ability.
    :rtype: ...dataformat.forward_ref.ForwardRef
    """
    if isinstance(line, GenieVillagerGroup):
        gatherers = line.variants[0].line

    else:
        gatherers = [line.line[0]]

    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    gather_lookup_dict = internal_name_lookups.get_gather_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.ResourceStorage"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "ResourceStorage",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.ResourceStorage")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Create containers
    containers = []
    for gatherer in gatherers:
        unit_commands = gatherer["unit_commands"].value
        resource = None

        used_command = None
        for command in unit_commands:
            # Find a gather ability. It doesn't matter which one because
            # they should all produce the same resource for one genie unit.
            type_id = command["type"].value

            if type_id not in (5, 110, 111):
                continue

            resource_id = command["resource_out"].value

            # If resource_out is not specified, the gatherer harvests resource_in
            if resource_id == -1:
                resource_id = command["resource_in"].value

            if resource_id == 0:
                resource = dataset.pregen_nyan_objects["util.resource.types.Food"].get_nyan_object(
                )

            elif resource_id == 1:
                resource = dataset.pregen_nyan_objects["util.resource.types.Wood"].get_nyan_object(
                )

            elif resource_id == 2:
                resource = dataset.pregen_nyan_objects["util.resource.types.Stone"].get_nyan_object(
                )

            elif resource_id == 3:
                resource = dataset.pregen_nyan_objects["util.resource.types.Gold"].get_nyan_object(
                )

            elif type_id == 111:
                target_id = command["unit_id"].value
                if target_id not in dataset.building_lines.keys():
                    # Skips the trade workshop trading which is never used
                    continue

                # Trade goods --> gold
                resource = dataset.pregen_nyan_objects["util.resource.types.Gold"].get_nyan_object(
                )

            else:
                continue

            used_command = command

        if not used_command:
            # The unit uses no gathering command or we don't recognize it
            continue

        container_name = None
        if line.is_gatherer():
            gatherer_unit_id = gatherer.get_id()
            if gatherer_unit_id not in gather_lookup_dict:
                # Skips hunting wolves
                continue

            container_name = f"{gather_lookup_dict[gatherer_unit_id][0]}Container"

        elif used_command["type"].value == 111:
            # Trading
            container_name = "TradeContainer"

        container_ref = f"{ability_ref}.{container_name}"
        container_raw_api_object = RawAPIObject(container_ref,
                                                container_name,
                                                dataset.nyan_api_objects)
        container_raw_api_object.add_raw_parent("engine.util.storage.ResourceContainer")
        container_location = ForwardRef(line, ability_ref)
        container_raw_api_object.set_location(container_location)

        # Resource
        container_raw_api_object.add_raw_member("resource",
                                                resource,
                                                "engine.util.storage.ResourceContainer")

        # Carry capacity
        carry_capacity = None
        if line.is_gatherer():
            carry_capacity = gatherer["resource_capacity"].value

        elif used_command["type"].value == 111:
            # No restriction for trading
            carry_capacity = MemberSpecialValue.NYAN_INF

        container_raw_api_object.add_raw_member("max_amount",
                                                carry_capacity,
                                                "engine.util.storage.ResourceContainer")

        # Carry progress
        carry_progress = []
        carry_move_animation_id = used_command["carry_sprite_id"].value
        if carry_move_animation_id > -1:
            # =================================================================================
            progress_ref = f"{ability_ref}.{container_name}CarryProgress"
            progress_raw_api_object = RawAPIObject(progress_ref,
                                                   f"{container_name}CarryProgress",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
            progress_location = ForwardRef(line, container_ref)
            progress_raw_api_object.set_location(progress_location)

            line.add_raw_api_object(progress_raw_api_object)

            # Type
            progress_raw_api_object.add_raw_member("type",
                                                   api_objects["engine.util.progress_type.type.Carry"],
                                                   "engine.util.progress.Progress")

            # Interval = (20.0, 100.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   20.0,
                                                   "engine.util.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   100.0,
                                                   "engine.util.progress.Progress")

            # Progress properties
            properties = {}
            # =================================================================================
            # Animated property (animation overrides)
            # =================================================================================
            property_ref = f"{progress_ref}.Animated"
            property_raw_api_object = RawAPIObject(property_ref,
                                                   "Animated",
                                                   dataset.nyan_api_objects)
            property_raw_api_object.add_raw_parent(
                "engine.util.progress.property.type.Animated")
            property_location = ForwardRef(line, progress_ref)
            property_raw_api_object.set_location(property_location)

            line.add_raw_api_object(property_raw_api_object)
            # =================================================================================
            overrides = []
            # =================================================================================
            # Move override
            # =================================================================================
            override_ref = f"{property_ref}.MoveOverride"
            override_raw_api_object = RawAPIObject(override_ref,
                                                   "MoveOverride",
                                                   dataset.nyan_api_objects)
            override_raw_api_object.add_raw_parent(
                "engine.util.animation_override.AnimationOverride")
            override_location = ForwardRef(line, property_ref)
            override_raw_api_object.set_location(override_location)

            line.add_raw_api_object(override_raw_api_object)

            move_forward_ref = ForwardRef(line, f"{game_entity_name}.Move")
            override_raw_api_object.add_raw_member("ability",
                                                   move_forward_ref,
                                                   "engine.util.animation_override.AnimationOverride")

            # Animation
            animations_set = []
            animation_forward_ref = create_animation(line,
                                                     carry_move_animation_id,
                                                     override_ref,
                                                     "Move",
                                                     "move_carry_override_")

            animations_set.append(animation_forward_ref)
            override_raw_api_object.add_raw_member("animations",
                                                   animations_set,
                                                   "engine.util.animation_override.AnimationOverride")

            override_raw_api_object.add_raw_member("priority",
                                                   1,
                                                   "engine.util.animation_override.AnimationOverride")

            override_forward_ref = ForwardRef(line, override_ref)
            overrides.append(override_forward_ref)
            # =================================================================================
            # TODO: Idle override (stops on last used frame of Move override?)
            # =================================================================================
            # =================================================================================
            property_raw_api_object.add_raw_member("overrides",
                                                   overrides,
                                                   "engine.util.progress.property.type.Animated")

            property_forward_ref = ForwardRef(line, property_ref)

            properties.update({
                api_objects["engine.util.progress.property.type.Animated"]: property_forward_ref
            })
            # =================================================================================
            progress_raw_api_object.add_raw_member("properties",
                                                   properties,
                                                   "engine.util.progress.Progress")

            progress_forward_ref = ForwardRef(line, progress_ref)
            carry_progress.append(progress_forward_ref)

        container_raw_api_object.add_raw_member("carry_progress",
                                                carry_progress,
                                                "engine.util.storage.ResourceContainer")

        line.add_raw_api_object(container_raw_api_object)

        container_forward_ref = ForwardRef(line, container_ref)
        containers.append(container_forward_ref)

    ability_raw_api_object.add_raw_member("containers",
                                          containers,
                                          "engine.ability.type.ResourceStorage")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
