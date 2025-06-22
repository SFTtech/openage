# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create the activities for unit behaviour in AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject, ConverterObjectGroup
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer

# activity parent and location
ACTIVITY_PARENT = "engine.util.activity.Activity"
ACTIVITY_LOCATION = "data/util/activity/"

# Node types
START_PARENT = "engine.util.activity.node.type.Start"
END_PARENT = "engine.util.activity.node.type.End"
ABILITY_PARENT = "engine.util.activity.node.type.Ability"
TASK_PARENT = "engine.util.activity.node.type.Task"
XOR_PARENT = "engine.util.activity.node.type.XORGate"
XOR_EVENT_PARENT = "engine.util.activity.node.type.XOREventGate"
XOR_SWITCH_PARENT = "engine.util.activity.node.type.XORSwitchGate"

# Condition types
CONDITION_PARENT = "engine.util.activity.condition.Condition"
COND_ABILITY_PARENT = "engine.util.activity.condition.type.AbilityUsable"
COND_QUEUE_PARENT = "engine.util.activity.condition.type.CommandInQueue"
COND_TARGET_PARENT = "engine.util.activity.condition.type.TargetInRange"
COND_COMMAND_SWITCH_PARENT = (
    "engine.util.activity.switch_condition.type.NextCommand"
)


