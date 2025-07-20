# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Gather ability.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberSpecialValue
from ......util.ordered_set import OrderedSet
from .....entity_object.conversion.aoc.genie_unit import GenieVillagerGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from ...aoc.ability_subprocessor import AoCAbilitySubprocessor

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def gather_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Gather abilities to a line. Unlike the other methods, this
    creates multiple abilities.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward references for the abilities.
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

    abilities = []
    for gatherer in gatherers:
        unit_commands = gatherer["unit_commands"].value
        resource = None
        ability_animation_id = -1
        harvestable_class_ids = OrderedSet()
        harvestable_unit_ids = OrderedSet()

        for command in unit_commands:
            # Find a gather ability. It doesn't matter which one because
            # they should all produce the same resource for one genie unit.
            type_id = command["type"].value

            if type_id not in (5, 110):
                continue

            target_class_id = command["class_id"].value
            if target_class_id > -1:
                harvestable_class_ids.add(target_class_id)

            target_unit_id = command["unit_id"].value
            if target_unit_id > -1:
                harvestable_unit_ids.add(target_unit_id)

            resource_id = command["resource_out"].value

            # If resource_out is not specified, the gatherer harvests resource_in
            if resource_id == -1:
                resource_id = command["resource_in"].value

            if resource_id == 0:
                resource = dataset.pregen_nyan_objects[
                    "util.resource.types.Food"
                ].get_nyan_object()

            elif resource_id == 1:
                resource = dataset.pregen_nyan_objects[
                    "util.resource.types.Carbon"
                ].get_nyan_object()

            elif resource_id == 2:
                resource = dataset.pregen_nyan_objects[
                    "util.resource.types.Ore"
                ].get_nyan_object()

            elif resource_id == 3:
                resource = dataset.pregen_nyan_objects[
                    "util.resource.types.Nova"
                ].get_nyan_object()

            else:
                continue

            if type_id == 110:
                ability_animation_id = command["work_sprite_id"].value

            else:
                ability_animation_id = command["proceed_sprite_id"].value

        # Look for the harvestable groups that match the class IDs and unit IDs
        check_groups = []
        check_groups.extend(dataset.unit_lines.values())
        check_groups.extend(dataset.building_lines.values())
        check_groups.extend(dataset.ambient_groups.values())

        harvestable_groups = []
        for group in check_groups:
            if not group.is_harvestable():
                continue

            if group.get_class_id() in harvestable_class_ids:
                harvestable_groups.append(group)
                continue

            for unit_id in harvestable_unit_ids:
                if group.contains_entity(unit_id):
                    harvestable_groups.append(group)

        if len(harvestable_groups) == 0:
            # If no matching groups are found, then we don't
            # need to create an ability.
            continue

        gatherer_unit_id = gatherer.get_id()
        if gatherer_unit_id not in gather_lookup_dict:
            # Skips hunting wolves
            continue

        ability_name = gather_lookup_dict[gatherer_unit_id][0]

        ability_ref = f"{game_entity_name}.{ability_name}"
        ability_raw_api_object = RawAPIObject(
            ability_ref, ability_name, dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Gather")
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        line.add_raw_api_object(ability_raw_api_object)

        # Ability properties
        properties = {}

        # Animation
        if ability_animation_id > -1:
            property_ref = f"{ability_ref}.Animated"
            property_raw_api_object = RawAPIObject(property_ref,
                                                   "Animated",
                                                   dataset.nyan_api_objects)
            property_raw_api_object.add_raw_parent("engine.ability.property.type.Animated")
            property_location = ForwardRef(line, ability_ref)
            property_raw_api_object.set_location(property_location)

            line.add_raw_api_object(property_raw_api_object)

            animations_set = []
            animation_forward_ref = AoCAbilitySubprocessor.create_animation(
                line,
                ability_animation_id,
                property_ref,
                ability_name,
                f"{gather_lookup_dict[gatherer_unit_id][1]}_"
            )
            animations_set.append(animation_forward_ref)
            property_raw_api_object.add_raw_member("animations", animations_set,
                                                   "engine.ability.property.type.Animated")

            property_forward_ref = ForwardRef(line, property_ref)
            properties.update({
                api_objects["engine.ability.property.type.Animated"]: property_forward_ref
            })

        # Diplomacy settings
        property_ref = f"{ability_ref}.Diplomatic"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "Diplomatic",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.ability.property.type.Diplomatic")
        property_location = ForwardRef(line, ability_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        diplomatic_stances = [
            dataset.nyan_api_objects["engine.util.diplomatic_stance.type.Self"]]
        property_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                               "engine.ability.property.type.Diplomatic")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.ability.property.type.Diplomatic"]: property_forward_ref
        })

        ability_raw_api_object.add_raw_member("properties",
                                              properties,
                                              "engine.ability.Ability")

        # Auto resume
        ability_raw_api_object.add_raw_member("auto_resume",
                                              True,
                                              "engine.ability.type.Gather")

        # search range
        ability_raw_api_object.add_raw_member("resume_search_range",
                                              MemberSpecialValue.NYAN_INF,
                                              "engine.ability.type.Gather")

        # Gather rate
        rate_name = f"{game_entity_name}.{ability_name}.GatherRate"
        rate_raw_api_object = RawAPIObject(rate_name, "GatherRate", dataset.nyan_api_objects)
        rate_raw_api_object.add_raw_parent("engine.util.resource.ResourceRate")
        rate_location = ForwardRef(line, ability_ref)
        rate_raw_api_object.set_location(rate_location)

        rate_raw_api_object.add_raw_member(
            "type", resource, "engine.util.resource.ResourceRate")

        gather_rate = gatherer["work_rate"].value
        rate_raw_api_object.add_raw_member(
            "rate", gather_rate, "engine.util.resource.ResourceRate")

        line.add_raw_api_object(rate_raw_api_object)

        rate_forward_ref = ForwardRef(line, rate_name)
        ability_raw_api_object.add_raw_member("gather_rate",
                                              rate_forward_ref,
                                              "engine.ability.type.Gather")

        # Resource container
        container_ref = (f"{game_entity_name}.ResourceStorage."
                         f"{gather_lookup_dict[gatherer_unit_id][0]}Container")
        container_forward_ref = ForwardRef(line, container_ref)
        ability_raw_api_object.add_raw_member("container",
                                              container_forward_ref,
                                              "engine.ability.type.Gather")

        # Targets (resource spots)
        entity_lookups = internal_name_lookups.get_entity_lookups(dataset.game_version)
        spot_forward_refs = []
        for group in harvestable_groups:
            group_id = group.get_head_unit_id()
            group_name = entity_lookups[group_id][0]

            spot_forward_ref = ForwardRef(
                group,
                f"{group_name}.Harvestable.{group_name}ResourceSpot"
            )
            spot_forward_refs.append(spot_forward_ref)

        ability_raw_api_object.add_raw_member("targets",
                                              spot_forward_refs,
                                              "engine.ability.type.Gather")

        ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())
        abilities.append(ability_forward_ref)

    return abilities
