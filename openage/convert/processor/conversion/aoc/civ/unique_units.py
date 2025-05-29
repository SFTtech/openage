# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for civ unique units.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberOperator
from .....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_civ import GenieCivilizationGroup


def setup_unique_units(civ_group: GenieCivilizationGroup) -> list[ForwardRef]:
    """
    Patches the unique units into their train location.

    :param civ_group: Civ group representing an AoC civilization.
    """
    patches = []

    civ_id = civ_group.get_id()
    dataset = civ_group.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)

    civ_name = civ_lookup_dict[civ_id][0]

    for unique_line in civ_group.unique_entities.values():
        head_unit_id = unique_line.get_head_unit_id()
        game_entity_name = name_lookup_dict[head_unit_id][0]

        # Get train location of line
        train_location_id = unique_line.get_train_location_id()
        if isinstance(unique_line, GenieBuildingLineGroup):
            train_location = dataset.unit_lines[train_location_id]
            train_location_name = name_lookup_dict[train_location_id][0]

        else:
            train_location = dataset.building_lines[train_location_id]
            train_location_name = name_lookup_dict[train_location_id][0]

        patch_target_ref = f"{train_location_name}.Create"
        patch_target_forward_ref = ForwardRef(train_location, patch_target_ref)

        # Wrapper
        wrapper_name = f"Add{game_entity_name}CreatableWrapper"
        wrapper_ref = f"{civ_name}.{wrapper_name}"
        wrapper_location = ForwardRef(civ_group, civ_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

        # Nyan patch
        nyan_patch_name = f"Add{game_entity_name}Creatable"
        nyan_patch_ref = f"{civ_name}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(civ_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        # Add creatable
        creatable_ref = f"{game_entity_name}.CreatableGameEntity"
        creatable_forward_ref = ForwardRef(unique_line, creatable_ref)
        nyan_patch_raw_api_object.add_raw_patch_member("creatables",
                                                       [creatable_forward_ref],
                                                       "engine.ability.type.Create",
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
