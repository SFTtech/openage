# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Helper functions for AoC ability upgrades.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberOperator
from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup, \
    GenieVariantGroup
from .....entity_object.conversion.combined_sound import CombinedSound
from .....entity_object.conversion.combined_sprite import CombinedSprite
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.converter_object import ConverterObjectGroup
    from openage.convert.entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def create_animation(
    converter_group: ConverterObjectGroup,
    line: GenieGameEntityGroup,
    animation_id: int,
    container_obj_ref: str,
    animation_name: str,
    filename_prefix: str
) -> ForwardRef:
    """
    Generates an animation for an ability.
    """
    dataset = converter_group.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)

    if isinstance(converter_group, GenieVariantGroup):
        group_name = str(animation_id)

    else:
        tech_id = converter_group.get_id()
        group_name = tech_lookup_dict[tech_id][1]

    animation_ref = f"{container_obj_ref}.{animation_name}Animation"
    animation_obj_name = f"{animation_name}Animation"
    animation_raw_api_object = RawAPIObject(animation_ref, animation_obj_name,
                                            dataset.nyan_api_objects)
    animation_raw_api_object.add_raw_parent("engine.util.graphics.Animation")
    animation_location = ForwardRef(converter_group, container_obj_ref)
    animation_raw_api_object.set_location(animation_location)

    if animation_id in dataset.combined_sprites.keys():
        animation_sprite = dataset.combined_sprites[animation_id]

    else:
        if isinstance(line, GenieBuildingLineGroup):
            animation_filename = (f"{filename_prefix}"
                                  f"{name_lookup_dict[line.get_head_unit_id()][1]}_"
                                  f"{group_name}")

        else:
            animation_filename = f"{filename_prefix}{group_name}"

        animation_sprite = CombinedSprite(animation_id,
                                          animation_filename,
                                          dataset)
        dataset.combined_sprites.update({animation_sprite.get_id(): animation_sprite})

    animation_sprite.add_reference(animation_raw_api_object)

    animation_raw_api_object.add_raw_member("sprite", animation_sprite,
                                            "engine.util.graphics.Animation")

    converter_group.add_raw_api_object(animation_raw_api_object)

    animation_forward_ref = ForwardRef(converter_group, animation_ref)

    return animation_forward_ref


def create_sound(
    converter_group: ConverterObjectGroup,
    sound_id: int,
    container_obj_ref: str,
    sound_name: str,
    filename_prefix: str
) -> ForwardRef:
    """
    Generates a sound for an ability.
    """
    dataset = converter_group.data

    sound_ref = f"{container_obj_ref}.{sound_name}Sound"
    sound_obj_name = f"{sound_name}Sound"
    sound_raw_api_object = RawAPIObject(sound_ref, sound_obj_name,
                                        dataset.nyan_api_objects)
    sound_raw_api_object.add_raw_parent("engine.util.sound.Sound")
    sound_location = ForwardRef(converter_group, container_obj_ref)
    sound_raw_api_object.set_location(sound_location)

    # Search for the sound if it exists
    sounds_set = []

    genie_sound = dataset.genie_sounds[sound_id]
    file_ids = genie_sound.get_sounds(civ_id=-1)

    for file_id in file_ids:
        if file_id in dataset.combined_sounds:
            sound = dataset.combined_sounds[file_id]

        else:
            sound_filename = f"{filename_prefix}sound_{str(file_id)}"

            sound = CombinedSound(sound_id,
                                  file_id,
                                  sound_filename,
                                  dataset)
            dataset.combined_sounds.update({file_id: sound})

        sound.add_reference(sound_raw_api_object)
        sounds_set.append(sound)

    sound_raw_api_object.add_raw_member("play_delay",
                                        0,
                                        "engine.util.sound.Sound")
    sound_raw_api_object.add_raw_member("sounds",
                                        sounds_set,
                                        "engine.util.sound.Sound")

    converter_group.add_raw_api_object(sound_raw_api_object)

    sound_forward_ref = ForwardRef(converter_group, sound_ref)

    return sound_forward_ref


def create_language_strings(
    converter_group: ConverterObjectGroup,
    string_id: int,
    obj_ref: str,
    obj_name_prefix: str
) -> list[ForwardRef]:
    """
    Generates a language string for an ability.
    """
    dataset = converter_group.data
    string_resources = dataset.strings.get_tables()

    string_objs = []
    for language, strings in string_resources.items():
        if string_id in strings.keys():
            string_name = f"{obj_name_prefix}String"
            string_ref = f"{obj_ref}.{string_name}"
            string_raw_api_object = RawAPIObject(string_ref, string_name,
                                                 dataset.nyan_api_objects)
            string_raw_api_object.add_raw_parent("engine.util.language.LanguageTextPair")
            string_location = ForwardRef(converter_group, obj_ref)
            string_raw_api_object.set_location(string_location)

            # Language identifier
            lang_forward_ref = dataset.pregen_nyan_objects[
                f"util.language.{language}"
            ].get_nyan_object()
            string_raw_api_object.add_raw_member("language",
                                                 lang_forward_ref,
                                                 "engine.util.language.LanguageTextPair")

            # String
            string_raw_api_object.add_raw_member("string",
                                                 strings[string_id],
                                                 "engine.util.language.LanguageTextPair")

            converter_group.add_raw_api_object(string_raw_api_object)
            string_forward_ref = ForwardRef(converter_group, string_ref)
            string_objs.append(string_forward_ref)

    return string_objs


