# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates patches for technologies.
"""
from openage.convert.dataformat.aoc.genie_unit import GenieBuildingLineGroup,\
    GenieUnitLineGroup
from openage.convert.processor.aoc.upgrade_ability_subprocessor import AoCUpgradeAbilitySubprocessor
from openage.convert.processor.aoc.upgrade_attribute_subprocessor import AoCUpgradeAttributeSubprocessor
from openage.convert.processor.aoc.upgrade_resource_subprocessor import AoCUpgradeResourceSubprocessor
from openage.convert.processor.ror.upgrade_ability_subprocessor import RoRUpgradeAbilitySubprocessor
from openage.convert.processor.ror.upgrade_attribute_subprocessor import RoRUpgradeAttributeSubprocessor
from openage.convert.processor.ror.upgrade_resource_subprocessor import RoRUpgradeResourceSubprocessor
from openage.convert.service import internal_name_lookups
from openage.nyan.nyan_structs import MemberOperator


class RoRTechSubprocessor:

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
        19: RoRUpgradeAttributeSubprocessor.ballistics_upgrade,
        100: AoCUpgradeAttributeSubprocessor.resource_cost_upgrade,
        101: RoRUpgradeAttributeSubprocessor.population_upgrade,
    }

    upgrade_resource_funcs = {
        4: AoCUpgradeResourceSubprocessor.starting_population_space_upgrade,
        27: AoCUpgradeResourceSubprocessor.monk_conversion_upgrade,
        28: RoRUpgradeResourceSubprocessor.building_conversion_upgrade,
        32: AoCUpgradeResourceSubprocessor.bonus_population_upgrade,
        35: AoCUpgradeResourceSubprocessor.faith_recharge_rate_upgrade,
        36: AoCUpgradeResourceSubprocessor.farm_food_upgrade,
        46: AoCUpgradeResourceSubprocessor.tribute_inefficiency_upgrade,
        47: AoCUpgradeResourceSubprocessor.gather_gold_efficiency_upgrade,
        50: AoCUpgradeResourceSubprocessor.reveal_ally_upgrade,
        56: RoRUpgradeResourceSubprocessor.heal_bonus_upgrade,
        57: RoRUpgradeResourceSubprocessor.martyrdom_upgrade,
    }

    @classmethod
    def get_patches(cls, converter_group):
        """
        Returns the patches for a converter group, depending on the type
        of its effects.
        """
        patches = []
        effects = converter_group.get_effects()
        for effect in effects:
            type_id = effect.get_type()

            if type_id in (0, 4, 5):
                patches.extend(cls._attribute_modify_effect(converter_group, effect))

            elif type_id == 1:
                patches.extend(cls._resource_modify_effect(converter_group, effect))

            elif type_id == 2:
                # Enabling/disabling units: Handled in creatable conditions
                pass

            elif type_id == 3:
                patches.extend(cls._upgrade_unit_effect(converter_group, effect))

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

        upgrade_func = RoRTechSubprocessor.upgrade_attribute_funcs[attribute_type]
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

        if resource_id in (-1, 6, 21, 30):
            # -1 = invalid ID
            # 6  = set current age (unused)
            # 21 = tech count (unused)
            # 30 = building limits (unused)
            return patches

        upgrade_func = RoRTechSubprocessor.upgrade_resource_funcs[resource_id]
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

        tech_lookup_dict = internal_name_lookups.get_tech_lookups(dataset.game_version)

        head_unit_id = effect["attr_a"].get_value()
        upgrade_target_id = effect["attr_b"].get_value()

        if head_unit_id not in dataset.unit_ref.keys() or\
                upgrade_target_id not in dataset.unit_ref.keys():
            # Skip annexes or transform units
            return patches

        line = dataset.unit_ref[head_unit_id]
        upgrade_target_pos = line.get_unit_position(upgrade_target_id)
        upgrade_source_pos = upgrade_target_pos - 1

        upgrade_source = line.line[upgrade_source_pos]
        upgrade_target = line.line[upgrade_target_pos]
        tech_name = tech_lookup_dict[tech_id][0]

        diff = upgrade_source.diff(upgrade_target)

        patches.extend(AoCUpgradeAbilitySubprocessor.death_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUpgradeAbilitySubprocessor.despawn_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUpgradeAbilitySubprocessor.idle_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUpgradeAbilitySubprocessor.live_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUpgradeAbilitySubprocessor.los_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUpgradeAbilitySubprocessor.named_ability(converter_group, line, tech_name, diff))
        # patches.extend(AoCUpgradeAbilitySubprocessor.resistance_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUpgradeAbilitySubprocessor.selectable_ability(converter_group, line, tech_name, diff))
        patches.extend(AoCUpgradeAbilitySubprocessor.turn_ability(converter_group, line, tech_name, diff))

        if line.is_projectile_shooter():
            patches.extend(RoRUpgradeAbilitySubprocessor.shoot_projectile_ability(converter_group, line,
                                                                                  tech_name,
                                                                                  7, diff))

        elif line.is_melee() or line.is_ranged():
            if line.has_command(7):
                # Attack
                patches.extend(AoCUpgradeAbilitySubprocessor.apply_discrete_effect_ability(converter_group,
                                                                                           line, tech_name,
                                                                                           7,
                                                                                           line.is_ranged(),
                                                                                           diff))

        if isinstance(line, GenieUnitLineGroup):
            patches.extend(AoCUpgradeAbilitySubprocessor.move_ability(converter_group, line,
                                                                      tech_name, diff))

        if isinstance(line, GenieBuildingLineGroup):
            # TODO: Damage percentages change
            # patches.extend(AoCUpgradeAbilitySubprocessor.attribute_change_tracker_ability(converter_group, line,
            #                                                                               tech_name, diff))
            pass

        return patches
