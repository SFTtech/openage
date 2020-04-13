# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for resource modification effects in AoC.
"""
from openage.convert.dataformat.aoc.genie_unit import GenieBuildingLineGroup,\
    GenieAmbientGroup
from openage.convert.dataformat.aoc.internal_nyan_names import BUILDING_LINE_LOOKUPS,\
    AMBIENT_GROUP_LOOKUPS, UNIT_LINE_LOOKUPS, TECH_GROUP_LOOKUPS,\
    ARMOR_CLASS_LOOKUPS
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.converter_object import RawAPIObject


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
        patches = []

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
        patches = []

        return patches

    @staticmethod
    def building_conversion_upgrade(tech_group, value, operator):
        """
        Creates a patch for the building consion effect (ID: 28).

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
        patches = []

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
        patches = []

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
        patches = []

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
        patches = []

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
        patches = []

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
