# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Restock ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieVillagerGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .util import create_animation

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def restock_ability(line: GenieGameEntityGroup, restock_target_id: int) -> ForwardRef:
    """
    Adds the Restock ability to a line.

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :returns: The forward reference for the ability.
    :rtype: ...dataformat.forward_ref.ForwardRef
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    # get the restock target
    converter_groups = {}
    converter_groups.update(dataset.unit_lines)
    converter_groups.update(dataset.building_lines)
    converter_groups.update(dataset.ambient_groups)

    restock_target = converter_groups[restock_target_id]

    if not restock_target.is_harvestable():
        raise RuntimeError(f"{restock_target} cannot be restocked: is not harvestable")

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    restock_lookup_dict = internal_name_lookups.get_restock_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]
    ability_ref = f"{game_entity_name}.{restock_lookup_dict[restock_target_id][0]}"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          restock_lookup_dict[restock_target_id][0],
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Restock")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Ability properties
    properties = {}

    ability_animation_id = -1
    if isinstance(line, GenieVillagerGroup) and restock_target_id == 50:
        # Search for the build graphic of farms
        restock_unit = line.get_units_with_command(101)[0]
        commands = restock_unit["unit_commands"].value
        for command in commands:
            type_id = command["type"].value

            if type_id == 101:
                ability_animation_id = command["work_sprite_id"].value

    if ability_animation_id > -1:
        # Make the ability animated
        property_ref = f"{ability_ref}.Animated"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "Animated",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.ability.property.type.Animated")
        property_location = ForwardRef(line, ability_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        animations_set = []
        animation_forward_ref = create_animation(
            line,
            ability_animation_id,
            property_ref,
            restock_lookup_dict[restock_target_id][0],
            f"{restock_lookup_dict[restock_target_id][1]}_"
        )
        animations_set.append(animation_forward_ref)
        property_raw_api_object.add_raw_member("animations",
                                               animations_set,
                                               "engine.ability.property.type.Animated")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.ability.property.type.Animated"]: property_forward_ref
        })

    ability_raw_api_object.add_raw_member("properties",
                                          properties,
                                          "engine.ability.Ability")

    # Auto restock
    ability_raw_api_object.add_raw_member("auto_restock",
                                          True,  # always True since AoC
                                          "engine.ability.type.Restock")

    # Target
    restock_target_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    restock_target_name = restock_target_lookup_dict[restock_target_id][0]
    spot_forward_ref = ForwardRef(restock_target,
                                  (f"{restock_target_name}.Harvestable."
                                   f"{restock_target_name}ResourceSpot"))
    ability_raw_api_object.add_raw_member("target",
                                          spot_forward_ref,
                                          "engine.ability.type.Restock")

    # restock time
    restock_time = restock_target.get_head_unit()["creation_time"].value
    ability_raw_api_object.add_raw_member("restock_time",
                                          restock_time,
                                          "engine.ability.type.Restock")

    # Manual/Auto Cost
    # Link to the same Cost object as Create
    cost_forward_ref = ForwardRef(restock_target,
                                  (f"{restock_target_name}.CreatableGameEntity."
                                   f"{restock_target_name}Cost"))
    ability_raw_api_object.add_raw_member("manual_cost",
                                          cost_forward_ref,
                                          "engine.ability.type.Restock")
    ability_raw_api_object.add_raw_member("auto_cost",
                                          cost_forward_ref,
                                          "engine.ability.type.Restock")

    # Amount
    restock_amount = restock_target.get_head_unit()["resource_capacity"].value
    if restock_target_id == 50:
        # Farm food amount (hardcoded in civ)
        restock_amount = dataset.genie_civs[1]["resources"][36].value

    elif restock_target_id == 199:
        # Fish trap added food amount (hardcoded in civ)
        restock_amount += dataset.genie_civs[1]["resources"][88].value

    ability_raw_api_object.add_raw_member("amount",
                                          restock_amount,
                                          "engine.ability.type.Restock")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
