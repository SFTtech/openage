# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for attribute modification effects in AoC.
"""
from openage.convert.dataformat.aoc.genie_unit import GenieBuildingLineGroup,\
    GenieAmbientGroup
from openage.convert.dataformat.aoc.internal_nyan_names import BUILDING_LINE_LOOKUPS,\
    AMBIENT_GROUP_LOOKUPS, UNIT_LINE_LOOKUPS, TECH_GROUP_LOOKUPS,\
    ARMOR_CLASS_LOOKUPS
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.converter_object import RawAPIObject


class AoCUpgradeAttributeSubprocessor:

    @staticmethod
    def accuracy_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the accuracy modify effect (ID: 11).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def armor_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the armor modify effect (ID: 8).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        armor_class = int(value) >> 8
        armor_amount = int(value) & 0x0F

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]
        class_name = ARMOR_CLASS_LOOKUPS[armor_class]

        patch_target_ref = "%s.Resistance.%s.BlockAmount" % (game_entity_name, class_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%s%sResistanceWrapper" % (game_entity_name, class_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%s%sResistance" % (game_entity_name, class_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                       armor_amount,
                                                       "engine.aux.attribute.AttributeAmount",
                                                       operator)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def attack_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the attack modify effect (ID: 9).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        attack_amount = int(value) & 0x0F
        armor_class = int(value) >> 8

        if armor_class == -1:
            return patches

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]
        class_name = ARMOR_CLASS_LOOKUPS[armor_class]

        if line.is_projectile_shooter():
            primary_projectile_id = line.get_head_unit()["attack_projectile_primary_unit_id"].get_value()
            if primary_projectile_id == -1:
                # Upgrade is skipped if the primary projectile is not defined
                return patches

            patch_target_ref = ("%s.ShootProjectile.Projectile0.Attack.%s.ChangeAmount"
                                % (game_entity_name, class_name))
            patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        else:
            patch_target_ref = "%s.Attack.%s.ChangeAmount" % (game_entity_name, class_name)
            patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%s%sAttackWrapper" % (game_entity_name, class_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%s%sAttack" % (game_entity_name, class_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                       attack_amount,
                                                       "engine.aux.attribute.AttributeAmount",
                                                       operator)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def ballistics_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the ballistics modify effect (ID: 19).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def blast_radius_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the blast radius modify effect (ID: 22).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def carry_capacity_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the carry capacity modify effect (ID: 14).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def cost_food_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the food cost modify effect (ID: 103).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def cost_wood_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the wood cost modify effect (ID: 104).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def cost_gold_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the food cost modify effect (ID: 105).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def cost_stone_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the food cost modify effect (ID: 106).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def creation_time_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the creation time modify effect (ID: 101).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "%s.CreatableGameEntity" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sCreationTimeWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sCreationTime" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("creation_time",
                                                       value,
                                                       "engine.aux.create.CreatableGameEntity",
                                                       operator)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def garrison_capacity_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the garrison capacity modify effect (ID: 2).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def garrison_heal_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the garrison heal rate modify effect (ID: 108).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def graphics_angle_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the graphics angle modify effect (ID: 17).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def hp_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the HP modify effect (ID: 0).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "%s.Live.Health" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sMaxHealthWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sMaxHealth" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("max_value",
                                                       value,
                                                       "engine.aux.attribute.AttributeSetting",
                                                       operator)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def los_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the line of sight modify effect (ID: 1).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: int, float
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "%s.LineOfSight" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sLineOfSightWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sLineOfSight" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("range",
                                                       value,
                                                       "engine.ability.type.LineOfSight",
                                                       operator)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def max_projectiles_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the max projectiles modify effect (ID: 107).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "%s.Attack" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sMaxProjectilesWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sMaxProjectiles" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("max_projectiles",
                                                       value,
                                                       "engine.ability.type.ShootProjectile",
                                                       operator)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def min_projectiles_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the min projectiles modify effect (ID: 102).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "%s.Attack" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sMinProjectilesWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sMinProjectiles" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("min_projectiles",
                                                       value,
                                                       "engine.ability.type.ShootProjectile",
                                                       operator)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def max_range_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the max range modify effect (ID: 12).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def min_range_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the min range modify effect (ID: 20).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def move_speed_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the move speed modify effect (ID: 5).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        elif isinstance(line, GenieAmbientGroup):
            name_lookup_dict = AMBIENT_GROUP_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[head_unit_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "%s.Move" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sMoveSpeedWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sMoveSpeed" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("speed",
                                                       value,
                                                       "engine.ability.type.Move",
                                                       operator)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def projectile_unit_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the projectile modify effect (ID: 16).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def reload_time_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the reload time modify effect (ID: 10).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def resource_cost_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the resource modify effect (ID: 100).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def resource_storage_1_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the resource storage 1 modify effect (ID: 21).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def rotation_speed_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the move speed modify effect (ID: 6).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Unused in AoC

        return patches

    @staticmethod
    def search_radius_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the search radius modify effect (ID: 23).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Tied to LineOfsight in openage

        return patches

    @staticmethod
    def terrain_defense_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the terrain defense modify effect (ID: 18).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches

    @staticmethod
    def unit_size_x_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the unit size x modify effect (ID: 3).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Unused in AoC

        return patches

    @staticmethod
    def unit_size_y_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the unit size y modify effect (ID: 4).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        # TODO: Unused in AoC

        return patches

    @staticmethod
    def work_rate_upgrade(tech_group, line, value, operator):
        """
        Creates a patch for the work rate modify effect (ID: 13).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that has the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches
