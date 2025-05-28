# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for death via the PassiveTransformTo ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieGarrisonMode, \
    GenieUnitLineGroup, GenieBuildingLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .util import create_animation, create_civ_animation

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def death_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds a PassiveTransformTo ability to a line that is used to make entities die
    based on a condition.

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
    gset_lookup_dict = internal_name_lookups.get_graphic_set_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Death"
    ability_raw_api_object = RawAPIObject(ability_ref, "Death", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.PassiveTransformTo")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Ability properties
    properties = {}

    # Animation
    ability_animation_id = current_unit["dying_graphic"].value
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
                                                 ability_ref,
                                                 "Death",
                                                 "death_")
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

            civ_animation_id = civ["units"][current_unit_id]["dying_graphic"].value

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

                obj_prefix = f"{gset_lookup_dict[graphics_set_id][1]}Death"
                filename_prefix = f"death_{gset_lookup_dict[graphics_set_id][2]}_"
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

    # Death condition
    death_condition = [
        dataset.pregen_nyan_objects["util.logic.literal.death.StandardHealthDeathLiteral"].get_nyan_object(
        )
    ]
    ability_raw_api_object.add_raw_member("condition",
                                          death_condition,
                                          "engine.ability.type.PassiveTransformTo")

    # Transform time
    # Use the time of the dying graphics
    if ability_animation_id > -1:
        dying_animation = dataset.genie_graphics[ability_animation_id]
        death_time = dying_animation.get_animation_length()

    else:
        death_time = 0.0

    ability_raw_api_object.add_raw_member("transform_time",
                                          death_time,
                                          "engine.ability.type.PassiveTransformTo")

    # Target state
    # =====================================================================================
    target_state_name = f"{game_entity_name}.Death.DeadState"
    target_state_raw_api_object = RawAPIObject(target_state_name,
                                               "DeadState",
                                               dataset.nyan_api_objects)
    target_state_raw_api_object.add_raw_parent("engine.util.state_machine.StateChanger")
    target_state_location = ForwardRef(line, ability_ref)
    target_state_raw_api_object.set_location(target_state_location)

    # Priority
    target_state_raw_api_object.add_raw_member("priority",
                                               1000,
                                               "engine.util.state_machine.StateChanger")

    # Enabled abilities
    target_state_raw_api_object.add_raw_member("enable_abilities",
                                               [],
                                               "engine.util.state_machine.StateChanger")

    # Disabled abilities
    disabled_forward_refs = []
    if isinstance(line, (GenieUnitLineGroup, GenieBuildingLineGroup)):
        disabled_forward_refs.append(ForwardRef(line,
                                                f"{game_entity_name}.LineOfSight"))

    if isinstance(line, GenieBuildingLineGroup):
        disabled_forward_refs.append(ForwardRef(line,
                                                f"{game_entity_name}.AttributeChangeTracker"))

    if len(line.creates) > 0:
        disabled_forward_refs.append(ForwardRef(line,
                                                f"{game_entity_name}.Create"))

        if isinstance(line, GenieBuildingLineGroup):
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.ProductionQueue"))
    if len(line.researches) > 0:
        disabled_forward_refs.append(ForwardRef(line,
                                                f"{game_entity_name}.Research"))

    if line.is_projectile_shooter():
        disabled_forward_refs.append(ForwardRef(line,
                                                f"{game_entity_name}.Attack"))

    if line.is_garrison():
        disabled_forward_refs.append(ForwardRef(line,
                                                f"{game_entity_name}.Storage"))
        disabled_forward_refs.append(ForwardRef(line,
                                                f"{game_entity_name}.RemoveStorage"))

        garrison_mode = line.get_garrison_mode()

        if garrison_mode == GenieGarrisonMode.NATURAL:
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.SendBackToTask"))

        if garrison_mode in (GenieGarrisonMode.NATURAL, GenieGarrisonMode.SELF_PRODUCED):
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.RallyPoint"))

    if line.is_harvestable():
        disabled_forward_refs.append(ForwardRef(line,
                                                f"{game_entity_name}.Harvestable"))

    if isinstance(line, GenieBuildingLineGroup) and line.is_dropsite():
        disabled_forward_refs.append(ForwardRef(line,
                                                f"{game_entity_name}.DropSite"))

    if isinstance(line, GenieBuildingLineGroup) and line.is_trade_post():
        disabled_forward_refs.append(ForwardRef(line,
                                                f"{game_entity_name}.TradePost"))

    target_state_raw_api_object.add_raw_member("disable_abilities",
                                               disabled_forward_refs,
                                               "engine.util.state_machine.StateChanger")

    # Enabled modifiers
    target_state_raw_api_object.add_raw_member("enable_modifiers",
                                               [],
                                               "engine.util.state_machine.StateChanger")

    # Disabled modifiers
    target_state_raw_api_object.add_raw_member("disable_modifiers",
                                               [],
                                               "engine.util.state_machine.StateChanger")

    line.add_raw_api_object(target_state_raw_api_object)
    # =====================================================================================
    target_state_forward_ref = ForwardRef(line, target_state_name)
    ability_raw_api_object.add_raw_member("target_state",
                                          target_state_forward_ref,
                                          "engine.ability.type.PassiveTransformTo")

    # Transform progress
    # =====================================================================================
    progress_ref = f"{ability_ref}.DeathProgress"
    progress_raw_api_object = RawAPIObject(progress_ref,
                                           "DeathProgress",
                                           dataset.nyan_api_objects)
    progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
    progress_location = ForwardRef(line, ability_ref)
    progress_raw_api_object.set_location(progress_location)

    line.add_raw_api_object(progress_raw_api_object)

    # Type
    progress_raw_api_object.add_raw_member("type",
                                           api_objects["engine.util.progress_type.type.AttributeChange"],
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

    # State change = target state
    property_raw_api_object.add_raw_member("state_change",
                                           target_state_forward_ref,
                                           "engine.util.progress.property.type.StateChange")

    property_forward_ref = ForwardRef(line, property_ref)
    properties.update({
        api_objects["engine.util.progress.property.type.StateChange"]: property_forward_ref
    })
    # =====================================================================================
    progress_raw_api_object.add_raw_member("properties",
                                           properties,
                                           "engine.util.progress.Progress")
    # =====================================================================================
    progress_forward_ref = ForwardRef(line, progress_ref)
    ability_raw_api_object.add_raw_member("transform_progress",
                                          [progress_forward_ref],
                                          "engine.ability.type.PassiveTransformTo")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
