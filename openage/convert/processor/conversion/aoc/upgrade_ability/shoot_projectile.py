# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create patches for upgrading the ShootProjectile ability.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberOperator
from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .....value_object.read.value_members import NoDiffMember
from .util import create_animation_patch, create_command_sound_patch

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieUnitObject
    from .....entity_object.conversion.converter_object import ConverterObject, \
        ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def shoot_projectile_ability(
    converter_group: ConverterObjectGroup,
    line: GenieGameEntityGroup,
    container_obj_ref: str,
    upgrade_source: GenieUnitObject,
    upgrade_target: GenieUnitObject,
    command_id: int,
    diff: ConverterObject = None
) -> list[ForwardRef]:
    """
    Creates a patch for the ShootProjectile ability of a line.

    :param converter_group: Group that gets the patch.
    :param line: Unit/Building line that has the ability.
    :param container_obj_ref: Reference of the raw API object the patch is nested in.
    :param diff: A diff between two ConvertObject instances.
    :returns: The forward references for the generated patches.
    """
    head_unit_id = line.get_head_unit_id()
    tech_id = converter_group.get_id()
    dataset = line.data

    patches = []

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
    command_lookup_dict = internal_name_lookups.get_command_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[head_unit_id][0]
    ability_name = command_lookup_dict[command_id][0]

    data_changed = False

    diff_animation = diff["attack_sprite_id"]
    diff_comm_sound = diff["command_sound_id"]
    diff_min_projectiles = diff["projectile_min_count"]
    diff_max_projectiles = diff["projectile_max_count"]
    diff_min_range = diff["weapon_range_min"]
    diff_max_range = diff["weapon_range_min"]
    diff_reload_time = diff["attack_speed"]
    # spawn delay also depends on animation
    diff_spawn_delay = diff["frame_delay"]
    diff_spawn_area_offsets = diff["weapon_offset"]
    diff_spawn_area_width = diff["projectile_spawning_area_width"]
    diff_spawn_area_height = diff["projectile_spawning_area_length"]
    diff_spawn_area_randomness = diff["projectile_spawning_area_randomness"]

    if any(not isinstance(value, NoDiffMember) for value in (
        diff_min_projectiles,
        diff_max_projectiles,
        diff_reload_time,
        diff_spawn_delay,
        diff_spawn_area_offsets,
        diff_spawn_area_width,
        diff_spawn_area_height,
        diff_spawn_area_randomness
    )):
        data_changed = True

    if any(not isinstance(value, NoDiffMember) for value in (
        diff_min_range,
        diff_max_range
    )):
        patch_target_ref = f"{game_entity_name}.{ability_name}.Ranged"
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = f"Change{game_entity_name}{ability_name}RangedWrapper"
        wrapper_ref = f"{container_obj_ref}.{wrapper_name}"
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects)
        wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

        if isinstance(line, GenieBuildingLineGroup):
            wrapper_raw_api_object.set_location(("data/game_entity/generic/"
                                                 f"{name_lookup_dict[head_unit_id][1]}/"))
            wrapper_raw_api_object.set_filename(f"{tech_lookup_dict[tech_id][1]}_upgrade")

        else:
            wrapper_raw_api_object.set_location(ForwardRef(converter_group, container_obj_ref))

        # Nyan patch
        nyan_patch_name = f"Change{game_entity_name}{ability_name}Ranged"
        nyan_patch_ref = ForwardRef(line, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        if not isinstance(diff_min_range, NoDiffMember):
            min_range = diff_min_range.value
            nyan_patch_raw_api_object.add_raw_patch_member("min_range",
                                                           min_range,
                                                           "engine.ability.property.type.Ranged",
                                                           MemberOperator.ADD)

        if not isinstance(diff_max_range, NoDiffMember):
            max_range = diff_max_range.value
            nyan_patch_raw_api_object.add_raw_patch_member("max_range",
                                                           max_range,
                                                           "engine.ability.property.type.Ranged",
                                                           MemberOperator.ADD)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.util.patch.Patch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

    if not isinstance(diff_animation, NoDiffMember):
        diff_animation_id = diff_animation.value

        # Nyan patch
        patch_target_ref = f"{game_entity_name}.{ability_name}"
        nyan_patch_name = f"Change{game_entity_name}{ability_name}"
        wrapper, anim_patch_forward_ref = create_animation_patch(
            converter_group,
            line,
            patch_target_ref,
            nyan_patch_name,
            container_obj_ref,
            ability_name,
            f"{command_lookup_dict[command_id][1]}_",
            [diff_animation_id]
        )
        patches.append(anim_patch_forward_ref)

        if isinstance(line, GenieBuildingLineGroup):
            # Store building upgrades next to their game entity definition,
            # not in the Age up techs.
            wrapper.set_location(("data/game_entity/generic/"
                                  f"{name_lookup_dict[head_unit_id][1]}/"))
            wrapper.set_filename(f"{tech_lookup_dict[tech_id][1]}_upgrade")

    if not isinstance(diff_comm_sound, NoDiffMember):
        diff_comm_sound_id = diff_comm_sound.value

        # Nyan patch
        patch_target_ref = f"{game_entity_name}.{ability_name}"
        nyan_patch_name = f"Change{game_entity_name}{ability_name}"
        wrapper, sound_patch_forward_ref = create_command_sound_patch(
            converter_group,
            line,
            patch_target_ref,
            nyan_patch_name,
            container_obj_ref,
            ability_name,
            f"{command_lookup_dict[command_id][1]}_",
            [diff_comm_sound_id]
        )
        patches.append(sound_patch_forward_ref)

        if isinstance(line, GenieBuildingLineGroup):
            # Store building upgrades next to their game entity definition,
            # not in the Age up techs.
            wrapper.set_location(("data/game_entity/generic/"
                                  f"{name_lookup_dict[head_unit_id][1]}/"))
            wrapper.set_filename(f"{tech_lookup_dict[tech_id][1]}_upgrade")

    if data_changed:
        patch_target_ref = f"{game_entity_name}.{ability_name}"
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = f"Change{game_entity_name}{ability_name}Wrapper"
        wrapper_ref = f"{container_obj_ref}.{wrapper_name}"
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects)
        wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

        if isinstance(line, GenieBuildingLineGroup):
            # Store building upgrades next to their game entity definition,
            # not in the Age up techs.
            wrapper_raw_api_object.set_location(("data/game_entity/generic/"
                                                 f"{name_lookup_dict[head_unit_id][1]}/"))
            wrapper_raw_api_object.set_filename(f"{tech_lookup_dict[tech_id][1]}_upgrade")

        else:
            wrapper_raw_api_object.set_location(ForwardRef(converter_group, container_obj_ref))

        # Nyan patch
        nyan_patch_name = f"Change{game_entity_name}{ability_name}"
        nyan_patch_ref = f"{container_obj_ref}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        if not isinstance(diff_min_projectiles, NoDiffMember):
            min_projectiles = diff_min_projectiles.value
            source_min_count = upgrade_source["projectile_min_count"].value
            source_max_count = upgrade_source["projectile_max_count"].value
            target_min_count = upgrade_target["projectile_min_count"].value
            target_max_count = upgrade_target["projectile_max_count"].value

            # Account for a special case where the number of projectiles are 0
            # in the .dat, but the game still counts this as 1 when a projectile
            # is defined.
            if source_min_count == 0 and source_max_count == 0:
                min_projectiles -= 1

            if target_min_count == 0 and target_max_count == 0:
                min_projectiles += 1

            if min_projectiles != 0:
                nyan_patch_raw_api_object.add_raw_patch_member(
                    "min_projectiles",
                    min_projectiles,
                    "engine.ability.type.ShootProjectile",
                    MemberOperator.ADD
                )

        if not isinstance(diff_max_projectiles, NoDiffMember):
            max_projectiles = diff_max_projectiles.value
            source_min_count = upgrade_source["projectile_min_count"].value
            source_max_count = upgrade_source["projectile_max_count"].value
            target_min_count = upgrade_target["projectile_min_count"].value
            target_max_count = upgrade_target["projectile_max_count"].value

            # Account for a special case where the number of projectiles are 0
            # in the .dat, but the game still counts this as 1 when a projectile
            # is defined.
            if source_min_count == 0 and source_max_count == 0:
                max_projectiles -= 1

            if target_min_count == 0 and target_max_count == 0:
                max_projectiles += 1

            if max_projectiles != 0:
                nyan_patch_raw_api_object.add_raw_patch_member(
                    "max_projectiles",
                    max_projectiles,
                    "engine.ability.type.ShootProjectile",
                    MemberOperator.ADD
                )

        if not isinstance(diff_reload_time, NoDiffMember):
            reload_time = diff_reload_time.value
            nyan_patch_raw_api_object.add_raw_patch_member(
                "reload_time",
                reload_time,
                "engine.ability.type.ShootProjectile",
                MemberOperator.ADD
            )

        if not isinstance(diff_spawn_delay, NoDiffMember):
            if not isinstance(diff_animation, NoDiffMember):
                attack_graphic_id = diff_animation.value

            else:
                attack_graphic_id = diff_animation.ref.value

            attack_graphic = dataset.genie_graphics[attack_graphic_id]
            frame_rate = attack_graphic.get_frame_rate()
            frame_delay = diff_spawn_delay.value
            spawn_delay = frame_rate * frame_delay

            nyan_patch_raw_api_object.add_raw_patch_member(
                "spawn_delay",
                spawn_delay,
                "engine.ability.type.ShootProjectile",
                MemberOperator.ASSIGN
            )

        if not isinstance(diff_spawn_area_offsets, NoDiffMember):
            diff_spawn_area_x = diff_spawn_area_offsets[0]
            diff_spawn_area_y = diff_spawn_area_offsets[1]
            diff_spawn_area_z = diff_spawn_area_offsets[2]

            if not isinstance(diff_spawn_area_x, NoDiffMember):
                spawn_area_x = diff_spawn_area_x.value

                nyan_patch_raw_api_object.add_raw_patch_member(
                    "spawning_area_offset_x",
                    spawn_area_x,
                    "engine.ability.type.ShootProjectile",
                    MemberOperator.ADD
                )

            if not isinstance(diff_spawn_area_y, NoDiffMember):
                spawn_area_y = diff_spawn_area_y.value

                nyan_patch_raw_api_object.add_raw_patch_member(
                    "spawning_area_offset_y",
                    spawn_area_y,
                    "engine.ability.type.ShootProjectile",
                    MemberOperator.ADD
                )

            if not isinstance(diff_spawn_area_z, NoDiffMember):
                spawn_area_z = diff_spawn_area_z.value

                nyan_patch_raw_api_object.add_raw_patch_member(
                    "spawning_area_offset_z",
                    spawn_area_z,
                    "engine.ability.type.ShootProjectile",
                    MemberOperator.ADD
                )

        if not isinstance(diff_spawn_area_width, NoDiffMember):
            spawn_area_width = diff_spawn_area_width.value

            nyan_patch_raw_api_object.add_raw_patch_member(
                "spawning_area_width",
                spawn_area_width,
                "engine.ability.type.ShootProjectile",
                MemberOperator.ADD
            )

        if not isinstance(diff_spawn_area_height, NoDiffMember):
            spawn_area_height = diff_spawn_area_height.value

            nyan_patch_raw_api_object.add_raw_patch_member(
                "spawning_area_height",
                spawn_area_height,
                "engine.ability.type.ShootProjectile",
                MemberOperator.ADD
            )

        if not isinstance(diff_spawn_area_randomness, NoDiffMember):
            spawn_area_randomness = diff_spawn_area_randomness.value

            nyan_patch_raw_api_object.add_raw_patch_member(
                "spawning_area_randomness",
                spawn_area_randomness,
                "engine.ability.type.ShootProjectile",
                MemberOperator.ADD
            )

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.util.patch.Patch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

    return patches
