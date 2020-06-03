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

    @staticmethod
    def death_ability(line):
        """
        Adds a PassiveTransformTo ability to a line that is used to make entities die.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.death_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def harvestable_ability(line):
        """
        Adds the Harvestable ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.harvestable_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def hitbox_ability(line):
        """
        Adds the Hitbox ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.hitbox_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def idle_ability(line):
        """
        Adds the Idle ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.idle_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def live_ability(line):
        """
        Adds the Live ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.live_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def los_ability(line):
        """
        Adds the LineOfSight ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.los_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def move_ability(line):
        """
        Adds the Move ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.move_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def named_ability(line):
        """
        Adds the Named ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.named_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def provide_contingent_ability(line):
        """
        Adds the ProvideContingent ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.provide_contingent_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def restock_ability(line, restock_target_id):
        """
        Adds the Restock ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.restock_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def selectable_ability(line):
        """
        Adds Selectable abilities to a line. Units will get two of these,
        one Rectangle box for the Self stance and one MatchToSprite box
        for other stances.

        :param line: Unit/Building line that gets the abilities.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the abilities.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.selectable_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def shoot_projectile_ability(line, command_id):
        """
        Adds the ShootProjectile ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.shoot_projectile_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref

    @staticmethod
    def turn_ability(line):
        """
        Adds the Turn ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The forward reference for the ability.
        :rtype: ...dataformat.forward_ref.ForwardRef
        """
        ability_forward_ref = AoCAbilitySubprocessor.turn_ability(line)

        # TODO: Implement diffing of civ lines

        return ability_forward_ref
