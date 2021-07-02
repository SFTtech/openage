# Copyright 2020-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-lines,too-many-statements,too-many-public-methods
#
# TODO: Remove when all methods are implemented
# pylint: disable=unused-argument,line-too-long

"""
Creates upgrade patches for attribute modification effects in SWGB.
"""
from ....entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup
from ....entity_object.conversion.converter_object import RawAPIObject
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef


class SWGBCCUpgradeAttributeSubprocessor:
    """
    Creates raw API objects for attribute upgrade effects in SWGB.
    """

    @staticmethod
    def cost_carbon_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the carbon cost modify effect (ID: 104).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
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

        patch_target_ref = "%s.CreatableGameEntity.%sCost.CarbonAmount" % (game_entity_name,
                                                                           game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = f"Change{game_entity_name}CarbonCostWrapper"
        wrapper_ref = f"{obj_name}.{wrapper_name}"
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

        # Nyan patch
        nyan_patch_name = f"Change{game_entity_name}CarbonCost"
        nyan_patch_ref = f"{obj_name}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                       value,
                                                       "engine.util.resource.ResourceAmount",
                                                       operator)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.util.patch.Patch")

        if team:
            team_property = dataset.pregen_nyan_objects["util.patch.property.types.Team"].get_nyan_object()
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

    @staticmethod
    def cost_nova_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the nova cost modify effect (ID: 105).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
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

        patch_target_ref = "%s.CreatableGameEntity.%sCost.NovaAmount" % (game_entity_name,
                                                                         game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = f"Change{game_entity_name}NovaCostWrapper"
        wrapper_ref = f"{obj_name}.{wrapper_name}"
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

        # Nyan patch
        nyan_patch_name = f"Change{game_entity_name}NovaCost"
        nyan_patch_ref = f"{obj_name}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                       value,
                                                       "engine.util.resource.ResourceAmount",
                                                       operator)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.util.patch.Patch")

        if team:
            team_property = dataset.pregen_nyan_objects["util.patch.property.types.Team"].get_nyan_object()
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

    @staticmethod
    def cost_ore_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the ore cost modify effect (ID: 106).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
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

        patch_target_ref = "%s.CreatableGameEntity.%sCost.OreAmount" % (game_entity_name,
                                                                        game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = f"Change{game_entity_name}OreCostWrapper"
        wrapper_ref = f"{obj_name}.{wrapper_name}"
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

        # Nyan patch
        nyan_patch_name = f"Change{game_entity_name}OreCost"
        nyan_patch_ref = f"{obj_name}.{wrapper_name}.{nyan_patch_name}"
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                       value,
                                                       "engine.util.resource.ResourceAmount",
                                                       operator)

        patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_forward_ref,
                                              "engine.util.patch.Patch")

        if team:
            team_property = dataset.pregen_nyan_objects["util.patch.property.types.Team"].get_nyan_object()
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

    @staticmethod
    def resource_cost_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the resource modify effect (ID: 100).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The forward references for the generated patches.
        :rtype: list
        """
        head_unit = line.get_head_unit()
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

        for resource_amount in head_unit["resource_cost"].get_value():
            resource_id = resource_amount["type_id"].get_value()

            resource_name = ""
            if resource_id == -1:
                # Not a valid resource
                continue

            if resource_id == 0:
                resource_name = "Food"

            elif resource_id == 1:
                resource_name = "Carbon"

            elif resource_id == 2:
                resource_name = "Ore"

            elif resource_id == 3:
                resource_name = "Nova"

            else:
                # Other resource ids are handled differently
                continue

            # Skip resources that are only expected to be there
            if not resource_amount["enabled"].get_value():
                continue

            patch_target_ref = "%s.CreatableGameEntity.%sCost.%sAmount" % (game_entity_name,
                                                                           game_entity_name,
                                                                           resource_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)

            # Wrapper
            wrapper_name = f"Change{game_entity_name}{resource_name}CostWrapper"
            wrapper_ref = f"{obj_name}.{wrapper_name}"
            wrapper_location = ForwardRef(converter_group, obj_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.util.patch.Patch")

            # Nyan patch
            nyan_patch_name = f"Change{game_entity_name}{resource_name}Cost"
            nyan_patch_ref = f"{obj_name}.{wrapper_name}.{nyan_patch_name}"
            nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.util.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

            nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                           value,
                                                           "engine.util.resource.ResourceAmount",
                                                           operator)

            patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_forward_ref,
                                                  "engine.util.patch.Patch")

            if team:
                team_property = dataset.pregen_nyan_objects["util.patch.property.types.Team"].get_nyan_object()
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
