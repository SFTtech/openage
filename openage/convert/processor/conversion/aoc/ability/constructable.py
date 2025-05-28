# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Constructable ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_unit import GenieGarrisonMode
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .util import create_animation

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def constructable_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Constructable ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Constructable"
    ability_raw_api_object = RawAPIObject(
        ability_ref, "Constructable", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Constructable")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Starting progress (always 0)
    ability_raw_api_object.add_raw_member("starting_progress",
                                          0,
                                          "engine.ability.type.Constructable")

    construction_animation_id = current_unit["construction_graphic_id"].value

    # Construction progress
    progress_forward_refs = []
    if line.get_class_id() == 49:
        # Farms
        # =====================================================================================
        progress_ref = f"{ability_ref}.ConstructionProgress0"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "ConstructionProgress0",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member("type",
                                               api_objects["engine.util.progress_type.type.Construct"],
                                               "engine.util.progress.Progress")

        # Interval = (0.0, 0.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               0.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               0.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =====================================================================================
        # Terrain overlay property
        # =====================================================================================
        property_ref = f"{progress_ref}.TerrainOverlay"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "TerrainOverlay",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent(
            "engine.util.progress.property.type.TerrainOverlay")
        property_location = ForwardRef(line, progress_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # Terrain overlay
        terrain_ref = "FarmConstruction1"
        terrain_group = dataset.terrain_groups[29]
        terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
        property_raw_api_object.add_raw_member("terrain_overlay",
                                               terrain_forward_ref,
                                               "engine.util.progress.property.type.TerrainOverlay")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.TerrainOverlay"]: property_forward_ref
        })
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

        # State change
        # =====================================================================================
        init_state_name = f"{ability_ref}.InitState"
        init_state_raw_api_object = RawAPIObject(init_state_name,
                                                 "InitState",
                                                 dataset.nyan_api_objects)
        init_state_raw_api_object.add_raw_parent("engine.util.state_machine.StateChanger")
        init_state_location = ForwardRef(line, property_ref)
        init_state_raw_api_object.set_location(init_state_location)

        line.add_raw_api_object(init_state_raw_api_object)

        # Priority
        init_state_raw_api_object.add_raw_member("priority",
                                                 1,
                                                 "engine.util.state_machine.StateChanger")

        # Enabled abilities
        enabled_forward_refs = [
            ForwardRef(line,
                       f"{game_entity_name}.VisibilityConstruct0")
        ]
        init_state_raw_api_object.add_raw_member("enable_abilities",
                                                 enabled_forward_refs,
                                                 "engine.util.state_machine.StateChanger")

        # Disabled abilities
        disabled_forward_refs = [
            ForwardRef(line,
                       f"{game_entity_name}.AttributeChangeTracker"),
            ForwardRef(line,
                       f"{game_entity_name}.LineOfSight"),
            ForwardRef(line,
                       f"{game_entity_name}.Visibility")
        ]
        if len(line.creates) > 0:
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.Create"))
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

        if line.is_dropsite():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.DropSite"))

        if line.is_trade_post():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.TradePost"))

        init_state_raw_api_object.add_raw_member("disable_abilities",
                                                 disabled_forward_refs,
                                                 "engine.util.state_machine.StateChanger")

        # Enabled modifiers
        init_state_raw_api_object.add_raw_member("enable_modifiers",
                                                 [],
                                                 "engine.util.state_machine.StateChanger")

        # Disabled modifiers
        init_state_raw_api_object.add_raw_member("disable_modifiers",
                                                 [],
                                                 "engine.util.state_machine.StateChanger")
        # =====================================================================================
        init_state_forward_ref = ForwardRef(line, init_state_name)
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

        progress_forward_refs.append(ForwardRef(line, progress_ref))
        # =====================================================================================
        progress_ref = f"{ability_ref}.ConstructionProgress33"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "ConstructionProgress33",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member("type",
                                               api_objects["engine.util.progress_type.type.Construct"],
                                               "engine.util.progress.Progress")

        # Interval = (0.0, 33.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               0.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               33.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =====================================================================================
        # Terrain overlay property
        # =====================================================================================
        property_ref = f"{progress_ref}.TerrainOverlay"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "TerrainOverlay",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent(
            "engine.util.progress.property.type.TerrainOverlay")
        property_location = ForwardRef(line, progress_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # Terrain overlay
        terrain_ref = "FarmConstruction1"
        terrain_group = dataset.terrain_groups[29]
        terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
        property_raw_api_object.add_raw_member("terrain_overlay",
                                               terrain_forward_ref,
                                               "engine.util.progress.property.type.TerrainOverlay")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.TerrainOverlay"]: property_forward_ref
        })
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

        # State change
        # =====================================================================================
        construct_state_name = f"{ability_ref}.ConstructState"
        construct_state_raw_api_object = RawAPIObject(construct_state_name,
                                                      "ConstructState",
                                                      dataset.nyan_api_objects)
        construct_state_raw_api_object.add_raw_parent("engine.util.state_machine.StateChanger")
        construct_state_location = ForwardRef(line, ability_ref)
        construct_state_raw_api_object.set_location(construct_state_location)

        line.add_raw_api_object(construct_state_raw_api_object)

        # Priority
        construct_state_raw_api_object.add_raw_member("priority",
                                                      1,
                                                      "engine.util.state_machine.StateChanger")

        # Enabled abilities
        construct_state_raw_api_object.add_raw_member("enable_abilities",
                                                      [],
                                                      "engine.util.state_machine.StateChanger")

        # Disabled abilities
        disabled_forward_refs = [ForwardRef(line,
                                            f"{game_entity_name}.AttributeChangeTracker")]
        if len(line.creates) > 0:
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.Create"))
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

        if line.is_dropsite():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.DropSite"))

        if line.is_trade_post():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.TradePost"))

        construct_state_raw_api_object.add_raw_member("disable_abilities",
                                                      disabled_forward_refs,
                                                      "engine.util.state_machine.StateChanger")

        # Enabled modifiers
        construct_state_raw_api_object.add_raw_member("enable_modifiers",
                                                      [],
                                                      "engine.util.state_machine.StateChanger")

        # Disabled modifiers
        construct_state_raw_api_object.add_raw_member("disable_modifiers",
                                                      [],
                                                      "engine.util.state_machine.StateChanger")

        # =====================================================================================
        construct_state_forward_ref = ForwardRef(line, construct_state_name)
        property_raw_api_object.add_raw_member("state_change",
                                               construct_state_forward_ref,
                                               "engine.util.progress.property.type.StateChange")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.StateChange"]: property_forward_ref
        })
        # =====================================================================================
        progress_raw_api_object.add_raw_member("properties",
                                               properties,
                                               "engine.util.progress.Progress")

        progress_forward_refs.append(ForwardRef(line, progress_ref))
        # =====================================================================================
        progress_ref = f"{ability_ref}.ConstructionProgress66"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "ConstructionProgress66",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member("type",
                                               api_objects["engine.util.progress_type.type.Construct"],
                                               "engine.util.progress.Progress")

        # Interval = (33.0, 66.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               33.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               66.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =====================================================================================
        # Terrain overlay property
        # =====================================================================================
        property_ref = f"{progress_ref}.TerrainOverlay"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "TerrainOverlay",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent(
            "engine.util.progress.property.type.TerrainOverlay")
        property_location = ForwardRef(line, progress_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # Terrain overlay
        terrain_ref = "FarmConstruction2"
        terrain_group = dataset.terrain_groups[30]
        terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
        property_raw_api_object.add_raw_member("terrain_overlay",
                                               terrain_forward_ref,
                                               "engine.util.progress.property.type.TerrainOverlay")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.TerrainOverlay"]: property_forward_ref
        })
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

        # State change
        property_raw_api_object.add_raw_member("state_change",
                                               construct_state_forward_ref,
                                               "engine.util.progress.property.type.StateChange")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.StateChange"]: property_forward_ref
        })
        # =====================================================================================
        progress_raw_api_object.add_raw_member("properties",
                                               properties,
                                               "engine.util.progress.Progress")

        progress_forward_refs.append(ForwardRef(line, progress_ref))
        # =====================================================================================
        progress_ref = f"{ability_ref}.ConstructionProgress100"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "ConstructionProgress100",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member("type",
                                               api_objects["engine.util.progress_type.type.Construct"],
                                               "engine.util.progress.Progress")

        # Interval = (66.0, 100.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               66.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               100.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =====================================================================================
        # Terrain overlay property
        # =====================================================================================
        property_ref = f"{progress_ref}.TerrainOverlay"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "TerrainOverlay",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent(
            "engine.util.progress.property.type.TerrainOverlay")
        property_location = ForwardRef(line, progress_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # Terrain overlay
        terrain_ref = "FarmConstruction3"
        terrain_group = dataset.terrain_groups[31]
        terrain_forward_ref = ForwardRef(terrain_group, terrain_ref)
        property_raw_api_object.add_raw_member("terrain_overlay",
                                               terrain_forward_ref,
                                               "engine.util.progress.property.type.TerrainOverlay")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.TerrainOverlay"]: property_forward_ref
        })
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

        # State change
        property_raw_api_object.add_raw_member("state_change",
                                               construct_state_forward_ref,
                                               "engine.util.progress.property.type.StateChange")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.StateChange"]: property_forward_ref
        })
        # =====================================================================================
        progress_raw_api_object.add_raw_member("properties",
                                               properties,
                                               "engine.util.progress.Progress")

        progress_forward_refs.append(ForwardRef(line, progress_ref))

    else:
        progress_ref = f"{ability_ref}.ConstructionProgress0"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "ConstructionProgress0",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member("type",
                                               api_objects["engine.util.progress_type.type.Construct"],
                                               "engine.util.progress.Progress")

        # Interval = (0.0, 0.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               0.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               0.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =================================================================================
        # Idle override
        # =================================================================================
        if construction_animation_id > -1:
            property_ref = f"{progress_ref}.Animated"
            property_raw_api_object = RawAPIObject(property_ref,
                                                   "Animated",
                                                   dataset.nyan_api_objects)
            property_raw_api_object.add_raw_parent(
                "engine.util.progress.property.type.Animated")
            property_location = ForwardRef(line, progress_ref)
            property_raw_api_object.set_location(property_location)

            line.add_raw_api_object(property_raw_api_object)

            overrides = []
            override_ref = f"{property_ref}.IdleOverride"
            override_raw_api_object = RawAPIObject(override_ref,
                                                   "IdleOverride",
                                                   dataset.nyan_api_objects)
            override_raw_api_object.add_raw_parent(
                "engine.util.animation_override.AnimationOverride")
            override_location = ForwardRef(line, property_ref)
            override_raw_api_object.set_location(override_location)

            line.add_raw_api_object(override_raw_api_object)

            idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
            override_raw_api_object.add_raw_member("ability",
                                                   idle_forward_ref,
                                                   "engine.util.animation_override.AnimationOverride")

            # Animation
            animations_set = []
            animation_forward_ref = create_animation(line,
                                                     construction_animation_id,
                                                     override_ref,
                                                     "Idle",
                                                     "idle_construct0_override_")

            animations_set.append(animation_forward_ref)
            override_raw_api_object.add_raw_member("animations",
                                                   animations_set,
                                                   "engine.util.animation_override.AnimationOverride")

            override_raw_api_object.add_raw_member("priority",
                                                   1,
                                                   "engine.util.animation_override.AnimationOverride")

            override_forward_ref = ForwardRef(line, override_ref)
            overrides.append(override_forward_ref)
            # =================================================================================
            property_raw_api_object.add_raw_member("overrides",
                                                   overrides,
                                                   "engine.util.progress.property.type.Animated")

            property_forward_ref = ForwardRef(line, property_ref)
            properties.update({
                api_objects["engine.util.progress.property.type.Animated"]: property_forward_ref
            })

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

        # State change
        # =====================================================================================
        init_state_name = f"{ability_ref}.InitState"
        init_state_raw_api_object = RawAPIObject(init_state_name,
                                                 "InitState",
                                                 dataset.nyan_api_objects)
        init_state_raw_api_object.add_raw_parent("engine.util.state_machine.StateChanger")
        init_state_location = ForwardRef(line, property_ref)
        init_state_raw_api_object.set_location(init_state_location)

        line.add_raw_api_object(init_state_raw_api_object)

        # Priority
        init_state_raw_api_object.add_raw_member("priority",
                                                 1,
                                                 "engine.util.state_machine.StateChanger")

        # Enabled abilities
        enabled_forward_refs = [
            ForwardRef(line,
                       f"{game_entity_name}.VisibilityConstruct0")
        ]
        init_state_raw_api_object.add_raw_member("enable_abilities",
                                                 enabled_forward_refs,
                                                 "engine.util.state_machine.StateChanger")

        # Disabled abilities
        disabled_forward_refs = [
            ForwardRef(line,
                       f"{game_entity_name}.AttributeChangeTracker"),
            ForwardRef(line,
                       f"{game_entity_name}.LineOfSight"),
            ForwardRef(line,
                       f"{game_entity_name}.Visibility")
        ]
        if len(line.creates) > 0:
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.Create"))
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

        if line.is_dropsite():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.DropSite"))

        if line.is_trade_post():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.TradePost"))

        init_state_raw_api_object.add_raw_member("disable_abilities",
                                                 disabled_forward_refs,
                                                 "engine.util.state_machine.StateChanger")

        # Enabled modifiers
        init_state_raw_api_object.add_raw_member("enable_modifiers",
                                                 [],
                                                 "engine.util.state_machine.StateChanger")

        # Disabled modifiers
        init_state_raw_api_object.add_raw_member("disable_modifiers",
                                                 [],
                                                 "engine.util.state_machine.StateChanger")
        # =====================================================================================
        init_state_forward_ref = ForwardRef(line, init_state_name)
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

        progress_forward_refs.append(ForwardRef(line, progress_ref))
        # =====================================================================================
        progress_ref = f"{ability_ref}.ConstructionProgress25"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "ConstructionProgress25",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member("type",
                                               api_objects["engine.util.progress_type.type.Construct"],
                                               "engine.util.progress.Progress")

        # Interval = (0.0, 25.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               0.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               25.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =================================================================================
        # Idle override
        # =================================================================================
        if construction_animation_id > -1:
            property_ref = f"{progress_ref}.Animated"
            property_raw_api_object = RawAPIObject(property_ref,
                                                   "Animated",
                                                   dataset.nyan_api_objects)
            property_raw_api_object.add_raw_parent(
                "engine.util.progress.property.type.Animated")
            property_location = ForwardRef(line, progress_ref)
            property_raw_api_object.set_location(property_location)

            line.add_raw_api_object(property_raw_api_object)

            overrides = []
            override_ref = f"{progress_ref}.IdleOverride"
            override_raw_api_object = RawAPIObject(override_ref,
                                                   "IdleOverride",
                                                   dataset.nyan_api_objects)
            override_raw_api_object.add_raw_parent(
                "engine.util.animation_override.AnimationOverride")
            override_location = ForwardRef(line, property_ref)
            override_raw_api_object.set_location(override_location)

            line.add_raw_api_object(override_raw_api_object)

            idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
            override_raw_api_object.add_raw_member("ability",
                                                   idle_forward_ref,
                                                   "engine.util.animation_override.AnimationOverride")

            # Animation
            animations_set = []
            animation_forward_ref = create_animation(line,
                                                     construction_animation_id,
                                                     override_ref,
                                                     "Idle",
                                                     "idle_construct25_override_")

            animations_set.append(animation_forward_ref)
            override_raw_api_object.add_raw_member("animations",
                                                   animations_set,
                                                   "engine.util.animation_override.AnimationOverride")

            override_raw_api_object.add_raw_member("priority",
                                                   1,
                                                   "engine.util.animation_override.AnimationOverride")

            override_forward_ref = ForwardRef(line, override_ref)
            overrides.append(override_forward_ref)
            # =================================================================================
            property_raw_api_object.add_raw_member("overrides",
                                                   overrides,
                                                   "engine.util.progress.property.type.Animated")

            property_forward_ref = ForwardRef(line, property_ref)
            properties.update({
                api_objects["engine.util.progress.property.type.Animated"]: property_forward_ref
            })

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

        # State change
        # =====================================================================================
        construct_state_name = f"{ability_ref}.ConstructState"
        construct_state_raw_api_object = RawAPIObject(construct_state_name,
                                                      "ConstructState",
                                                      dataset.nyan_api_objects)
        construct_state_raw_api_object.add_raw_parent("engine.util.state_machine.StateChanger")
        construct_state_location = ForwardRef(line, property_ref)
        construct_state_raw_api_object.set_location(construct_state_location)

        line.add_raw_api_object(construct_state_raw_api_object)

        # Priority
        construct_state_raw_api_object.add_raw_member("priority",
                                                      1,
                                                      "engine.util.state_machine.StateChanger")

        # Enabled abilities
        construct_state_raw_api_object.add_raw_member("enable_abilities",
                                                      [],
                                                      "engine.util.state_machine.StateChanger")

        # Disabled abilities
        disabled_forward_refs = [ForwardRef(line,
                                            f"{game_entity_name}.AttributeChangeTracker")]
        if len(line.creates) > 0:
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.Create"))
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

        if line.is_dropsite():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.DropSite"))

        if line.is_trade_post():
            disabled_forward_refs.append(ForwardRef(line,
                                                    f"{game_entity_name}.TradePost"))

        construct_state_raw_api_object.add_raw_member("disable_abilities",
                                                      disabled_forward_refs,
                                                      "engine.util.state_machine.StateChanger")

        # Enabled modifiers
        construct_state_raw_api_object.add_raw_member("enable_modifiers",
                                                      [],
                                                      "engine.util.state_machine.StateChanger")

        # Disabled modifiers
        construct_state_raw_api_object.add_raw_member("disable_modifiers",
                                                      [],
                                                      "engine.util.state_machine.StateChanger")
        # =====================================================================================
        construct_state_forward_ref = ForwardRef(line, construct_state_name)
        property_raw_api_object.add_raw_member("state_change",
                                               construct_state_forward_ref,
                                               "engine.util.progress.property.type.StateChange")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.StateChange"]: property_forward_ref
        })
        # =====================================================================================
        progress_raw_api_object.add_raw_member("properties",
                                               properties,
                                               "engine.util.progress.Progress")

        progress_forward_refs.append(ForwardRef(line, progress_ref))
        # =====================================================================================
        progress_ref = f"{ability_ref}.ConstructionProgress50"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "ConstructionProgress50",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member("type",
                                               api_objects["engine.util.progress_type.type.Construct"],
                                               "engine.util.progress.Progress")

        # Interval = (25.0, 50.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               25.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               50.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =================================================================================
        # Idle override
        # =================================================================================
        if construction_animation_id > -1:
            property_ref = f"{progress_ref}.Animated"
            property_raw_api_object = RawAPIObject(property_ref,
                                                   "Animated",
                                                   dataset.nyan_api_objects)
            property_raw_api_object.add_raw_parent(
                "engine.util.progress.property.type.Animated")
            property_location = ForwardRef(line, progress_ref)
            property_raw_api_object.set_location(property_location)

            line.add_raw_api_object(property_raw_api_object)

            overrides = []
            override_ref = f"{progress_ref}.IdleOverride"
            override_raw_api_object = RawAPIObject(override_ref,
                                                   "IdleOverride",
                                                   dataset.nyan_api_objects)
            override_raw_api_object.add_raw_parent(
                "engine.util.animation_override.AnimationOverride")
            override_location = ForwardRef(line, property_ref)
            override_raw_api_object.set_location(override_location)

            line.add_raw_api_object(override_raw_api_object)

            idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
            override_raw_api_object.add_raw_member("ability",
                                                   idle_forward_ref,
                                                   "engine.util.animation_override.AnimationOverride")

            # Animation
            animations_set = []
            animation_forward_ref = create_animation(line,
                                                     construction_animation_id,
                                                     override_ref,
                                                     "Idle",
                                                     "idle_construct50_override_")

            animations_set.append(animation_forward_ref)
            override_raw_api_object.add_raw_member("animations",
                                                   animations_set,
                                                   "engine.util.animation_override.AnimationOverride")

            override_raw_api_object.add_raw_member("priority",
                                                   1,
                                                   "engine.util.animation_override.AnimationOverride")

            override_forward_ref = ForwardRef(line, override_ref)
            overrides.append(override_forward_ref)
            # =================================================================================
            property_raw_api_object.add_raw_member("overrides",
                                                   overrides,
                                                   "engine.util.progress.property.type.Animated")

            property_forward_ref = ForwardRef(line, property_ref)
            properties.update({
                api_objects["engine.util.progress.property.type.Animated"]: property_forward_ref
            })

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

        # State change
        property_raw_api_object.add_raw_member("state_change",
                                               construct_state_forward_ref,
                                               "engine.util.progress.property.type.StateChange")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.StateChange"]: property_forward_ref
        })
        # =====================================================================================
        progress_raw_api_object.add_raw_member("properties",
                                               properties,
                                               "engine.util.progress.Progress")

        progress_forward_refs.append(ForwardRef(line, progress_ref))
        # =====================================================================================
        progress_ref = f"{ability_ref}.ConstructionProgress75"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "ConstructionProgress75",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member("type",
                                               api_objects["engine.util.progress_type.type.Construct"],
                                               "engine.util.progress.Progress")

        # Interval = (50.0, 75.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               50.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               75.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =================================================================================
        # Idle override
        # =================================================================================
        if construction_animation_id > -1:
            property_ref = f"{progress_ref}.Animated"
            property_raw_api_object = RawAPIObject(property_ref,
                                                   "Animated",
                                                   dataset.nyan_api_objects)
            property_raw_api_object.add_raw_parent(
                "engine.util.progress.property.type.Animated")
            property_location = ForwardRef(line, progress_ref)
            property_raw_api_object.set_location(property_location)

            line.add_raw_api_object(property_raw_api_object)

            overrides = []
            override_ref = f"{progress_ref}.IdleOverride"
            override_raw_api_object = RawAPIObject(override_ref,
                                                   "IdleOverride",
                                                   dataset.nyan_api_objects)
            override_raw_api_object.add_raw_parent(
                "engine.util.animation_override.AnimationOverride")
            override_location = ForwardRef(line, property_ref)
            override_raw_api_object.set_location(override_location)

            line.add_raw_api_object(override_raw_api_object)

            idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
            override_raw_api_object.add_raw_member("ability",
                                                   idle_forward_ref,
                                                   "engine.util.animation_override.AnimationOverride")

            # Animation
            animations_set = []
            animation_forward_ref = create_animation(line,
                                                     construction_animation_id,
                                                     override_ref,
                                                     "Idle",
                                                     "idle_construct75_override_")

            animations_set.append(animation_forward_ref)
            override_raw_api_object.add_raw_member("animations",
                                                   animations_set,
                                                   "engine.util.animation_override.AnimationOverride")

            override_raw_api_object.add_raw_member("priority",
                                                   1,
                                                   "engine.util.animation_override.AnimationOverride")

            override_forward_ref = ForwardRef(line, override_ref)
            overrides.append(override_forward_ref)
            # =================================================================================
            property_raw_api_object.add_raw_member("overrides",
                                                   overrides,
                                                   "engine.util.progress.property.type.Animated")

            property_forward_ref = ForwardRef(line, property_ref)
            properties.update({
                api_objects["engine.util.progress.property.type.Animated"]: property_forward_ref
            })

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

        # State change
        property_raw_api_object.add_raw_member("state_change",
                                               construct_state_forward_ref,
                                               "engine.util.progress.property.type.StateChange")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.StateChange"]: property_forward_ref
        })
        # =====================================================================================
        progress_raw_api_object.add_raw_member("properties",
                                               properties,
                                               "engine.util.progress.Progress")

        progress_forward_refs.append(ForwardRef(line, progress_ref))
        # =====================================================================================
        progress_ref = f"{ability_ref}.ConstructionProgress100"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               "ConstructionProgress100",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member("type",
                                               api_objects["engine.util.progress_type.type.Construct"],
                                               "engine.util.progress.Progress")

        # Interval = (75.0, 100.0)
        progress_raw_api_object.add_raw_member("left_boundary",
                                               75.0,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               100.0,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =================================================================================
        # Idle override
        # =================================================================================
        if construction_animation_id > -1:
            property_ref = f"{progress_ref}.Animated"
            property_raw_api_object = RawAPIObject(property_ref,
                                                   "Animated",
                                                   dataset.nyan_api_objects)
            property_raw_api_object.add_raw_parent(
                "engine.util.progress.property.type.Animated")
            property_location = ForwardRef(line, progress_ref)
            property_raw_api_object.set_location(property_location)

            line.add_raw_api_object(property_raw_api_object)

            overrides = []
            override_ref = f"{progress_ref}.IdleOverride"
            override_raw_api_object = RawAPIObject(override_ref,
                                                   "IdleOverride",
                                                   dataset.nyan_api_objects)
            override_raw_api_object.add_raw_parent(
                "engine.util.animation_override.AnimationOverride")
            override_location = ForwardRef(line, progress_ref)
            override_raw_api_object.set_location(override_location)

            line.add_raw_api_object(override_raw_api_object)

            idle_forward_ref = ForwardRef(line, f"{game_entity_name}.Idle")
            override_raw_api_object.add_raw_member("ability",
                                                   idle_forward_ref,
                                                   "engine.util.animation_override.AnimationOverride")

            # Animation
            animations_set = []
            animation_forward_ref = create_animation(line,
                                                     construction_animation_id,
                                                     override_ref,
                                                     "Idle",
                                                     "idle_construct100_override_")

            animations_set.append(animation_forward_ref)
            override_raw_api_object.add_raw_member("animations",
                                                   animations_set,
                                                   "engine.util.animation_override.AnimationOverride")

            override_raw_api_object.add_raw_member("priority",
                                                   1,
                                                   "engine.util.animation_override.AnimationOverride")

            override_forward_ref = ForwardRef(line, override_ref)
            overrides.append(override_forward_ref)
            # =================================================================================
            property_raw_api_object.add_raw_member("overrides",
                                                   overrides,
                                                   "engine.util.progress.property.type.Animated")

            property_forward_ref = ForwardRef(line, property_ref)
            properties.update({
                api_objects["engine.util.progress.property.type.Animated"]: property_forward_ref
            })

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

        # State change
        property_raw_api_object.add_raw_member("state_change",
                                               construct_state_forward_ref,
                                               "engine.util.progress.property.type.StateChange")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.util.progress.property.type.StateChange"]: property_forward_ref
        })
        # =====================================================================================
        progress_raw_api_object.add_raw_member("properties",
                                               properties,
                                               "engine.util.progress.Progress")

        progress_forward_refs.append(ForwardRef(line, progress_ref))
    # =====================================================================================
    ability_raw_api_object.add_raw_member("construction_progress",
                                          progress_forward_refs,
                                          "engine.ability.type.Constructable")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