def generate_activities(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the activities for game entity behaviour.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    _generate_default_activity(full_data_set, pregen_converter_group)
    _generate_unit_activity(full_data_set, pregen_converter_group)


def _generate_default_activity(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate a default activity with a start, idle and end node.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    # Activity
    default_ref_in_modpack = "util.activity.types.Default"
    default_raw_api_object = RawAPIObject(default_ref_in_modpack,
                                          "Default", api_objects,
                                          ACTIVITY_LOCATION)
    default_raw_api_object.set_filename("types")
    default_raw_api_object.add_raw_parent(ACTIVITY_PARENT)

    start_forward_ref = ForwardRef(pregen_converter_group,
                                   "util.activity.types.Default.Start")
    default_raw_api_object.add_raw_member("start", start_forward_ref,
                                          ACTIVITY_PARENT)

    pregen_converter_group.add_raw_api_object(default_raw_api_object)
    pregen_nyan_objects.update({default_ref_in_modpack: default_raw_api_object})

    unit_forward_ref = ForwardRef(pregen_converter_group, default_ref_in_modpack)

    # Start
    start_ref_in_modpack = "util.activity.types.Default.Start"
    start_raw_api_object = RawAPIObject(start_ref_in_modpack,
                                        "Start", api_objects)
    start_raw_api_object.set_location(unit_forward_ref)
    start_raw_api_object.add_raw_parent(START_PARENT)

    idle_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.activity.types.Default.Idle")
    start_raw_api_object.add_raw_member("next", idle_forward_ref,
                                        START_PARENT)

    pregen_converter_group.add_raw_api_object(start_raw_api_object)
    pregen_nyan_objects.update({start_ref_in_modpack: start_raw_api_object})

    # Idle
    idle_ref_in_modpack = "util.activity.types.Default.Idle"
    idle_raw_api_object = RawAPIObject(idle_ref_in_modpack,
                                       "Idle", api_objects)
    idle_raw_api_object.set_location(unit_forward_ref)
    idle_raw_api_object.add_raw_parent(ABILITY_PARENT)

    end_forward_ref = ForwardRef(pregen_converter_group,
                                 "util.activity.types.Default.End")
    idle_raw_api_object.add_raw_member("next", end_forward_ref,
                                       ABILITY_PARENT)
    idle_raw_api_object.add_raw_member("ability",
                                       api_objects["engine.ability.type.Idle"],
                                       ABILITY_PARENT)

    pregen_converter_group.add_raw_api_object(idle_raw_api_object)
    pregen_nyan_objects.update({idle_ref_in_modpack: idle_raw_api_object})

    # End
    end_ref_in_modpack = "util.activity.types.Default.End"
    end_raw_api_object = RawAPIObject(end_ref_in_modpack,
                                      "End", api_objects)
    end_raw_api_object.set_location(unit_forward_ref)
    end_raw_api_object.add_raw_parent(END_PARENT)

    pregen_converter_group.add_raw_api_object(end_raw_api_object)
    pregen_nyan_objects.update({end_ref_in_modpack: end_raw_api_object})


def _generate_unit_activity(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup,
) -> None:
    """
    Generate a unit activity with various nodes for unit commands.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    unit_ref_in_modpack = "util.activity.types.Unit"
    unit_raw_api_object = RawAPIObject(unit_ref_in_modpack,
                                       "Unit", api_objects,
                                       ACTIVITY_LOCATION)
    unit_raw_api_object.set_filename("types")
    unit_raw_api_object.add_raw_parent(ACTIVITY_PARENT)

    start_forward_ref = ForwardRef(pregen_converter_group,
                                   "util.activity.types.Unit.Start")
    unit_raw_api_object.add_raw_member("start", start_forward_ref,
                                       ACTIVITY_PARENT)

    pregen_converter_group.add_raw_api_object(unit_raw_api_object)
    pregen_nyan_objects.update({unit_ref_in_modpack: unit_raw_api_object})

    unit_forward_ref = ForwardRef(pregen_converter_group, unit_ref_in_modpack)

    # Start
    start_ref_in_modpack = "util.activity.types.Unit.Start"
    start_raw_api_object = RawAPIObject(start_ref_in_modpack,
                                        "Start", api_objects)
    start_raw_api_object.set_location(unit_forward_ref)
    start_raw_api_object.add_raw_parent(START_PARENT)

    idle_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.activity.types.Unit.Idle")
    start_raw_api_object.add_raw_member("next", idle_forward_ref,
                                        START_PARENT)

    pregen_converter_group.add_raw_api_object(start_raw_api_object)
    pregen_nyan_objects.update({start_ref_in_modpack: start_raw_api_object})

    # Idle
    idle_ref_in_modpack = "util.activity.types.Unit.Idle"
    idle_raw_api_object = RawAPIObject(idle_ref_in_modpack,
                                       "Idle", api_objects)
    idle_raw_api_object.set_location(unit_forward_ref)
    idle_raw_api_object.add_raw_parent(ABILITY_PARENT)

    queue_forward_ref = ForwardRef(pregen_converter_group,
                                   "util.activity.types.Unit.CheckQueue")
    idle_raw_api_object.add_raw_member("next", queue_forward_ref,
                                       ABILITY_PARENT)
    idle_raw_api_object.add_raw_member("ability",
                                       api_objects["engine.ability.type.Idle"],
                                       ABILITY_PARENT)

    pregen_converter_group.add_raw_api_object(idle_raw_api_object)
    pregen_nyan_objects.update({idle_ref_in_modpack: idle_raw_api_object})

    # Check if command is in queue
    queue_ref_in_modpack = "util.activity.types.Unit.CheckQueue"
    queue_raw_api_object = RawAPIObject(queue_ref_in_modpack,
                                        "CheckQueue", api_objects)
    queue_raw_api_object.set_location(unit_forward_ref)
    queue_raw_api_object.add_raw_parent(XOR_PARENT)

    condition_forward_ref = ForwardRef(pregen_converter_group,
                                       "util.activity.types.Unit.CommandInQueue")
    queue_raw_api_object.add_raw_member("next",
                                        [condition_forward_ref],
                                        XOR_PARENT)
    command_forward_ref = ForwardRef(pregen_converter_group,
                                     "util.activity.types.Unit.WaitForCommand")
    queue_raw_api_object.add_raw_member("default",
                                        command_forward_ref,
                                        XOR_PARENT)

    pregen_converter_group.add_raw_api_object(queue_raw_api_object)
    pregen_nyan_objects.update({queue_ref_in_modpack: queue_raw_api_object})

    # condition for command in queue
    condition_ref_in_modpack = "util.activity.types.Unit.CommandInQueue"
    condition_raw_api_object = RawAPIObject(condition_ref_in_modpack,
                                            "CommandInQueue", api_objects)
    condition_raw_api_object.set_location(queue_forward_ref)
    condition_raw_api_object.add_raw_parent(COND_QUEUE_PARENT)

    branch_forward_ref = ForwardRef(pregen_converter_group,
                                    "util.activity.types.Unit.BranchCommand")
    condition_raw_api_object.add_raw_member("next",
                                            branch_forward_ref,
                                            CONDITION_PARENT)

    pregen_converter_group.add_raw_api_object(condition_raw_api_object)
    pregen_nyan_objects.update({condition_ref_in_modpack: condition_raw_api_object})

    # Wait for Command
    command_ref_in_modpack = "util.activity.types.Unit.WaitForCommand"
    command_raw_api_object = RawAPIObject(command_ref_in_modpack,
                                          "WaitForCommand", api_objects)
    command_raw_api_object.set_location(unit_forward_ref)
    command_raw_api_object.add_raw_parent(XOR_EVENT_PARENT)

    event_api_object = api_objects["engine.util.activity.event.type.CommandInQueue"]
    branch_forward_ref = ForwardRef(pregen_converter_group,
                                    "util.activity.types.Unit.BranchCommand")
    command_raw_api_object.add_raw_member("next",
                                          {event_api_object: branch_forward_ref},
                                          XOR_EVENT_PARENT)

    pregen_converter_group.add_raw_api_object(command_raw_api_object)
    pregen_nyan_objects.update({command_ref_in_modpack: command_raw_api_object})

    # Branch on command type
    branch_ref_in_modpack = "util.activity.types.Unit.BranchCommand"
    branch_raw_api_object = RawAPIObject(branch_ref_in_modpack,
                                         "BranchCommand", api_objects)
    branch_raw_api_object.set_location(unit_forward_ref)
    branch_raw_api_object.add_raw_parent(XOR_SWITCH_PARENT)

    switch_forward_ref = ForwardRef(pregen_converter_group,
                                    "util.activity.types.Unit.NextCommandSwitch")
    branch_raw_api_object.add_raw_member("switch",
                                         switch_forward_ref,
                                         XOR_SWITCH_PARENT)
    idle_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.activity.types.Unit.Idle")
    branch_raw_api_object.add_raw_member("default",
                                         idle_forward_ref,
                                         XOR_SWITCH_PARENT)

    pregen_converter_group.add_raw_api_object(branch_raw_api_object)
    pregen_nyan_objects.update({branch_ref_in_modpack: branch_raw_api_object})

    # condition for branching based on command
    condition_ref_in_modpack = "util.activity.types.Unit.NextCommandSwitch"
    condition_raw_api_object = RawAPIObject(condition_ref_in_modpack,
                                            "NextCommandSwitch", api_objects)
    condition_raw_api_object.set_location(branch_forward_ref)
    condition_raw_api_object.add_raw_parent(COND_COMMAND_SWITCH_PARENT)

    ability_check_forward_ref = ForwardRef(pregen_converter_group,
                                           "util.activity.types.Unit.ApplyEffectUsableCheck")
    move_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.activity.types.Unit.Move")
    next_nodes_lookup = {
        api_objects["engine.util.command.type.ApplyEffect"]: ability_check_forward_ref,
        api_objects["engine.util.command.type.Move"]: move_forward_ref,
    }
    condition_raw_api_object.add_raw_member("next",
                                            next_nodes_lookup,
                                            COND_COMMAND_SWITCH_PARENT)

    pregen_converter_group.add_raw_api_object(condition_raw_api_object)
    pregen_nyan_objects.update({condition_ref_in_modpack: condition_raw_api_object})

    # Ability usability gate
    ability_check_ref_in_modpack = "util.activity.types.Unit.ApplyEffectUsableCheck"
    ability_check_raw_api_object = RawAPIObject(ability_check_ref_in_modpack,
                                                "ApplyEffectUsableCheck", api_objects)
    ability_check_raw_api_object.set_location(unit_forward_ref)
    ability_check_raw_api_object.add_raw_parent(XOR_PARENT)

    condition_forward_ref = ForwardRef(pregen_converter_group,
                                       "util.activity.types.Unit.ApplyEffectUsable")
    ability_check_raw_api_object.add_raw_member("next",
                                                [condition_forward_ref],
                                                XOR_PARENT)
    pop_command_forward_ref = ForwardRef(pregen_converter_group,
                                         "util.activity.types.Unit.PopCommand")
    ability_check_raw_api_object.add_raw_member("default",
                                                pop_command_forward_ref,
                                                XOR_PARENT)

    pregen_converter_group.add_raw_api_object(ability_check_raw_api_object)
    pregen_nyan_objects.update({ability_check_ref_in_modpack: ability_check_raw_api_object})

    # Apply effect usability condition
    apply_effect_ref_in_modpack = "util.activity.types.Unit.ApplyEffectUsable"
    apply_effect_raw_api_object = RawAPIObject(apply_effect_ref_in_modpack,
                                               "ApplyEffectUsable", api_objects)
    apply_effect_raw_api_object.set_location(unit_forward_ref)
    apply_effect_raw_api_object.add_raw_parent(COND_ABILITY_PARENT)

    target_in_range_forward_ref = ForwardRef(pregen_converter_group,
                                             "util.activity.types.Unit.RangeCheck")
    apply_effect_raw_api_object.add_raw_member("next",
                                               target_in_range_forward_ref,
                                               CONDITION_PARENT)
    apply_effect_raw_api_object.add_raw_member(
        "ability",
        api_objects["engine.ability.type.ApplyDiscreteEffect"],
        COND_ABILITY_PARENT
    )

    pregen_converter_group.add_raw_api_object(apply_effect_raw_api_object)
    pregen_nyan_objects.update({apply_effect_ref_in_modpack: apply_effect_raw_api_object})

    # Pop command task
    pop_command_ref_in_modpack = "util.activity.types.Unit.PopCommand"
    pop_command_raw_api_object = RawAPIObject(pop_command_ref_in_modpack,
                                              "PopCommand", api_objects)
    pop_command_raw_api_object.set_location(unit_forward_ref)
    pop_command_raw_api_object.add_raw_parent(TASK_PARENT)

    idle_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.activity.types.Unit.Idle")
    pop_command_raw_api_object.add_raw_member("next", idle_forward_ref,
                                              TASK_PARENT)
    pop_command_raw_api_object.add_raw_member(
        "task",
        api_objects["engine.util.activity.task.type.PopCommandQueue"],
        TASK_PARENT
    )

    pregen_converter_group.add_raw_api_object(pop_command_raw_api_object)
    pregen_nyan_objects.update({pop_command_ref_in_modpack: pop_command_raw_api_object})

    # Target in range gate
    range_check_ref_in_modpack = "util.activity.types.Unit.RangeCheck"
    range_check_raw_api_object = RawAPIObject(range_check_ref_in_modpack,
                                              "RangeCheck", api_objects)
    range_check_raw_api_object.set_location(unit_forward_ref)
    range_check_raw_api_object.add_raw_parent(XOR_PARENT)

    target_in_range_forward_ref = ForwardRef(pregen_converter_group,
                                             "util.activity.types.Unit.TargetInRange")
    range_check_raw_api_object.add_raw_member("next",
                                              [target_in_range_forward_ref],
                                              XOR_PARENT)
    move_to_target_forward_ref = ForwardRef(pregen_converter_group,
                                            "util.activity.types.Unit.MoveToTarget")
    range_check_raw_api_object.add_raw_member("default",
                                              move_to_target_forward_ref,
                                              XOR_PARENT)

    pregen_converter_group.add_raw_api_object(range_check_raw_api_object)
    pregen_nyan_objects.update({range_check_ref_in_modpack: range_check_raw_api_object})

    # Target in range condition
    target_in_range_ref_in_modpack = "util.activity.types.Unit.TargetInRange"
    target_in_range_raw_api_object = RawAPIObject(target_in_range_ref_in_modpack,
                                                  "TargetInRange", api_objects)
    target_in_range_raw_api_object.set_location(unit_forward_ref)
    target_in_range_raw_api_object.add_raw_parent(COND_TARGET_PARENT)

    apply_effect_forward_ref = ForwardRef(pregen_converter_group,
                                          "util.activity.types.Unit.ApplyEffect")
    target_in_range_raw_api_object.add_raw_member("next",
                                                  apply_effect_forward_ref,
                                                  CONDITION_PARENT)

    target_in_range_raw_api_object.add_raw_member(
        "ability",
        api_objects["engine.ability.type.ApplyDiscreteEffect"],
        COND_TARGET_PARENT
    )

    pregen_converter_group.add_raw_api_object(target_in_range_raw_api_object)
    pregen_nyan_objects.update(
        {target_in_range_ref_in_modpack: target_in_range_raw_api_object}
    )

    # Move to target task
    move_to_target_ref_in_modpack = "util.activity.types.Unit.MoveToTarget"
    move_to_target_raw_api_object = RawAPIObject(move_to_target_ref_in_modpack,
                                                 "MoveToTarget", api_objects)
    move_to_target_raw_api_object.set_location(unit_forward_ref)
    move_to_target_raw_api_object.add_raw_parent(TASK_PARENT)

    wait_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.activity.types.Unit.WaitMoveToTarget")
    move_to_target_raw_api_object.add_raw_member("next",
                                                 wait_forward_ref,
                                                 TASK_PARENT)
    move_to_target_raw_api_object.add_raw_member(
        "task",
        api_objects["engine.util.activity.task.type.MoveToTarget"],
        TASK_PARENT
    )

    pregen_converter_group.add_raw_api_object(move_to_target_raw_api_object)
    pregen_nyan_objects.update(
        {move_to_target_ref_in_modpack: move_to_target_raw_api_object}
    )

    # Wait for MoveToTarget task (for movement to finish)
    wait_ref_in_modpack = "util.activity.types.Unit.WaitMoveToTarget"
    wait_raw_api_object = RawAPIObject(wait_ref_in_modpack,
                                       "WaitMoveToTarget", api_objects)
    wait_raw_api_object.set_location(unit_forward_ref)
    wait_raw_api_object.add_raw_parent(XOR_EVENT_PARENT)

    wait_finish = api_objects["engine.util.activity.event.type.WaitAbility"]
    wait_command = api_objects["engine.util.activity.event.type.CommandInQueue"]
    range_check_forward_ref = ForwardRef(pregen_converter_group,
                                         "util.activity.types.Unit.RangeCheck")
    branch_forward_ref = ForwardRef(pregen_converter_group,
                                    "util.activity.types.Unit.BranchCommand")
    wait_raw_api_object.add_raw_member("next",
                                       {
                                           wait_finish: range_check_forward_ref,
                                           wait_command: branch_forward_ref
                                       },
                                       XOR_EVENT_PARENT)

    pregen_converter_group.add_raw_api_object(wait_raw_api_object)
    pregen_nyan_objects.update({wait_ref_in_modpack: wait_raw_api_object})

    # Apply effect
    apply_effect_ref_in_modpack = "util.activity.types.Unit.ApplyEffect"
    apply_effect_raw_api_object = RawAPIObject(apply_effect_ref_in_modpack,
                                               "ApplyEffect", api_objects)
    apply_effect_raw_api_object.set_location(unit_forward_ref)
    apply_effect_raw_api_object.add_raw_parent(ABILITY_PARENT)

    wait_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.activity.types.Unit.WaitAbility")
    apply_effect_raw_api_object.add_raw_member("next", wait_forward_ref,
                                               ABILITY_PARENT)
    apply_effect_raw_api_object.add_raw_member(
        "ability",
        api_objects["engine.ability.type.ApplyDiscreteEffect"],
        ABILITY_PARENT
    )

    pregen_converter_group.add_raw_api_object(apply_effect_raw_api_object)
    pregen_nyan_objects.update({apply_effect_ref_in_modpack: apply_effect_raw_api_object})

    # Move
    move_ref_in_modpack = "util.activity.types.Unit.Move"
    move_raw_api_object = RawAPIObject(move_ref_in_modpack,
                                       "Move", api_objects)
    move_raw_api_object.set_location(unit_forward_ref)
    move_raw_api_object.add_raw_parent(ABILITY_PARENT)

    wait_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.activity.types.Unit.WaitAbility")
    move_raw_api_object.add_raw_member("next", wait_forward_ref,
                                       ABILITY_PARENT)
    move_raw_api_object.add_raw_member("ability",
                                       api_objects["engine.ability.type.Move"],
                                       ABILITY_PARENT)

    pregen_converter_group.add_raw_api_object(move_raw_api_object)
    pregen_nyan_objects.update({move_ref_in_modpack: move_raw_api_object})

    # Wait after ability usage (for Move/ApplyEffect or new command)
    wait_ref_in_modpack = "util.activity.types.Unit.WaitAbility"
    wait_raw_api_object = RawAPIObject(wait_ref_in_modpack,
                                       "Wait", api_objects)
    wait_raw_api_object.set_location(unit_forward_ref)
    wait_raw_api_object.add_raw_parent(XOR_EVENT_PARENT)

    wait_finish = api_objects["engine.util.activity.event.type.WaitAbility"]
    wait_command = api_objects["engine.util.activity.event.type.CommandInQueue"]
    wait_raw_api_object.add_raw_member("next",
                                       {
                                           wait_finish: idle_forward_ref,
                                           wait_command: branch_forward_ref
                                       },
                                       XOR_EVENT_PARENT)

    pregen_converter_group.add_raw_api_object(wait_raw_api_object)
    pregen_nyan_objects.update({wait_ref_in_modpack: wait_raw_api_object})

    # End
    end_ref_in_modpack = "util.activity.types.Unit.End"
    end_raw_api_object = RawAPIObject(end_ref_in_modpack,
                                      "End", api_objects)
    end_raw_api_object.set_location(unit_forward_ref)
    end_raw_api_object.add_raw_parent(END_PARENT)

    pregen_converter_group.add_raw_api_object(end_raw_api_object)
    pregen_nyan_objects.update({end_ref_in_modpack: end_raw_api_object})
