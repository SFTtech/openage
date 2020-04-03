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
            # TODO: Use a common object here
            #===================================================================
            # attack_raw_api_object.add_raw_member("min_change_value",
            #                                      TODO,
            #                                      effect_parent)
            #===================================================================

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

        # TODO: Fallback type

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
