# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for min range in AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from ......nyan.nyan_structs import MemberOperator
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def min_range_upgrade(
    converter_group: ConverterObjectGroup,
    line: GenieGameEntityGroup,
    value: typing.Union[int, float],
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the min range modify effect (ID: 20).

    :param converter_group: Tech/Civ that gets the patch.
    :param line: Unit/Building line that has the ability.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    head_unit_id = line.get_head_unit_id()
    dataset = line.data

    patches = []

    obj_id = converter_group.get_id()
    if isinstance(converter_group, GenieTechEffectBundleGroup):
        tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
        obj_name = tech_lookup_dict[obj_id][0]

    else:
        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
        obj_name = civ_lookup_dict[obj_id][0]

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[head_unit_id][0]

    patch_target_parent = "engine.ability.property.type.Ranged"
    if line.is_projectile_shooter():
        patch_target_ref = f"{game_entity_name}.Attack.Ranged"
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

    elif line.is_melee():
        patch_target_ref = f"{game_entity_name}.Attack.Ranged"
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

    elif line.has_command(104):
        patch_target_ref = f"{game_entity_name}.Convert.Ranged"
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

    else:
        return []

    # Wrapper
    wrapper_name = f"Change{game_entity_name}MinRangeWrapper"
    wrapper_ref = f"{obj_name}.{wrapper_name}"
    wrapper_location = ForwardRef(converter_group, obj_name)
    wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                          wrapper_name,
                                          dataset.nyan_api_objects,
                                          wrapper_location)
    wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

    # Nyan patch
    nyan_patch_name = f"Change{game_entity_name}MinRange"
    nyan_patch_ref = f"{obj_name}.{wrapper_name}.{nyan_patch_name}"
    nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
    nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                             nyan_patch_name,
                                             dataset.nyan_api_objects,
                                             nyan_patch_location)
    nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
    nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

    nyan_patch_raw_api_object.add_raw_patch_member("min_range",
                                                   value,
                                                   patch_target_parent,
                                                   operator)

    patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
    wrapper_raw_api_object.add_raw_member("patch",
                                          patch_forward_ref,
                                          "engine.util.patch.Patch")

    if team:
        team_property = dataset.pregen_nyan_objects["util.patch.property.types.Team"].get_nyan_object(
        )
        properties = {
            dataset.nyan_api_objects["engine.util.patch.property.type.Diplomatic"]: team_property
        }
        wrapper_raw_api_object.add_raw_member("properties",
                                              properties,
                                              "engine.util.patch.Patch")

    converter_group.add_raw_api_object(wrapper_raw_api_object)
    converter_group.add_raw_api_object(nyan_patch_raw_api_object)

    wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
    patches.append(wrapper_forward_ref)

    return patches
