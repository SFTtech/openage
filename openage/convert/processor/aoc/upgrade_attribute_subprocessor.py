# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-lines,too-many-statements,too-many-public-methods
#
# TODO: Remove when all methods are implemented
# pylint: disable=unused-argument,line-too-long

"""
Creates upgrade patches for attribute modification effects in AoC.
"""
from ...entity_object.conversion.aoc.genie_tech import GenieTechEffectBundleGroup
from ...entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from ...entity_object.conversion.converter_object import RawAPIObject
from ...service import internal_name_lookups
from ...value_object.conversion.forward_ref import ForwardRef


class AoCUpgradeAttributeSubprocessor:
    """
    Creates raw API objects for attribute upgrade effects in AoC.
    """

    @staticmethod
    def accuracy_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the accuracy modify effect (ID: 11).

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

        patch_target_ref = "%s.ShootProjectile.Projectile0.Projectile.Accuracy" % (game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sAccuracyWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sAccuracy" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("accuracy",
                                                       value,
                                                       "engine.aux.accuracy.Accuracy",
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
    def armor_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the armor modify effect (ID: 8).

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

        armor_class = int(value) >> 8
        armor_amount = int(value) & 0x0F

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        armor_lookup_dict = internal_name_lookups.get_armor_class_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[head_unit_id][0]
        class_name = armor_lookup_dict[armor_class]

        if line.has_armor(armor_class):
            patch_target_ref = "%s.Resistance.%s.BlockAmount" % (game_entity_name, class_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        else:
            # TODO: Create new attack resistance
            return patches

        # Wrapper
        wrapper_name = "Change%s%sResistanceWrapper" % (game_entity_name, class_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%s%sResistance" % (game_entity_name, class_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                       armor_amount,
                                                       "engine.aux.attribute.AttributeAmount",
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
    def attack_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the attack modify effect (ID: 9).

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

        attack_amount = int(value) & 0x0F
        armor_class = int(value) >> 8

        if armor_class == -1:
            return patches

        if not line.has_armor(armor_class):
            # TODO: Happens sometimes in AoE1; what do we do?
            return patches

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        armor_lookup_dict = internal_name_lookups.get_armor_class_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[head_unit_id][0]
        class_name = armor_lookup_dict[armor_class]

        if line.is_projectile_shooter():
            primary_projectile_id = line.get_head_unit()["attack_projectile_primary_unit_id"].get_value()
            if primary_projectile_id == -1:
                # Upgrade is skipped if the primary projectile is not defined
                return patches

            patch_target_ref = ("%s.ShootProjectile.Projectile0.Attack.%s.ChangeAmount"
                                % (game_entity_name, class_name))
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        else:
            patch_target_ref = "%s.Attack.%s.ChangeAmount" % (game_entity_name, class_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        if not line.has_attack(armor_class):
            # TODO: Create new attack effect
            return patches

        # Wrapper
        wrapper_name = "Change%s%sAttackWrapper" % (game_entity_name, class_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%s%sAttack" % (game_entity_name, class_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                       attack_amount,
                                                       "engine.aux.attribute.AttributeAmount",
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
        projectile_id1 = head_unit["attack_projectile_secondary_unit_id"].get_value()

        if projectile_id0 > -1:
            patch_target_ref = "%s.ShootProjectile.Projectile0.Projectile" % (game_entity_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)

            # Wrapper
            wrapper_name = "Change%sProjectile0TargetModeWrapper" % (game_entity_name)
            wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
            wrapper_location = ForwardRef(converter_group, obj_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = "Change%sProjectile0TargetMode" % (game_entity_name)
            nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
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

        if projectile_id1 > -1:
            patch_target_ref = "%s.ShootProjectile.Projectile1.Projectile" % (game_entity_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)

            # Wrapper
            wrapper_name = "Change%sProjectile1TargetModeWrapper" % (game_entity_name)
            wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
            wrapper_location = ForwardRef(converter_group, obj_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = "Change%sProjectile1TargetMode" % (game_entity_name)
            nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
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
    def blast_radius_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the blast radius modify effect (ID: 22).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def carry_capacity_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the carry capacity modify effect (ID: 14).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def cost_food_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the food cost modify effect (ID: 103).

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

        # Check if the unit line actually costs food
        for resource_amount in head_unit["resource_cost"].get_value():
            resource_id = resource_amount["type_id"].get_value()

            if resource_id == 0:
                break

        else:
            # Skip patch generation if no food cost was found
            return patches

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[head_unit_id][0]

        patch_target_ref = "%s.CreatableGameEntity.%sCost.FoodAmount" % (game_entity_name,
                                                                         game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sFoodCostWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sFoodCost" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                       value,
                                                       "engine.aux.resource.ResourceAmount",
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
    def cost_wood_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the wood cost modify effect (ID: 104).

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

        # Check if the unit line actually costs wood
        for resource_amount in head_unit["resource_cost"].get_value():
            resource_id = resource_amount["type_id"].get_value()

            if resource_id == 1:
                break

        else:
            # Skip patch generation if no wood cost was found
            return patches

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[head_unit_id][0]

        patch_target_ref = "%s.CreatableGameEntity.%sCost.WoodAmount" % (game_entity_name,
                                                                         game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sWoodCostWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sWoodCost" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                       value,
                                                       "engine.aux.resource.ResourceAmount",
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
    def cost_gold_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the gold cost modify effect (ID: 105).

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

        # Check if the unit line actually costs gold
        for resource_amount in head_unit["resource_cost"].get_value():
            resource_id = resource_amount["type_id"].get_value()

            if resource_id == 3:
                break

        else:
            # Skip patch generation if no gold cost was found
            return patches

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[head_unit_id][0]

        patch_target_ref = "%s.CreatableGameEntity.%sCost.GoldAmount" % (game_entity_name,
                                                                         game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sGoldCostWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sGoldCost" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                       value,
                                                       "engine.aux.resource.ResourceAmount",
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
    def cost_stone_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the stone cost modify effect (ID: 106).

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

        # Check if the unit line actually costs stone
        for resource_amount in head_unit["resource_cost"].get_value():
            resource_id = resource_amount["type_id"].get_value()

            if resource_id == 2:
                break

        else:
            # Skip patch generation if no stone cost was found
            return patches

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)
            obj_name = tech_lookup_dict[obj_id][0]

        else:
            civ_lookup_dict = internal_name_lookups.get_civ_lookups(dataset.game_version)
            obj_name = civ_lookup_dict[obj_id][0]

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[head_unit_id][0]

        patch_target_ref = "%s.CreatableGameEntity.%sCost.StoneAmount" % (game_entity_name,
                                                                          game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sStoneCostWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sStoneCost" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                       value,
                                                       "engine.aux.resource.ResourceAmount",
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
    def creation_time_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the creation time modify effect (ID: 101).

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

        patch_target_ref = "%s.CreatableGameEntity" % (game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sCreationTimeWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sCreationTime" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("creation_time",
                                                       value,
                                                       "engine.aux.create.CreatableGameEntity",
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
    def garrison_capacity_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the garrison capacity modify effect (ID: 2).

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

        if not line.is_garrison():
            # TODO: Patch ability in
            return patches

        patch_target_ref = "%s.Storage.%sContainer" % (game_entity_name, game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sCreationTimeWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sCreationTime" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("slots",
                                                       value,
                                                       "engine.aux.storage.Container",
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
    def garrison_heal_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the garrison heal rate modify effect (ID: 108).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def graphics_angle_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the graphics angle modify effect (ID: 17).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def hp_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the HP modify effect (ID: 0).

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

        patch_target_ref = "%s.Live.Health" % (game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sMaxHealthWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sMaxHealth" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("max_value",
                                                       value,
                                                       "engine.aux.attribute.AttributeSetting",
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
    def los_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the line of sight modify effect (ID: 1).

        :param converter_group: Tech/Civ that gets the patch.
        :type converter_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: int, float
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

        patch_target_ref = "%s.LineOfSight" % (game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sLineOfSightWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sLineOfSight" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("range",
                                                       value,
                                                       "engine.ability.type.LineOfSight",
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
    def max_projectiles_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the max projectiles modify effect (ID: 107).

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

        patch_target_ref = "%s.Attack" % (game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sMaxProjectilesWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sMaxProjectiles" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("max_projectiles",
                                                       value,
                                                       "engine.ability.type.ShootProjectile",
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
    def min_projectiles_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the min projectiles modify effect (ID: 102).

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

        patch_target_ref = "%s.Attack" % (game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sMinProjectilesWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sMinProjectiles" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("min_projectiles",
                                                       value,
                                                       "engine.ability.type.ShootProjectile",
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
    def max_range_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the max range modify effect (ID: 12).

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

        if line.is_projectile_shooter():
            patch_target_ref = "%s.Attack" % (game_entity_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)
            patch_target_parent = "engine.ability.type.ShootProjectile"

        elif line.is_melee():
            if line.is_ranged():
                patch_target_ref = "%s.Attack" % (game_entity_name)
                patch_target_forward_ref = ForwardRef(line, patch_target_ref)
                patch_target_parent = "engine.ability.type.RangedDiscreteEffect"

            else:
                # excludes ram upgrades
                return patches

        elif line.has_command(104):
            patch_target_ref = "%s.Convert" % (game_entity_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)
            patch_target_parent = "engine.ability.type.RangedDiscreteEffect"

        else:
            # no matching ability
            return patches

        # Wrapper
        wrapper_name = "Change%sMaxRangeWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sMaxRange" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("max_range",
                                                       value,
                                                       patch_target_parent,
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
    def min_range_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the min range modify effect (ID: 20).

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

        if line.is_projectile_shooter():
            patch_target_ref = "%s.Attack" % (game_entity_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)
            patch_target_parent = "engine.ability.type.ShootProjectile"

        elif line.is_melee():
            patch_target_ref = "%s.Attack" % (game_entity_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)
            patch_target_parent = "engine.ability.type.RangedDiscreteEffect"

        elif line.has_command(104):
            patch_target_ref = "%s.Convert" % (game_entity_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)
            patch_target_parent = "engine.ability.type.RangedDiscreteEffect"

        # Wrapper
        wrapper_name = "Change%sMinRangeWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sMinRange" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("min_range",
                                                       value,
                                                       patch_target_parent,
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
    def move_speed_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the move speed modify effect (ID: 5).

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

        patch_target_ref = "%s.Move" % (game_entity_name)
        patch_target_forward_ref = ForwardRef(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sMoveSpeedWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sMoveSpeed" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("speed",
                                                       value,
                                                       "engine.ability.type.Move",
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
    def projectile_unit_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the projectile modify effect (ID: 16).

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
        patches = []

        # Unused in AoC

        return patches

    @staticmethod
    def reload_time_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the reload time modify effect (ID: 10).

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

        if line.is_projectile_shooter():
            patch_target_ref = "%s.Attack" % (game_entity_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)
            patch_target_parent = "engine.ability.type.ShootProjectile"

        elif line.is_melee():
            patch_target_ref = "%s.Attack" % (game_entity_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)
            patch_target_parent = "engine.ability.type.ApplyDiscreteEffect"

        elif line.has_command(104):
            patch_target_ref = "%s.Convert" % (game_entity_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)
            patch_target_parent = "engine.ability.type.ApplyDiscreteEffect"

        else:
            # No matching ability
            return patches

        # Wrapper
        wrapper_name = "Change%sReloadTimeWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sReloadTime" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        nyan_patch_raw_api_object.add_raw_patch_member("reload_time",
                                                       value,
                                                       patch_target_parent,
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
                resource_name = "Wood"

            elif resource_id == 2:
                resource_name = "Stone"

            elif resource_id == 3:
                resource_name = "Gold"

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
            wrapper_name = "Change%s%sCostWrapper" % (game_entity_name,
                                                      resource_name)
            wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
            wrapper_location = ForwardRef(converter_group, obj_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = "Change%s%sCost" % (game_entity_name,
                                                  resource_name)
            nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
            nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

            nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                           value,
                                                           "engine.aux.resource.ResourceAmount",
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
    def resource_storage_1_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the resource storage 1 modify effect (ID: 21).

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

        if line.is_harvestable():
            patch_target_ref = "%s.Harvestable.%sResourceSpot" % (game_entity_name, game_entity_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)
            wrapper_name = "Change%sHarvestableAmountWrapper" % (game_entity_name)
            nyan_patch_name = "Change%sHarvestableAmount" % (game_entity_name)

        else:
            patch_target_ref = "%s.ProvideContingent.PopSpace" % (game_entity_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)
            wrapper_name = "Change%sPopSpaceWrapper" % (game_entity_name)
            nyan_patch_name = "Change%sPopSpace" % (game_entity_name)

        # Wrapper
        wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
        wrapper_location = ForwardRef(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

        if line.is_harvestable():
            nyan_patch_raw_api_object.add_raw_patch_member("max_amount",
                                                           value,
                                                           "engine.aux.resource_spot.ResourceSpot",
                                                           operator)

        else:
            nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                           value,
                                                           "engine.aux.resource.ResourceAmount",
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
    def rotation_speed_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the move speed modify effect (ID: 6).

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
        patches = []

        # Unused in AoC

        return patches

    @staticmethod
    def search_radius_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the search radius modify effect (ID: 23).

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

        if isinstance(line, GenieBuildingLineGroup) and not line.is_projectile_shooter():
            # Does not have the ability
            return patches

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[head_unit_id][0]

        stance_names = ["Aggressive", "Defensive", "StandGround", "Passive"]

        for stance_name in stance_names:
            patch_target_ref = "%s.GameEntityStance.%s" % (game_entity_name, stance_name)
            patch_target_forward_ref = ForwardRef(line, patch_target_ref)

            # Wrapper
            wrapper_name = "Change%s%sSearchRangeWrapper" % (game_entity_name, stance_name)
            wrapper_ref = "%s.%s" % (obj_name, wrapper_name)
            wrapper_location = ForwardRef(converter_group, obj_name)
            wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                  wrapper_name,
                                                  dataset.nyan_api_objects,
                                                  wrapper_location)
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

            # Nyan patch
            nyan_patch_name = "Change%s%sSearchRange" % (game_entity_name, stance_name)
            nyan_patch_ref = "%s.%s.%s" % (obj_name, wrapper_name, nyan_patch_name)
            nyan_patch_location = ForwardRef(converter_group, wrapper_ref)
            nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                     nyan_patch_name,
                                                     dataset.nyan_api_objects,
                                                     nyan_patch_location)
            nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
            nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

            nyan_patch_raw_api_object.add_raw_patch_member("search_range",
                                                           value,
                                                           "engine.aux.game_entity_stance.GameEntityStance",
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
    def terrain_defense_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the terrain defense modify effect (ID: 18).

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
        patches = []

        # Unused in AoC

        return patches

    @staticmethod
    def unit_size_x_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the unit size x modify effect (ID: 3).

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
        patches = []

        # Unused in AoC

        return patches

    @staticmethod
    def unit_size_y_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the unit size y modify effect (ID: 4).

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
        patches = []

        # Unused in AoC

        return patches

    @staticmethod
    def work_rate_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the work rate modify effect (ID: 13).

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
        patches = []

        # TODO: Implement

        return patches
