# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-lines,too-many-statements
# pylint: disable=too-few-public-methods,too-many-branches
#
# TODO:
# pylint: disable=line-too-long

"""
Creates upgrade patches for abilities.
"""
from .....nyan.nyan_structs import MemberOperator
from ....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from ....entity_object.conversion.converter_object import RawAPIObject
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef
from ....value_object.read.value_members import NoDiffMember
from ..aoc.upgrade_ability_subprocessor import AoCUpgradeAbilitySubprocessor


class RoRUpgradeAbilitySubprocessor:
    """
    Creates raw API objects for ability upgrade effects in RoR.
    """

    @staticmethod
    def shoot_projectile_ability(converter_group, line, container_obj_ref,
                                 command_id, diff=None):
        """
        Creates a patch for the Selectable ability of a line.

        :param converter_group: Group that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param container_obj_ref: Reference of the raw API object the patch is nested in.
        :type container_obj_ref: str
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The forward references for the generated patches.
        :rtype: list
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

        changed = False
        if diff:
            diff_animation = diff["attack_sprite_id"]
            diff_comm_sound = diff["command_sound_id"]
            diff_min_range = diff["weapon_range_min"]
            diff_max_range = diff["weapon_range_min"]
            diff_reload_time = diff["attack_speed"]
            # spawn delay also depends on animation
            diff_spawn_delay = diff["frame_delay"]
            diff_spawn_area_offsets = diff["weapon_offset"]

            if any(not isinstance(value, NoDiffMember) for value in (diff_animation,
                                                                     diff_comm_sound,
                                                                     diff_min_range,
                                                                     diff_max_range,
                                                                     diff_reload_time,
                                                                     diff_spawn_delay,
                                                                     diff_spawn_area_offsets)):
                changed = True

        if changed:
            patch_target_ref = f"{game_entity_name}.{ability_name}"
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)

            # Wrapper
            wrapper_name = f"Change{game_entity_name}{ability_name}Wrapper"
            wrapper_ref = f"{container_obj_ref}.{wrapper_name}"
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            if isinstance(line, GenieBuildingLineGroup):
                # Store building upgrades next to their game entity definition,
                # not in the Age up techs.
                wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                    % (name_lookup_dict[head_unit_id][1]))
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
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

            if not isinstance(diff_animation, NoDiffMember):
                animations_set = []
                diff_animation_id = diff_animation.get_value()
                if diff_animation_id > -1:
                    # Patch the new animation in
                    animation_forward_ref = AoCUpgradeAbilitySubprocessor.create_animation(converter_group,
                                                                                           line,
                                                                                           diff_animation_id,
                                                                                           nyan_patch_ref,
                                                                                           ability_name,
                                                                                           "%s_"
                                                                                           % command_lookup_dict[command_id][1])
                    animations_set.append(animation_forward_ref)

                nyan_patch_raw_api_object.add_raw_patch_member("animations",
                                                               animations_set,
                                                               "engine.ability.specialization.AnimatedAbility",
                                                               MemberOperator.ASSIGN)

            if not isinstance(diff_comm_sound, NoDiffMember):
                sounds_set = []
                diff_comm_sound_id = diff_comm_sound.get_value()
                if diff_comm_sound_id > -1:
                    # Patch the new sound in
                    sound_forward_ref = AoCUpgradeAbilitySubprocessor.create_sound(converter_group,
                                                                                   diff_comm_sound_id,
                                                                                   nyan_patch_ref,
                                                                                   ability_name,
                                                                                   "%s_"
                                                                                   % command_lookup_dict[command_id][1])
                    sounds_set.append(sound_forward_ref)

                nyan_patch_raw_api_object.add_raw_patch_member("sounds",
                                                               sounds_set,
                                                               "engine.ability.specialization.CommandSoundAbility",
                                                               MemberOperator.ASSIGN)

            if not isinstance(diff_min_range, NoDiffMember):
                min_range = diff_min_range.get_value()

                nyan_patch_raw_api_object.add_raw_patch_member("min_range",
                                                               min_range,
                                                               "engine.ability.type.ShootProjectile",
                                                               MemberOperator.ADD)

            if not isinstance(diff_max_range, NoDiffMember):
                max_range = diff_max_range.get_value()

                nyan_patch_raw_api_object.add_raw_patch_member("max_range",
                                                               max_range,
                                                               "engine.ability.type.ShootProjectile",
                                                               MemberOperator.ADD)

            if not isinstance(diff_reload_time, NoDiffMember):
                reload_time = diff_reload_time.get_value()

                nyan_patch_raw_api_object.add_raw_patch_member("reload_time",
                                                               reload_time,
                                                               "engine.ability.type.ShootProjectile",
                                                               MemberOperator.ADD)

            if not isinstance(diff_spawn_delay, NoDiffMember):
                if not isinstance(diff_animation, NoDiffMember):
                    attack_graphic_id = diff_animation.get_value()

                else:
                    attack_graphic_id = diff_animation.value.get_value()

                attack_graphic = dataset.genie_graphics[attack_graphic_id]
                frame_rate = attack_graphic.get_frame_rate()
                frame_delay = diff_spawn_delay.get_value()
                spawn_delay = frame_rate * frame_delay

                nyan_patch_raw_api_object.add_raw_patch_member("spawn_delay",
                                                               spawn_delay,
                                                               "engine.ability.type.ShootProjectile",
                                                               MemberOperator.ASSIGN)

            if not isinstance(diff_spawn_area_offsets, NoDiffMember):
                diff_spawn_area_x = diff_spawn_area_offsets[0]
                diff_spawn_area_y = diff_spawn_area_offsets[1]
                diff_spawn_area_z = diff_spawn_area_offsets[2]

                if not isinstance(diff_spawn_area_x, NoDiffMember):
                    spawn_area_x = diff_spawn_area_x.get_value()

                    nyan_patch_raw_api_object.add_raw_patch_member("spawning_area_offset_x",
                                                                   spawn_area_x,
                                                                   "engine.ability.type.ShootProjectile",
                                                                   MemberOperator.ADD)

                if not isinstance(diff_spawn_area_y, NoDiffMember):
                    spawn_area_y = diff_spawn_area_y.get_value()

                    nyan_patch_raw_api_object.add_raw_patch_member("spawning_area_offset_y",
                                                                   spawn_area_y,
                                                                   "engine.ability.type.ShootProjectile",
                                                                   MemberOperator.ADD)

                if not isinstance(diff_spawn_area_z, NoDiffMember):
                    spawn_area_z = diff_spawn_area_z.get_value()

                    nyan_patch_raw_api_object.add_raw_patch_member("spawning_area_offset_z",
                                                                   spawn_area_z,
                                                                   "engine.ability.type.ShootProjectile",
                                                                   MemberOperator.ADD)

            patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_forward_ref,
                                                  "engine.aux.patch.Patch")

            converter_group.add_raw_api_object(wrapper_raw_api_object)
            converter_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
            patches.append(wrapper_forward_ref)

        return patches
