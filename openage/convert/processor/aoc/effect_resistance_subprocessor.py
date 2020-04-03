# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates effects and resistances for the Apply*Effect and Resistance
abilities.
"""
from openage.convert.dataformat.aoc.internal_nyan_names import ARMOR_CLASS_LOOKUPS
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer


class AoCEffectResistanceSubprocessor:

    @staticmethod
    def get_attack_effects(line, ability_ref):
        """
        Creates effects that are used for attacking (unit command: 7)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The expected pointers for the effects.
        :rtype: list
        """
        current_unit = line.get_head_unit()
        dataset = line.data

        effects = []

        # FlatAttributeChangeDecrease
        effect_parent = "engine.effect.discrete.flat_attribute_change.FlatAttributeChange"
        attack_parent = "engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"

        attacks = current_unit["attacks"].get_value()

        for attack in attacks:
            armor_class = attack["type_id"].get_value()
            attack_amount = attack["amount"].get_value()
            class_name = ARMOR_CLASS_LOOKUPS[armor_class]

            attack_name = "%s.%s" % (ability_ref, class_name)
            attack_raw_api_object = RawAPIObject(attack_name,
                                                 class_name,
                                                 dataset.nyan_api_objects)
            attack_raw_api_object.add_raw_parent(attack_parent)
            attack_location = ExpectedPointer(line, ability_ref)
            attack_raw_api_object.set_location(attack_location)

            # Type
            type_ref = "aux.attribute_change_type.types.%s" % (class_name)
            change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
            attack_raw_api_object.add_raw_member("type",
                                                 change_type,
                                                 effect_parent)

            # Min value (optional)
            min_value = dataset.pregen_nyan_objects[("effect.discrete.flat_attribute_change."
                                                     "min_damage.AoE2MinChangeAmount")].get_nyan_object()
            attack_raw_api_object.add_raw_member("min_change_value",
                                                 min_value,
                                                 effect_parent)

            # Max value (optional; not added because there is none in AoE2)

            # Change value
            # =================================================================================
            amount_name = "%s.%s.ChangeAmount" % (ability_ref, class_name)
            amount_raw_api_object = RawAPIObject(amount_name, "ChangeAmount", dataset.nyan_api_objects)
            amount_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeAmount")
            amount_location = ExpectedPointer(line, attack_name)
            amount_raw_api_object.set_location(amount_location)

            attribute = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
            amount_raw_api_object.add_raw_member("type",
                                                 attribute,
                                                 "engine.aux.attribute.AttributeAmount")
            amount_raw_api_object.add_raw_member("amount",
                                                 attack_amount,
                                                 "engine.aux.attribute.AttributeAmount")

            line.add_raw_api_object(amount_raw_api_object)
            # =================================================================================
            amount_expected_pointer = ExpectedPointer(line, amount_name)
            attack_raw_api_object.add_raw_member("change_value",
                                                 amount_expected_pointer,
                                                 effect_parent)

            # Ignore protection
            attack_raw_api_object.add_raw_member("ignore_protection",
                                                 [],
                                                 effect_parent)

            line.add_raw_api_object(attack_raw_api_object)
            attack_expected_pointer = ExpectedPointer(line, attack_name)
            effects.append(attack_expected_pointer)

        # Fallback effect
        fallback_effect = dataset.pregen_nyan_objects[("effect.discrete.flat_attribute_change."
                                                       "fallback.AoE2AttackFallback")].get_nyan_object()
        effects.append(fallback_effect)

        return effects

    @staticmethod
    def get_convert_effects(line, ability_ref):
        """
        Creates effects that are used for attacking (unit command: 104)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The expected pointers for the effects.
        :rtype: list
        """
        current_unit = line.get_head_unit()
        dataset = line.data

        effects = []

        effect_parent = "engine.effect.discrete.convert.Convert"
        convert_parent = "engine.effect.discrete.convert.type.AoE2Convert"

        unit_commands = current_unit.get_member("unit_commands").get_value()
        for command in unit_commands:
            # Find the Heal command.
            type_id = command.get_value()["type"].get_value()

            if type_id == 104:
                break

        else:
            # Return the empty set
            return effects

        convert_name = "%s.ConvertEffect" % (ability_ref)
        convert_raw_api_object = RawAPIObject(convert_name,
                                              "ConvertEffect",
                                              dataset.nyan_api_objects)
        convert_raw_api_object.add_raw_parent(convert_parent)
        convert_location = ExpectedPointer(line, ability_ref)
        convert_raw_api_object.set_location(convert_location)

        # Type
        type_ref = "aux.convert_type.types.Convert"
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        convert_raw_api_object.add_raw_member("type",
                                              change_type,
                                              effect_parent)

        # Min success (optional; not added because there is none in AoE2)
        # Max success (optional; not added because there is none in AoE2)

        # Chance
        chance_success = 0.25   # hardcoded
        convert_raw_api_object.add_raw_member("chance_success",
                                              chance_success,
                                              effect_parent)

        # Fail cost (optional; not added because there is none in AoE2)

        # Guaranteed rounds skip
        convert_raw_api_object.add_raw_member("skip_guaranteed_rounds",
                                              0,
                                              convert_parent)

        # Protected rounds skip
        convert_raw_api_object.add_raw_member("skip_protected_rounds",
                                              0,
                                              convert_parent)

        line.add_raw_api_object(convert_raw_api_object)
        attack_expected_pointer = ExpectedPointer(line, convert_name)
        effects.append(attack_expected_pointer)

        return effects

    @staticmethod
    def get_heal_effects(line, ability_ref):
        """
        Creates effects that are used for healing (unit command: 105)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The expected pointers for the effects.
        :rtype: list
        """
        current_unit = line.get_head_unit()
        dataset = line.data

        effects = []

        effect_parent = "engine.effect.continuous.flat_attribute_change.FlatAttributeChange"
        attack_parent = "engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease"

        unit_commands = current_unit.get_member("unit_commands").get_value()
        heal_command = None

        for command in unit_commands:
            # Find the Heal command.
            type_id = command.get_value()["type"].get_value()

            if type_id == 105:
                heal_command = command
                break

        else:
            # Return the empty set
            return effects

        heal_rate = heal_command.get_value()["work_value1"].get_value()

        heal_name = "%s.HealEffect" % (ability_ref)
        heal_raw_api_object = RawAPIObject(heal_name,
                                           "HealEffect",
                                           dataset.nyan_api_objects)
        heal_raw_api_object.add_raw_parent(attack_parent)
        heal_location = ExpectedPointer(line, ability_ref)
        heal_raw_api_object.set_location(heal_location)

        # Type
        type_ref = "aux.attribute_change_type.types.Heal"
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        heal_raw_api_object.add_raw_member("type",
                                           change_type,
                                           effect_parent)

        # Min value (optional)
        min_value = dataset.pregen_nyan_objects[("effect.discrete.flat_attribute_change."
                                                 "min_heal.AoE2MinChangeAmount")].get_nyan_object()
        heal_raw_api_object.add_raw_member("min_change_rate",
                                           min_value,
                                           effect_parent)

        # Max value (optional; not added because there is none in AoE2)

        # Change rate
        # =================================================================================
        rate_name = "%s.Heal.ChangeRate" % (ability_ref)
        rate_raw_api_object = RawAPIObject(rate_name, "ChangeRate", dataset.nyan_api_objects)
        rate_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeRate")
        rate_location = ExpectedPointer(line, heal_name)
        rate_raw_api_object.set_location(rate_location)

        attribute = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
        rate_raw_api_object.add_raw_member("type",
                                           attribute,
                                           "engine.aux.attribute.AttributeRate")
        rate_raw_api_object.add_raw_member("rate",
                                           heal_rate,
                                           "engine.aux.attribute.AttributeRate")

        line.add_raw_api_object(rate_raw_api_object)
        # =================================================================================
        rate_expected_pointer = ExpectedPointer(line, rate_name)
        heal_raw_api_object.add_raw_member("change_rate",
                                           rate_expected_pointer,
                                           effect_parent)

        # Ignore protection
        heal_raw_api_object.add_raw_member("ignore_protection",
                                           [],
                                           effect_parent)

        line.add_raw_api_object(heal_raw_api_object)
        heal_expected_pointer = ExpectedPointer(line, heal_name)
        effects.append(heal_expected_pointer)

        return effects

    @staticmethod
    def get_attack_resistances(line, ability_ref):
        """
        Creates resistances that are used for attacking (unit command: 7)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The expected pointers for the effects.
        :rtype: list
        """
        current_unit = line.get_head_unit()
        dataset = line.data

        resistances = []

        # FlatAttributeChangeDecrease
        resistance_parent = "engine.resistance.discrete.flat_attribute_change.FlatAttributeChange"
        armor_parent = "engine.resistance.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"

        if current_unit.has_member("armors"):
            armors = current_unit["armors"].get_value()

        else:
            # TODO: Trees and blast defense
            armors = []

        for armor in armors:
            armor_class = armor["type_id"].get_value()
            armor_amount = armor["amount"].get_value()
            class_name = ARMOR_CLASS_LOOKUPS[armor_class]

            armor_name = "%s.%s" % (ability_ref, class_name)
            armor_raw_api_object = RawAPIObject(armor_name, class_name, dataset.nyan_api_objects)
            armor_raw_api_object.add_raw_parent(armor_parent)
            armor_location = ExpectedPointer(line, ability_ref)
            armor_raw_api_object.set_location(armor_location)

            # Type
            type_ref = "aux.attribute_change_type.types.%s" % (class_name)
            change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
            armor_raw_api_object.add_raw_member("type",
                                                change_type,
                                                resistance_parent)

            # Block value
            # =================================================================================
            amount_name = "%s.%s.BlockAmount" % (ability_ref, class_name)
            amount_raw_api_object = RawAPIObject(amount_name, "BlockAmount", dataset.nyan_api_objects)
            amount_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeAmount")
            amount_location = ExpectedPointer(line, armor_name)
            amount_raw_api_object.set_location(amount_location)

            attribute = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
            amount_raw_api_object.add_raw_member("type",
                                                 attribute,
                                                 "engine.aux.attribute.AttributeAmount")
            amount_raw_api_object.add_raw_member("amount",
                                                 armor_amount,
                                                 "engine.aux.attribute.AttributeAmount")

            line.add_raw_api_object(amount_raw_api_object)
            # =================================================================================
            amount_expected_pointer = ExpectedPointer(line, amount_name)
            armor_raw_api_object.add_raw_member("block_value",
                                                amount_expected_pointer,
                                                resistance_parent)

            line.add_raw_api_object(armor_raw_api_object)
            armor_expected_pointer = ExpectedPointer(line, armor_name)
            resistances.append(armor_expected_pointer)

        # TODO: Fallback type

        return resistances
