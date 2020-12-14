# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-statements,too-many-branches

"""
Creates patches and modifiers for civs.
"""
from .....nyan.nyan_structs import MemberOperator
from ....entity_object.conversion.converter_object import RawAPIObject
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef
from ..aoc.civ_subprocessor import AoCCivSubprocessor
from .tech_subprocessor import DE2TechSubprocessor


class DE2CivSubprocessor:
    """
    Creates raw API objects for civs in DE2.
    """

    @classmethod
    def get_civ_setup(cls, civ_group):
        """
        Returns the patches for the civ setup which configures architecture sets
        unique units, unique techs, team boni and unique stat upgrades.
        """
        patches = []

        patches.extend(AoCCivSubprocessor.setup_unique_units(civ_group))
        patches.extend(AoCCivSubprocessor.setup_unique_techs(civ_group))
        patches.extend(AoCCivSubprocessor.setup_tech_tree(civ_group))
        patches.extend(cls.setup_civ_bonus(civ_group))

        if len(civ_group.get_team_bonus_effects()) > 0:
            patches.extend(DE2TechSubprocessor.get_patches(civ_group.team_bonus))

        return patches

    @classmethod
    def setup_civ_bonus(cls, civ_group):
        """
        Returns global modifiers of a civ.
        """
        patches = []

        civ_id = civ_group.get_id()
        dataset = civ_group.data

        tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

        civ_name = civ_lookup_dict[civ_id][0]

        # key: tech_id; value patched in patches
        tech_patches = {}

        for civ_bonus in civ_group.civ_boni.values():
            if not civ_bonus.replaces_researchable_tech():
                bonus_patches = DE2TechSubprocessor.get_patches(civ_bonus)

                # civ boni might be unlocked by age ups. if so, patch them into the age up
                # patches are queued here
                required_tech_count = civ_bonus.tech["required_tech_count"].get_value()
                if required_tech_count > 0 and len(bonus_patches) > 0:
                    if required_tech_count == 1:
                        tech_id = civ_bonus.tech["required_techs"][0].get_value()

                    elif required_tech_count == 2:
                        # Try to patch them into the second listed tech
                        # This tech is usually unlocked by an age up
                        tech_id = civ_bonus.tech["required_techs"][1].get_value()

                        if not dataset.tech_groups[tech_id].is_researchable():
                            # Fall back to the first tech if the second is not researchable
                            tech_id = civ_bonus.tech["required_techs"][0].get_value()

                    if tech_id == 104:
                        # Skip Dark Age; it is not a tech in openage
                        patches.extend(bonus_patches)

                    if tech_id not in dataset.tech_groups.keys() or\
                            not dataset.tech_groups[tech_id].is_researchable():
                        # TODO: Bonus unlocked by something else
                        continue

                    if tech_id in tech_patches.keys():
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
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = f"{tech_name}CivBonus"
            nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
            nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

            nyan_patch_raw_api_object.add_raw_patch_member("updates",
                                                           patches,
                                                           "engine.aux.tech.Tech",
                                                           MemberOperator.ADD)

            patch_forward_ref = ForwardRef(civ_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_forward_ref,
                                                  "engine.aux.patch.Patch")

            civ_group.add_raw_api_object(wrapper_raw_api_object)
            civ_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_forward_ref = ForwardRef(civ_group, wrapper_ref)
            patches.append(wrapper_forward_ref)

        return patches
