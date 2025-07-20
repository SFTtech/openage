# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for civ bonuses.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberOperator
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from ..tech_subprocessor import AoCTechSubprocessor

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup


def setup_civ_bonus(civ_group: GenieCivilizationGroup) -> list[ForwardRef]:
    """
    Returns global modifiers of a civ.

    :param civ_group: Civ group representing an AoC civilization.
    """
    patches: list = []

    civ_id = civ_group.get_id()
    dataset = civ_group.data

    tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
    civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

    civ_name = civ_lookup_dict[civ_id][0]

    # key: tech_id; value patched in patches
    tech_patches = {}

    for civ_bonus in civ_group.civ_boni.values():
        if not civ_bonus.replaces_researchable_tech():
            bonus_patches = AoCTechSubprocessor.get_patches(civ_bonus)

            # civ boni might be unlocked by age ups. if so, patch them into the age up
            # patches are queued here
            required_tech_count = civ_bonus.tech["required_tech_count"].value
            if required_tech_count > 0 and len(bonus_patches) > 0:
                if required_tech_count == 1:
                    tech_id = civ_bonus.tech["required_techs"][0].value

                elif required_tech_count == 2:
                    tech_id = civ_bonus.tech["required_techs"][1].value

                if tech_id == 104:
                    # Skip Dark Age; it is not a tech in openage
                    patches.extend(bonus_patches)

                elif tech_id in tech_patches:
                    tech_patches[tech_id].extend(bonus_patches)

                else:
                    tech_patches[tech_id] = bonus_patches

            else:
                patches.extend(bonus_patches)

    for tech_id, patches in tech_patches.items():
        tech_group = dataset.tech_groups[tech_id]
        tech_name = tech_lookup_dict[tech_id][0]

        patch_target_ref = f"{tech_name}"
        patch_target_forward_ref = ForwardRef(tech_group, patch_target_ref)

        # Wrapper
        wrapper_name = f"{tech_name}CivBonusWrapper"
        wrapper_ref = f"{civ_name}.{wrapper_name}"
        wrapper_location = ForwardRef(civ_group, civ_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

        # Nyan patch
        nyan_patch_name = f"{tech_name}CivBonus"
        nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("updates",
                                                       patches,
                                                       "engine.util.tech.Tech",
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
