# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the ApplyDiscreteEffect ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieVillagerGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from ...aoc.ability_subprocessor import AoCAbilitySubprocessor
from ...aoc.effect_subprocessor import AoCEffectSubprocessor

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def apply_discrete_effect_ability(
    line: GenieGameEntityGroup,
    command_id: int,
    ranged: bool = False,
    projectile: int = -1
) -> ForwardRef:
    """
    Adds the ApplyDiscreteEffect ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    if isinstance(line, GenieVillagerGroup):
        current_unit = line.get_units_with_command(command_id)[0]
        current_unit_id = current_unit["id0"].value

    else:
        current_unit = line.get_head_unit()
        current_unit_id = line.get_head_unit_id()

    head_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    command_lookup_dict = internal_name_lookups.get_command_lookups(dataset.game_version)
    gset_lookup_dict = internal_name_lookups.get_graphic_set_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[head_unit_id][0]

    ability_name = command_lookup_dict[command_id][0]
    ability_parent = "engine.ability.type.ApplyDiscreteEffect"

    if projectile == -1:
        ability_ref = f"{game_entity_name}.{ability_name}"
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              ability_name,
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent(ability_parent)
        ability_location = ForwardRef(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        if command_id == 104:
            # Get animation from commands proceed sprite
            unit_commands = current_unit["unit_commands"].value
            for command in unit_commands:
                type_id = command["type"].value

                if type_id != command_id:
                    continue

                ability_animation_id = command["proceed_sprite_id"].value
                break

            else:
                ability_animation_id = -1

        else:
            ability_animation_id = current_unit["attack_sprite_id"].value

    else:
        ability_ref = (f"{game_entity_name}.ShootProjectile."
                       f"Projectile{projectile}.{ability_name}")
        ability_raw_api_object = RawAPIObject(ability_ref,
                                              ability_name,
                                              dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent(ability_parent)
        ability_location = ForwardRef(line,
                                      (f"{game_entity_name}.ShootProjectile."
                                       f"Projectile{projectile}"))
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = -1

    # Ability properties
    properties = {}

    # Animated
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
        property_raw_api_object.add_raw_member("animations", animations_set,
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

            civ_animation_id = civ["units"][current_unit_id]["attack_sprite_id"].value

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

                obj_prefix = f"{gset_lookup_dict[graphics_set_id][1]}{ability_name}"
                filename_prefix = (f"{command_lookup_dict[command_id][1]}_"
                                   f"{gset_lookup_dict[graphics_set_id][2]}_")
                AoCAbilitySubprocessor.create_civ_animation(line,
                                                            civ_group,
                                                            civ_animation_id,
                                                            f"{ability_ref}.Animated",
                                                            obj_prefix,
                                                            filename_prefix,
                                                            obj_exists)

    # Command Sound
    if projectile == -1:
        ability_comm_sound_id = current_unit["command_sound_id"].value

    else:
        ability_comm_sound_id = -1

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

        if projectile == -1:
            sound_obj_prefix = ability_name

        else:
            sound_obj_prefix = "ProjectileAttack"

        sound_forward_ref = AoCAbilitySubprocessor.create_sound(line,
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

    # Range
    if ranged:
        # Range
        property_ref = f"{ability_ref}.Ranged"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "Ranged",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.ability.property.type.Ranged")
        property_location = ForwardRef(line, ability_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # Min range
        min_range = current_unit["weapon_range_min"].value
        property_raw_api_object.add_raw_member("min_range",
                                               min_range,
                                               "engine.ability.property.type.Ranged")

        # Max range
        max_range = current_unit["weapon_range_max"].value
        property_raw_api_object.add_raw_member("max_range",
                                               max_range,
                                               "engine.ability.property.type.Ranged")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            dataset.nyan_api_objects["engine.ability.property.type.Ranged"]: property_forward_ref
        })

    # Effects
    batch_ref = f"{ability_ref}.Batch"
    batch_raw_api_object = RawAPIObject(batch_ref, "Batch", dataset.nyan_api_objects)
    batch_raw_api_object.add_raw_parent("engine.util.effect_batch.type.UnorderedBatch")
    batch_location = ForwardRef(line, ability_ref)
    batch_raw_api_object.set_location(batch_location)

    line.add_raw_api_object(batch_raw_api_object)

    # Effects
    effects = []
    if command_id == 7:
        # Attack
        if projectile != 1:
            effects = AoCEffectSubprocessor.get_attack_effects(line, batch_ref)

        else:
            effects = AoCEffectSubprocessor.get_attack_effects(line, batch_ref, projectile=1)

    elif command_id == 104:
        # TODO: Convert
        # effects = AoCEffectSubprocessor.get_convert_effects(line, ability_ref)
        pass

    batch_raw_api_object.add_raw_member("effects",
                                        effects,
                                        "engine.util.effect_batch.EffectBatch")

    batch_forward_ref = ForwardRef(line, batch_ref)
    ability_raw_api_object.add_raw_member("batches",
                                          [batch_forward_ref],
                                          "engine.ability.type.ApplyDiscreteEffect")

    # Reload time
    if projectile == -1:
        reload_time = current_unit["attack_speed"].value

    else:
        reload_time = 0

    ability_raw_api_object.add_raw_member("reload_time",
                                          reload_time,
                                          "engine.ability.type.ApplyDiscreteEffect")

    # Application delay
    if projectile == -1:
        apply_graphic = dataset.genie_graphics[ability_animation_id]
        frame_rate = apply_graphic.get_frame_rate()
        frame_delay = current_unit["frame_delay"].value
        application_delay = frame_rate * frame_delay

    else:
        application_delay = 0

    ability_raw_api_object.add_raw_member("application_delay",
                                          application_delay,
                                          "engine.ability.type.ApplyDiscreteEffect")

    # Allowed types (all buildings/units)
    if command_id == 104:
        # Convert
        allowed_types = [
            dataset.pregen_nyan_objects["util.game_entity_type.types.Unit"].get_nyan_object()
        ]

    else:
        allowed_types = [
            dataset.pregen_nyan_objects["util.game_entity_type.types.Unit"].get_nyan_object(),
            dataset.pregen_nyan_objects["util.game_entity_type.types.Building"].get_nyan_object(
            )
        ]

    ability_raw_api_object.add_raw_member("allowed_types",
                                          allowed_types,
                                          "engine.ability.type.ApplyDiscreteEffect")

    if command_id == 104:
        # Convert
        blacklisted_entities = []
        for unit_line in dataset.unit_lines.values():
            if unit_line.has_command(104):
                # Blacklist other monks
                blacklisted_name = name_lookup_dict[unit_line.get_head_unit_id()][0]
                blacklisted_entities.append(ForwardRef(unit_line, blacklisted_name))
                continue

    else:
        blacklisted_entities = []

    ability_raw_api_object.add_raw_member("blacklisted_entities",
                                          blacklisted_entities,
                                          "engine.ability.type.ApplyDiscreteEffect")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
