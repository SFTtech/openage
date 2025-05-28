# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Storage ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieGarrisonMode, GenieMonkGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .util import create_animation

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def storage_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Storage ability to a line.

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :returns: The forward reference for the ability.
    :rtype: ...dataformat.forward_ref.ForwardRef
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Storage"
    ability_raw_api_object = RawAPIObject(ability_ref, "Storage", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Storage")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Container
    # ==============================================================================
    container_name = f"{game_entity_name}.Storage.{game_entity_name}Container"
    container_raw_api_object = RawAPIObject(container_name,
                                            f"{game_entity_name}Container",
                                            dataset.nyan_api_objects)
    container_raw_api_object.add_raw_parent("engine.util.storage.EntityContainer")
    container_location = ForwardRef(line, ability_ref)
    container_raw_api_object.set_location(container_location)

    garrison_mode = line.get_garrison_mode()

    # Allowed types
    # TODO: Any should be fine for now, since Enter/Exit abilities limit the stored elements
    allowed_types = [dataset.nyan_api_objects["engine.util.game_entity_type.type.Any"]]

    container_raw_api_object.add_raw_member("allowed_types",
                                            allowed_types,
                                            "engine.util.storage.EntityContainer")

    # Blacklisted entities
    container_raw_api_object.add_raw_member("blacklisted_entities",
                                            [],
                                            "engine.util.storage.EntityContainer")

    # Define storage elements
    storage_element_defs = []
    if garrison_mode is GenieGarrisonMode.UNIT_GARRISON:
        for storage_element in line.garrison_entities:
            storage_element_name = name_lookup_dict[storage_element.get_head_unit_id()][0]
            storage_def_ref = (f"{game_entity_name}.Storage."
                               f"{game_entity_name}Container."
                               f"{storage_element_name}StorageDef")
            storage_def_raw_api_object = RawAPIObject(storage_def_ref,
                                                      f"{storage_element_name}StorageDef",
                                                      dataset.nyan_api_objects)
            storage_def_raw_api_object.add_raw_parent(
                "engine.util.storage.StorageElementDefinition")
            storage_def_location = ForwardRef(line, container_name)
            storage_def_raw_api_object.set_location(storage_def_location)

            # Storage element
            storage_element_forward_ref = ForwardRef(storage_element, storage_element_name)
            storage_def_raw_api_object.add_raw_member("storage_element",
                                                      storage_element_forward_ref,
                                                      "engine.util.storage.StorageElementDefinition")

            # Elements per slot
            storage_def_raw_api_object.add_raw_member("elements_per_slot",
                                                      1,
                                                      "engine.util.storage.StorageElementDefinition")

            # Conflicts
            storage_def_raw_api_object.add_raw_member("conflicts",
                                                      [],
                                                      "engine.util.storage.StorageElementDefinition")

            # TODO: State change (optional) -> speed boost

            storage_def_forward_ref = ForwardRef(line, storage_def_ref)
            storage_element_defs.append(storage_def_forward_ref)
            line.add_raw_api_object(storage_def_raw_api_object)

    container_raw_api_object.add_raw_member("storage_element_defs",
                                            storage_element_defs,
                                            "engine.util.storage.EntityContainer")

    # Container slots
    slots = current_unit["garrison_capacity"].value
    if garrison_mode is GenieGarrisonMode.MONK:
        slots = 1

    container_raw_api_object.add_raw_member("slots",
                                            slots,
                                            "engine.util.storage.EntityContainer")

    # Carry progress
    carry_progress = []
    if garrison_mode is GenieGarrisonMode.MONK and isinstance(line, GenieMonkGroup):
        switch_unit = line.get_switch_unit()
        carry_idle_animation_id = switch_unit["idle_graphic0"].value
        carry_move_animation_id = switch_unit["move_graphics"].value

        progress_ref = f"{ability_ref}.CarryProgress"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "CarryProgress",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member("type",
                                               api_objects["engine.util.progress_type.type.Carry"],
                                               "engine.util.progress.Progress")

        # Interval = (0.0, 100.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               0.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               100.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =====================================================================================
        # Animated property (animation overrides)
        # =====================================================================================
        property_ref = f"{progress_ref}.Animated"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "Animated",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.util.progress.property.type.Animated")
        property_location = ForwardRef(line, progress_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)
        # =====================================================================================
        overrides = []
        # Idle override
        # =====================================================================================
        override_ref = f"{property_ref}.IdleOverride"
        override_raw_api_object = RawAPIObject(override_ref,
                                               "IdleOverride",
                                               dataset.nyan_api_objects)
        override_raw_api_object.add_raw_parent(
            "engine.util.animation_override.AnimationOverride")
        override_location = ForwardRef(line, property_ref)
        override_raw_api_object.set_location(override_location)

        idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
        override_raw_api_object.add_raw_member("ability",
                                               idle_forward_ref,
                                               "engine.util.animation_override.AnimationOverride")

        # Animation
        animations_set = []
        animation_forward_ref = create_animation(line,
                                                 carry_idle_animation_id,
                                                 override_ref,
                                                 "Idle",
                                                 "idle_carry_override_")

        animations_set.append(animation_forward_ref)
        override_raw_api_object.add_raw_member("animations",
                                               animations_set,
                                               "engine.util.animation_override.AnimationOverride")

        override_raw_api_object.add_raw_member("priority",
                                               1,
                                               "engine.util.animation_override.AnimationOverride")

        override_forward_ref = ForwardRef(line, override_ref)
        overrides.append(override_forward_ref)
        line.add_raw_api_object(override_raw_api_object)
        # =====================================================================================
        # Move override
        # =====================================================================================
        override_ref = f"{property_ref}.MoveOverride"
        override_raw_api_object = RawAPIObject(override_ref,
                                               "MoveOverride",
                                               dataset.nyan_api_objects)
        override_raw_api_object.add_raw_parent(
            "engine.util.animation_override.AnimationOverride")
        override_location = ForwardRef(line, property_ref)
        override_raw_api_object.set_location(override_location)

        idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Move")
        override_raw_api_object.add_raw_member("ability",
                                               idle_forward_ref,
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
        line.add_raw_api_object(override_raw_api_object)
        # =====================================================================================
        property_raw_api_object.add_raw_member("overrides",
                                               overrides,
                                               "engine.util.progress.property.type.Animated")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.Animated"]: property_forward_ref
        })

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
        # =====================================================================================
        carry_state_name = f"{property_ref}.CarryRelicState"
        carry_state_raw_api_object = RawAPIObject(carry_state_name,
                                                  "CarryRelicState",
                                                  dataset.nyan_api_objects)
        carry_state_raw_api_object.add_raw_parent("engine.util.state_machine.StateChanger")
        carry_state_location = ForwardRef(line, property_ref)
        carry_state_raw_api_object.set_location(carry_state_location)

        # Priority
        carry_state_raw_api_object.add_raw_member("priority",
                                                  1,
                                                  "engine.util.state_machine.StateChanger")

        # Enabled abilities
        carry_state_raw_api_object.add_raw_member("enable_abilities",
                                                  [],
                                                  "engine.util.state_machine.StateChanger")

        # Disabled abilities
        disabled_forward_refs = []

        if line.has_command(104):
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.Convert"))

        if line.has_command(105):
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.Heal"))

        carry_state_raw_api_object.add_raw_member("disable_abilities",
                                                  disabled_forward_refs,
                                                  "engine.util.state_machine.StateChanger")

        # Enabled modifiers
        carry_state_raw_api_object.add_raw_member("enable_modifiers",
                                                  [],
                                                  "engine.util.state_machine.StateChanger")

        # Disabled modifiers
        carry_state_raw_api_object.add_raw_member("disable_modifiers",
                                                  [],
                                                  "engine.util.state_machine.StateChanger")

        line.add_raw_api_object(carry_state_raw_api_object)
        # =====================================================================================
        init_state_forward_ref = ForwardRef(line, carry_state_name)
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

        progress_forward_ref = ForwardRef(line, progress_ref)
        carry_progress.append(progress_forward_ref)

    else:
        # Garrison graphics
        if current_unit.has_member("garrison_graphic"):
            garrison_animation_id = current_unit["garrison_graphic"].value

        else:
            garrison_animation_id = -1

        if garrison_animation_id > -1:
            progress_ref = f"{ability_ref}.CarryProgress"
            progress_raw_api_object = RawAPIObject(progress_ref,
                                                   "CarryProgress",
                                                   dataset.nyan_api_objects)
            progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
            progress_location = ForwardRef(line, ability_ref)
            progress_raw_api_object.set_location(progress_location)

            # Type
            progress_raw_api_object.add_raw_member("type",
                                                   api_objects["engine.util.progress_type.type.Carry"],
                                                   "engine.util.progress.Progress")

            # Interval = (0.0, 100.0)
            progress_raw_api_object.add_raw_member("left_boundary",
                                                   0.0,
                                                   "engine.util.progress.Progress")
            progress_raw_api_object.add_raw_member("right_boundary",
                                                   100.0,
                                                   "engine.util.progress.Progress")

            # Progress properties
            properties = {}
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
            override_ref = f"{property_ref}.IdleOverride"
            override_raw_api_object = RawAPIObject(override_ref,
                                                   "IdleOverride",
                                                   dataset.nyan_api_objects)
            override_raw_api_object.add_raw_parent(
                "engine.util.animation_override.AnimationOverride")
            override_location = ForwardRef(line, property_ref)
            override_raw_api_object.set_location(override_location)

            idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
            override_raw_api_object.add_raw_member("ability",
                                                   idle_forward_ref,
                                                   "engine.util.animation_override.AnimationOverride")

            # Animation
            animations_set = []
            animation_forward_ref = create_animation(line,
                                                     garrison_animation_id,
                                                     override_ref,
                                                     "Idle",
                                                     "idle_garrison_override_")

            animations_set.append(animation_forward_ref)
            override_raw_api_object.add_raw_member("animations",
                                                   animations_set,
                                                   "engine.util.animation_override.AnimationOverride")

            override_raw_api_object.add_raw_member("priority",
                                                   1,
                                                   "engine.util.animation_override.AnimationOverride")

            line.add_raw_api_object(override_raw_api_object)
            # =================================================================================
            override_forward_ref = ForwardRef(line, override_ref)
            property_raw_api_object.add_raw_member("overrides",
                                                   [override_forward_ref],
                                                   "engine.util.progress.property.type.Animated")

            property_forward_ref = ForwardRef(line, property_ref)

            properties.update({
                api_objects["engine.util.progress.property.type.Animated"]: property_forward_ref
            })
            # =====================================================================================
            progress_raw_api_object.add_raw_member("properties",
                                                   properties,
                                                   "engine.util.progress.Progress")

            progress_forward_ref = ForwardRef(line, progress_ref)
            carry_progress.append(progress_forward_ref)
            line.add_raw_api_object(progress_raw_api_object)

    container_raw_api_object.add_raw_member("carry_progress",
                                            carry_progress,
                                            "engine.util.storage.EntityContainer")

    line.add_raw_api_object(container_raw_api_object)
    # ==============================================================================
    container_forward_ref = ForwardRef(line, container_name)
    ability_raw_api_object.add_raw_member("container",
                                          container_forward_ref,
                                          "engine.ability.type.Storage")

    # Empty condition
    if garrison_mode in (GenieGarrisonMode.UNIT_GARRISON, GenieGarrisonMode.MONK):
        # Empty before death
        condition = [
            dataset.pregen_nyan_objects["util.logic.literal.death.StandardHealthDeathLiteral"].get_nyan_object()]

    elif garrison_mode in (GenieGarrisonMode.NATURAL, GenieGarrisonMode.SELF_PRODUCED):
        # Empty when HP < 20%
        condition = [
            dataset.pregen_nyan_objects["util.logic.literal.garrison.BuildingDamageEmpty"].get_nyan_object()]

    else:
        # Never empty automatically (transport ships)
        condition = []

    ability_raw_api_object.add_raw_member("empty_condition",
                                          condition,
                                          "engine.ability.type.Storage")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
