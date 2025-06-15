# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create patches for upgrading the Turn ability.
"""
from __future__ import annotations
import typing

from math import degrees

from ......nyan.nyan_structs import MemberOperator, MemberSpecialValue
from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .....value_object.read.value_members import NoDiffMember

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieUnitObject
    from .....entity_object.conversion.converter_object import ConverterObject, \
        ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def turn_ability(
    converter_group: ConverterObjectGroup,
    line: GenieGameEntityGroup,
    container_obj_ref: str,
    diff: ConverterObject = None
) -> list[ForwardRef]:
    """
    Creates a patch for the Turn ability of a line.

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
        diff_turn_speed = diff["turn_speed"]
        if isinstance(diff_turn_speed, NoDiffMember):
            return patches

        diff_turn_speed_value = diff_turn_speed.value

    else:
        return patches

    patch_target_ref = f"{game_entity_name}.Turn"
    patch_target_forward_ref = ForwardRef(line, patch_target_ref)

    # Wrapper
    wrapper_name = f"Change{game_entity_name}TurnWrapper"
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
    nyan_patch_name = f"Change{game_entity_name}Turn"
    nyan_patch_ref = f"{container_obj_ref}.{wrapper_name}.{nyan_patch_name}"
    nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
    nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                             nyan_patch_name,
                                             dataset.nyan_api_objects,
                                             nyan_patch_location)
    nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
    nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

    # Speed
    turn_speed_unmodified = diff_turn_speed_value
    turn_speed = MemberSpecialValue.NYAN_INF
    # Ships/Trebuchets turn slower
    if turn_speed_unmodified > 0:
        turn_yaw = diff["max_yaw_per_sec_moving"].value
        turn_speed = degrees(turn_yaw)

    nyan_patch_raw_api_object.add_raw_patch_member("turn_speed",
                                                   turn_speed,
                                                   "engine.ability.type.Turn",
                                                   MemberOperator.ASSIGN)

    patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
    wrapper_raw_api_object.add_raw_member("patch",
                                          patch_forward_ref,
                                          "engine.util.patch.Patch")

    converter_group.add_raw_api_object(wrapper_raw_api_object)
    converter_group.add_raw_api_object(nyan_patch_raw_api_object)

    wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
    patches.append(wrapper_forward_ref)

    return patches
