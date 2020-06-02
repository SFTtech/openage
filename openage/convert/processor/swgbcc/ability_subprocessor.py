# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Derives and adds abilities to lines. Subroutine of the
nyan subprocessor.

For SWGB we use the functions of the AoCAbilitySubprocessor, but additionally
create a diff for every civ line.
"""
from openage.convert.processor.aoc.ability_subprocessor import AoCAbilitySubprocessor


class SWGBAbilitySubprocessor:

    @staticmethod
    def active_transform_to_ability(line):
        """
        Adds the ActiveTransformTo ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.active_transform_to_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref
