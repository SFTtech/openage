# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Idle ability.
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


def idle_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Idle ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    gset_lookup_dict = internal_name_lookups.get_graphic_set_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Idle"
    ability_raw_api_object = RawAPIObject(ability_ref, "Idle", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Idle")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Ability properties
    properties = {}

    # Animation
    ability_animation_id = current_unit["idle_graphic0"].value
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
                                                 "Idle",
                                                 "idle_")
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

            civ_animation_id = civ["units"][current_unit_id]["idle_graphic0"].value

            if civ_animation_id != ability_animation_id:
                # Find the corresponding graphics set
                for set_id, items in gset_lookup_dict.items():
                    if civ_id in items[0]:
                        graphics_set_id = set_id
                        break

                else:
                    raise RuntimeError(f"No graphics set found for civ id {civ_id}")

                # Check if the object for the animation has been created before
                obj_exists = graphics_set_id in handled_graphics_set_ids
                if not obj_exists:
                    handled_graphics_set_ids.add(graphics_set_id)

                obj_prefix = f"{gset_lookup_dict[graphics_set_id][1]}Idle"
                filename_prefix = f"idle_{gset_lookup_dict[graphics_set_id][2]}_"
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

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