def create_animation_patch(
    converter_group: ConverterObjectGroup,
    line: ConverterObjectGroup,
    ability_ref: str,
    patch_name_prefix: str,
    container_obj_ref: str,
    animation_name_prefix: str,
    filename_prefix: str,
    animation_ids: list[int]
) -> tuple[RawAPIObject, ForwardRef]:
    """
    Create a patch for the Animated property of an ability.

    :param converter_group: Converter group for storing the patch.
    :param line: Line that has the ability.
    :param ability_ref: Reference of the ability that has the Animated property.
    :param patch_name_prefix: Prefix to the name of the patch.
    :param container_obj_ref: Reference of the API object that should contain the
                                patch as a nested object.
    :param animation_name_prefix: Prefix to the name of the animation.
    :param filename_prefix: Prefix to the filename of the animation.
    :param animation_ids: IDs of the animations to patch in.
    :return: A 2-tuple containing the wrapper RawAPIObject and its ForwardRef.
    """
    dataset = converter_group.data

    patch_target_ref = f"{ability_ref}.Animated"
    patch_target_forward_ref = ForwardRef(line, patch_target_ref)

    # Wrapper
    wrapper_name = f"{patch_name_prefix}AnimationWrapper"
    wrapper_ref = f"{container_obj_ref}.{wrapper_name}"
    wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                          wrapper_name,
                                          dataset.nyan_api_objects)
    wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")
    wrapper_location = ForwardRef(converter_group, container_obj_ref)
    wrapper_raw_api_object.set_location(wrapper_location)

    # Nyan patch
    nyan_patch_name = f"{patch_name_prefix}Animation"
    nyan_patch_ref = f"{container_obj_ref}.{wrapper_name}.{nyan_patch_name}"
    nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
    nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                             nyan_patch_name,
                                             dataset.nyan_api_objects,
                                             nyan_patch_location)
    nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
    nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

    animations: list[ForwardRef] = []
    for idx, anim_id in enumerate(animation_ids):
        if anim_id < 0:
            continue

        if len(animation_ids) == 1:
            # don't append index if there is only one animation
            anim_obj_name = animation_name_prefix

        else:
            anim_obj_name = f"{animation_name_prefix}{idx}"

        anim_forward_ref = create_animation(
            converter_group,
            line,
            anim_id,
            nyan_patch_ref,
            anim_obj_name,
            filename_prefix
        )
        animations.append(anim_forward_ref)

    nyan_patch_raw_api_object.add_raw_patch_member("animations",
                                                   animations,
                                                   "engine.ability.property.type.Animated",
                                                   MemberOperator.ASSIGN)

    patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
    wrapper_raw_api_object.add_raw_member("patch",
                                          patch_forward_ref,
                                          "engine.util.patch.Patch")

    converter_group.add_raw_api_object(wrapper_raw_api_object)
    converter_group.add_raw_api_object(nyan_patch_raw_api_object)

    wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)

    return wrapper_raw_api_object, wrapper_forward_ref


def create_command_sound_patch(
    converter_group: ConverterObjectGroup,
    line: ConverterObjectGroup,
    ability_ref: str,
    patch_name_prefix: str,
    container_obj_ref: str,
    sound_name_prefix: str,
    filename_prefix: str,
    sound_ids: list[int]
) -> tuple[RawAPIObject, ForwardRef]:
    """
    Create a patch for the CommandSound property of an ability.

    :param converter_group: Converter group for storing the patch.
    :param line: Line that has the ability.
    :param ability_ref: Reference of the ability that has the CommandSound property.
    :param patch_name_prefix: Prefix to the name of the patch.
    :param container_obj_ref: Reference of the API object that should contain the
                                patch as a nested object.
    :param sound_name_prefix: Prefix to the name of the sound.
    :param filename_prefix: Prefix to the filename of the sound.
    :param sound_ids: IDs of the sounds to patch in.
    :return: A 2-tuple containing the wrapper RawAPIObject and its ForwardRef.
    """
    dataset = converter_group.data

    patch_target_ref = f"{ability_ref}.CommandSound"
    patch_target_forward_ref = ForwardRef(line, patch_target_ref)

    # Wrapper
    wrapper_name = f"{patch_name_prefix}CommandSoundWrapper"
    wrapper_ref = f"{container_obj_ref}.{wrapper_name}"
    wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                          wrapper_name,
                                          dataset.nyan_api_objects)
    wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")
    wrapper_location = ForwardRef(converter_group, container_obj_ref)
    wrapper_raw_api_object.set_location(wrapper_location)

    # Nyan patch
    nyan_patch_name = f"{patch_name_prefix}CommandSound"
    nyan_patch_ref = f"{container_obj_ref}.{wrapper_name}.{nyan_patch_name}"
    nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
    nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                             nyan_patch_name,
                                             dataset.nyan_api_objects,
                                             nyan_patch_location)
    nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
    nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

    sounds: list[ForwardRef] = []
    for idx, sound_id in enumerate(sound_ids):
        if sound_id < 0:
            continue

        if len(sound_ids) == 1:
            # don't append index if there is only one sound
            sound_obj_name = sound_name_prefix

        else:
            sound_obj_name = f"{sound_name_prefix}{idx}"

        sound_forward_ref = create_sound(
            converter_group,
            sound_id,
            nyan_patch_ref,
            sound_obj_name,
            filename_prefix
        )
        sounds.append(sound_forward_ref)

    nyan_patch_raw_api_object.add_raw_patch_member("sounds",
                                                   sounds,
                                                   "engine.ability.property.type.CommandSound",
                                                   MemberOperator.ASSIGN)

    patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
    wrapper_raw_api_object.add_raw_member("patch",
                                          patch_forward_ref,
                                          "engine.util.patch.Patch")

    converter_group.add_raw_api_object(wrapper_raw_api_object)
    converter_group.add_raw_api_object(nyan_patch_raw_api_object)

    wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)

    return wrapper_raw_api_object, wrapper_forward_ref
