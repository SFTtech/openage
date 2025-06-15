# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the ShootProjectile ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from ...aoc.ability_subprocessor import AoCAbilitySubprocessor

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def shoot_projectile_ability(line: GenieGameEntityGroup, command_id: int) -> ForwardRef:
    """
    Adds the ShootProjectile ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    command_lookup_dict = internal_name_lookups.get_command_lookups(dataset.game_version)

    ability_name = command_lookup_dict[command_id][0]

    game_entity_name = name_lookup_dict[current_unit_id][0]
    ability_ref = f"{game_entity_name}.{ability_name}"
    ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.ShootProjectile")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Ability properties
    properties = {}

    # Range
    property_ref = f"{ability_ref}.Ranged"
    property_raw_api_object = RawAPIObject(property_ref,
                                           "Ranged",
                                           dataset.nyan_api_objects)
    property_raw_api_object.add_raw_parent("engine.ability.property.type.Ranged")
    property_location = ForwardRef(line, ability_ref)
    property_raw_api_object.set_location(property_location)

    line.add_raw_api_object(property_raw_api_object)

    min_range = current_unit["weapon_range_min"].value
    property_raw_api_object.add_raw_member("min_range",
                                           min_range,
                                           "engine.ability.property.type.Ranged")
    max_range = current_unit["weapon_range_max"].value
    property_raw_api_object.add_raw_member("max_range",
                                           max_range,
                                           "engine.ability.property.type.Ranged")

    property_forward_ref = ForwardRef(line, property_ref)
    properties.update({
        dataset.nyan_api_objects["engine.ability.property.type.Ranged"]: property_forward_ref
    })

    # Animation
    ability_animation_id = current_unit["attack_sprite_id"].value
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
            f"{command_lookup_dict[command_id][1]}_"
        )
        animations_set.append(animation_forward_ref)
        property_raw_api_object.add_raw_member("animations",
                                               animations_set,
                                               "engine.ability.property.type.Animated")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.ability.property.type.Animated"]: property_forward_ref
        })

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
        sound_forward_ref = AoCAbilitySubprocessor.create_sound(line,
                                                                ability_comm_sound_id,
                                                                property_ref,
                                                                ability_name,
                                                                "command_")
        sounds_set.append(sound_forward_ref)
        property_raw_api_object.add_raw_member("sounds",
                                               sounds_set,
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

    # Projectile
    projectiles = []
    projectile_primary = current_unit["projectile_id0"].value
    if projectile_primary > -1:
        projectiles.append(ForwardRef(line,
                                      f"{game_entity_name}.ShootProjectile.Projectile0"))

    ability_raw_api_object.add_raw_member("projectiles",
                                          projectiles,
                                          "engine.ability.type.ShootProjectile")

    # Projectile count (does not exist in RoR)
    min_projectiles = 1
    max_projectiles = 1

    ability_raw_api_object.add_raw_member("min_projectiles",
                                          min_projectiles,
                                          "engine.ability.type.ShootProjectile")
    ability_raw_api_object.add_raw_member("max_projectiles",
                                          max_projectiles,
                                          "engine.ability.type.ShootProjectile")

    # Reload time and delay
    reload_time = current_unit["attack_speed"].value
    ability_raw_api_object.add_raw_member("reload_time",
                                          reload_time,
                                          "engine.ability.type.ShootProjectile")

    if ability_animation_id > -1:
        animation = dataset.genie_graphics[ability_animation_id]
        frame_rate = animation.get_frame_rate()

    else:
        frame_rate = 0

    spawn_delay_frames = current_unit["frame_delay"].value
    spawn_delay = frame_rate * spawn_delay_frames
    ability_raw_api_object.add_raw_member("spawn_delay",
                                          spawn_delay,
                                          "engine.ability.type.ShootProjectile")

    # Projectile delay (unused because RoR has no multiple projectiles)
    ability_raw_api_object.add_raw_member("projectile_delay",
                                          0.0,
                                          "engine.ability.type.ShootProjectile")

    # Turning
    if isinstance(line, GenieBuildingLineGroup):
        require_turning = False

    else:
        require_turning = True

    ability_raw_api_object.add_raw_member("require_turning",
                                          require_turning,
                                          "engine.ability.type.ShootProjectile")

    # Manual aiming
    manual_aiming_allowed = line.get_head_unit_id() in (35, 250)
    ability_raw_api_object.add_raw_member("manual_aiming_allowed",
                                          manual_aiming_allowed,
                                          "engine.ability.type.ShootProjectile")

    # Spawning area
    spawning_area_offset_x = current_unit["weapon_offset"][0].value
    spawning_area_offset_y = current_unit["weapon_offset"][1].value
    spawning_area_offset_z = current_unit["weapon_offset"][2].value

    ability_raw_api_object.add_raw_member("spawning_area_offset_x",
                                          spawning_area_offset_x,
                                          "engine.ability.type.ShootProjectile")
    ability_raw_api_object.add_raw_member("spawning_area_offset_y",
                                          spawning_area_offset_y,
                                          "engine.ability.type.ShootProjectile")
    ability_raw_api_object.add_raw_member("spawning_area_offset_z",
                                          spawning_area_offset_z,
                                          "engine.ability.type.ShootProjectile")

    # Spawn Area (does not exist in RoR)
    spawning_area_width = 0
    spawning_area_height = 0
    spawning_area_randomness = 0

    ability_raw_api_object.add_raw_member("spawning_area_width",
                                          spawning_area_width,
                                          "engine.ability.type.ShootProjectile")
    ability_raw_api_object.add_raw_member("spawning_area_height",
                                          spawning_area_height,
                                          "engine.ability.type.ShootProjectile")
    ability_raw_api_object.add_raw_member("spawning_area_randomness",
                                          spawning_area_randomness,
                                          "engine.ability.type.ShootProjectile")

    # Restrictions on targets (only units and buildings allowed)
    allowed_types = [
        dataset.pregen_nyan_objects["util.game_entity_type.types.Building"].get_nyan_object(),
        dataset.pregen_nyan_objects["util.game_entity_type.types.Unit"].get_nyan_object()
    ]
    ability_raw_api_object.add_raw_member("allowed_types",
                                          allowed_types,
                                          "engine.ability.type.ShootProjectile")
    ability_raw_api_object.add_raw_member("blacklisted_entities",
                                          [],
                                          "engine.ability.type.ShootProjectile")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
