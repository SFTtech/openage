# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for abilities.
"""
from openage.convert.dataformat.aoc.genie_unit import GenieBuildingLineGroup,\
    GenieAmbientGroup
from openage.convert.dataformat.aoc.internal_nyan_names import BUILDING_LINE_LOOKUPS,\
    AMBIENT_GROUP_LOOKUPS, UNIT_LINE_LOOKUPS, TECH_GROUP_LOOKUPS
from openage.convert.dataformat.value_members import NoDiffMember,\
    LeftMissingMember, RightMissingMember
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.nyan.nyan_structs import MemberOperator
from openage.convert.dataformat.aoc.combined_sprite import CombinedSprite
from openage.convert.dataformat.aoc.combined_sound import CombinedSound


class AoCUgradeAbilitySubprocessor:

    @staticmethod
    def idle_ability(tech_group, line, diff=None, member_dict=None):
        """
        Creates a patch for the Idle ability of a line. You can either supply a
        diff between two units in the line or name the updated members specifically
        with a member dict.

        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

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
                return []

            if isinstance(diff_animation, LeftMissingMember):
                # TODO: Implement
                return []

            if isinstance(diff_animation, RightMissingMember):
                # TODO: Implement
                return []

            diff_animation_id = diff_animation.get_value()

        elif "Idle.animation_id" in member_dict.keys():
            diff_animation_id = member_dict["Idle.animation_id"]

        else:
            return []

        patch_target_ref = "%s.Idle" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sIdleAnimationWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

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

        return [wrapper_expected_pointer]

    @staticmethod
    def _create_animation(tech_group, animation_id, nyan_patch_ref, animation_name, filename_prefix):
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
            animation_sprite = CombinedSprite(animation_id,
                                              "%s%s" % (filename_prefix,
                                                        TECH_GROUP_LOOKUPS[tech_id][1]),
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
                sound = CombinedSound(sound_id,
                                      file_id,
                                      "%ssound_%s" % (filename_prefix, str(file_id)),
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
