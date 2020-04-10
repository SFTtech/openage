# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Upgrades effects and resistances for the Apply*Effect and Resistance
abilities.
"""
from openage.convert.dataformat.value_members import NoDiffMember,\
    LeftMissingMember, RightMissingMember
from openage.convert.dataformat.aoc.internal_nyan_names import ARMOR_CLASS_LOOKUPS,\
    TECH_GROUP_LOOKUPS
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.nyan.nyan_structs import MemberOperator


class AoCUpgradeEffectSubprocessor:

    @staticmethod
    def get_attack_effects(tech_group, line, diff, ability_ref):
        """
        Upgrades effects that are used for attacking (unit command: 7)

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The expected pointers for the effects.
        :rtype: list
        """
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        diff_attacks = diff["attacks"].get_value()
        for diff_attack in diff_attacks.values():
            if isinstance(diff_attack, NoDiffMember):
                continue

            elif isinstance(diff_attack, LeftMissingMember):
                # TODO:
                pass

            elif isinstance(diff_attack, RightMissingMember):
                # TODO:
                pass

            else:
                diff_armor_class = diff_attack["type_id"]
                if not isinstance(diff_armor_class, NoDiffMember):
                    # TODO: If this happens then the attacks are out of order
                    # and we have to try something else
                    raise Exception("Could not create effect upgrade for line %s: Out of order"
                                    % (line))

                armor_class = diff_armor_class.get_reference().get_value()
                attack_amount = diff_attack["amount"].get_value()

                class_name = ARMOR_CLASS_LOOKUPS[armor_class]

                patch_target_ref = "%s.%s.ChangeAmount" % (ability_ref, class_name)
                patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

                # Wrapper
                wrapper_name = "Change%sAttackWrapper" % (class_name)
                wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
                wrapper_location = ExpectedPointer(tech_group, tech_name)
                wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                      wrapper_name,
                                                      dataset.nyan_api_objects,
                                                      wrapper_location)
                wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

                # Nyan patch
                nyan_patch_name = "Change%sAttack" % (class_name)
                nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
                nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
                nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                         nyan_patch_name,
                                                         dataset.nyan_api_objects,
                                                         nyan_patch_location)
                nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
                nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

                nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                               attack_amount,
                                                               "engine.aux.attribute.AttributeAmount",
                                                               MemberOperator.ADD)

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
    def get_attack_resistances(tech_group, line, diff, ability_ref):
        """
        Upgrades resistances that are used for attacking (unit command: 7)

        :param tech_group: Tech that gets the patch.
        :type tech_group: ...dataformat.converter_object.ConverterObjectGroup
        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :param diff: A diff between two ConvertObject instances.
        :type diff: ...dataformat.converter_object.ConverterObject
        :param ability_ref: Reference of the ability raw API object the effects are added to.
        :type ability_ref: str
        :returns: The expected pointers for the effects.
        :rtype: list
        """
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        diff_armors = diff["armors"].get_value()
        for diff_armor in diff_armors.values():
            if isinstance(diff_armor, NoDiffMember):
                continue

            elif isinstance(diff_armor, LeftMissingMember):
                # TODO:
                pass

            elif isinstance(diff_armor, RightMissingMember):
                # TODO:
                pass

            else:
                diff_armor_class = diff_armor["type_id"]
                if not isinstance(diff_armor_class, NoDiffMember):
                    # TODO: If this happens then the attacks are out of order
                    # and we have to try something else
                    raise Exception("Could not create effect upgrade for line %s: Out of order"
                                    % (line))

                armor_class = diff_armor_class.get_reference().get_value()
                armor_amount = diff_armor["amount"].get_value()

                class_name = ARMOR_CLASS_LOOKUPS[armor_class]

                patch_target_ref = "%s.%s.ChangeAmount" % (ability_ref, class_name)
                patch_target_expected_pointer = ExpectedPointer(line, patch_target_ref)

                # Wrapper
                wrapper_name = "Change%sResistanceWrapper" % (class_name)
                wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
                wrapper_location = ExpectedPointer(tech_group, tech_name)
                wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                      wrapper_name,
                                                      dataset.nyan_api_objects,
                                                      wrapper_location)
                wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

                # Nyan patch
                nyan_patch_name = "Change%sResistance" % (class_name)
                nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
                nyan_patch_location = ExpectedPointer(tech_group, wrapper_ref)
                nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                         nyan_patch_name,
                                                         dataset.nyan_api_objects,
                                                         nyan_patch_location)
                nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
                nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

                nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                               armor_amount,
                                                               "engine.aux.attribute.AttributeAmount",
                                                               MemberOperator.ADD)

                patch_expected_pointer = ExpectedPointer(tech_group, nyan_patch_ref)
                wrapper_raw_api_object.add_raw_member("patch",
                                                      patch_expected_pointer,
                                                      "engine.aux.patch.Patch")

                tech_group.add_raw_api_object(wrapper_raw_api_object)
                tech_group.add_raw_api_object(nyan_patch_raw_api_object)

                wrapper_expected_pointer = ExpectedPointer(tech_group, wrapper_ref)
                patches.append(wrapper_expected_pointer)

        return patches
