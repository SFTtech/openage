# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create patches for upgrading the Selectable ability.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberOperator
from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup, \
    GenieUnitLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .....value_object.read.value_members import NoDiffMember
from .util import create_command_sound_patch

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObject, \
        ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def selectable_ability(
    converter_group: ConverterObjectGroup,
    line: GenieGameEntityGroup,
    container_obj_ref: str,
    diff: ConverterObject = None
) -> list[ForwardRef]:
    """
    Creates a patch for the Selectable ability of a line.

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

    # First patch: Sound for the SelectableSelf ability
    changed = False
    if diff:
        diff_selection_sound = diff["selection_sound_id"]
        if not isinstance(diff_selection_sound, NoDiffMember):
            changed = True

    if isinstance(line, GenieUnitLineGroup):
        ability_name = "SelectableSelf"

    else:
        ability_name = "Selectable"

    if changed:
        patch_target_ref = f"{game_entity_name}.{ability_name}"
        nyan_patch_name = f"Change{game_entity_name}{ability_name}"

        # Change sound
        diff_selection_sound_id = diff_selection_sound.value
        # Nyan patch
        wrapper, sound_patch_forward_ref = create_command_sound_patch(
            converter_group,
            line,
            patch_target_ref,
            nyan_patch_name,
            container_obj_ref,
            ability_name,
            "select_",
            [diff_selection_sound_id]
        )
        patches.append(sound_patch_forward_ref)

        if isinstance(line, GenieBuildingLineGroup):
            # Store building upgrades next to their game entity definition,
            # not in the Age up techs.
            wrapper.set_location(("data/game_entity/generic/"
                                  f"{name_lookup_dict[head_unit_id][1]}/"))
            wrapper.set_filename(f"{tech_lookup_dict[tech_id][1]}_upgrade")

    # Second patch: Selection box
    changed = False
    if diff:
        diff_radius_x = diff["selection_shape_x"]
        diff_radius_y = diff["selection_shape_y"]
        if any(not isinstance(value, NoDiffMember) for value in (diff_radius_x,
                                                                 diff_radius_y)):
            changed = True

    if changed:
        patch_target_ref = f"{game_entity_name}.{ability_name}.Rectangle"
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = f"Change{game_entity_name}{ability_name}RectangleWrapper"
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
        nyan_patch_name = f"Change{game_entity_name}{ability_name}Rectangle"
        nyan_patch_ref = f"{container_obj_ref}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        if not isinstance(diff_radius_x, NoDiffMember):
            diff_width_value = diff_radius_x.value

            nyan_patch_raw_api_object.add_raw_patch_member(
                "width",
                diff_width_value,
                "engine.util.selection_box.type.Rectangle",
                MemberOperator.ADD
            )

        if not isinstance(diff_radius_y, NoDiffMember):
            diff_height_value = diff_radius_y.value

            nyan_patch_raw_api_object.add_raw_patch_member(
                "height",
                diff_height_value,
                "engine.util.selection_box.type.Rectangle",
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
