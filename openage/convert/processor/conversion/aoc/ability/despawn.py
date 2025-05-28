# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for deleting a unit via the PassiveTransformTo ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .util import create_animation, create_civ_animation

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def despawn_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Despawn ability to a line.

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :returns: The forward reference for the ability.
    :rtype: ...dataformat.forward_ref.ForwardRef
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    # Animation and time come from dead unit
    death_animation_id = current_unit["dying_graphic"].value
    dead_unit_id = current_unit["dead_unit_id"].value
    dead_unit = None
    if dead_unit_id > -1:
        dead_unit = dataset.genie_units[dead_unit_id]

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    gset_lookup_dict = internal_name_lookups.get_graphic_set_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Despawn"
    ability_raw_api_object = RawAPIObject(ability_ref, "Despawn", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Despawn")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Ability properties
    properties = {}

    # Animation
    ability_animation_id = -1
    if dead_unit:
        ability_animation_id = dead_unit["idle_graphic0"].value

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
        animation_forward_ref = create_animation(line,
                                                 ability_animation_id,
                                                 property_ref,
                                                 "Despawn",
                                                 "despawn_")
        animations_set.append(animation_forward_ref)
        property_raw_api_object.add_raw_member("animations",
                                               animations_set,
                                               "engine.ability.property.type.Animated")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.ability.property.type.Animated"]: property_forward_ref
        })

        # Create custom civ graphics
        handled_graphics_set_ids = set()
        for civ_group in dataset.civ_groups.values():
            civ = civ_group.civ
            civ_id = civ_group.get_id()

            # Only proceed if the civ stores the unit in the line
            if current_unit_id not in civ["units"].value.keys():
                continue

            civ_unit = civ["units"][current_unit_id]
            civ_dead_unit_id = civ_unit["dead_unit_id"].value
            civ_dead_unit = None
            if civ_dead_unit_id > -1:
                civ_dead_unit = dataset.genie_units[civ_dead_unit_id]

            civ_animation_id = civ_dead_unit["idle_graphic0"].value

            if civ_animation_id != ability_animation_id:
                # Find the corresponding graphics set
                graphics_set_id = -1
                for set_id, items in gset_lookup_dict.items():
                    if civ_id in items[0]:
                        graphics_set_id = set_id
                        break

                # Check if the object for the animation has been created before
                obj_exists = graphics_set_id in handled_graphics_set_ids
                if not obj_exists:
                    handled_graphics_set_ids.add(graphics_set_id)

                obj_prefix = f"{gset_lookup_dict[graphics_set_id][1]}Despawn"
                filename_prefix = f"despawn_{gset_lookup_dict[graphics_set_id][2]}_"
                create_civ_animation(line,
                                     civ_group,
                                     civ_animation_id,
                                     property_ref,
                                     obj_prefix,
                                     filename_prefix,
                                     obj_exists)

    ability_raw_api_object.add_raw_member("properties",
                                          properties,
                                          "engine.ability.Ability")

    # Activation condition
    # Uses the death condition of the units
    activation_condition = [
        dataset.pregen_nyan_objects["util.logic.literal.death.StandardHealthDeathLiteral"].get_nyan_object(
        )
    ]
    ability_raw_api_object.add_raw_member("activation_condition",
                                          activation_condition,
                                          "engine.ability.type.Despawn")

    # Despawn condition
    ability_raw_api_object.add_raw_member("despawn_condition",
                                          [],
                                          "engine.ability.type.Despawn")

    # Despawn time = corpse decay time (dead unit) or Death animation time (if no dead unit exist)
    despawn_time = 0
    if dead_unit:
        resource_storage = dead_unit["resource_storage"].value
        for storage in resource_storage:
            resource_id = storage["type"].value

            if resource_id == 12:
                despawn_time = storage["amount"].value

    elif death_animation_id > -1:
        despawn_time = dataset.genie_graphics[death_animation_id].get_animation_length()

    ability_raw_api_object.add_raw_member("despawn_time",
                                          despawn_time,
                                          "engine.ability.type.Despawn")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
