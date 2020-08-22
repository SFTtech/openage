# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,invalid-name
#
# TODO:
# pylint: disable=line-too-long

"""
Creates effects and resistances for the Apply*Effect and Resistance
abilities.
"""
from .....nyan.nyan_structs import MemberSpecialValue
from ....entity_object.conversion.aoc.genie_unit import GenieUnitLineGroup,\
    GenieBuildingLineGroup
from ....entity_object.conversion.converter_object import RawAPIObject
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef


class AoCEffectSubprocessor:
    """
    Creates raw API objects for attacks/resistances in AoC.
    """

    @staticmethod
    def get_attack_effects(line, ability_ref, projectile=-1):
        """
        Creates effects that are used for attacking (unit command: 7)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The forward references for the effects.
        :rtype: list
        """
        dataset = line.data

        if projectile != 1:
            current_unit = line.get_head_unit()

        else:
            projectile_id = line.get_head_unit()["attack_projectile_secondary_unit_id"].get_value()
            current_unit = dataset.genie_units[projectile_id]

        effects = []

        armor_lookup_dict = internal_name_lookups.get_armor_class_lookups(dataset.game_version)

        # FlatAttributeChangeDecrease
        effect_parent = "engine.effect.discrete.flat_attribute_change.FlatAttributeChange"
        attack_parent = "engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"

        attacks = current_unit["attacks"].get_value()

        for attack in attacks.values():
            armor_class = attack["type_id"].get_value()
            attack_amount = attack["amount"].get_value()
            class_name = armor_lookup_dict[armor_class]

            attack_ref = "%s.%s" % (ability_ref, class_name)
            attack_raw_api_object = RawAPIObject(attack_ref,
                                                 class_name,
                                                 dataset.nyan_api_objects)
            attack_raw_api_object.add_raw_parent(attack_parent)
            attack_location = ForwardRef(line, ability_ref)
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
            amount_location = ForwardRef(line, attack_ref)
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
            amount_forward_ref = ForwardRef(line, amount_name)
            attack_raw_api_object.add_raw_member("change_value",
                                                 amount_forward_ref,
                                                 effect_parent)

            # Ignore protection
            attack_raw_api_object.add_raw_member("ignore_protection",
                                                 [],
                                                 effect_parent)

            line.add_raw_api_object(attack_raw_api_object)
            attack_forward_ref = ForwardRef(line, attack_ref)
            effects.append(attack_forward_ref)

        # Fallback effect
        fallback_effect = dataset.pregen_nyan_objects[("effect.discrete.flat_attribute_change."
                                                       "fallback.AoE2AttackFallback")].get_nyan_object()
        effects.append(fallback_effect)

        return effects

    @staticmethod
    def get_convert_effects(line, ability_ref):
        """
        Creates effects that are used for conversion (unit command: 104)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The forward references for the effects.
        :rtype: list
        """
        current_unit = line.get_head_unit()
        dataset = line.data

        effects = []

        effect_parent = "engine.effect.discrete.convert.Convert"
        convert_parent = "engine.effect.discrete.convert.type.AoE2Convert"

        unit_commands = current_unit["unit_commands"].get_value()
        for command in unit_commands:
            # Find the Heal command.
            type_id = command["type"].get_value()

            if type_id == 104:
                skip_guaranteed_rounds = -1 * command["work_value1"].get_value()
                skip_protected_rounds = -1 * command["work_value2"].get_value()
                break

        else:
            # Return the empty set
            return effects

        # Unit conversion
        convert_ref = "%s.ConvertUnitEffect" % (ability_ref)
        convert_raw_api_object = RawAPIObject(convert_ref,
                                              "ConvertUnitEffect",
                                              dataset.nyan_api_objects)
        convert_raw_api_object.add_raw_parent(convert_parent)
        convert_location = ForwardRef(line, ability_ref)
        convert_raw_api_object.set_location(convert_location)

        # Type
        type_ref = "aux.convert_type.types.UnitConvert"
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        convert_raw_api_object.add_raw_member("type",
                                              change_type,
                                              effect_parent)

        # Min success (optional; not added because there is none in AoE2)
        # Max success (optional; not added because there is none in AoE2)

        # Chance
        chance_success = dataset.genie_civs[0]["resources"][182].get_value() / 100  # hardcoded resource
        convert_raw_api_object.add_raw_member("chance_success",
                                              chance_success,
                                              effect_parent)

        # Fail cost (optional; not added because there is none in AoE2)

        # Guaranteed rounds skip
        convert_raw_api_object.add_raw_member("skip_guaranteed_rounds",
                                              skip_guaranteed_rounds,
                                              convert_parent)

        # Protected rounds skip
        convert_raw_api_object.add_raw_member("skip_protected_rounds",
                                              skip_protected_rounds,
                                              convert_parent)

        line.add_raw_api_object(convert_raw_api_object)
        attack_forward_ref = ForwardRef(line, convert_ref)
        effects.append(attack_forward_ref)

        # Building conversion
        convert_ref = "%s.ConvertBuildingEffect" % (ability_ref)
        convert_raw_api_object = RawAPIObject(convert_ref,
                                              "ConvertBuildingUnitEffect",
                                              dataset.nyan_api_objects)
        convert_raw_api_object.add_raw_parent(convert_parent)
        convert_location = ForwardRef(line, ability_ref)
        convert_raw_api_object.set_location(convert_location)

        # Type
        type_ref = "aux.convert_type.types.BuildingConvert"
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        convert_raw_api_object.add_raw_member("type",
                                              change_type,
                                              effect_parent)

        # Min success (optional; not added because there is none in AoE2)
        # Max success (optional; not added because there is none in AoE2)

        # Chance
        chance_success = dataset.genie_civs[0]["resources"][182].get_value() / 100  # hardcoded resource
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
        attack_forward_ref = ForwardRef(line, convert_ref)
        effects.append(attack_forward_ref)

        return effects

    @staticmethod
    def get_heal_effects(line, ability_ref):
        """
        Creates effects that are used for healing (unit command: 105)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The forward references for the effects.
        :rtype: list
        """
        current_unit = line.get_head_unit()
        dataset = line.data

        effects = []

        effect_parent = "engine.effect.continuous.flat_attribute_change.FlatAttributeChange"
        heal_parent = "engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease"

        unit_commands = current_unit["unit_commands"].get_value()
        heal_command = None

        for command in unit_commands:
            # Find the Heal command.
            type_id = command["type"].get_value()

            if type_id == 105:
                heal_command = command
                break

        else:
            # Return the empty set
            return effects

        heal_rate = heal_command["work_value1"].get_value()

        heal_ref = "%s.HealEffect" % (ability_ref)
        heal_raw_api_object = RawAPIObject(heal_ref,
                                           "HealEffect",
                                           dataset.nyan_api_objects)
        heal_raw_api_object.add_raw_parent(heal_parent)
        heal_location = ForwardRef(line, ability_ref)
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
        rate_name = "%s.HealEffect.ChangeRate" % (ability_ref)
        rate_raw_api_object = RawAPIObject(rate_name, "ChangeRate", dataset.nyan_api_objects)
        rate_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeRate")
        rate_location = ForwardRef(line, heal_ref)
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
        rate_forward_ref = ForwardRef(line, rate_name)
        heal_raw_api_object.add_raw_member("change_rate",
                                           rate_forward_ref,
                                           effect_parent)

        # Ignore protection
        heal_raw_api_object.add_raw_member("ignore_protection",
                                           [],
                                           effect_parent)

        line.add_raw_api_object(heal_raw_api_object)
        heal_forward_ref = ForwardRef(line, heal_ref)
        effects.append(heal_forward_ref)

        return effects

    @staticmethod
    def get_repair_effects(line, ability_ref):
        """
        Creates effects that are used for repairing (unit command: 106)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The forward references for the effects.
        :rtype: list
        """
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        effects = []

        effect_parent = "engine.effect.continuous.flat_attribute_change.FlatAttributeChange"
        repair_parent = "engine.effect.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease"

        repairable_lines = []
        repairable_lines.extend(dataset.building_lines.values())
        for unit_line in dataset.unit_lines.values():
            if unit_line.is_repairable():
                repairable_lines.append(unit_line)

        for repairable_line in repairable_lines:
            game_entity_name = name_lookup_dict[repairable_line.get_head_unit_id()][0]

            repair_name = "%sRepairEffect" % (game_entity_name)
            repair_ref = "%s.%s" % (ability_ref, repair_name)
            repair_raw_api_object = RawAPIObject(repair_ref,
                                                 repair_name,
                                                 dataset.nyan_api_objects)
            repair_raw_api_object.add_raw_parent(repair_parent)
            repair_location = ForwardRef(line, ability_ref)
            repair_raw_api_object.set_location(repair_location)

            # Type
            type_ref = "aux.attribute_change_type.types.%sRepair" % (game_entity_name)
            change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
            repair_raw_api_object.add_raw_member("type",
                                                 change_type,
                                                 effect_parent)

            # Min value (optional; not added because buildings don't block repairing)

            # Max value (optional; not added because there is none in AoE2)

            # Change rate
            # =================================================================================
            rate_name = "%s.%s.ChangeRate" % (ability_ref, repair_name)
            rate_raw_api_object = RawAPIObject(rate_name, "ChangeRate", dataset.nyan_api_objects)
            rate_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeRate")
            rate_location = ForwardRef(line, repair_ref)
            rate_raw_api_object.set_location(rate_location)

            attribute = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
            rate_raw_api_object.add_raw_member("type",
                                               attribute,
                                               "engine.aux.attribute.AttributeRate")

            # Hardcoded repair rate:
            # - Buildings: 750 HP/min = 12.5 HP/s
            # - Ships/Siege: 187.5 HP/min = 3.125 HP/s
            if isinstance(repairable_line, GenieBuildingLineGroup):
                repair_rate = 12.5

            else:
                repair_rate = 3.125

            rate_raw_api_object.add_raw_member("rate",
                                               repair_rate,
                                               "engine.aux.attribute.AttributeRate")

            line.add_raw_api_object(rate_raw_api_object)
            # =================================================================================
            rate_forward_ref = ForwardRef(line, rate_name)
            repair_raw_api_object.add_raw_member("change_rate",
                                                 rate_forward_ref,
                                                 effect_parent)

            # Ignore protection
            repair_raw_api_object.add_raw_member("ignore_protection",
                                                 [],
                                                 effect_parent)

            # Repair cost
            repair_raw_api_object.add_raw_parent("engine.effect.specialization.CostEffect")
            cost_ref = "%s.CreatableGameEntity.%sRepairCost" % (game_entity_name, game_entity_name)
            cost_forward_ref = ForwardRef(repairable_line, cost_ref)
            repair_raw_api_object.add_raw_member("cost",
                                                 cost_forward_ref,
                                                 "engine.effect.specialization.CostEffect")

            line.add_raw_api_object(repair_raw_api_object)
            repair_forward_ref = ForwardRef(line, repair_ref)
            effects.append(repair_forward_ref)

        return effects

    @staticmethod
    def get_construct_effects(line, ability_ref):
        """
        Creates effects that are used for construction (unit command: 101)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The forward references for the effects.
        :rtype: list
        """
        dataset = line.data

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        effects = []

        progress_effect_parent = "engine.effect.continuous.time_relative_progress.TimeRelativeProgressChange"
        progress_construct_parent = "engine.effect.continuous.time_relative_progress.type.TimeRelativeProgressIncrease"
        attr_effect_parent = "engine.effect.continuous.time_relative_attribute.TimeRelativeAttributeChange"
        attr_construct_parent = "engine.effect.continuous.time_relative_attribute.type.TimeRelativeAttributeIncrease"

        constructable_lines = []
        constructable_lines.extend(dataset.building_lines.values())

        for constructable_line in constructable_lines:
            game_entity_name = name_lookup_dict[constructable_line.get_head_unit_id()][0]

            # Construction progress
            contruct_progress_name = "%sConstructProgressEffect" % (game_entity_name)
            contruct_progress_ref = "%s.%s" % (ability_ref, contruct_progress_name)
            contruct_progress_raw_api_object = RawAPIObject(contruct_progress_ref,
                                                            contruct_progress_name,
                                                            dataset.nyan_api_objects)
            contruct_progress_raw_api_object.add_raw_parent(progress_construct_parent)
            contruct_progress_location = ForwardRef(line, ability_ref)
            contruct_progress_raw_api_object.set_location(contruct_progress_location)

            # Type
            type_ref = "aux.construct_type.types.%sConstruct" % (game_entity_name)
            change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
            contruct_progress_raw_api_object.add_raw_member("type",
                                                            change_type,
                                                            progress_effect_parent)

            # Total change time
            change_time = constructable_line.get_head_unit()["creation_time"].get_value()
            contruct_progress_raw_api_object.add_raw_member("total_change_time",
                                                            change_time,
                                                            progress_effect_parent)

            line.add_raw_api_object(contruct_progress_raw_api_object)
            contruct_progress_forward_ref = ForwardRef(line, contruct_progress_ref)
            effects.append(contruct_progress_forward_ref)

            # HP increase during construction
            contruct_hp_name = "%sConstructHPEffect" % (game_entity_name)
            contruct_hp_ref = "%s.%s" % (ability_ref, contruct_hp_name)
            contruct_hp_raw_api_object = RawAPIObject(contruct_hp_ref,
                                                      contruct_hp_name,
                                                      dataset.nyan_api_objects)
            contruct_hp_raw_api_object.add_raw_parent(attr_construct_parent)
            contruct_hp_location = ForwardRef(line, ability_ref)
            contruct_hp_raw_api_object.set_location(contruct_hp_location)

            # Type
            type_ref = "aux.attribute_change_type.types.%sConstruct" % (game_entity_name)
            change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
            contruct_hp_raw_api_object.add_raw_member("type",
                                                      change_type,
                                                      attr_effect_parent)

            # Total change time
            change_time = constructable_line.get_head_unit()["creation_time"].get_value()
            contruct_hp_raw_api_object.add_raw_member("total_change_time",
                                                      change_time,
                                                      attr_effect_parent)

            # Ignore protection
            contruct_hp_raw_api_object.add_raw_member("ignore_protection",
                                                      [],
                                                      attr_effect_parent)

            line.add_raw_api_object(contruct_hp_raw_api_object)
            contruct_hp_forward_ref = ForwardRef(line, contruct_hp_ref)
            effects.append(contruct_hp_forward_ref)

        return effects

    @staticmethod
    def get_attack_resistances(line, ability_ref):
        """
        Creates resistances that are used for attacking (unit command: 7)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The forward references for the effects.
        :rtype: list
        """
        current_unit = line.get_head_unit()
        dataset = line.data

        armor_lookup_dict = internal_name_lookups.get_armor_class_lookups(dataset.game_version)

        resistances = []

        # FlatAttributeChangeDecrease
        resistance_parent = "engine.resistance.discrete.flat_attribute_change.FlatAttributeChange"
        armor_parent = "engine.resistance.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"

        if current_unit.has_member("armors"):
            armors = current_unit["armors"].get_value()

        else:
            # TODO: Trees and blast defense
            armors = {}

        for armor in armors.values():
            armor_class = armor["type_id"].get_value()
            armor_amount = armor["amount"].get_value()
            class_name = armor_lookup_dict[armor_class]

            armor_ref = "%s.%s" % (ability_ref, class_name)
            armor_raw_api_object = RawAPIObject(armor_ref, class_name, dataset.nyan_api_objects)
            armor_raw_api_object.add_raw_parent(armor_parent)
            armor_location = ForwardRef(line, ability_ref)
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
            amount_location = ForwardRef(line, armor_ref)
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
            amount_forward_ref = ForwardRef(line, amount_name)
            armor_raw_api_object.add_raw_member("block_value",
                                                amount_forward_ref,
                                                resistance_parent)

            line.add_raw_api_object(armor_raw_api_object)
            armor_forward_ref = ForwardRef(line, armor_ref)
            resistances.append(armor_forward_ref)

        # Fallback effect
        fallback_effect = dataset.pregen_nyan_objects[("resistance.discrete.flat_attribute_change."
                                                       "fallback.AoE2AttackFallback")].get_nyan_object()
        resistances.append(fallback_effect)

        return resistances

    @staticmethod
    def get_convert_resistances(line, ability_ref):
        """
        Creates resistances that are used for conversion (unit command: 104)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The forward references for the effects.
        :rtype: list
        """
        dataset = line.data

        resistances = []

        # AoE2Convert
        resistance_parent = "engine.resistance.discrete.convert.Convert"
        convert_parent = "engine.resistance.discrete.convert.type.AoE2Convert"

        resistance_ref = "%s.Convert" % (ability_ref)
        resistance_raw_api_object = RawAPIObject(resistance_ref, "Convert", dataset.nyan_api_objects)
        resistance_raw_api_object.add_raw_parent(convert_parent)
        resistance_location = ForwardRef(line, ability_ref)
        resistance_raw_api_object.set_location(resistance_location)

        # Type
        if isinstance(line, GenieUnitLineGroup):
            type_ref = "aux.convert_type.types.UnitConvert"

        else:
            type_ref = "aux.convert_type.types.BuildingConvert"

        convert_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        resistance_raw_api_object.add_raw_member("type",
                                                 convert_type,
                                                 resistance_parent)

        # Chance resist
        chance_resist = dataset.genie_civs[0]["resources"][77].get_value() / 100  # hardcoded resource
        resistance_raw_api_object.add_raw_member("chance_resist",
                                                 chance_resist,
                                                 resistance_parent)

        if isinstance(line, GenieUnitLineGroup):
            guaranteed_rounds = dataset.genie_civs[0]["resources"][178].get_value()
            protected_rounds = dataset.genie_civs[0]["resources"][179].get_value()

        else:
            guaranteed_rounds = dataset.genie_civs[0]["resources"][180].get_value()
            protected_rounds = dataset.genie_civs[0]["resources"][181].get_value()

        # Guaranteed rounds
        resistance_raw_api_object.add_raw_member("guaranteed_resist_rounds",
                                                 guaranteed_rounds,
                                                 convert_parent)

        # Protected rounds
        resistance_raw_api_object.add_raw_member("protected_rounds",
                                                 protected_rounds,
                                                 convert_parent)

        # Protection recharge
        resistance_raw_api_object.add_raw_member("protection_round_recharge_time",
                                                 0.0,
                                                 convert_parent)

        line.add_raw_api_object(resistance_raw_api_object)
        resistance_forward_ref = ForwardRef(line, resistance_ref)
        resistances.append(resistance_forward_ref)

        return resistances

    @staticmethod
    def get_heal_resistances(line, ability_ref):
        """
        Creates resistances that are used for healing (unit command: 105)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The forward references for the effects.
        :rtype: list
        """
        dataset = line.data

        resistances = []

        resistance_parent = "engine.resistance.continuous.flat_attribute_change.FlatAttributeChange"
        heal_parent = "engine.resistance.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease"

        resistance_ref = "%s.Heal" % (ability_ref)
        resistance_raw_api_object = RawAPIObject(resistance_ref,
                                                 "Heal",
                                                 dataset.nyan_api_objects)
        resistance_raw_api_object.add_raw_parent(heal_parent)
        resistance_location = ForwardRef(line, ability_ref)
        resistance_raw_api_object.set_location(resistance_location)

        # Type
        type_ref = "aux.attribute_change_type.types.Heal"
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        resistance_raw_api_object.add_raw_member("type",
                                                 change_type,
                                                 resistance_parent)

        # Block rate
        # =================================================================================
        rate_name = "%s.Heal.BlockRate" % (ability_ref)
        rate_raw_api_object = RawAPIObject(rate_name, "BlockRate", dataset.nyan_api_objects)
        rate_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeRate")
        rate_location = ForwardRef(line, resistance_ref)
        rate_raw_api_object.set_location(rate_location)

        attribute = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
        rate_raw_api_object.add_raw_member("type",
                                           attribute,
                                           "engine.aux.attribute.AttributeRate")
        rate_raw_api_object.add_raw_member("rate",
                                           0.0,
                                           "engine.aux.attribute.AttributeRate")

        line.add_raw_api_object(rate_raw_api_object)
        # =================================================================================
        rate_forward_ref = ForwardRef(line, rate_name)
        resistance_raw_api_object.add_raw_member("block_rate",
                                                 rate_forward_ref,
                                                 resistance_parent)

        line.add_raw_api_object(resistance_raw_api_object)
        resistance_forward_ref = ForwardRef(line, resistance_ref)
        resistances.append(resistance_forward_ref)

        return resistances

    @staticmethod
    def get_repair_resistances(line, ability_ref):
        """
        Creates resistances that are used for repairing (unit command: 106)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The forward references for the effects.
        :rtype: list
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        resistances = []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        resistance_parent = "engine.resistance.continuous.flat_attribute_change.FlatAttributeChange"
        repair_parent = "engine.resistance.continuous.flat_attribute_change.type.FlatAttributeChangeIncrease"

        resistance_ref = "%s.Repair" % (ability_ref)
        resistance_raw_api_object = RawAPIObject(resistance_ref,
                                                 "Repair",
                                                 dataset.nyan_api_objects)
        resistance_raw_api_object.add_raw_parent(repair_parent)
        resistance_location = ForwardRef(line, ability_ref)
        resistance_raw_api_object.set_location(resistance_location)

        # Type
        type_ref = "aux.attribute_change_type.types.%sRepair" % (game_entity_name)
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        resistance_raw_api_object.add_raw_member("type",
                                                 change_type,
                                                 resistance_parent)

        # Block rate
        # =================================================================================
        rate_name = "%s.Repair.BlockRate" % (ability_ref)
        rate_raw_api_object = RawAPIObject(rate_name, "BlockRate", dataset.nyan_api_objects)
        rate_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeRate")
        rate_location = ForwardRef(line, resistance_ref)
        rate_raw_api_object.set_location(rate_location)

        attribute = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
        rate_raw_api_object.add_raw_member("type",
                                           attribute,
                                           "engine.aux.attribute.AttributeRate")
        rate_raw_api_object.add_raw_member("rate",
                                           0.0,
                                           "engine.aux.attribute.AttributeRate")

        line.add_raw_api_object(rate_raw_api_object)
        # =================================================================================
        rate_forward_ref = ForwardRef(line, rate_name)
        resistance_raw_api_object.add_raw_member("block_rate",
                                                 rate_forward_ref,
                                                 resistance_parent)

        # Stacking of villager repair HP increase
        resistance_raw_api_object.add_raw_parent("engine.resistance.specialization.StackedResistance")

        # Stack limit
        resistance_raw_api_object.add_raw_member("stack_limit",
                                                 MemberSpecialValue.NYAN_INF,
                                                 "engine.resistance.specialization.StackedResistance")

        # Calculation type
        calculation_type = dataset.pregen_nyan_objects["aux.calculation_type.construct_calculation.BuildingConstruct"].get_nyan_object()
        resistance_raw_api_object.add_raw_member("calculation_type",
                                                 calculation_type,
                                                 "engine.resistance.specialization.StackedResistance")

        # Calculation type
        distribution_type = dataset.nyan_api_objects["engine.aux.distribution_type.type.Mean"]
        resistance_raw_api_object.add_raw_member("distribution_type",
                                                 distribution_type,
                                                 "engine.resistance.specialization.StackedResistance")

        line.add_raw_api_object(resistance_raw_api_object)
        resistance_forward_ref = ForwardRef(line, resistance_ref)
        resistances.append(resistance_forward_ref)

        return resistances

    @staticmethod
    def get_construct_resistances(line, ability_ref):
        """
        Creates resistances that are used for constructing (unit command: 101)

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The forward references for the effects.
        :rtype: list
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        resistances = []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

        game_entity_name = name_lookup_dict[current_unit_id][0]

        progress_resistance_parent = "engine.resistance.continuous.time_relative_progress.TimeRelativeProgressChange"
        progress_construct_parent = "engine.resistance.continuous.time_relative_progress.type.TimeRelativeProgressIncrease"
        attr_resistance_parent = "engine.resistance.continuous.time_relative_attribute.TimeRelativeAttributeChange"
        attr_construct_parent = "engine.resistance.continuous.time_relative_attribute.type.TimeRelativeAttributeIncrease"

        # Progress
        resistance_ref = "%s.ConstructProgress" % (ability_ref)
        resistance_raw_api_object = RawAPIObject(resistance_ref,
                                                 "ConstructProgress",
                                                 dataset.nyan_api_objects)
        resistance_raw_api_object.add_raw_parent(progress_construct_parent)
        resistance_location = ForwardRef(line, ability_ref)
        resistance_raw_api_object.set_location(resistance_location)

        # Type
        type_ref = "aux.construct_type.types.%sConstruct" % (game_entity_name)
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        resistance_raw_api_object.add_raw_member("type",
                                                 change_type,
                                                 progress_resistance_parent)

        line.add_raw_api_object(resistance_raw_api_object)
        resistance_forward_ref = ForwardRef(line, resistance_ref)
        resistances.append(resistance_forward_ref)

        # Stacking of villager construction times
        resistance_raw_api_object.add_raw_parent("engine.resistance.specialization.StackedResistance")

        # Stack limit
        resistance_raw_api_object.add_raw_member("stack_limit",
                                                 MemberSpecialValue.NYAN_INF,
                                                 "engine.resistance.specialization.StackedResistance")

        # Calculation type
        calculation_type = dataset.pregen_nyan_objects["aux.calculation_type.construct_calculation.BuildingConstruct"].get_nyan_object()
        resistance_raw_api_object.add_raw_member("calculation_type",
                                                 calculation_type,
                                                 "engine.resistance.specialization.StackedResistance")

        # Calculation type
        distribution_type = dataset.nyan_api_objects["engine.aux.distribution_type.type.Mean"]
        resistance_raw_api_object.add_raw_member("distribution_type",
                                                 distribution_type,
                                                 "engine.resistance.specialization.StackedResistance")

        # Health
        resistance_ref = "%s.ConstructHP" % (ability_ref)
        resistance_raw_api_object = RawAPIObject(resistance_ref,
                                                 "ConstructHP",
                                                 dataset.nyan_api_objects)
        resistance_raw_api_object.add_raw_parent(attr_construct_parent)
        resistance_location = ForwardRef(line, ability_ref)
        resistance_raw_api_object.set_location(resistance_location)

        # Type
        type_ref = "aux.attribute_change_type.types.%sConstruct" % (game_entity_name)
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        resistance_raw_api_object.add_raw_member("type",
                                                 change_type,
                                                 attr_resistance_parent)

        # Stacking of villager construction HP increase
        resistance_raw_api_object.add_raw_parent("engine.resistance.specialization.StackedResistance")

        # Stack limit
        resistance_raw_api_object.add_raw_member("stack_limit",
                                                 MemberSpecialValue.NYAN_INF,
                                                 "engine.resistance.specialization.StackedResistance")

        # Calculation type
        calculation_type = dataset.pregen_nyan_objects["aux.calculation_type.construct_calculation.BuildingConstruct"].get_nyan_object()
        resistance_raw_api_object.add_raw_member("calculation_type",
                                                 calculation_type,
                                                 "engine.resistance.specialization.StackedResistance")

        # Calculation type
        distribution_type = dataset.nyan_api_objects["engine.aux.distribution_type.type.Mean"]
        resistance_raw_api_object.add_raw_member("distribution_type",
                                                 distribution_type,
                                                 "engine.resistance.specialization.StackedResistance")

        line.add_raw_api_object(resistance_raw_api_object)
        resistance_forward_ref = ForwardRef(line, resistance_ref)
        resistances.append(resistance_forward_ref)

        return resistances
