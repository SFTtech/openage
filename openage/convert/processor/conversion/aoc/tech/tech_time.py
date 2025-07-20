# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates patches for modifying tech research times.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberOperator
from .....entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....value_object.conversion.forward_ref import ForwardRef
from .....service.conversion import internal_name_lookups

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_effect import GenieEffectObject


def tech_time_modify_effect(
    converter_group: ConverterObjectGroup,
    effect: GenieEffectObject,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates the patches for modifying tech research times.
    """
    patches = []
    dataset = converter_group.data

    tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
    civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

    obj_id = converter_group.get_id()
    if isinstance(converter_group, GenieTechEffectBundleGroup):
        obj_name = tech_lookup_dict[obj_id][0]

    else:
        obj_name = civ_lookup_dict[obj_id][0]

    tech_id = effect["attr_a"].value
    mode = effect["attr_c"].value
    research_time = effect["attr_d"].value

    if tech_id not in tech_lookup_dict:
        # Skips some legacy techs from AoK such as the tech for bombard cannon
        return patches

    tech_group = dataset.tech_groups[tech_id]
    tech_name = tech_lookup_dict[tech_id][0]

    if mode == 0:
        operator = MemberOperator.ASSIGN

    else:
        operator = MemberOperator.ADD

    patch_target_ref = f"{tech_name}.ResearchableTech"
    patch_target_forward_ref = ForwardRef(tech_group, patch_target_ref)

    # Wrapper
    wrapper_name = f"Change{tech_name}ResearchTimeWrapper"
    wrapper_ref = f"{tech_name}.{wrapper_name}"
    wrapper_location = ForwardRef(converter_group, obj_name)
    wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                          wrapper_name,
                                          dataset.nyan_api_objects,
                                          wrapper_location)
    wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

    # Nyan patch
    nyan_patch_name = f"Change{tech_name}ResearchTime"
    nyan_patch_ref = f"{tech_name}.{wrapper_name}.{nyan_patch_name}"
    nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
    nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                             nyan_patch_name,
                                             dataset.nyan_api_objects,
                                             nyan_patch_location)
    nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
    nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

    nyan_patch_raw_api_object.add_raw_patch_member("research_time",
                                                   research_time,
                                                   "engine.util.research.ResearchableTech",
                                                   operator)

    if team:
        team_property = dataset.pregen_nyan_objects[
            "util.patch.property.types.Team"
        ].get_nyan_object()
        properties = {
            dataset.nyan_api_objects["engine.util.patch.property.type.Diplomatic"]: team_property
        }
        wrapper_raw_api_object.add_raw_member("properties",
                                              properties,
                                              "engine.util.patch.Patch")

    patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
    wrapper_raw_api_object.add_raw_member("patch",
                                          patch_forward_ref,
                                          "engine.util.patch.Patch")

    converter_group.add_raw_api_object(wrapper_raw_api_object)
    converter_group.add_raw_api_object(nyan_patch_raw_api_object)

    wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
    patches.append(wrapper_forward_ref)

    return patches
