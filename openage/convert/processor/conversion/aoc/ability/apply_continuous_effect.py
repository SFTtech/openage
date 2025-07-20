# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the ApplyContinuousEffect ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieVillagerGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .util import create_animation, create_civ_animation, create_sound
from ..effect_subprocessor import AoCEffectSubprocessor

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def apply_continuous_effect_ability(
    line: GenieGameEntityGroup,
    command_id: int,
    ranged: bool = False
) -> ForwardRef:
    """
    Adds the ApplyContinuousEffect ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    if isinstance(line, GenieVillagerGroup):
        current_unit = line.get_units_with_command(command_id)[0]

    else:
        current_unit = line.get_head_unit()

    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    command_lookup_dict = internal_name_lookups.get_command_lookups(dataset.game_version)
    gset_lookup_dict = internal_name_lookups.get_graphic_set_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_name = command_lookup_dict[command_id][0]

    ability_ref = f"{game_entity_name}.{ability_name}"
    ability_raw_api_object = RawAPIObject(ability_ref, ability_name, dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.ApplyContinuousEffect")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Ability properties
    properties = {}

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
        animation_forward_ref = create_animation(
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
        sound_forward_ref = create_sound(line,
                                         ability_comm_sound_id,
                                         property_ref,
                                         ability_name,
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
        if command_id == 105:
            # Heal
            max_range = 4

        else:
            max_range = current_unit["weapon_range_max"].value

        property_raw_api_object.add_raw_member("max_range",
                                               max_range,
                                               "engine.ability.property.type.Ranged")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            dataset.nyan_api_objects["engine.ability.property.type.Ranged"]: property_forward_ref
        })

    # Effects
    effects = None
    allowed_types = None
    if command_id == 101:
        # Construct
        effects = AoCEffectSubprocessor.get_construct_effects(line, ability_ref)
        allowed_types = [
            dataset.pregen_nyan_objects["util.game_entity_type.types.Building"].get_nyan_object(
            )
        ]

    elif command_id == 105:
        # Heal
        effects = AoCEffectSubprocessor.get_heal_effects(line, ability_ref)
        allowed_types = [
            dataset.pregen_nyan_objects["util.game_entity_type.types.Unit"].get_nyan_object()
        ]

    elif command_id == 106:
        # Repair
        effects = AoCEffectSubprocessor.get_repair_effects(line, ability_ref)
        allowed_types = [
            dataset.pregen_nyan_objects["util.game_entity_type.types.Building"].get_nyan_object(
            )
        ]

    ability_raw_api_object.add_raw_member("effects",
                                          effects,
                                          "engine.ability.type.ApplyContinuousEffect")

    # Application delay
    apply_graphic = dataset.genie_graphics[ability_animation_id]
    frame_rate = apply_graphic.get_frame_rate()
    frame_delay = current_unit["frame_delay"].value
    application_delay = frame_rate * frame_delay
    ability_raw_api_object.add_raw_member("application_delay",
                                          application_delay,
                                          "engine.ability.type.ApplyContinuousEffect")

    # Allowed types
    ability_raw_api_object.add_raw_member("allowed_types",
                                          allowed_types,
                                          "engine.ability.type.ApplyContinuousEffect")
    ability_raw_api_object.add_raw_member("blacklisted_entities",
                                          [],
                                          "engine.ability.type.ApplyContinuousEffect")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
