# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for building conversion in SWBG.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberOperator
from .....entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup


def convert_building_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the building conversion effect (ID: 28).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    force_ids = [115, 180]
    dataset = converter_group.data

    patches = []

    for force_id in force_ids:
        line = dataset.unit_lines[force_id]

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        game_entity_name = name_lookup_dict[force_id][0]

        patch_target_ref = f"{game_entity_name}.Convert"
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Building conversion

        # Wrapper
        wrapper_name = "EnableBuildingConversionWrapper"
        wrapper_ref = f"{obj_name}.{wrapper_name}"
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

        # Nyan patch
        nyan_patch_name = "EnableBuildingConversion"
        nyan_patch_ref = f"{obj_name}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        # New allowed types
        allowed_types = [
            dataset.pregen_nyan_objects["util.game_entity_type.types.Building"].get_nyan_object(
            )
        ]
        nyan_patch_raw_api_object.add_raw_patch_member("allowed_types",
                                                       allowed_types,
                                                       "engine.ability.type.ApplyDiscreteEffect",
                                                       MemberOperator.ADD)

        # Blacklisted buildings
        tc_line = dataset.building_lines[109]
        farm_line = dataset.building_lines[50]
        temple_line = dataset.building_lines[104]
        wonder_line = dataset.building_lines[276]

        blacklisted_forward_refs = [ForwardRef(tc_line, "CommandCenter"),
                                    ForwardRef(farm_line, "Farm"),
                                    ForwardRef(temple_line, "Temple"),
                                    ForwardRef(wonder_line, "Monument"),
                                    ]
        nyan_patch_raw_api_object.add_raw_patch_member("blacklisted_entities",
                                                       blacklisted_forward_refs,
                                                       "engine.ability.type.ApplyDiscreteEffect",
                                                       MemberOperator.ADD)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.util.patch.Patch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

    return patches
