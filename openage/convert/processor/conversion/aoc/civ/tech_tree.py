# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for civ tech tree.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberOperator
from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from ..tech_subprocessor import AoCTechSubprocessor

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup


def setup_tech_tree(civ_group: GenieCivilizationGroup) -> list[ForwardRef]:
    """
    Patches standard techs and units out of Research and Create.

    :param civ_group: Civ group representing an AoC civilization.
    """
    patches = []

    civ_id = civ_group.get_id()
    dataset = civ_group.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
    civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

    civ_name = civ_lookup_dict[civ_id][0]

    disabled_techs = {}
    disabled_entities = {}

    tech_tree = civ_group.get_tech_tree_effects()
    for effect in tech_tree:
        type_id = effect.get_type()

        if type_id == 101:
            patches.extend(AoCTechSubprocessor.tech_cost_modify_effect(civ_group, effect))
            continue

        if type_id == 103:
            patches.extend(AoCTechSubprocessor.tech_time_modify_effect(civ_group, effect))
            continue

        if type_id != 102:
            continue

        # Get tech id
        tech_id = int(effect["attr_d"].value)

        # Check what the purpose of the tech is
        if tech_id in dataset.unit_unlocks.keys():
            unlock_tech = dataset.unit_unlocks[tech_id]
            unlocked_line = unlock_tech.get_unlocked_line()
            train_location_id = unlocked_line.get_train_location_id()

            if isinstance(unlocked_line, GenieBuildingLineGroup):
                train_location = dataset.unit_lines[train_location_id]

            else:
                train_location = dataset.building_lines[train_location_id]

            if train_location in disabled_entities:
                disabled_entities[train_location].append(unlocked_line)

            else:
                disabled_entities[train_location] = [unlocked_line]

        elif tech_id in dataset.civ_boni.keys():
            # Disables civ boni of other civs
            continue

        elif tech_id in dataset.tech_groups.keys():
            tech_group = dataset.tech_groups[tech_id]
            if tech_group.is_researchable():
                research_location_id = tech_group.get_research_location_id()
                research_location = dataset.building_lines[research_location_id]

                if research_location in disabled_techs:
                    disabled_techs[research_location].append(tech_group)

                else:
                    disabled_techs[research_location] = [tech_group]

        else:
            continue

    for train_location, entities in disabled_entities.items():
        train_location_id = train_location.get_head_unit_id()
        train_location_name = name_lookup_dict[train_location_id][0]

        patch_target_ref = f"{train_location_name}.Create"
        patch_target_forward_ref = ForwardRef(train_location, patch_target_ref)

        # Wrapper
        wrapper_name = f"Disable{train_location_name}CreatablesWrapper"
        wrapper_ref = f"{civ_name}.{wrapper_name}"
        wrapper_location = ForwardRef(civ_group, civ_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

        # Nyan patch
        nyan_patch_name = f"Disable{train_location_name}Creatables"
        nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        entities_forward_refs = []
        for entity in entities:
            entity_id = entity.get_head_unit_id()
            game_entity_name = name_lookup_dict[entity_id][0]

            disabled_ref = f"{game_entity_name}.CreatableGameEntity"
            disabled_forward_ref = ForwardRef(entity, disabled_ref)
            entities_forward_refs.append(disabled_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("creatables",
                                                       entities_forward_refs,
                                                       "engine.ability.type.Create",
                                                       MemberOperator.SUBTRACT)

        patch_forward_ref = ForwardRef(civ_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.util.patch.Patch")

        civ_group.add_raw_api_object(wrapper_raw_api_object)
        civ_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(civ_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

    for research_location, techs in disabled_techs.items():
        research_location_id = research_location.get_head_unit_id()
        research_location_name = name_lookup_dict[research_location_id][0]

        patch_target_ref = f"{research_location_name}.Research"
        patch_target_forward_ref = ForwardRef(research_location, patch_target_ref)

        # Wrapper
        wrapper_name = f"Disable{research_location_name}ResearchablesWrapper"
        wrapper_ref = f"{civ_name}.{wrapper_name}"
        wrapper_location = ForwardRef(civ_group, civ_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

        # Nyan patch
        nyan_patch_name = f"Disable{research_location_name}Researchables"
        nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        entities_forward_refs = []
        for tech_group in techs:
            tech_id = tech_group.get_id()
            tech_name = tech_lookup_dict[tech_id][0]

            disabled_ref = f"{tech_name}.ResearchableTech"
            disabled_forward_ref = ForwardRef(tech_group, disabled_ref)
            entities_forward_refs.append(disabled_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("researchables",
                                                       entities_forward_refs,
                                                       "engine.ability.type.Research",
                                                       MemberOperator.SUBTRACT)

        patch_forward_ref = ForwardRef(civ_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.util.patch.Patch")

        civ_group.add_raw_api_object(wrapper_raw_api_object)
        civ_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(civ_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

    return patches
