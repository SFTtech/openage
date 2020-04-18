# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for abilities.
"""
from openage.convert.dataformat.aoc.genie_unit import GenieBuildingLineGroup,\
    GenieAmbientGroup
from openage.convert.dataformat.aoc.internal_nyan_names import BUILDING_LINE_LOOKUPS,\
    AMBIENT_GROUP_LOOKUPS, UNIT_LINE_LOOKUPS, TECH_GROUP_LOOKUPS,\
    COMMAND_TYPE_LOOKUPS
from openage.convert.dataformat.value_members import NoDiffMember
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.nyan.nyan_structs import MemberOperator, MemberSpecialValue
from openage.convert.dataformat.aoc.combined_sprite import CombinedSprite
from openage.convert.dataformat.aoc.combined_sound import CombinedSound
from math import degrees
from openage.convert.processor.aoc.upgrade_effect_subprocessor import AoCUpgradeEffectSubprocessor


class AoCUgradeAbilitySubprocessor:

    @staticmethod
    def apply_continuous_effect_ability(tech_group, line, command_id, ranged=False, diff=None):
        """
        Creates a patch for the ApplyContinuousEffect ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]
        ability_name = COMMAND_TYPE_LOOKUPS[command_id][0]

        changed = False
        diff_animation = diff.get_member("attack_sprite_id")
        diff_comm_sound = diff.get_member("command_sound_id")
        diff_frame_delay = diff.get_member("frame_delay")
        if any(not isinstance(value, NoDiffMember) for value in (diff_animation,
                                                                 diff_comm_sound,
                                                                 diff_frame_delay)):
            changed = True

        # Command types Heal, Construct, Repair are not upgraded by lines

        diff_min_range = None
        diff_max_range = None
        if not changed and ranged:
            diff_min_range = diff.get_member("weapon_range_min")
            diff_max_range = diff.get_member("weapon_range_max")
            if any(not isinstance(value, NoDiffMember) for value in (diff_min_range,
                                                                     diff_max_range)):
                changed = True

        if changed:
            patch_target_ref = "%s.%s" % (game_entity_name, ability_name)
            patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

            # Wrapper
            wrapper_name = "Change%s%sWrapper" % (game_entity_name, ability_name)
            wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            if isinstance(line, GenieBuildingLineGroup):
                # Store building upgrades next to their game entity definition,
                # not in the Age up techs.
                wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                    % (BUILDING_LINE_LOOKUPS[head_unit_id][1]))
                wrapper_raw_api_object.set_filename("%s_upgrade" % TECH_GROUP_LOOKUPS[tech_id][1])

            else:
                wrapper_raw_api_object.set_location(ExpectedPointer(tech_group, tech_name))

            # Nyan patch
            nyan_patch_name = "Change%s%s" % (game_entity_name, ability_name)
            nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
            nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

            if not isinstance(diff_animation, NoDiffMember):
                diff_animation_id = diff_animation.get_value()
                animations_set = []
                if diff_animation_id > -1:
                    # Patch the new animation in
                    animation_expected_pointer = AoCUgradeAbilitySubprocessor._create_animation(tech_group,
                                                                                                line,
                                                                                                diff_animation_id,
                                                                                                nyan_patch_ref,
                                                                                                ability_name,
                                                                                                "%s_"
                                                                                                % COMMAND_TYPE_LOOKUPS[command_id][1])
                    animations_set.append(animation_expected_pointer)

                nyan_patch_raw_api_object.add_raw_patch_member("animations",
                                                               animations_set,
                                                               "engine.ability.specialization.AnimatedAbility",
                                                               MemberOperator.ASSIGN)

            if not isinstance(diff_comm_sound, NoDiffMember):
                sounds_set = []
                diff_comm_sound_id = diff_comm_sound.get_value()
                if diff_comm_sound_id > -1:
                    # Patch the new sound in
                    sound_expected_pointer = AoCUgradeAbilitySubprocessor._create_sound(tech_group,
                                                                                        diff_comm_sound_id,
                                                                                        nyan_patch_ref,
                                                                                        ability_name,
                                                                                        "%s_"
                                                                                        % COMMAND_TYPE_LOOKUPS[command_id][1])
                    sounds_set.append(sound_expected_pointer)

                nyan_patch_raw_api_object.add_raw_patch_member("sounds",
                                                               sounds_set,
                                                               "engine.ability.specialization.CommandSoundAbility",
                                                               MemberOperator.ASSIGN)

            if not isinstance(diff_frame_delay, NoDiffMember):
                # TODO: Calculate this
                pass

            if ranged:
                if not isinstance(diff_min_range, NoDiffMember):
                    min_range = diff_min_range.get_value()

                    nyan_patch_raw_api_object.add_raw_patch_member("min_range",
                                                                   min_range,
                                                                   "engine.ability.type.RangedApplyContinuousEffect",
                                                                   MemberOperator.ADD)

                if not isinstance(diff_max_range, NoDiffMember):
                    max_range = diff_max_range.get_value()

                    nyan_patch_raw_api_object.add_raw_patch_member("max_range",
                                                                   max_range,
                                                                   "engine.ability.type.RangedApplyContinuousEffect",
                                                                   MemberOperator.ADD)

            patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_expected_pointer,
                                                  "engine.aux.patch.Patch")

            tech_group.add_raw_api_object(wrapper_raw_api_object)
            tech_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
            patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def apply_discrete_effect_ability(tech_group, line, command_id, ranged=False, diff=None):
        """
        Creates a patch for the ApplyDiscreteEffect ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]
        ability_name = COMMAND_TYPE_LOOKUPS[command_id][0]

        changed = False
        diff_animation = diff.get_member("attack_sprite_id")
        diff_comm_sound = diff.get_member("command_sound_id")
        diff_reload_time = diff.get_member("attack_speed")
        diff_frame_delay = diff.get_member("frame_delay")
        if any(not isinstance(value, NoDiffMember) for value in (diff_animation,
                                                                 diff_comm_sound,
                                                                 diff_reload_time,
                                                                 diff_frame_delay)):
            changed = True

        diff_min_range = None
        diff_max_range = None
        if not changed and ranged:
            diff_min_range = diff.get_member("weapon_range_min")
            diff_max_range = diff.get_member("weapon_range_max")
            if any(not isinstance(value, NoDiffMember) for value in (diff_min_range,
                                                                     diff_max_range)):
                changed = True

        if changed:
            patch_target_ref = "%s.%s" % (game_entity_name, ability_name)
            patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

            # Wrapper
            wrapper_name = "Change%s%sWrapper" % (game_entity_name, ability_name)
            wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            if isinstance(line, GenieBuildingLineGroup):
                # Store building upgrades next to their game entity definition,
                # not in the Age up techs.
                wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                    % (BUILDING_LINE_LOOKUPS[head_unit_id][1]))
                wrapper_raw_api_object.set_filename("%s_upgrade" % TECH_GROUP_LOOKUPS[tech_id][1])

            else:
                wrapper_raw_api_object.set_location(ExpectedPointer(tech_group, tech_name))

            # Nyan patch
            nyan_patch_name = "Change%s%s" % (game_entity_name, ability_name)
            nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
            nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

            if not isinstance(diff_animation, NoDiffMember):
                diff_animation_id = diff_animation.get_value()
                animations_set = []
                if diff_animation_id > -1:
                    # Patch the new animation in
                    animation_expected_pointer = AoCUgradeAbilitySubprocessor._create_animation(tech_group,
                                                                                                line,
                                                                                                diff_animation_id,
                                                                                                nyan_patch_ref,
                                                                                                ability_name,
                                                                                                "%s_"
                                                                                                % COMMAND_TYPE_LOOKUPS[command_id][1])
                    animations_set.append(animation_expected_pointer)

                nyan_patch_raw_api_object.add_raw_patch_member("animations",
                                                               animations_set,
                                                               "engine.ability.specialization.AnimatedAbility",
                                                               MemberOperator.ASSIGN)

            if not isinstance(diff_comm_sound, NoDiffMember):
                sounds_set = []
                diff_comm_sound_id = diff_comm_sound.get_value()
                if diff_comm_sound_id > -1:
                    # Patch the new sound in
                    sound_expected_pointer = AoCUgradeAbilitySubprocessor._create_sound(tech_group,
                                                                                        diff_comm_sound_id,
                                                                                        nyan_patch_ref,
                                                                                        ability_name,
                                                                                        "%s_"
                                                                                        % COMMAND_TYPE_LOOKUPS[command_id][1])
                    sounds_set.append(sound_expected_pointer)

                nyan_patch_raw_api_object.add_raw_patch_member("sounds",
                                                               sounds_set,
                                                               "engine.ability.specialization.CommandSoundAbility",
                                                               MemberOperator.ASSIGN)

            if not isinstance(diff_reload_time, NoDiffMember):
                reload_time = diff_reload_time.get_value()

                nyan_patch_raw_api_object.add_raw_patch_member("reload_time",
                                                               reload_time,
                                                               "engine.ability.type.ApplyDiscreteEffect",
                                                               MemberOperator.ADD)

            if not isinstance(diff_frame_delay, NoDiffMember):
                # TODO: Calculate this
                pass

            if ranged:
                if not isinstance(diff_min_range, NoDiffMember):
                    min_range = diff_min_range.get_value()

                    nyan_patch_raw_api_object.add_raw_patch_member("min_range",
                                                                   min_range,
                                                                   "engine.ability.type.RangedApplyDiscreteEffect",
                                                                   MemberOperator.ADD)

                if not isinstance(diff_max_range, NoDiffMember):
                    max_range = diff_max_range.get_value()

                    nyan_patch_raw_api_object.add_raw_patch_member("max_range",
                                                                   max_range,
                                                                   "engine.ability.type.RangedApplyDiscreteEffect",
                                                                   MemberOperator.ADD)

            patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_expected_pointer,
                                                  "engine.aux.patch.Patch")

            tech_group.add_raw_api_object(wrapper_raw_api_object)
            tech_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
            patches.append(wrapper_expected_pointer)

        # Seperate because effects get their own wrappers from the subprocessor
        changed = False
        diff_attacks = None
        if not changed and command_id == 7:
            diff_attacks = diff.get_member("attacks")
            if not isinstance(diff_attacks, NoDiffMember):
                changed = True

        if changed:
            patch_target_ref = "%s.%s" % (game_entity_name, ability_name)
            if command_id == 7 and not isinstance(diff_attacks, NoDiffMember):
                patches.extend(AoCUpgradeEffectSubprocessor.get_attack_effects(tech_group,
                                                                               line, diff,
                                                                               patch_target_ref))

        return patches

    @staticmethod
    def attribute_change_tracker_ability(tech_group, line, diff=None):
        """
        Creates a patch for the AttributeChangeTracker ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def death_ability(tech_group, line, diff=None):
        """
        Creates a patch for the Death ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        if diff:
            diff_animation = diff.get_member("dying_graphic")
            if isinstance(diff_animation, NoDiffMember):
                return patches

            diff_animation_id = diff_animation.get_value()

        else:
            return patches

        patch_target_ref = "%s.Death" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sDeathAnimationWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        if isinstance(line, GenieBuildingLineGroup):
            # Store building upgrades next to their game entity definition,
            # not in the Age up techs.
            wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                % (BUILDING_LINE_LOOKUPS[head_unit_id][1]))
            wrapper_raw_api_object.set_filename("%s_upgrade" % TECH_GROUP_LOOKUPS[tech_id][1])

        else:
            wrapper_raw_api_object.set_location(ExpectedPointer(tech_group, tech_name))

        # Nyan patch
        nyan_patch_name = "Change%sDeathAnimation" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        animations_set = []
        if diff_animation_id > -1:
            # Patch the new animation in
            animation_expected_pointer = AoCUgradeAbilitySubprocessor._create_animation(tech_group,
                                                                                        line,
                                                                                        diff_animation_id,
                                                                                        nyan_patch_ref,
                                                                                        "Death",
                                                                                        "death_")
            animations_set.append(animation_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("animations",
                                                       animations_set,
                                                       "engine.ability.specialization.AnimatedAbility",
                                                       MemberOperator.ASSIGN)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def despawn_ability(tech_group, line, diff=None):
        """
        Creates a patch for the Despawn ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        if diff:
            diff_dead_unit = diff.get_member("dead_unit_id")
            if isinstance(diff_dead_unit, NoDiffMember):
                return patches

            diff_animation_id = dataset.genie_units[diff_dead_unit.get_value()]["idle_graphic0"].get_value()

        else:
            return patches

        patch_target_ref = "%s.Despawn" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sDespawnAnimationWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        if isinstance(line, GenieBuildingLineGroup):
            # Store building upgrades next to their game entity definition,
            # not in the Age up techs.
            wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                % (BUILDING_LINE_LOOKUPS[head_unit_id][1]))
            wrapper_raw_api_object.set_filename("%s_upgrade" % TECH_GROUP_LOOKUPS[tech_id][1])

        else:
            wrapper_raw_api_object.set_location(ExpectedPointer(tech_group, tech_name))

        # Nyan patch
        nyan_patch_name = "Change%sDespawnAnimation" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        animations_set = []
        if diff_animation_id > -1:
            # Patch the new animation in
            animation_expected_pointer = AoCUgradeAbilitySubprocessor._create_animation(tech_group,
                                                                                        line,
                                                                                        diff_animation_id,
                                                                                        nyan_patch_ref,
                                                                                        "Despawn",
                                                                                        "despawn_")
            animations_set.append(animation_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("animations",
                                                       animations_set,
                                                       "engine.ability.specialization.AnimatedAbility",
                                                       MemberOperator.ASSIGN)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def idle_ability(tech_group, line, diff=None):
        """
        Creates a patch for the Idle ability of a line.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        if diff:
            diff_animation = diff.get_member("idle_graphic0")
            if isinstance(diff_animation, NoDiffMember):
                return patches

            diff_animation_id = diff_animation.get_value()

        else:
            return patches

        patch_target_ref = "%s.Idle" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sIdleAnimationWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        if isinstance(line, GenieBuildingLineGroup):
            # Store building upgrades next to their game entity definition,
            # not in the Age up techs.
            wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                % (BUILDING_LINE_LOOKUPS[head_unit_id][1]))
            wrapper_raw_api_object.set_filename("%s_upgrade" % TECH_GROUP_LOOKUPS[tech_id][1])

        else:
            wrapper_raw_api_object.set_location(ExpectedPointer(tech_group, tech_name))

        # Nyan patch
        nyan_patch_name = "Change%sIdleAnimation" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        animations_set = []
        if diff_animation_id > -1:
            # Patch the new animation in
            animation_expected_pointer = AoCUgradeAbilitySubprocessor._create_animation(tech_group,
                                                                                        line,
                                                                                        diff_animation_id,
                                                                                        nyan_patch_ref,
                                                                                        "Idle",
                                                                                        "idle_")
            animations_set.append(animation_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("animations",
                                                       animations_set,
                                                       "engine.ability.specialization.AnimatedAbility",
                                                       MemberOperator.ASSIGN)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def live_ability(tech_group, line, diff=None):
        """
        Creates a patch for the Live ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        if diff:
            diff_hp = diff.get_member("hit_points")
            if isinstance(diff_hp, NoDiffMember):
                return patches

            diff_hp_value = diff_hp.get_value()

        else:
            return patches

        patch_target_ref = "%s.Live.Health" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sHealthWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        if isinstance(line, GenieBuildingLineGroup):
            # Store building upgrades next to their game entity definition,
            # not in the Age up techs.
            wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                % (BUILDING_LINE_LOOKUPS[head_unit_id][1]))
            wrapper_raw_api_object.set_filename("%s_upgrade" % TECH_GROUP_LOOKUPS[tech_id][1])

        else:
            wrapper_raw_api_object.set_location(ExpectedPointer(tech_group, tech_name))

        # Nyan patch
        nyan_patch_name = "Change%sHealth" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        # HP max value
        nyan_patch_raw_api_object.add_raw_patch_member("max_value",
                                                       diff_hp_value,
                                                       "engine.aux.attribute.AttributeSetting",
                                                       MemberOperator.ADD)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def los_ability(tech_group, line, diff=None):
        """
        Creates a patch for the LineOfSight ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        if diff:
            diff_line_of_sight = diff.get_member("line_of_sight")
            if isinstance(diff_line_of_sight, NoDiffMember):
                return patches

            diff_los_range = diff_line_of_sight.get_value()

        else:
            return patches

        patch_target_ref = "%s.LineOfSight" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sLineOfSightWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        if isinstance(line, GenieBuildingLineGroup):
            # Store building upgrades next to their game entity definition,
            # not in the Age up techs.
            wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                % (BUILDING_LINE_LOOKUPS[head_unit_id][1]))
            wrapper_raw_api_object.set_filename("%s_upgrade" % TECH_GROUP_LOOKUPS[tech_id][1])

        else:
            wrapper_raw_api_object.set_location(ExpectedPointer(tech_group, tech_name))

        # Nyan patch
        nyan_patch_name = "Change%sLineOfSight" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        # Line of Sight
        nyan_patch_raw_api_object.add_raw_patch_member("range",
                                                       diff_los_range,
                                                       "engine.ability.type.LineOfSight",
                                                       MemberOperator.ADD)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def move_ability(tech_group, line, diff=None):
        """
        Creates a patch for the Move ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        changed = False
        diff_move_animation = diff.get_member("move_graphics")
        diff_comm_sound = diff.get_member("command_sound_id")
        diff_move_speed = diff.get_member("speed")
        if any(not isinstance(value, NoDiffMember) for value in (diff_move_animation,
                                                                 diff_comm_sound,
                                                                 diff_move_speed)):
            changed = True

        if changed:
            patch_target_ref = "%s.Move" % (game_entity_name)
            patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

            # Wrapper
            wrapper_name = "Change%sMoveWrapper" % (game_entity_name)
            wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            if isinstance(line, GenieBuildingLineGroup):
                # Store building upgrades next to their game entity definition,
                # not in the Age up techs.
                wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                    % (BUILDING_LINE_LOOKUPS[head_unit_id][1]))
                wrapper_raw_api_object.set_filename("%s_upgrade" % TECH_GROUP_LOOKUPS[tech_id][1])

            else:
                wrapper_raw_api_object.set_location(ExpectedPointer(tech_group, tech_name))

            # Nyan patch
            nyan_patch_name = "Change%sMove" % (game_entity_name)
            nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
            nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

            if not isinstance(diff_move_animation, NoDiffMember):
                animations_set = []
                diff_animation_id = diff_move_animation.get_value()
                if diff_animation_id > -1:
                    # Patch the new animation in
                    animation_expected_pointer = AoCUgradeAbilitySubprocessor._create_animation(tech_group,
                                                                                                line,
                                                                                                diff_animation_id,
                                                                                                nyan_patch_ref,
                                                                                                "Move",
                                                                                                "move_")
                    animations_set.append(animation_expected_pointer)

                nyan_patch_raw_api_object.add_raw_patch_member("animations",
                                                               animations_set,
                                                               "engine.ability.specialization.AnimatedAbility",
                                                               MemberOperator.ASSIGN)

            if not isinstance(diff_comm_sound, NoDiffMember):
                sounds_set = []
                diff_comm_sound_id = diff_comm_sound.get_value()
                if diff_comm_sound_id > -1:
                    # Patch the new sound in
                    sound_expected_pointer = AoCUgradeAbilitySubprocessor._create_sound(tech_group,
                                                                                        diff_comm_sound_id,
                                                                                        nyan_patch_ref,
                                                                                        "Move",
                                                                                        "move_")
                    sounds_set.append(sound_expected_pointer)

                nyan_patch_raw_api_object.add_raw_patch_member("sounds",
                                                               sounds_set,
                                                               "engine.ability.specialization.CommandSoundAbility",
                                                               MemberOperator.ASSIGN)

            if not isinstance(diff_move_speed, NoDiffMember):
                diff_speed_value = diff_move_speed.get_value()

                nyan_patch_raw_api_object.add_raw_patch_member("speed",
                                                               diff_speed_value,
                                                               "engine.ability.type.Move",
                                                               MemberOperator.ADD)

            patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_expected_pointer,
                                                  "engine.aux.patch.Patch")

            tech_group.add_raw_api_object(wrapper_raw_api_object)
            tech_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
            patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def named_ability(tech_group, line, diff=None):
        """
        Creates a patch for the Named ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        diff_name = diff.get_member("language_dll_name")
        if not isinstance(diff_name, NoDiffMember):
            # TODO: Read from DLL file
            pass

        diff_long_description = diff.get_member("language_dll_help")
        if not isinstance(diff_long_description, NoDiffMember):
            # TODO: Read from DLL file
            pass

        return patches

    @staticmethod
    def resistance_ability(tech_group, line, diff=None):
        """
        Creates a patch for the Resistance ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]

        diff_armors = diff.get_member("armors")
        if not isinstance(diff_armors, NoDiffMember):
            patch_target_ref = "%s.Resistance" % (game_entity_name)
            patches.extend(AoCUpgradeEffectSubprocessor.get_attack_resistances(tech_group,
                                                                               line, diff,
                                                                               patch_target_ref))

        # TODO: Other resistance types

        return patches

    @staticmethod
    def selectable_ability(tech_group, line, diff=None):
        """
        Creates a patch for the Selectable ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        # First patch: Sound for the SelectableSelf ability
        changed = False
        if diff:
            diff_selection_sound = diff.get_member("selection_sound_id")
            if not isinstance(diff_selection_sound, NoDiffMember):
                changed = True

        if changed:
            patch_target_ref = "%s.SelectableSelf" % (game_entity_name)
            patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

            # Wrapper
            wrapper_name = "Change%sSelectableSelfWrapper" % (game_entity_name)
            wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            if isinstance(line, GenieBuildingLineGroup):
                # Store building upgrades next to their game entity definition,
                # not in the Age up techs.
                wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                    % (BUILDING_LINE_LOOKUPS[head_unit_id][1]))
                wrapper_raw_api_object.set_filename("%s_upgrade" % TECH_GROUP_LOOKUPS[tech_id][1])

            else:
                wrapper_raw_api_object.set_location(ExpectedPointer(tech_group, tech_name))

            # Nyan patch
            nyan_patch_name = "Change%sSelectableSelf" % (game_entity_name)
            nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
            nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

            # Change sound
            diff_selection_sound_id = diff_selection_sound.get_value()
            sounds_set = []
            if diff_selection_sound_id > -1:
                # Patch the new sound in
                sound_expected_pointer = AoCUgradeAbilitySubprocessor._create_sound(tech_group,
                                                                                    diff_selection_sound_id,
                                                                                    nyan_patch_ref,
                                                                                    "SelectableSelf",
                                                                                    "select_")
                sounds_set.append(sound_expected_pointer)

            nyan_patch_raw_api_object.add_raw_patch_member("sounds",
                                                           sounds_set,
                                                           "engine.ability.specialization.SoundAbility",
                                                           MemberOperator.ASSIGN)

            patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_expected_pointer,
                                                  "engine.aux.patch.Patch")

            tech_group.add_raw_api_object(wrapper_raw_api_object)
            tech_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
            patches.append(wrapper_expected_pointer)

        # Second patch: Selection box
        changed = False
        if diff:
            diff_radius_x = diff.get_member("selection_shape_x")
            diff_radius_y = diff.get_member("selection_shape_y")
            if any(not isinstance(value, NoDiffMember) for value in (diff_radius_x,
                                                                     diff_radius_y)):
                changed = True

        if changed:
            patch_target_ref = "%s.SelectableSelf.Rectangle" % (game_entity_name)
            patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

            # Wrapper
            wrapper_name = "Change%sSelectableRectangleWrapper" % (game_entity_name)
            wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            if isinstance(line, GenieBuildingLineGroup):
                # Store building upgrades next to their game entity definition,
                # not in the Age up techs.
                wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                    % (BUILDING_LINE_LOOKUPS[head_unit_id][1]))
                wrapper_raw_api_object.set_filename("%s_upgrade" % TECH_GROUP_LOOKUPS[tech_id][1])

            else:
                wrapper_raw_api_object.set_location(ExpectedPointer(tech_group, tech_name))

            # Nyan patch
            nyan_patch_name = "Change%sSelectableRectangle" % (game_entity_name)
            nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
            nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

            if not isinstance(diff_radius_x, NoDiffMember):
                diff_radius_x_value = diff_radius_x.get_value()

                nyan_patch_raw_api_object.add_raw_patch_member("radius_x",
                                                               diff_radius_x_value,
                                                               "engine.aux.selection_box.type.Rectangle",
                                                               MemberOperator.ADD)

            if not isinstance(diff_radius_y, NoDiffMember):
                diff_radius_y_value = diff_radius_y.get_value()

                nyan_patch_raw_api_object.add_raw_patch_member("radius_y",
                                                               diff_radius_y_value,
                                                               "engine.aux.selection_box.type.Rectangle",
                                                               MemberOperator.ADD)

            patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_expected_pointer,
                                                  "engine.aux.patch.Patch")

            tech_group.add_raw_api_object(wrapper_raw_api_object)
            tech_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
            patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def shoot_projectile_ability(tech_group, line, upgrade_source, upgrade_target,
                                 command_id, diff=None):
        """
        Creates a patch for the Selectable ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :param member_dict: A dict that supplies info for the patched members. The
                            keys in the dict should reference the changed member.
                            Values should be tuples in the form of:
                            (<patch value>, <operator>)
        :type member_dict: dict
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]
        ability_name = COMMAND_TYPE_LOOKUPS[command_id][0]

        changed = False
        if diff:
            diff_animation = diff.get_member("attack_sprite_id")
            diff_comm_sound = diff.get_member("command_sound_id")
            diff_min_projectiles = diff.get_member("attack_projectile_count")
            diff_max_projectiles = diff.get_member("attack_projectile_max_count")
            diff_min_range = diff.get_member("weapon_range_min")
            diff_max_range = diff.get_member("weapon_range_min")
            diff_reload_time = diff.get_member("attack_speed")
            # spawn delay also depends on animation
            diff_spawn_delay = diff.get_member("frame_delay")
            diff_spawn_area_offsets = diff.get_member("weapon_offset")
            diff_spawn_area_width = diff.get_member("attack_projectile_spawning_area_width")
            diff_spawn_area_height = diff.get_member("attack_projectile_spawning_area_length")
            diff_spawn_area_randomness = diff.get_member("attack_projectile_spawning_area_randomness")

            if any(not isinstance(value, NoDiffMember) for value in (diff_animation,
                                                                     diff_comm_sound,
                                                                     diff_min_projectiles,
                                                                     diff_max_projectiles,
                                                                     diff_min_range,
                                                                     diff_max_range,
                                                                     diff_reload_time,
                                                                     diff_spawn_delay,
                                                                     diff_spawn_area_offsets,
                                                                     diff_spawn_area_width,
                                                                     diff_spawn_area_height,
                                                                     diff_spawn_area_randomness)):
                changed = True

        if changed:
            patch_target_ref = "%s.%s" % (game_entity_name, ability_name)
            patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

            # Wrapper
            wrapper_name = "Change%s%sWrapper" % (game_entity_name, ability_name)
            wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            if isinstance(line, GenieBuildingLineGroup):
                # Store building upgrades next to their game entity definition,
                # not in the Age up techs.
                wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                    % (BUILDING_LINE_LOOKUPS[head_unit_id][1]))
                wrapper_raw_api_object.set_filename("%s_upgrade" % TECH_GROUP_LOOKUPS[tech_id][1])

            else:
                wrapper_raw_api_object.set_location(ExpectedPointer(tech_group, tech_name))

            # Nyan patch
            nyan_patch_name = "Change%s%s" % (game_entity_name, ability_name)
            nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
            nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

            if not isinstance(diff_animation, NoDiffMember):
                animations_set = []
                diff_animation_id = diff_animation.get_value()
                if diff_animation_id > -1:
                    # Patch the new animation in
                    animation_expected_pointer = AoCUgradeAbilitySubprocessor._create_animation(tech_group,
                                                                                                line,
                                                                                                diff_animation_id,
                                                                                                nyan_patch_ref,
                                                                                                ability_name,
                                                                                                "%s_"
                                                                                                % COMMAND_TYPE_LOOKUPS[command_id][1])
                    animations_set.append(animation_expected_pointer)

                nyan_patch_raw_api_object.add_raw_patch_member("animations",
                                                               animations_set,
                                                               "engine.ability.specialization.AnimatedAbility",
                                                               MemberOperator.ASSIGN)

            if not isinstance(diff_comm_sound, NoDiffMember):
                sounds_set = []
                diff_comm_sound_id = diff_comm_sound.get_value()
                if diff_comm_sound_id > -1:
                    # Patch the new sound in
                    sound_expected_pointer = AoCUgradeAbilitySubprocessor._create_sound(tech_group,
                                                                                        diff_comm_sound_id,
                                                                                        nyan_patch_ref,
                                                                                        ability_name,
                                                                                        "%s_"
                                                                                        % COMMAND_TYPE_LOOKUPS[command_id][1])
                    sounds_set.append(sound_expected_pointer)

                nyan_patch_raw_api_object.add_raw_patch_member("sounds",
                                                               sounds_set,
                                                               "engine.ability.specialization.CommandSoundAbility",
                                                               MemberOperator.ASSIGN)

            if not isinstance(diff_min_projectiles, NoDiffMember):
                min_projectiles = diff_min_projectiles.get_value()
                source_min_count = upgrade_source["attack_projectile_count"].get_value()
                source_max_count = upgrade_source["attack_projectile_max_count"].get_value()
                target_min_count = upgrade_target["attack_projectile_count"].get_value()
                target_max_count = upgrade_target["attack_projectile_max_count"].get_value()

                # Account for a special case where the number of projectiles are 0
                # in the .dat, but the game still counts this as 1 when a projectile
                # is defined.
                if source_min_count == 0 and source_max_count == 0:
                    min_projectiles -= 1

                if target_min_count == 0 and target_max_count == 0:
                    min_projectiles += 1

                if min_projectiles != 0:
                    nyan_patch_raw_api_object.add_raw_patch_member("min_projectiles",
                                                                   min_projectiles,
                                                                   "engine.ability.type.ShootProjectile",
                                                                   MemberOperator.ADD)

            if not isinstance(diff_max_projectiles, NoDiffMember):
                max_projectiles = diff_max_projectiles.get_value()
                source_min_count = upgrade_source["attack_projectile_count"].get_value()
                source_max_count = upgrade_source["attack_projectile_max_count"].get_value()
                target_min_count = upgrade_target["attack_projectile_count"].get_value()
                target_max_count = upgrade_target["attack_projectile_max_count"].get_value()

                # Account for a special case where the number of projectiles are 0
                # in the .dat, but the game still counts this as 1 when a projectile
                # is defined.
                if source_min_count == 0 and source_max_count == 0:
                    max_projectiles -= 1

                if target_min_count == 0 and target_max_count == 0:
                    max_projectiles += 1

                if max_projectiles != 0:
                    nyan_patch_raw_api_object.add_raw_patch_member("max_projectiles",
                                                                   max_projectiles,
                                                                   "engine.ability.type.ShootProjectile",
                                                                   MemberOperator.ADD)

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

            if not (isinstance(diff_spawn_delay, NoDiffMember)
                    and isinstance(diff_animation, NoDiffMember)):
                # TODO: Compare times
                pass

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

            if not isinstance(diff_spawn_area_width, NoDiffMember):
                spawn_area_width = diff_spawn_area_width.get_value()

                nyan_patch_raw_api_object.add_raw_patch_member("spawning_area_width",
                                                               spawn_area_width,
                                                               "engine.ability.type.ShootProjectile",
                                                               MemberOperator.ADD)

            if not isinstance(diff_spawn_area_height, NoDiffMember):
                spawn_area_height = diff_spawn_area_height.get_value()

                nyan_patch_raw_api_object.add_raw_patch_member("spawning_area_height",
                                                               spawn_area_height,
                                                               "engine.ability.type.ShootProjectile",
                                                               MemberOperator.ADD)

            if not isinstance(diff_spawn_area_randomness, NoDiffMember):
                spawn_area_randomness = diff_spawn_area_randomness.get_value()

                nyan_patch_raw_api_object.add_raw_patch_member("spawning_area_randomness",
                                                               spawn_area_randomness,
                                                               "engine.ability.type.ShootProjectile",
                                                               MemberOperator.ADD)

            patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_expected_pointer,
                                                  "engine.aux.patch.Patch")

            tech_group.add_raw_api_object(wrapper_raw_api_object)
            tech_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
            patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def turn_ability(tech_group, line, diff=None):
        """
        Creates a patch for the Turn ability of a line.

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        if diff:
            diff_turn_speed = diff.get_member("turn_speed")
            if isinstance(diff_turn_speed, NoDiffMember):
                return patches

            diff_turn_speed_value = diff_turn_speed.get_value()

        else:
            return patches

        patch_target_ref = "%s.Turn" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sTurnWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        if isinstance(line, GenieBuildingLineGroup):
            # Store building upgrades next to their game entity definition,
            # not in the Age up techs.
            wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                % (BUILDING_LINE_LOOKUPS[head_unit_id][1]))
            wrapper_raw_api_object.set_filename("%s_upgrade" % TECH_GROUP_LOOKUPS[tech_id][1])

        else:
            wrapper_raw_api_object.set_location(ExpectedPointer(tech_group, tech_name))

        # Nyan patch
        nyan_patch_name = "Change%sTurn" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        # Speed
        turn_speed_unmodified = diff_turn_speed_value
        turn_speed = MemberSpecialValue.NYAN_INF
        # Ships/Trebuchets turn slower
        if turn_speed_unmodified > 0:
            turn_yaw = diff.get_member("max_yaw_per_sec_moving").get_value()
            turn_speed = degrees(turn_yaw)

        nyan_patch_raw_api_object.add_raw_patch_member("turn_speed",
                                                       turn_speed,
                                                       "engine.ability.type.Turn",
                                                       MemberOperator.ASSIGN)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def _create_animation(tech_group, line, animation_id, nyan_patch_ref, animation_name, filename_prefix):
        """
        Generates an animation for an ability.
        """
        dataset = tech_group.data
        tech_id = tech_group.get_id()

        animation_ref = "%s.%sAnimation" % (nyan_patch_ref, animation_name)
        animation_obj_name = "%sAnimation" % (animation_name)
        animation_raw_api_object = RawAPIObject(animation_ref, animation_obj_name,
                                                dataset.nyan_api_objects)
        animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
        animation_location = ExpectedPointer(tech_group, nyan_patch_ref)
        animation_raw_api_object.set_location(animation_location)

        if animation_id in dataset.combined_sprites.keys():
            animation_sprite = dataset.combined_sprites[animation_id]

        else:
            if isinstance(line, GenieBuildingLineGroup):
                animation_filename = "%s%s_%s" % (filename_prefix,
                                                  BUILDING_LINE_LOOKUPS[line.get_head_unit_id()][1],
                                                  TECH_GROUP_LOOKUPS[tech_id][1])

            else:
                animation_filename = "%s%s" % (filename_prefix, TECH_GROUP_LOOKUPS[tech_id][1])

            animation_sprite = CombinedSprite(animation_id,
                                              animation_filename,
                                              dataset)
            dataset.combined_sprites.update({animation_sprite.get_id(): animation_sprite})

        animation_sprite.add_reference(animation_raw_api_object)

        animation_raw_api_object.add_raw_member("sprite", animation_sprite,
                                                "engine.aux.graphics.Animation")

        tech_group.add_raw_api_object(animation_raw_api_object)

        animation_expected_pointer = ExpectedPointer(tech_group, animation_ref)

        return animation_expected_pointer

    @staticmethod
    def _create_sound(tech_group, sound_id, nyan_patch_ref, sound_name, filename_prefix):
        """
        Generates a sound for an ability.
        """
        dataset = tech_group.data

        sound_ref = "%s.%sSound" % (nyan_patch_ref, sound_name)
        sound_obj_name = "%sSound" % (sound_name)
        sound_raw_api_object = RawAPIObject(sound_ref, sound_obj_name,
                                            dataset.nyan_api_objects)
        sound_raw_api_object.add_raw_parent("engine.aux.sound.Sound")
        sound_location = ExpectedPointer(tech_group, nyan_patch_ref)
        sound_raw_api_object.set_location(sound_location)

        # Search for the sound if it exists
        sounds_set = []

        genie_sound = dataset.genie_sounds[sound_id]
        file_ids = genie_sound.get_sounds(civ_id=-1)

        for file_id in file_ids:
            if file_id in dataset.combined_sounds:
                sound = dataset.combined_sounds[file_id]

            else:
                sound_filename = "%ssound_%s" % (filename_prefix, str(file_id))

                sound = CombinedSound(sound_id,
                                      file_id,
                                      sound_filename,
                                      dataset)
                dataset.combined_sounds.update({file_id: sound})

            sound.add_reference(sound_raw_api_object)
            sounds_set.append(sound)

        sound_raw_api_object.add_raw_member("play_delay",
                                            0,
                                            "engine.aux.sound.Sound")
        sound_raw_api_object.add_raw_member("sounds",
                                            sounds_set,
                                            "engine.aux.sound.Sound")

        tech_group.add_raw_api_object(sound_raw_api_object)

        sound_expected_pointer = ExpectedPointer(tech_group, sound_ref)

        return sound_expected_pointer
