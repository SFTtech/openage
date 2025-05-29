# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for civ unique techs.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberOperator
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup


def setup_unique_techs(civ_group: GenieCivilizationGroup) -> list[ForwardRef]:
    """
    Patches the unique techs into their research location.

    :param civ_group: Civ group representing an AoC civilization.
    """
    patches = []

    civ_id = civ_group.get_id()
    dataset = civ_group.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
    civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

    civ_name = civ_lookup_dict[civ_id][0]

    for unique_tech in civ_group.unique_techs.values():
        tech_id = unique_tech.get_id()
        tech_name = tech_lookup_dict[tech_id][0]

        # Get train location of line
        research_location_id = unique_tech.get_research_location_id()
        research_location = dataset.building_lines[research_location_id]
        research_location_name = name_lookup_dict[research_location_id][0]

        patch_target_ref = f"{research_location_name}.Research"
        patch_target_forward_ref = ForwardRef(research_location, patch_target_ref)

        # Wrapper
        wrapper_name = f"Add{tech_name}ResearchableWrapper"
        wrapper_ref = f"{civ_name}.{wrapper_name}"
        wrapper_location = ForwardRef(civ_group, civ_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

        # Nyan patch
        nyan_patch_name = f"Add{tech_name}Researchable"
        nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        # Add creatable
        researchable_ref = f"{tech_name}.ResearchableTech"
        researchable_forward_ref = ForwardRef(unique_tech, researchable_ref)
        nyan_patch_raw_api_object.add_raw_patch_member("researchables",
                                                       [researchable_forward_ref],
                                                       "engine.ability.type.Research",
                                                       MemberOperator.ADD)

        patch_forward_ref = ForwardRef(civ_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.util.patch.Patch")

        civ_group.add_raw_api_object(wrapper_raw_api_object)
        civ_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(civ_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

    return patches
