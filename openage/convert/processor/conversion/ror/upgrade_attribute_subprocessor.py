# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-lines,too-many-statements,too-many-public-methods
#
# TODO: Remove when all methods are implemented
# pylint: disable=unused-argument,line-too-long

"""
Creates upgrade patches for attribute modification effects in RoR.
"""
from ....entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup
from ....entity_object.conversion.converter_object import RawAPIObject
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef


class RoRUpgradeAttributeSubprocessor:
    """
    Creates raw API objects for attribute upgrade effects in RoR.
    """

    @staticmethod
    def ballistics_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the ballistics modify effect (ID: 19).

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

        if value == 0:
            target_mode = dataset.nyan_api_objects["engine.aux.target_mode.type.CurrentPosition"]

        elif value == 1:
            target_mode = dataset.nyan_api_objects["engine.aux.target_mode.type.ExpectedPosition"]

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[head_unit_id][0]

        projectile_id0 = head_unit["attack_projectile_primary_unit_id"].get_value()
        if projectile_id0 > -1:
            patch_target_ref = f"{game_entity_name}.ShootProjectile.Projectile0.Projectile"
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)

            # Wrapper
            wrapper_name = f"Change{game_entity_name}Projectile0TargetModeWrapper"
            wrapper_ref = f"{obj_name}.{wrapper_name}"
            wrapper_location = ForwardRef(converter_group, obj_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = f"Change{game_entity_name}Projectile0TargetMode"
            nyan_patch_ref = f"{obj_name}.{wrapper_name}.{nyan_patch_name}"
            nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

            nyan_patch_raw_api_object.add_raw_patch_member("target_mode",
                                                           target_mode,
                                                           "engine.ability.type.Projectile",
                                                           operator)

            patch_forward_ref = ForwardRef(converter_group, nyan_patch_ref)
            wrapper_raw_api_object.add_raw_member("patch",
                                                  patch_forward_ref,
                                                  "engine.aux.patch.Patch")

            if team:
                wrapper_raw_api_object.add_raw_parent("engine.aux.patch.type.DiplomaticPatch")
                stances = [
                    dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
                    dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()
                ]
                wrapper_raw_api_object.add_raw_member("stances",
                                                      stances,
                                                      "engine.aux.patch.type.DiplomaticPatch")

            converter_group.add_raw_api_object(wrapper_raw_api_object)
            converter_group.add_raw_api_object(nyan_patch_raw_api_object)

            wrapper_forward_ref = ForwardRef(converter_group, wrapper_ref)
            patches.append(wrapper_forward_ref)

        return patches

    @staticmethod
    def population_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the population effect (ID: 101).

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
