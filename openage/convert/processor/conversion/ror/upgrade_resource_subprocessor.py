# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-lines,too-many-statements,too-many-public-methods
#
# TODO: Remove when all methods are implemented
# pylint: disable=unused-argument

"""
Creates upgrade patches for resource modification effects in RoR.
"""
from .....nyan.nyan_structs import MemberOperator
from ....entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup
from ....entity_object.conversion.converter_object import RawAPIObject
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef


class RoRUpgradeResourceSubprocessor:
    """
    Creates raw API objects for resource upgrade effects in RoR.
    """

    @staticmethod
    def building_conversion_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the building conversion effect (ID: 28).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        monk_id = 125
        dataset = converter_group.data
        line = dataset.unit_lines[monk_id]

        patches = []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        game_entity_name = name_lookup_dict[monk_id][0]

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
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "EnableBuildingConversion"
        nyan_patch_ref = f"{obj_name}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        # New allowed types
        allowed_types = [
            dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()
        ]
        nyan_patch_raw_api_object.add_raw_patch_member("allowed_types",
                                                       allowed_types,
                                                       "engine.ability.type.ApplyDiscreteEffect",
                                                       MemberOperator.ADD)

        # Blacklisted buildings
        tc_line = dataset.building_lines[109]
        farm_line = dataset.building_lines[50]
        monastery_line = dataset.building_lines[104]
        wonder_line = dataset.building_lines[276]

        blacklisted_forward_refs = [ForwardRef(tc_line, "TownCenter"),
                                    ForwardRef(farm_line, "Farm"),
                                    ForwardRef(monastery_line, "Temple"),
                                    ForwardRef(wonder_line, "Wonder"),
                                    ]
        nyan_patch_raw_api_object.add_raw_patch_member("blacklisted_entities",
                                                       blacklisted_forward_refs,
                                                       "engine.ability.type.ApplyDiscreteEffect",
                                                       MemberOperator.ADD)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.aux.patch.Patch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
        patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def heal_bonus_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the AoE1 heal bonus effect (ID: 56).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def martyrdom_upgrade(converter_group, value, operator, team=False):
        """
        Creates a patch for the martyrdom effect (ID: 57).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Implement

        return patches
