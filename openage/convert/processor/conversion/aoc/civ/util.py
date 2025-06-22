# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Helper functions for AoC civs.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.combined_sprite import CombinedSprite
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def create_animation(
    line: GenieGameEntityGroup,
    animation_id: int,
    nyan_patch_ref: str,
    animation_name: str,
    filename_prefix: str
) -> ForwardRef:
    """
    Creates an animation for an ability.

    :param line: ConverterObjectGroup that the animation object is added to.
    :param animation_id: ID of the animation in the dataset.
    :param nyan_patch_ref: Reference of the patch object the animation is nested in.
    :param animation_name: Name prefix for the animation object.
    :param filename_prefix: Prefix for the animation PNG and sprite files.
    """
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    animation_ref = f"{nyan_patch_ref}.{animation_name}Animation"
    animation_obj_name = f"{animation_name}Animation"
    animation_raw_api_object = RawAPIObject(animation_ref, animation_obj_name,
                                            dataset.nyan_api_objects)
    animation_raw_api_object.add_raw_parent("engine.util.graphics.Animation")
    animation_location = ForwardRef(line, nyan_patch_ref)
    animation_raw_api_object.set_location(animation_location)

    if animation_id in dataset.combined_sprites.keys():
        animation_sprite = dataset.combined_sprites[animation_id]

    else:
        animation_filename = f"{filename_prefix}{name_lookup_dict[line.get_head_unit_id()][1]}"
        animation_sprite = CombinedSprite(animation_id,
                                          animation_filename,
                                          dataset)
        dataset.combined_sprites.update({animation_sprite.get_id(): animation_sprite})

    animation_sprite.add_reference(animation_raw_api_object)

    animation_raw_api_object.add_raw_member("sprite", animation_sprite,
                                            "engine.util.graphics.Animation")

    line.add_raw_api_object(animation_raw_api_object)

    animation_forward_ref = ForwardRef(line, animation_ref)

    return animation_forward_ref
