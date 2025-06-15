# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create patches for upgrading the AttributeChangeTracker ability.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberOperator
from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .....value_object.read.value_members import NoDiffMember
from .util import create_animation

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObject, \
        ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def attribute_change_tracker_ability(
    converter_group: ConverterObjectGroup,
    line: GenieGameEntityGroup,
    container_obj_ref: str,
    diff: ConverterObject = None
) -> list[ForwardRef]:
    """
    Creates a patch for the AttributeChangeTracker ability of a line.

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

    game_entity_name = name_lookup_dict[head_unit_id][0]

    if diff:
        diff_damage_graphics = diff["damage_graphics"]
        if isinstance(diff_damage_graphics, NoDiffMember):
            return patches

        diff_damage_animations = diff_damage_graphics.value

    else:
        return patches

    percentage = 0
    for diff_damage_animation in diff_damage_animations:
        if isinstance(diff_damage_animation, NoDiffMember) or\
                isinstance(diff_damage_animation["graphic_id"], NoDiffMember):
            continue

        # This should be a NoDiffMember
        percentage = diff_damage_animation["damage_percent"].ref.value

        patch_target_ref = (f"{game_entity_name}.AttributeChangeTracker."
                            f"ChangeProgress{percentage}.AnimationOverlay")
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = f"Change{game_entity_name}DamageGraphic{percentage}Wrapper"
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
        nyan_patch_name = f"Change{game_entity_name}DamageGraphic{str(percentage)}"
        nyan_patch_ref = f"{container_obj_ref}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        animations_set = []
        diff_animation_id = diff_damage_animation["graphic_id"].value
        if diff_animation_id > -1:
            # Patch the new animation in
            animation_forward_ref = create_animation(
                converter_group,
                line,
                diff_animation_id,
                nyan_patch_ref,
                "Idle",
                f"idle_damage_override_{percentage}_"
            )
            animations_set.append(animation_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member(
            "overlays",
            animations_set,
            "engine.util.progress.property.type.AnimationOverlay",
            MemberOperator.ASSIGN
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
