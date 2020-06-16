# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates upgrade patches for attribute modification effects in DE2.
"""


class DE2UpgradeAttributeSubprocessor:

    @staticmethod
    def regeneration_rate_upgrade(converter_group, line, value, operator, team=False):
        """
        Creates a patch for the regeneration rate modify effect (ID: 109).

        TODO: Move into AK processor.

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
