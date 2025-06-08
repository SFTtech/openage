# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for starting resources in AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from ......nyan.nyan_structs import MemberOperator


def starting_food_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the starting food modify effect (ID: 91).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def starting_wood_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the starting wood modify effect (ID: 92).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def starting_stone_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the starting stone modify effect (ID: 93).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def starting_gold_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the starting gold modify effect (ID: 94).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def starting_villagers_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Any,
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the starting villagers modify effect (ID: 84).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    patches = []

    # TODO: Implement

    return patches


def starting_population_space_upgrade(
    converter_group: ConverterObjectGroup,
    value: typing.Union[int, float],
    operator: MemberOperator,
    team: bool = False
) -> list[ForwardRef]:
    """
    Creates a patch for the starting popspace modify effect (ID: 4).

    :param converter_group: Tech/Civ that gets the patch.
    :param value: Value used for patching the member.
    :param operator: Operator used for patching the member.
    :returns: The forward references for the generated patches.
    """
    dataset = converter_group.data

    patches = []

    obj_id = converter_group.get_id()
    if isinstance(converter_group, GenieTechEffectBundleGroup):
        tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
        obj_name = tech_lookup_dict[obj_id][0]

    else:
        civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
        obj_name = civ_lookup_dict[obj_id][0]

    patch_target_ref = "util.resource.types.PopulationSpace"
    patch_target = dataset.pregen_nyan_objects[patch_target_ref].get_nyan_object()

    # Wrapper
    wrapper_name = "ChangeInitialPopulationLimitWrapper"
    wrapper_ref = f"{obj_name}.{wrapper_name}"
    wrapper_location = ForwardRef(converter_group, obj_name)
    wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                          wrapper_name,
                                          dataset.nyan_api_objects,
                                          wrapper_location)
    wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

    # Nyan patch
    nyan_patch_name = "ChangeInitialPopulationLimit"
    nyan_patch_ref = f"{obj_name}.{wrapper_name}.{nyan_patch_name}"
    nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
    nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                             nyan_patch_name,
                                             dataset.nyan_api_objects,
                                             nyan_patch_location)
    nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
    nyan_patch_raw_api_object.set_patch_target(patch_target)

    nyan_patch_raw_api_object.add_raw_patch_member("min_amount",
                                                   value,
                                                   "engine.util.resource.ResourceContingent",
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
