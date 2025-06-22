# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Move ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .util import create_animation, create_civ_animation, create_sound

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def move_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Move ability to a line.

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

    ability_ref = f"{game_entity_name}.Move"
    ability_raw_api_object = RawAPIObject(ability_ref, "Move", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Move")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Ability properties
    properties = {}

    # Animation
    ability_animation_id = current_unit["move_graphics"].value
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

        animation_obj_prefix = "Move"
        animation_filename_prefix = "move_"

        animation_forward_ref = create_animation(line,
                                                 ability_animation_id,
                                                 property_ref,
                                                 animation_obj_prefix,
                                                 animation_filename_prefix)
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

            civ_animation_id = civ["units"][current_unit_id]["move_graphics"].value

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

                obj_prefix = f"{gset_lookup_dict[graphics_set_id][1]}Move"
                filename_prefix = f"move_{gset_lookup_dict[graphics_set_id][2]}_"
                create_civ_animation(line,
                                     civ_group,
                                     civ_animation_id,
                                     property_ref,
                                     obj_prefix,
                                     filename_prefix,
                                     obj_exists)

    # Command Sound
    ability_comm_sound_id = current_unit["command_sound_id"].value
    if ability_comm_sound_id > -1:
        property_ref = f"{ability_ref}.CommandSound"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "CommandSound",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.ability.property.type.CommandSound")
        property_location = ForwardRef(line, ability_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        sounds_set = []

        sound_obj_prefix = "Move"

        sound_forward_ref = create_sound(line,
                                         ability_comm_sound_id,
                                         property_ref,
                                         sound_obj_prefix,
                                         "command_")
        sounds_set.append(sound_forward_ref)
        property_raw_api_object.add_raw_member("sounds", sounds_set,
                                               "engine.ability.property.type.CommandSound")
        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.ability.property.type.CommandSound"]: property_forward_ref
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

    diplomatic_stances = [dataset.nyan_api_objects["engine.util.diplomatic_stance.type.Self"]]
    property_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                           "engine.ability.property.type.Diplomatic")

    property_forward_ref = ForwardRef(line, property_ref)
    properties.update({
        api_objects["engine.ability.property.type.Diplomatic"]: property_forward_ref
    })

    ability_raw_api_object.add_raw_member("properties",
                                          properties,
                                          "engine.ability.Ability")

    # Speed
    speed = current_unit["speed"].value
    ability_raw_api_object.add_raw_member("speed", speed, "engine.ability.type.Move")

    # Standard move modes
    move_modes = [
        dataset.nyan_api_objects["engine.util.move_mode.type.AttackMove"],
        dataset.nyan_api_objects["engine.util.move_mode.type.Normal"],
        dataset.nyan_api_objects["engine.util.move_mode.type.Patrol"]
    ]

    # Follow
    ability_ref = f"{game_entity_name}.Move.Follow"
    follow_raw_api_object = RawAPIObject(ability_ref, "Follow", dataset.nyan_api_objects)
    follow_raw_api_object.add_raw_parent("engine.util.move_mode.type.Follow")
    follow_location = ForwardRef(line, f"{game_entity_name}.Move")
    follow_raw_api_object.set_location(follow_location)

    follow_range = current_unit["line_of_sight"].value - 1
    follow_raw_api_object.add_raw_member("range",
                                         follow_range,
                                         "engine.util.move_mode.type.Follow")

    line.add_raw_api_object(follow_raw_api_object)
    follow_forward_ref = ForwardRef(line, follow_raw_api_object.get_id())
    move_modes.append(follow_forward_ref)

    ability_raw_api_object.add_raw_member("modes", move_modes, "engine.ability.type.Move")

    # Path type
    path_type = dataset.pregen_nyan_objects["util.path.types.Land"].get_nyan_object()
    restrictions = current_unit["terrain_restriction"].value
    if restrictions in (0x00, 0x0C, 0x0E, 0x17):
        # air units
        path_type = dataset.pregen_nyan_objects["util.path.types.Air"].get_nyan_object()

    elif restrictions in (0x03, 0x0D, 0x0F):
        # ships
        path_type = dataset.pregen_nyan_objects["util.path.types.Water"].get_nyan_object()

    ability_raw_api_object.add_raw_member("path_type", path_type, "engine.ability.type.Move")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref


def move_projectile_ability(line: GenieGameEntityGroup, position: int = -1) -> ForwardRef:
    """
    Adds the Move ability to a projectile of the specified line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    if position == 0:
        current_unit_id = line.get_head_unit_id()
        projectile_id = line.get_head_unit()["projectile_id0"].value
        current_unit = dataset.genie_units[projectile_id]

    elif position == 1:
        current_unit_id = line.get_head_unit_id()
        projectile_id = line.get_head_unit()["projectile_id1"].value
        current_unit = dataset.genie_units[projectile_id]

    else:
        raise ValueError(f"Invalid projectile number: {position}")

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"Projectile{position}.Move"
    ability_raw_api_object = RawAPIObject(ability_ref, "Move", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Move")
    ability_location = ForwardRef(line,
                                  f"{game_entity_name}.ShootProjectile.Projectile{position}")
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Ability properties
    properties = {}

    # Animation
    ability_animation_id = current_unit["move_graphics"].value
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
        animation_obj_prefix = "ProjectileFly"
        animation_filename_prefix = "projectile_fly_"

        animation_forward_ref = create_animation(line,
                                                 ability_animation_id,
                                                 property_ref,
                                                 animation_obj_prefix,
                                                 animation_filename_prefix)

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

    # Speed
    speed = current_unit["speed"].value
    ability_raw_api_object.add_raw_member("speed", speed, "engine.ability.type.Move")

    # Move modes
    move_modes = [
        dataset.nyan_api_objects["engine.util.move_mode.type.Normal"],
    ]
    ability_raw_api_object.add_raw_member("modes", move_modes, "engine.ability.type.Move")

    # Path type
    path_type = dataset.pregen_nyan_objects["util.path.types.Air"].get_nyan_object()
    ability_raw_api_object.add_raw_member("path_type", path_type, "engine.ability.type.Move")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
