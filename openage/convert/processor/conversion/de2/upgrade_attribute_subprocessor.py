# Copyright 2020-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-few-public-methods
#
# TODO: Remove when all methods are implemented
# pylint: disable=unused-argument

"""
Creates upgrade patches for attribute modification effects in DE2.
"""
from __future__ import annotations
import typing


if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.converter_object import ConverterObjectGroup
    from openage.convert.entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from openage.nyan.nyan_structs import MemberOperator
    from openage.convert.value_object.conversion.forward_ref import ForwardRef


class DE2UpgradeAttributeSubprocessor:
    """
    Creates raw API objects for attribute upgrade effects in DE2.
    """

    @staticmethod
    def bfg_unknown_51_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for a BfG unknown attribute effect (ID: 51).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def bfg_unknown_71_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for a BfG unknown attribute effect (ID: 71).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def bfg_unknown_73_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for a BfG unknown attribute effect (ID: 73).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def charge_attack_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the charge attack modify effect (ID: 59).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def charge_event_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the charge event modify effect (ID: 61).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def charge_regen_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the charge regen modify effect (ID: 60).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def charge_type_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the charge type modify effect (ID: 62).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def convert_chance_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the convert chance modify effect (ID: 113).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def herdable_capacity_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the herdable garrison capacity modify effect (ID: 30).

        TODO: Move into AK processor.

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def min_convert_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the min convert interval modify effect (ID: 111).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def max_convert_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the max convert interval modify effect (ID: 112).

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def regeneration_rate_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the regeneration rate modify effect (ID: 109).

        TODO: Move into AK processor.

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
        patches = []

        # TODO: Implement

        return patches

    @staticmethod
    def villager_pop_space_upgrade(
        converter_group: ConverterObjectGroup,
        line: GenieGameEntityGroup,
        value: typing.Union[int, float],
        operator: MemberOperator,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates a patch for the villager pop space modify effect (ID: 110).

        TODO: Move into AK processor.

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
        patches = []

        # TODO: Implement

        return patches
