# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-statements,too-many-branches
#
# TODO:
# pylint: disable=line-too-long

"""
Upgrades effects and resistances for the Apply*Effect and Resistance
abilities.
"""
from .....nyan.nyan_structs import MemberOperator
from ....entity_object.conversion.aoc.genie_unit import GenieBuildingLineGroup
from ....entity_object.conversion.converter_object import RawAPIObject
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef
from ....value_object.read.value_members import NoDiffMember,\
    LeftMissingMember, RightMissingMember


class AoCUpgradeEffectSubprocessor:
    """
    Creates raw API objects for attack/resistance upgrades in AoC.
    """

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
        :returns: The forward references for the effects.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        armor_lookup_dict = internal_name_lookups.get_armor_class_lookups(dataset.game_version)
        tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)

        tech_name = tech_lookup_dict[tech_id][0]

        diff_attacks = diff["attacks"].get_value()
        for diff_attack in diff_attacks.values():
            if isinstance(diff_attack, NoDiffMember):
                continue

            if isinstance(diff_attack, LeftMissingMember):
                # Create a new attack effect, then patch it in
                attack = diff_attack.get_reference()

                armor_class = attack["type_id"].get_value()
                attack_amount = attack["amount"].get_value()

                if armor_class == -1:
                    continue

                class_name = armor_lookup_dict[armor_class]

                # FlatAttributeChangeDecrease
                effect_parent = "engine.effect.discrete.flat_attribute_change.FlatAttributeChange"
                attack_parent = "engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"

                patch_target_ref = "%s" % (ability_ref)
                patch_target_forward_ref = ForwardRef(line, patch_target_ref)

                # Wrapper
                wrapper_name = "Add%sAttackEffectWrapper" % (class_name)
                wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
                wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                      wrapper_name,
                                                      dataset.nyan_api_objects)
                wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

                if isinstance(line, GenieBuildingLineGroup):
                    # Store building upgrades next to their game entity definition,
                    # not in the Age up techs.
                    wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                        % (name_lookup_dict[head_unit_id][1]))
                    wrapper_raw_api_object.set_filename("%s_upgrade" % tech_lookup_dict[tech_id][1])

                else:
                    wrapper_raw_api_object.set_location(ForwardRef(tech_group, tech_name))

                # Nyan patch
                nyan_patch_name = "Add%sAttackEffect" % (class_name)
                nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
                nyan_patch_location = ForwardRef(tech_group, wrapper_ref)
                nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                         nyan_patch_name,
                                                         dataset.nyan_api_objects,
                                                         nyan_patch_location)
                nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
                nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

                # New attack effect
                # ============================================================================
                attack_ref = "%s.%s" % (nyan_patch_ref, class_name)
                attack_raw_api_object = RawAPIObject(attack_ref,
                                                     class_name,
                                                     dataset.nyan_api_objects)
                attack_raw_api_object.add_raw_parent(attack_parent)
                attack_location = ForwardRef(tech_group, nyan_patch_ref)
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
                amount_name = "%s.%s.ChangeAmount" % (nyan_patch_ref, class_name)
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

                # Effect is added to the line, so it can be referenced by other upgrades
                line.add_raw_api_object(attack_raw_api_object)
                # ============================================================================
                attack_forward_ref = ForwardRef(line, attack_ref)
                nyan_patch_raw_api_object.add_raw_patch_member("effects",
                                                               [attack_forward_ref],
                                                               "engine.ability.type.ApplyDiscreteEffect",
                                                               MemberOperator.ADD)

                patch_forward_ref = ForwardRef(tech_group, nyan_patch_ref)
                wrapper_raw_api_object.add_raw_member("patch",
                                                      patch_forward_ref,
                                                      "engine.aux.patch.Patch")

                tech_group.add_raw_api_object(wrapper_raw_api_object)
                tech_group.add_raw_api_object(nyan_patch_raw_api_object)

                wrapper_forward_ref = ForwardRef(tech_group, wrapper_ref)
                patches.append(wrapper_forward_ref)

            elif isinstance(diff_attack, RightMissingMember):
                # Patch the effect out of the ability
                attack = diff_attack.get_reference()

                armor_class = attack["type_id"].get_value()
                class_name = armor_lookup_dict[armor_class]

                patch_target_ref = "%s" % (ability_ref)
                patch_target_forward_ref = ForwardRef(line, patch_target_ref)

                # Wrapper
                wrapper_name = "Remove%sAttackEffectWrapper" % (class_name)
                wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
                wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                      wrapper_name,
                                                      dataset.nyan_api_objects)
                wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

                if isinstance(line, GenieBuildingLineGroup):
                    # Store building upgrades next to their game entity definition,
                    # not in the Age up techs.
                    wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                        % (name_lookup_dict[head_unit_id][1]))
                    wrapper_raw_api_object.set_filename("%s_upgrade" % tech_lookup_dict[tech_id][1])

                else:
                    wrapper_raw_api_object.set_location(ForwardRef(tech_group, tech_name))

                # Nyan patch
                nyan_patch_name = "Remove%sAttackEffect" % (class_name)
                nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
                nyan_patch_location = ForwardRef(tech_group, wrapper_ref)
                nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                         nyan_patch_name,
                                                         dataset.nyan_api_objects,
                                                         nyan_patch_location)
                nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
                nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

                attack_ref = "%s.%s" % (ability_ref, class_name)
                attack_forward_ref = ForwardRef(line, attack_ref)
                nyan_patch_raw_api_object.add_raw_patch_member("effects",
                                                               [attack_forward_ref],
                                                               "engine.ability.type.ApplyDiscreteEffect",
                                                               MemberOperator.SUBTRACT)

                patch_forward_ref = ForwardRef(tech_group, nyan_patch_ref)
                wrapper_raw_api_object.add_raw_member("patch",
                                                      patch_forward_ref,
                                                      "engine.aux.patch.Patch")

                tech_group.add_raw_api_object(wrapper_raw_api_object)
                tech_group.add_raw_api_object(nyan_patch_raw_api_object)

                wrapper_forward_ref = ForwardRef(tech_group, wrapper_ref)
                patches.append(wrapper_forward_ref)

            else:
                diff_armor_class = diff_attack["type_id"]
                if not isinstance(diff_armor_class, NoDiffMember):
                    # If this happens then the attacks are out of order
                    # and we have to try something else
                    raise Exception("Could not create effect upgrade for line %s: Out of order"
                                    % (line))

                armor_class = diff_armor_class.get_reference().get_value()
                attack_amount = diff_attack["amount"].get_value()

                class_name = armor_lookup_dict[armor_class]

                patch_target_ref = "%s.%s.ChangeAmount" % (ability_ref, class_name)
                patch_target_forward_ref = ForwardRef(line, patch_target_ref)

                # Wrapper
                wrapper_name = "Change%sAttackWrapper" % (class_name)
                wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
                wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                      wrapper_name,
                                                      dataset.nyan_api_objects)
                wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

                if isinstance(line, GenieBuildingLineGroup):
                    # Store building upgrades next to their game entity definition,
                    # not in the Age up techs.
                    wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                        % (name_lookup_dict[head_unit_id][1]))
                    wrapper_raw_api_object.set_filename("%s_upgrade" % tech_lookup_dict[tech_id][1])

                else:
                    wrapper_raw_api_object.set_location(ForwardRef(tech_group, tech_name))

                # Nyan patch
                nyan_patch_name = "Change%sAttack" % (class_name)
                nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
                nyan_patch_location = ForwardRef(tech_group, wrapper_ref)
                nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                         nyan_patch_name,
                                                         dataset.nyan_api_objects,
                                                         nyan_patch_location)
                nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
                nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

                nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                               attack_amount,
                                                               "engine.aux.attribute.AttributeAmount",
                                                               MemberOperator.ADD)

                patch_forward_ref = ForwardRef(tech_group, nyan_patch_ref)
                wrapper_raw_api_object.add_raw_member("patch",
                                                      patch_forward_ref,
                                                      "engine.aux.patch.Patch")

                tech_group.add_raw_api_object(wrapper_raw_api_object)
                tech_group.add_raw_api_object(nyan_patch_raw_api_object)

                wrapper_forward_ref = ForwardRef(tech_group, wrapper_ref)
                patches.append(wrapper_forward_ref)

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
        :returns: The forward references for the resistances.
        :rtype: list
        """
        head_unit_id = line.get_head_unit_id()
        tech_id = tech_group.get_id()
        dataset = line.data

        patches = []

        name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
        armor_lookup_dict = internal_name_lookups.get_armor_class_lookups(dataset.game_version)
        tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)

        tech_name = tech_lookup_dict[tech_id][0]

        diff_armors = diff["armors"].get_value()
        for diff_armor in diff_armors.values():
            if isinstance(diff_armor, NoDiffMember):
                continue

            if isinstance(diff_armor, LeftMissingMember):
                # Create a new attack resistance, then patch it in
                armor = diff_armor.get_reference()

                armor_class = armor["type_id"].get_value()
                armor_amount = armor["amount"].get_value()

                if armor_class == -1:
                    continue

                class_name = armor_lookup_dict[armor_class]

                # FlatAttributeChangeDecrease
                resistance_parent = "engine.resistance.discrete.flat_attribute_change.FlatAttributeChange"
                armor_parent = "engine.resistance.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"

                patch_target_ref = "%s" % (ability_ref)
                patch_target_forward_ref = ForwardRef(line, patch_target_ref)

                # Wrapper
                wrapper_name = "Add%sAttackResistanceWrapper" % (class_name)
                wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
                wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                      wrapper_name,
                                                      dataset.nyan_api_objects)
                wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

                if isinstance(line, GenieBuildingLineGroup):
                    # Store building upgrades next to their game entity definition,
                    # not in the Age up techs.
                    wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                        % (name_lookup_dict[head_unit_id][1]))
                    wrapper_raw_api_object.set_filename("%s_upgrade" % tech_lookup_dict[tech_id][1])

                else:
                    wrapper_raw_api_object.set_location(ForwardRef(tech_group, tech_name))

                # Nyan patch
                nyan_patch_name = "Add%sAttackResistance" % (class_name)
                nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
                nyan_patch_location = ForwardRef(tech_group, wrapper_ref)
                nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                         nyan_patch_name,
                                                         dataset.nyan_api_objects,
                                                         nyan_patch_location)
                nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
                nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

                # New attack effect
                # ============================================================================
                attack_ref = "%s.%s" % (nyan_patch_ref, class_name)
                attack_raw_api_object = RawAPIObject(attack_ref,
                                                     class_name,
                                                     dataset.nyan_api_objects)
                attack_raw_api_object.add_raw_parent(armor_parent)
                attack_location = ForwardRef(tech_group, nyan_patch_ref)
                attack_raw_api_object.set_location(attack_location)

                # Type
                type_ref = "aux.attribute_change_type.types.%s" % (class_name)
                change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
                attack_raw_api_object.add_raw_member("type",
                                                     change_type,
                                                     resistance_parent)

                # Block value
                # =================================================================================
                amount_name = "%s.%s.BlockAmount" % (nyan_patch_ref, class_name)
                amount_raw_api_object = RawAPIObject(amount_name, "BlockAmount", dataset.nyan_api_objects)
                amount_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeAmount")
                amount_location = ForwardRef(line, attack_ref)
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
                attack_raw_api_object.add_raw_member("block_value",
                                                     amount_forward_ref,
                                                     resistance_parent)

                # Resistance is added to the line, so it can be referenced by other upgrades
                line.add_raw_api_object(attack_raw_api_object)
                # ============================================================================
                attack_forward_ref = ForwardRef(line, attack_ref)
                nyan_patch_raw_api_object.add_raw_patch_member("resistances",
                                                               [attack_forward_ref],
                                                               "engine.ability.type.Resistance",
                                                               MemberOperator.ADD)

                patch_forward_ref = ForwardRef(tech_group, nyan_patch_ref)
                wrapper_raw_api_object.add_raw_member("patch",
                                                      patch_forward_ref,
                                                      "engine.aux.patch.Patch")

                tech_group.add_raw_api_object(wrapper_raw_api_object)
                tech_group.add_raw_api_object(nyan_patch_raw_api_object)

                wrapper_forward_ref = ForwardRef(tech_group, wrapper_ref)
                patches.append(wrapper_forward_ref)

            elif isinstance(diff_armor, RightMissingMember):
                # Patch the resistance out of the ability
                armor = diff_armor.get_reference()

                armor_class = armor["type_id"].get_value()
                class_name = armor_lookup_dict[armor_class]

                patch_target_ref = "%s" % (ability_ref)
                patch_target_forward_ref = ForwardRef(line, patch_target_ref)

                # Wrapper
                wrapper_name = "Remove%sAttackResistanceWrapper" % (class_name)
                wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
                wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                      wrapper_name,
                                                      dataset.nyan_api_objects)
                wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

                if isinstance(line, GenieBuildingLineGroup):
                    # Store building upgrades next to their game entity definition,
                    # not in the Age up techs.
                    wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                        % (name_lookup_dict[head_unit_id][1]))
                    wrapper_raw_api_object.set_filename("%s_upgrade" % tech_lookup_dict[tech_id][1])

                else:
                    wrapper_raw_api_object.set_location(ForwardRef(tech_group, tech_name))

                # Nyan patch
                nyan_patch_name = "Remove%sAttackResistance" % (class_name)
                nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
                nyan_patch_location = ForwardRef(tech_group, wrapper_ref)
                nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                         nyan_patch_name,
                                                         dataset.nyan_api_objects,
                                                         nyan_patch_location)
                nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
                nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

                attack_ref = "%s.%s" % (ability_ref, class_name)
                attack_forward_ref = ForwardRef(line, attack_ref)
                nyan_patch_raw_api_object.add_raw_patch_member("resistances",
                                                               [attack_forward_ref],
                                                               "engine.ability.type.Resistance",
                                                               MemberOperator.SUBTRACT)

                patch_forward_ref = ForwardRef(tech_group, nyan_patch_ref)
                wrapper_raw_api_object.add_raw_member("patch",
                                                      patch_forward_ref,
                                                      "engine.aux.patch.Patch")

                tech_group.add_raw_api_object(wrapper_raw_api_object)
                tech_group.add_raw_api_object(nyan_patch_raw_api_object)

                wrapper_forward_ref = ForwardRef(tech_group, wrapper_ref)
                patches.append(wrapper_forward_ref)

            else:
                diff_armor_class = diff_armor["type_id"]
                if not isinstance(diff_armor_class, NoDiffMember):
                    # If this happens then the armors are out of order
                    # and we have to try something else
                    raise Exception("Could not create effect upgrade for line %s: Out of order"
                                    % (line))

                armor_class = diff_armor_class.get_reference().get_value()
                armor_amount = diff_armor["amount"].get_value()

                class_name = armor_lookup_dict[armor_class]

                patch_target_ref = "%s.%s.BlockAmount" % (ability_ref, class_name)
                patch_target_forward_ref = ForwardRef(line, patch_target_ref)

                # Wrapper
                wrapper_name = "Change%sResistanceWrapper" % (class_name)
                wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
                wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                                      wrapper_name,
                                                      dataset.nyan_api_objects)
                wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

                if isinstance(line, GenieBuildingLineGroup):
                    # Store building upgrades next to their game entity definition,
                    # not in the Age up techs.
                    wrapper_raw_api_object.set_location("data/game_entity/generic/%s/"
                                                        % (name_lookup_dict[head_unit_id][1]))
                    wrapper_raw_api_object.set_filename("%s_upgrade" % tech_lookup_dict[tech_id][1])

                else:
                    wrapper_raw_api_object.set_location(ForwardRef(tech_group, tech_name))

                # Nyan patch
                nyan_patch_name = "Change%sResistance" % (class_name)
                nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
                nyan_patch_location = ForwardRef(tech_group, wrapper_ref)
                nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                         nyan_patch_name,
                                                         dataset.nyan_api_objects,
                                                         nyan_patch_location)
                nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
                nyan_patch_raw_api_object.set_patch_target(patch_target_forward_ref)

                nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                               armor_amount,
                                                               "engine.aux.attribute.AttributeAmount",
                                                               MemberOperator.ADD)

                patch_forward_ref = ForwardRef(tech_group, nyan_patch_ref)
                wrapper_raw_api_object.add_raw_member("patch",
                                                      patch_forward_ref,
                                                      "engine.aux.patch.Patch")

                tech_group.add_raw_api_object(wrapper_raw_api_object)
                tech_group.add_raw_api_object(nyan_patch_raw_api_object)

                wrapper_forward_ref = ForwardRef(tech_group, wrapper_ref)
                patches.append(wrapper_forward_ref)

        return patches
