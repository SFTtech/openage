# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates patches for technologies.
"""
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.dataformat.aoc.genie_tech import GenieTechEffectBundleGroup,\
    CivTeamBonus, CivBonus
from openage.convert.dataformat.aoc.genie_unit import GenieUnitLineGroup,\
    GenieBuildingLineGroup
from openage.convert.dataformat.aoc.internal_nyan_names import TECH_GROUP_LOOKUPS, CIV_GROUP_LOOKUPS
from openage.convert.dataformat.converter_object import RawAPIObject
from openage.convert.processor.aoc.upgrade_ability_subprocessor import AoCUgradeAbilitySubprocessor
from openage.convert.processor.aoc.upgrade_attribute_subprocessor import AoCUpgradeAttributeSubprocessor
from openage.convert.processor.aoc.upgrade_resource_subprocessor import AoCUpgradeResourceSubprocessor
from openage.nyan.nyan_structs import MemberOperator


class AoCTechSubprocessor:

    upgrade_attribute_funcs = {
        0: AoCUpgradeAttributeSubprocessor.hp_upgrade,
        1: AoCUpgradeAttributeSubprocessor.los_upgrade,
        2: AoCUpgradeAttributeSubprocessor.garrison_capacity_upgrade,
        3: AoCUpgradeAttributeSubprocessor.unit_size_x_upgrade,
        4: AoCUpgradeAttributeSubprocessor.unit_size_y_upgrade,
        5: AoCUpgradeAttributeSubprocessor.move_speed_upgrade,
        6: AoCUpgradeAttributeSubprocessor.rotation_speed_upgrade,
        8: AoCUpgradeAttributeSubprocessor.armor_upgrade,
        9: AoCUpgradeAttributeSubprocessor.attack_upgrade,
        10: AoCUpgradeAttributeSubprocessor.reload_time_upgrade,
        11: AoCUpgradeAttributeSubprocessor.accuracy_upgrade,
        12: AoCUpgradeAttributeSubprocessor.max_range_upgrade,
        13: AoCUpgradeAttributeSubprocessor.work_rate_upgrade,
        14: AoCUpgradeAttributeSubprocessor.carry_capacity_upgrade,
        16: AoCUpgradeAttributeSubprocessor.projectile_unit_upgrade,
        17: AoCUpgradeAttributeSubprocessor.graphics_angle_upgrade,
        18: AoCUpgradeAttributeSubprocessor.terrain_defense_upgrade,
        19: AoCUpgradeAttributeSubprocessor.ballistics_upgrade,
        20: AoCUpgradeAttributeSubprocessor.min_range_upgrade,
        21: AoCUpgradeAttributeSubprocessor.resource_storage_1_upgrade,
        22: AoCUpgradeAttributeSubprocessor.blast_radius_upgrade,
        23: AoCUpgradeAttributeSubprocessor.search_radius_upgrade,
        100: AoCUpgradeAttributeSubprocessor.resource_cost_upgrade,
        101: AoCUpgradeAttributeSubprocessor.creation_time_upgrade,
        102: AoCUpgradeAttributeSubprocessor.min_projectiles_upgrade,
        103: AoCUpgradeAttributeSubprocessor.cost_food_upgrade,
        104: AoCUpgradeAttributeSubprocessor.cost_wood_upgrade,
        105: AoCUpgradeAttributeSubprocessor.cost_gold_upgrade,
        106: AoCUpgradeAttributeSubprocessor.cost_stone_upgrade,
        107: AoCUpgradeAttributeSubprocessor.max_projectiles_upgrade,
        108: AoCUpgradeAttributeSubprocessor.garrison_heal_upgrade,
    }

    upgrade_resource_funcs = {
        4: AoCUpgradeResourceSubprocessor.starting_population_space_upgrade,
        27: AoCUpgradeResourceSubprocessor.monk_conversion_upgrade,
        28: AoCUpgradeResourceSubprocessor.building_conversion_upgrade,
        32: AoCUpgradeResourceSubprocessor.bonus_population_upgrade,
        35: AoCUpgradeResourceSubprocessor.faith_recharge_rate_upgrade,
        36: AoCUpgradeResourceSubprocessor.farm_food_upgrade,
        46: AoCUpgradeResourceSubprocessor.tribute_inefficiency_upgrade,
        47: AoCUpgradeResourceSubprocessor.gather_gold_efficiency_upgrade,
        50: AoCUpgradeResourceSubprocessor.reveal_ally_upgrade,
        77: AoCUpgradeResourceSubprocessor.conversion_resistance_upgrade,
        78: AoCUpgradeResourceSubprocessor.trade_penalty_upgrade,
        79: AoCUpgradeResourceSubprocessor.gather_stone_efficiency_upgrade,
        84: AoCUpgradeResourceSubprocessor.starting_villagers_upgrade,
        85: AoCUpgradeResourceSubprocessor.chinese_tech_discount_upgrade,
        89: AoCUpgradeResourceSubprocessor.heal_rate_upgrade,
        90: AoCUpgradeResourceSubprocessor.heal_range_upgrade,
        91: AoCUpgradeResourceSubprocessor.starting_food_upgrade,
        92: AoCUpgradeResourceSubprocessor.starting_wood_upgrade,
        96: AoCUpgradeResourceSubprocessor.berserk_heal_rate_upgrade,
        97: AoCUpgradeResourceSubprocessor.herding_dominance_upgrade,
        178: AoCUpgradeResourceSubprocessor.conversion_resistance_min_rounds_upgrade,
        179: AoCUpgradeResourceSubprocessor.conversion_resistance_max_rounds_upgrade,
        183: AoCUpgradeResourceSubprocessor.reveal_enemy_upgrade,
        189: AoCUpgradeResourceSubprocessor.gather_wood_efficiency_upgrade,
        190: AoCUpgradeResourceSubprocessor.gather_food_efficiency_upgrade,
        191: AoCUpgradeResourceSubprocessor.relic_gold_bonus_upgrade,
        192: AoCUpgradeResourceSubprocessor.heresy_upgrade,
        193: AoCUpgradeResourceSubprocessor.theocracy_upgrade,
        194: AoCUpgradeResourceSubprocessor.crenellations_upgrade,
        196: AoCUpgradeResourceSubprocessor.wonder_time_increase_upgrade,
        197: AoCUpgradeResourceSubprocessor.spies_discount_upgrade,
    }

    @classmethod
    def get_patches(cls, converter_group):
        """
        Returns the patches for a converter group, depending on the type
        of its effects.
        """
        patches = []
        dataset = converter_group.data
        team_bonus = False

        if isinstance(converter_group, CivTeamBonus):
            effects = converter_group.get_effects()

            # Change converter group here, so that the Civ object gets the patches
            converter_group = dataset.civ_groups[converter_group.get_civilization()]
            team_bonus = True

        elif isinstance(converter_group, CivBonus):
            effects = converter_group.get_effects()

            # Change converter group here, so that the Civ object gets the patches
            converter_group = dataset.civ_groups[converter_group.get_civilization()]

        else:
            effects = converter_group.get_effects()

        team_effect = False
        for effect in effects:
            type_id = effect.get_type()

            if team_bonus or type_id in (10, 11, 12, 13, 14, 15, 16):
                team_effect = True
                type_id -= 10

            if type_id in (0, 4, 5):
                patches.extend(cls._attribute_modify_effect(converter_group, effect, team=team_effect))

            elif type_id in (1, 6):
                patches.extend(cls._resource_modify_effect(converter_group, effect, team=team_effect))

            elif type_id == 2:
                # Enabling/disabling units: Handled in creatable conditions
                pass

            elif type_id == 3:
                patches.extend(cls._upgrade_unit_effect(converter_group, effect))

            elif type_id == 101:
                patches.extend(cls._tech_cost_modify_effect(converter_group, effect, team=team_effect))
                pass

            elif type_id == 102:
                # Tech disable: Only used for civ tech tree
                pass

            elif type_id == 103:
                patches.extend(cls._tech_time_modify_effect(converter_group, effect, team=team_effect))
                pass

            team_effect = False

        return patches

    @staticmethod
    def _attribute_modify_effect(converter_group, effect, team=False):
        """
        Creates the patches for modifying attributes of entities.
        """
        patches = []
        dataset = converter_group.data

        effect_type = effect.get_type()
        operator = None
        if effect_type == 0:
            operator = MemberOperator.ASSIGN

        elif effect_type == 4:
            operator = MemberOperator.ADD

        elif effect_type == 5:
            operator = MemberOperator.MULTIPLY

        else:
            raise Exception("Effect type %s is not a valid attribute effect"
                            % str(effect_type))

        unit_id = effect["attr_a"].get_value()
        class_id = effect["attr_b"].get_value()
        attribute_type = effect["attr_c"].get_value()
        value = effect["attr_d"].get_value()

        if attribute_type == -1:
            return patches

        affected_entities = []
        if unit_id != -1:
            entity_lines = {}
            entity_lines.update(dataset.unit_lines)
            entity_lines.update(dataset.building_lines)
            entity_lines.update(dataset.ambient_groups)

            for line in entity_lines.values():
                if line.contains_entity(unit_id):
                    affected_entities.append(line)

                elif attribute_type == 19:
                    if line.is_projectile_shooter() and line.has_projectile(unit_id):
                        affected_entities.append(line)

        elif class_id != -1:
            entity_lines = {}
            entity_lines.update(dataset.unit_lines)
            entity_lines.update(dataset.building_lines)
            entity_lines.update(dataset.ambient_groups)

            for line in entity_lines.values():
                if line.get_class_id() == class_id:
                    affected_entities.append(line)

        else:
            return patches

        upgrade_func = AoCTechSubprocessor.upgrade_attribute_funcs[attribute_type]
        for affected_entity in affected_entities:
            patches.extend(upgrade_func(converter_group, affected_entity, value, operator, team))

        return patches

    @staticmethod
    def _resource_modify_effect(converter_group, effect, team=False):
        """
        Creates the patches for modifying resources.
        """
        patches = []

        effect_type = effect.get_type()
        operator = None
        if effect_type == 1:
            mode = effect["attr_b"].get_value()

            if mode == 0:
                operator = MemberOperator.ASSIGN

            else:
                operator = MemberOperator.ADD

        elif effect_type == 6:
            operator = MemberOperator.MULTIPLY

        else:
            raise Exception("Effect type %s is not a valid resource effect"
                            % str(effect_type))

        resource_id = effect["attr_a"].get_value()
        value = effect["attr_d"].get_value()

        if resource_id in (-1, 6):
            # -1 = invalid ID
            # 6 = set current age (we don't use this)
            return patches

        upgrade_func = AoCTechSubprocessor.upgrade_resource_funcs[resource_id]
        patches.extend(upgrade_func(converter_group, value, operator, team))

        return patches

    @staticmethod
    def _upgrade_unit_effect(converter_group, effect):
        """
        Creates the patches for upgrading entities in a line.
        """
        patches = []
        tech_id = converter_group.get_id()
        dataset = converter_group.data

        upgrade_source_id = effect["attr_a"].get_value()
        upgrade_target_id = effect["attr_b"].get_value()

        if upgrade_source_id not in dataset.unit_ref.keys() or\
                upgrade_target_id not in dataset.unit_ref.keys():
            # Skip annexes or transform units
            return patches

        line = dataset.unit_ref[upgrade_source_id]
        upgrade_source_pos = line.get_unit_position(upgrade_source_id)
        upgrade_target_pos = line.get_unit_position(upgrade_target_id)

        if isinstance(line, GenieBuildingLineGroup):
            # Building upgrades always reference the head unit
            # so we use the decremented target id instead
            upgrade_source_pos = upgrade_target_pos - 1

        elif upgrade_target_pos - upgrade_source_pos != 1:
            # Skip effects that upgrades entities not next to each other in
            # the line.
            return patches

        upgrade_source = line.line[upgrade_source_pos]
        upgrade_target = line.line[upgrade_target_pos]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        diff = upgrade_source.diff(upgrade_target)

        patches.extend(AoCUgradeAbilitySubprocessor.death_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.despawn_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.idle_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.live_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.los_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.named_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.resistance_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.selectable_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.turn_ability(converter_group, line, tech_name, diff))

        if line.is_projectile_shooter():
            patches.extend(AoCUgradeAbilitySubprocessor.shoot_projectile_ability(converter_group, line,
                                                                                 tech_name,
                                                                                 upgrade_source,
                                                                                 upgrade_target,
                                                                                 7, diff))
        elif line.is_melee() or line.is_ranged():
            if line.has_command(7):
                # Attack
                patches.extend(AoCUgradeAbilitySubprocessor.apply_discrete_effect_ability(converter_group,
                                                                                          line, tech_name,
                                                                                          7,
                                                                                          line.is_ranged(),
                                                                                          diff))

        if isinstance(line, GenieUnitLineGroup):
            patches.extend(AoCUgradeAbilitySubprocessor.move_ability(converter_group, line,
                                                                     tech_name, diff))

        if isinstance(line, GenieBuildingLineGroup):
            patches.extend(AoCUgradeAbilitySubprocessor.attribute_change_tracker_ability(converter_group, line,
                                                                                         tech_name, diff))

        return patches

    @staticmethod
    def _tech_cost_modify_effect(converter_group, effect, team=False):
        """
        Creates the patches for modifying tech costs.
        """
        patches = []
        dataset = converter_group.data

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            obj_name = TECH_GROUP_LOOKUPS[obj_id][0]

        else:
            obj_name = CIV_GROUP_LOOKUPS[obj_id][0]

        tech_id = effect["attr_a"].get_value()
        resource_id = effect["attr_b"].get_value()
        mode = effect["attr_c"].get_value()
        amount = int(effect["attr_d"].get_value())

        if not tech_id in TECH_GROUP_LOOKUPS.keys():
            # Skips some legacy techs from AoK such as the tech for bombard cannon
            return patches

        tech_group = dataset.tech_groups[tech_id]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        if resource_id == 0:
            resource_name = "Food"

        elif resource_id == 1:
            resource_name = "Wood"

        elif resource_id == 2:
            resource_name = "Stone"

        elif resource_id == 3:
            resource_name = "Gold"

        else:
            raise Exception("no valid resource ID found")

        if mode == 0:
            operator = MemberOperator.ASSIGN

        else:
            operator = MemberOperator.ADD

        patch_target_ref = "%s.ResearchableTech.%sCost.%sAmount" % (tech_name,
                                                                    tech_name,
                                                                    resource_name)
        patch_target_expected_pointer = ExpectedPointer(tech_group, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sCostWrapper" % (tech_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sCost" % (tech_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("amount",
                                                       amount,
                                                       "engine.aux.resource.ResourceAmount",
                                                       operator)

        if team:
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.type.DiplomaticPatch")
            stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
                       dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()]
            wrapper_raw_api_object.add_raw_member("stances",
                                                  stances,
                                                  "engine.aux.patch.type.DiplomaticPatch")

        patch_expected_pointer = ExpectedPointer(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(converter_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches

    @staticmethod
    def _tech_time_modify_effect(converter_group, effect, team=False):
        """
        Creates the patches for modifying tech research times.
        """
        patches = []
        dataset = converter_group.data

        obj_id = converter_group.get_id()
        if isinstance(converter_group, GenieTechEffectBundleGroup):
            obj_name = TECH_GROUP_LOOKUPS[obj_id][0]

        else:
            obj_name = CIV_GROUP_LOOKUPS[obj_id][0]

        tech_id = effect["attr_a"].get_value()
        mode = effect["attr_c"].get_value()
        research_time = effect["attr_d"].get_value()

        if not tech_id in TECH_GROUP_LOOKUPS.keys():
            # Skips some legacy techs from AoK such as the tech for bombard cannon
            return patches

        tech_group = dataset.tech_groups[tech_id]
        tech_name = TECH_GROUP_LOOKUPS[tech_id][0]

        if mode == 0:
            operator = MemberOperator.ASSIGN

        else:
            operator = MemberOperator.ADD

        patch_target_ref = "%s.ResearchableTech" % (tech_name)
        patch_target_expected_pointer = ExpectedPointer(tech_group, patch_target_ref)

        # Wrapper
        wrapper_name = "Change%sResearchTimeWrapper" % (tech_name)
        wrapper_ref = "%s.%s" % (tech_name, wrapper_name)
        wrapper_location = ExpectedPointer(converter_group, obj_name)
        wrapper_raw_api_object = RawAPIObject(wrapper_ref,
                                              wrapper_name,
                                              dataset.nyan_api_objects,
                                              wrapper_location)
        wrapper_raw_api_object.add_raw_parent("engine.aux.patch.Patch")

        # Nyan patch
        nyan_patch_name = "Change%sResearchTime" % (tech_name)
        nyan_patch_ref = "%s.%s.%s" % (tech_name, wrapper_name, nyan_patch_name)
        nyan_patch_location = ExpectedPointer(converter_group, wrapper_ref)
        nyan_patch_raw_api_object = RawAPIObject(nyan_patch_ref,
                                                 nyan_patch_name,
                                                 dataset.nyan_api_objects,
                                                 nyan_patch_location)
        nyan_patch_raw_api_object.add_raw_parent("engine.aux.patch.NyanPatch")
        nyan_patch_raw_api_object.set_patch_target(patch_target_expected_pointer)

        nyan_patch_raw_api_object.add_raw_patch_member("research_time",
                                                       research_time,
                                                       "engine.aux.research.ResearchableTech",
                                                       operator)

        if team:
            wrapper_raw_api_object.add_raw_parent("engine.aux.patch.type.DiplomaticPatch")
            stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"],
                       dataset.pregen_nyan_objects["aux.diplomatic_stance.types.Friendly"].get_nyan_object()]
            wrapper_raw_api_object.add_raw_member("stances",
                                                  stances,
                                                  "engine.aux.patch.type.DiplomaticPatch")

        patch_expected_pointer = ExpectedPointer(converter_group, nyan_patch_ref)
        wrapper_raw_api_object.add_raw_member("patch",
                                              patch_expected_pointer,
                                              "engine.aux.patch.Patch")

        converter_group.add_raw_api_object(wrapper_raw_api_object)
        converter_group.add_raw_api_object(nyan_patch_raw_api_object)

        wrapper_expected_pointer = ExpectedPointer(converter_group, wrapper_ref)
        patches.append(wrapper_expected_pointer)

        return patches
