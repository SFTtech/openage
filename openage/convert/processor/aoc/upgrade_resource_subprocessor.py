# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for resource modification effects in AoC.
"""
from openage.convert.dataformat.aoc.internal_nyan_names import BUILDING_LINE_LOOKUPS,\
    UNIT_LINE_LOOKUPS, TECH_GROUP_LOOKUPS
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.nyan.nyan_structs import MemberOperator


class AoCUpgradeResourceSubprocessor:

    @staticmethod
    def berserk_heal_rate_upgrade(tech_group, value, operator):
        """
        Creates a patch for the berserk heal rate modify effect (ID: 96).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        berserk_id = 692
        tech_id = tech_group.get_id()
        dataset = tech_group.data
        line = dataset.unit_lines[berserk_id]

        patches = []

        game_entity_name = UNIT_LINE_LOOKUPS[berserk_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "%s.RegenerateHealth.HealthRate" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sHealthRegenerationWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sHealthRegeneration" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        # Regeneration is on a counter, so we have to invert the value
        value = 1 / value
        nyan_patch_raw_api_object.add_raw_patch_member("rate",
                                                       value,
                                                       "engine.aux.attribute.AttributeRate",
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
    def bonus_population_upgrade(tech_group, value, operator):
        """
        Creates a patch for the bonus population effect (ID: 32).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        tech_id = tech_group.get_id()
        dataset = tech_group.data

        patches = []

        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "aux.resource.types.PopulationSpace"
        patch_target = dataset.pregen_nyan_objects[patch_target_ref].get_nyan_object()

        # Wrapper
        wrapper_name = "ChangePopulationCapWrapper"
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "ChangePopulationCap"
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target)

        nyan_patch_raw_api_object.add_raw_patch_member("max_amount",
                                                       value,
                                                       "engine.aux.resource.ResourceContingent",
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
    def building_conversion_upgrade(tech_group, value, operator):
        """
        Creates a patch for the building conversion effect (ID: 28).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        monk_id = 125
        tech_id = tech_group.get_id()
        dataset = tech_group.data
        line = dataset.unit_lines[monk_id]

        patches = []

        game_entity_name = UNIT_LINE_LOOKUPS[monk_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "%s.Convert" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Building conversion

        # Wrapper
        wrapper_name = "EnableBuildingConversionWrapper"
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "EnableBuildingConversion"
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        # New allowed types
        allowed_types = [dataset.pregen_nyan_objects["aux.game_entity_type.types.Building"].get_nyan_object()]
        nyan_patch_raw_api_object.add_raw_patch_member("allowed_types",
                                                       allowed_types,
                                                       "engine.ability.type.ApplyDiscreteEffect",
                                                       MemberOperator.ADD)

        # Blacklisted buildings
        tc_line = dataset.building_lines[109]
        farm_line = dataset.building_lines[50]
        fish_trap_line = dataset.building_lines[199]
        monastery_line = dataset.building_lines[104]
        castle_line = dataset.building_lines[82]
        palisade_line = dataset.building_lines[72]
        stone_wall_line = dataset.building_lines[117]
        stone_gate_line = dataset.building_lines[64]
        wonder_line = dataset.building_lines[276]

        blacklisted_expected_pointers = [ExpectedPointer(tc_line, "TownCenter"),
                                         ExpectedPointer(farm_line, "Farm"),
                                         ExpectedPointer(fish_trap_line, "FishingTrap"),
                                         ExpectedPointer(monastery_line, "Monastery"),
                                         ExpectedPointer(castle_line, "Castle"),
                                         ExpectedPointer(palisade_line, "PalisadeWall"),
                                         ExpectedPointer(stone_wall_line, "StoneWall"),
                                         ExpectedPointer(stone_gate_line, "StoneGate"),
                                         ExpectedPointer(wonder_line, "Wonder"),
                                         ]
        nyan_patch_raw_api_object.add_raw_patch_member("blacklisted_entities",
                                                       blacklisted_expected_pointers,
                                                       "engine.ability.type.ApplyDiscreteEffect",
                                                       MemberOperator.ADD)

        patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        tech_group.add_raw_api_object(wrapper_raw_api_object)
        tech_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        # Siege unit conversion

        # Wrapper
        wrapper_name = "EnableSiegeUnitConversionWrapper"
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "EnableSiegeUnitConversion"
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        # Blacklisted buildings
        ram_line = dataset.unit_lines[35]
        mangonel_line = dataset.unit_lines[280]
        scorpion_line = dataset.unit_lines[279]

        blacklisted_entities = [ExpectedPointer(ram_line, "Ram"),
                                ExpectedPointer(mangonel_line, "Mangonel"),
                                ExpectedPointer(scorpion_line, "Scorpion")]

        nyan_patch_raw_api_object.add_raw_patch_member("blacklisted_entities",
                                                       blacklisted_entities,
                                                       "engine.ability.type.ApplyDiscreteEffect",
                                                       MemberOperator.SUBTRACT)

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
    def chinese_tech_discount_upgrade(tech_group, value, operator):
        """
        Creates a patch for the chinese tech discount effect (ID: 85).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def construction_speed_upgrade(tech_group, value, operator):
        """
        Creates a patch for the construction speed modify effect (ID: 195).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def conversion_resistance_upgrade(tech_group, value, operator):
        """
        Creates a patch for the conversion resistance modify effect (ID: 77).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def conversion_resistance_min_rounds_upgrade(tech_group, value, operator):
        """
        Creates a patch for the conversion resistance modify effect (ID: 178).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def conversion_resistance_max_rounds_upgrade(tech_group, value, operator):
        """
        Creates a patch for the conversion resistance modify effect (ID: 179).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def crenellations_upgrade(tech_group, value, operator):
        """
        Creates a patch for the crenellations effect (ID: 194).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def faith_recharge_rate_upgrade(tech_group, value, operator):
        """
        Creates a patch for the faith_recharge_rate modify effect (ID: 35).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        monk_id = 125
        tech_id = tech_group.get_id()
        dataset = tech_group.data
        line = dataset.unit_lines[monk_id]

        patches = []

        game_entity_name = UNIT_LINE_LOOKUPS[monk_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "%s.RegenerateFaith.FaithRate" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sFaithRegenerationWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sFaithRegeneration" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("rate",
                                                       value,
                                                       "engine.aux.attribute.AttributeRate",
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
    def farm_food_upgrade(tech_group, value, operator):
        """
        Creates a patch for the farm food modify effect (ID: 36).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        farm_id = 50
        tech_id = tech_group.get_id()
        dataset = tech_group.data
        line = dataset.building_lines[farm_id]

        patches = []

        game_entity_name = BUILDING_LINE_LOOKUPS[farm_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "%s.Harvestable.%sResourceSpot" % (game_entity_name, game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sFoodAmountWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sFoodAmount" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("max_amount",
                                                       value,
                                                       "engine.aux.resource_spot.ResourceSpot",
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
    def gather_food_efficiency_upgrade(tech_group, value, operator):
        """
        Creates a patch for the food gathering efficiency modify effect (ID: 190).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def gather_wood_efficiency_upgrade(tech_group, value, operator):
        """
        Creates a patch for the wood gathering efficiency modify effect (ID: 189).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def gather_gold_efficiency_upgrade(tech_group, value, operator):
        """
        Creates a patch for the gold gathering efficiency modify effect (ID: 47).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def gather_stone_efficiency_upgrade(tech_group, value, operator):
        """
        Creates a patch for the stone gathering efficiency modify effect (ID: 79).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def heal_range_upgrade(tech_group, value, operator):
        """
        Creates a patch for the heal range modify effect (ID: 90).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        monk_id = 125
        tech_id = tech_group.get_id()
        dataset = tech_group.data
        line = dataset.unit_lines[monk_id]

        patches = []

        game_entity_name = UNIT_LINE_LOOKUPS[monk_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "%s.Heal" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sHealRangeWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sHealRange" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("max_range",
                                                       value,
                                                       "engine.ability.type.RangedContinuousEffect",
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
    def heal_rate_upgrade(tech_group, value, operator):
        """
        Creates a patch for the heal rate modify effect (ID: 89).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        # Unused in AoC

        return patches

    @staticmethod
    def herding_dominance_upgrade(tech_group, value, operator):
        """
        Creates a patch for the herding dominance effect (ID: 97).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def heresy_upgrade(tech_group, value, operator):
        """
        Creates a patch for the heresy effect (ID: 192).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def monk_conversion_upgrade(tech_group, value, operator):
        """
        Creates a patch for the monk conversion effect (ID: 27).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        monk_id = 125
        tech_id = tech_group.get_id()
        dataset = tech_group.data
        line = dataset.unit_lines[monk_id]

        patches = []

        game_entity_name = UNIT_LINE_LOOKUPS[monk_id][0]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "%s.Convert" % (game_entity_name)
        patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

        # Wrapper
        wrapper_name = "Enable%sConversionWrapper" % (game_entity_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Enable%sConversion" % (game_entity_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        monk_expected_pointer = ExpectedPointer(line, "Monk")
        nyan_patch_raw_api_object.add_raw_patch_member("blacklisted_entities",
                                                       [monk_expected_pointer],
                                                       "engine.ability.type.ApplyDiscreteEffect",
                                                       MemberOperator.SUBTRACT)

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
    def relic_gold_bonus_upgrade(tech_group, value, operator):
        """
        Creates a patch for the relic gold bonus modify effect (ID: 191).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def reveal_ally_upgrade(tech_group, value, operator):
        """
        Creates a patch for the reveal ally modify effect (ID: 50).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def reveal_enemy_upgrade(tech_group, value, operator):
        """
        Creates a patch for the reveal enemy modify effect (ID: 183).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def ship_conversion_upgrade(tech_group, value, operator):
        """
        Creates a patch for the ship conversion effect (ID: 87).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        # Unused in AoC

        return patches

    @staticmethod
    def spies_discount_upgrade(tech_group, value, operator):
        """
        Creates a patch for the spies discount effect (ID: 197).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def starting_food_upgrade(tech_group, value, operator):
        """
        Creates a patch for the starting food modify effect (ID: 91).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def starting_wood_upgrade(tech_group, value, operator):
        """
        Creates a patch for the starting wood modify effect (ID: 92).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def starting_villagers_upgrade(tech_group, value, operator):
        """
        Creates a patch for the starting villagers modify effect (ID: 84).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def starting_population_space_upgrade(tech_group, value, operator):
        """
        Creates a patch for the starting popspace modify effect (ID: 4).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        tech_id = tech_group.get_id()
        dataset = tech_group.data

        patches = []

        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        patch_target_ref = "aux.resource.types.PopulationSpace"
        patch_target = dataset.pregen_nyan_objects[patch_target_ref].get_nyan_object()

        # Wrapper
        wrapper_name = "ChangeInitialPopulationLimitWrapper"
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(tech_group, tech_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "ChangeInitialPopulationLimit"
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target)

        nyan_patch_raw_api_object.add_raw_patch_member("min_amount",
                                                       value,
                                                       "engine.aux.resource.ResourceContingent",
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
    def theocracy_upgrade(tech_group, value, operator):
        """
        Creates a patch for the theocracy effect (ID: 193).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def trade_penalty_upgrade(tech_group, value, operator):
        """
        Creates a patch for the trade penalty modify effect (ID: 78).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def tribute_inefficiency_upgrade(tech_group, value, operator):
        """
        Creates a patch for the tribute inefficiency modify effect (ID: 46).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
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
    def wonder_time_increase_upgrade(tech_group, value, operator):
        """
        Creates a patch for the wonder time modify effect (ID: 196).

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param value: Value used for patching the member.
        :type value: MemberOperator
        :param operator: Operator used for patching the member.
        :type operator: MemberOperator
        :returns: The expected pointers for the generated patches.
        :rtype: list
        """
        patches = []

        return patches
