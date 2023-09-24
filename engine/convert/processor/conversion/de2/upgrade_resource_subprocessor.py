# Copyright 2020-2022 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-lines,too-many-statements,too-many-public-methods,invalid-name
#
# TODO: Remove when all methods are implemented
# pylint: disable=unused-argument

"""
Creates upgrade patches for resource modification effects in DE2.
"""
from __future__ import annotations
import typing


if typing.TYPE_CHECKING:
    from engine.convert.entity_object.conversion.converter_object import ConverterObjectGroup
    from engine.nyan.nyan_structs import MemberOperator
    from engine.convert.value_object.conversion.forward_ref import ForwardRef


class DE2UpgradeResourceSubprocessor:
    """
    Creates raw API objects for resource upgrade effects in DE2.
    """

    @staticmethod
    def bengali_conversion_resistance_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the bengali conversion resistance effect (ID: 262).

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
    def burgundian_vineyards_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the burgundian vineyards effect (ID: 236).

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
    def cliff_attack_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the cliff attack multiplier effect (ID: 212).

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
    def conversion_min_adjustment_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the conversion min adjustment modify effect (ID: 176).

        TODO: Move into AoC processor

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
    def conversion_max_adjustment_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the conversion max adjustment modify effect (ID: 177).

        TODO: Move into AoC processor

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
    def conversion_min_building_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the conversion min building modify effect (ID: 180).

        TODO: Move into AoC processor

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
    def conversion_max_building_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the conversion max building modify effect (ID: 181).

        TODO: Move into AoC processor

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
    def conversion_building_chance_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the conversion building chance modify effect (ID: 182).

        TODO: Move into AoC processor

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
    def cuman_tc_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the cuman TC modify effect (ID: 218).

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
    def current_food_amount_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the current food amount modify effect (ID: 0).

        TODO: Move into AoC processor

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
    def current_wood_amount_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the current wood amount modify effect (ID: 1).

        TODO: Move into AoC processor

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
    def current_stone_amount_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the current stone amount modify effect (ID: 2).

        TODO: Move into AoC processor

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
    def current_gold_amount_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the current gold amount modify effect (ID: 3).

        TODO: Move into AoC processor

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
    def doi_paper_money_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the Paper Money effect in Dynasties of India (ID: 266).

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
    def elevation_attack_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the elevation attack multiplier effect (ID: 211).

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
    def feitoria_gold_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the feitoria gold productivity effect (ID: 208).

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

        # TODO: Implement all resources

        return patches

    @staticmethod
    def first_crusade_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the first crusade effect (ID: 234).

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
    def fish_trap_food_amount_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the fish trap food amount modify effect (ID: 88).

        TODO: Move into AoC processor

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
    def folwark_collect_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the folwark collect amount modify effect (ID: 237).

        TODO: Move into AoC processor

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
    def folwark_flag_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the folwark flag effect (ID: 238).

        TODO: Move into AoC processor

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
    def folwark_mill_id_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the folwark mill ID set effect (ID: 239).

        TODO: Move into AoC processor

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
    def free_kipchaks_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the current gold amount modify effect (ID: 214).

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
    def herdable_garrison_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the herdable garrison effect (ID: 254).

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
    def relic_food_production_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the relic food production effect (ID: 220).

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
    def reveal_enemy_tcs_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the reveal enemy TCs effect (ID: 209).

        TODO: Move into Rajas processor

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
    def sheep_food_amount_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the sheep food amount modify effect (ID: 216).

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
    def stone_gold_gen_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the polish stone gold generation effect (ID: 241).

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
    def trade_food_bonus_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the trade food bonus effect (ID: 251).

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
    def workshop_food_gen_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the workshop food generation effect (ID: 242).

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
    def workshop_wood_gen_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the workshop wood generation effect (ID: 243).

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
    def workshop_stone_gen_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the workshop stone generation effect (ID: 244).

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
    def workshop_gold_gen_upgrade(
        converter_group: ConverterObjectGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the workshop gold generation effect (ID: 245).

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
