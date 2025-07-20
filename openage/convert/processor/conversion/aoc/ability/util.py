# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Helper functions for AoC ability conversion.
"""
from __future__ import annotations
import typing


from ......nyan.nyan_structs import MemberOperator
from .....entity_object.conversion.combined_sprite import CombinedSprite
from .....entity_object.conversion.combined_sound import CombinedSound
from .....entity_object.conversion.converter_object import RawAPIObject, RawMemberPush
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def create_animation(
    line: GenieGameEntityGroup,
    animation_id: int,
    location_ref: str,
    obj_name_prefix: str,
    filename_prefix: str
) -> ForwardRef:
    """
    Generates an animation for an ability.

    :param line: ConverterObjectGroup that the animation object is added to.
    :param animation_id: ID of the animation in the dataset.
    :param ability_ref: Reference of the object the animation is nested in.
    :param obj_name_prefix: Name prefix for the animation object.
    :param filename_prefix: Prefix for the animation PNG and sprite files.
    """
    dataset = line.data
    head_unit_id = line.get_head_unit_id()

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    animation_ref = f"{location_ref}.{obj_name_prefix}Animation"
    animation_obj_name = f"{obj_name_prefix}Animation"
    animation_raw_api_object = RawAPIObject(animation_ref, animation_obj_name,
                                            dataset.nyan_api_objects)
    animation_raw_api_object.add_raw_parent("engine.util.graphics.Animation")
    animation_location = ForwardRef(line, location_ref)
    animation_raw_api_object.set_location(animation_location)

    if animation_id in dataset.combined_sprites.keys():
        ability_sprite = dataset.combined_sprites[animation_id]

    else:
        ability_sprite = CombinedSprite(animation_id,
                                        (f"{filename_prefix}"
                                            f"{name_lookup_dict[head_unit_id][1]}"),
                                        dataset)
        dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})

    ability_sprite.add_reference(animation_raw_api_object)

    animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                            "engine.util.graphics.Animation")

    line.add_raw_api_object(animation_raw_api_object)

    animation_forward_ref = ForwardRef(line, animation_ref)

    return animation_forward_ref


def create_civ_animation(
    line: GenieGameEntityGroup,
    civ_group: GenieCivilizationGroup,
    animation_id: int,
    location_ref: str,
    obj_name_prefix: str,
    filename_prefix: str,
    exists: bool = False
) -> None:
    """
    Generates an animation as a patch for a civ.

    :param line: ConverterObjectGroup that the animation object is added to.
    :param civ_group: ConverterObjectGroup that patches the animation object into the ability.
    :param animation_id: ID of the animation in the dataset.
    :param location_ref: Reference of the object the resulting object is nested in.
    :param obj_name_prefix: Name prefix for the object.
    :param filename_prefix: Prefix for the animation PNG and sprite files.
    :param exists: Set to true if the animation object has already been created before.
    """
    dataset = civ_group.data
    head_unit_id = line.get_head_unit_id()
    civ_id = civ_group.get_id()

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[head_unit_id][0]
    civ_name = civ_lookup_dict[civ_id][0]

    patch_target_ref = f"{location_ref}"
    patch_target_forward_ref = ForwardRef(line, patch_target_ref)

    # Wrapper
    wrapper_name = f"{game_entity_name}{obj_name_prefix}AnimationWrapper"
    wrapper_ref = f"{civ_name}.{wrapper_name}"
    wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                          wrapper_name,
                                          dataset.nyan_api_objects)
    wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")
    wrapper_raw_api_object.set_location(ForwardRef(civ_group, civ_name))

    # Nyan patch
    nyan_patch_name = f"{game_entity_name}{obj_name_prefix}Animation"
    nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
    nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
    nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                             nyan_patch_name,
                                             dataset.nyan_api_objects,
                                             nyan_patch_location)
    nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
    nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

    if animation_id > -1:
        # If the animation object already exists, we do not need to create it again
        if exists:
            # Point to a previously created animation object
            animation_ref = f"{location_ref}.{obj_name_prefix}Animation"
            animation_forward_ref = ForwardRef(line, animation_ref)

        else:
            # Create the animation object
            animation_forward_ref = create_animation(line,
                                                     animation_id,
                                                     location_ref,
                                                     obj_name_prefix,
                                                     filename_prefix)

        # Patch animation into ability
        nyan_patch_raw_api_object.add_raw_patch_member(
            "animations",
            [animation_forward_ref],
            "engine.ability.property.type.Animated",
            MemberOperator.ASSIGN
        )

    else:
        # No animation -> empty the set
        nyan_patch_raw_api_object.add_raw_patch_member(
            "animations",
            [],
            "engine.ability.property.type.Animated",
            MemberOperator.ASSIGN
        )

    patch_forward_ref = ForwardRef(civ_group, nyan_patch_ref)
    wrapper_raw_api_object.add_raw_member("patch",
                                          patch_forward_ref,
                                          "engine.util.patch.Patch")

    civ_group.add_raw_api_object(wrapper_raw_api_object)
    civ_group.add_raw_api_object(nyan_patch_raw_api_object)

    # Add patch to game_setup
    civ_forward_ref = ForwardRef(civ_group, civ_name)
    wrapper_forward_ref = ForwardRef(civ_group, wrapper_ref)
    push_object = RawMemberPush(civ_forward_ref,
                                "game_setup",
                                "engine.util.setup.PlayerSetup",
                                [wrapper_forward_ref])
    civ_group.add_raw_member_push(push_object)


def create_sound(
    line: GenieGameEntityGroup,
    sound_id: int,
    location_ref: str,
    obj_name_prefix: str,
    filename_prefix: str
) -> ForwardRef:
    """
    Generates a sound for an ability.

    :param line: ConverterObjectGroup that the animation object is added to.
    :param sound_id: ID of the sound in the dataset.
    :param location_ref: Reference of the object the sound is nested in.
    :param obj_name_prefix: Name prefix for the sound object.
    :param filename_prefix: Prefix for the animation PNG and sprite files.
    """
    dataset = line.data

    sound_ref = f"{location_ref}.{obj_name_prefix}Sound"
    sound_obj_name = f"{obj_name_prefix}Sound"
    sound_raw_api_object = RawAPIObject(sound_ref, sound_obj_name,
                                        dataset.nyan_api_objects)
    sound_raw_api_object.add_raw_parent("engine.util.sound.Sound")
    sound_location = ForwardRef(line, location_ref)
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
                                  f"{filename_prefix}sound_{str(file_id)}",
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

    line.add_raw_api_object(sound_raw_api_object)

    sound_forward_ref = ForwardRef(line, sound_ref)

    return sound_forward_ref


def create_language_strings(
    line: GenieGameEntityGroup,
    string_id: int,
    location_ref: str,
    obj_name_prefix: str
) -> list[ForwardRef]:
    """
    Generates a language string for an ability.

    :param line: ConverterObjectGroup that the animation object is added to.
    :param string_id: ID of the string in the dataset.
    :param location_ref: Reference of the object the string is nested in.
    :param obj_name_prefix: Name prefix for the string object.
    """
    dataset = line.data
    string_resources = dataset.strings.get_tables()

    string_objs = []
    for language, strings in string_resources.items():
        if string_id in strings.keys():
            string_name = f"{obj_name_prefix}String"
            string_ref = f"{location_ref}.{string_name}"
            string_raw_api_object = RawAPIObject(string_ref, string_name,
                                                 dataset.nyan_api_objects)
            string_raw_api_object.add_raw_parent("engine.util.language.LanguageTextPair")
            string_location = ForwardRef(line, location_ref)
            string_raw_api_object.set_location(string_location)

            # Language identifier
            lang_ref = f"util.language.{language}"
            lang_forward_ref = dataset.pregen_nyan_objects[lang_ref].get_nyan_object()
            string_raw_api_object.add_raw_member("language",
                                                 lang_forward_ref,
                                                 "engine.util.language.LanguageTextPair")

            # String
            string_raw_api_object.add_raw_member("string",
                                                 strings[string_id],
                                                 "engine.util.language.LanguageTextPair")

            line.add_raw_api_object(string_raw_api_object)
            string_forward_ref = ForwardRef(line, string_ref)
            string_objs.append(string_forward_ref)

    return string_objs
