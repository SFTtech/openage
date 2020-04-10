# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates patches for technologies.
"""
from openage.convert.processor.aoc.upgrade_ability_subprocessor import AoCUgradeAbilitySubprocessor
from openage.convert.dataformat.aoc.genie_unit import GenieUnitLineGroup
from openage.nyan.nyan_structs import MemberOperator
from openage.convert.processor.aoc.upgrade_attribute_subprocessor import AoCUpgradeAttributeSubprocessor


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
        13: AoCUpgradeAttributeSubprocessor.min_range_upgrade,
        14: AoCUpgradeAttributeSubprocessor.carry_capacity_upgrade,
        15: None,
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

    @classmethod
    def get_patches(cls, tech_group):
        """
        Returns the patches for a tech group, depending on the type
        of its effects.
        """
        patches = []
        for effect in tech_group.effects.get_effects():
            type_id = effect.get_type()

            if type_id in (0, 4, 5):
                patches.extend(cls._attribute_modify_effect(tech_group, effect))

            if type_id == 3:
                patches.extend(cls._upgrade_unit_effect(tech_group, effect))

        return patches

    @staticmethod
    def _attribute_modify_effect(tech_group, effect):
        """
        Creates the patches for setting attributes of entities. .
        """
        patches = []
        dataset = tech_group.data

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

            upgrade_func(tech_group, affected_entity, value, operator)

        return patches

    @staticmethod
    def _upgrade_unit_effect(tech_group, effect):
        """
        Creates the patches for upgrading entities in a line.
        """
        patches = []
        dataset = tech_group.data

        upgrade_source_id = effect["attr_a"].get_value()
        upgrade_target_id = effect["attr_b"].get_value()

        if upgrade_source_id not in dataset.unit_ref.keys() or\
                upgrade_target_id not in dataset.unit_ref.keys():
            # Skip annexes or transform units
            return patches

        line = dataset.unit_ref[upgrade_source_id]
        upgrade_source_pos = line.get_unit_position(upgrade_source_id)
        upgrade_target_pos = line.get_unit_position(upgrade_target_id)

        if upgrade_target_pos - upgrade_source_pos != 1:
            # Skip effects that upgrades entities not next to each other in
            # the line. This is not used in the games anyway and we would handle
            # it differently.
            return patches

        upgrade_source = line.line[upgrade_source_pos]
        upgrade_target = line.line[upgrade_target_pos]

        diff = upgrade_source.diff(upgrade_target)

        patches.extend(AoCUgradeAbilitySubprocessor.death_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.despawn_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.idle_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.live_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.los_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.resistance_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.selectable_ability(tech_group, line, diff))
        patches.extend(AoCUgradeAbilitySubprocessor.turn_ability(tech_group, line, diff))

        if line.is_projectile_shooter():
            patches.extend(AoCUgradeAbilitySubprocessor.shoot_projectile_ability(tech_group, line,
                                                                                 upgrade_source,
                                                                                 upgrade_target,
                                                                                 7, diff))
        elif line.is_melee() or line.is_ranged():
            if line.has_command(7):
                # Attack
                patches.extend(AoCUgradeAbilitySubprocessor.apply_discrete_effect_ability(tech_group,
                                                                                          line,
                                                                                          7,
                                                                                          line.is_ranged(),
                                                                                          diff))

            # TODO: Other commands

        if isinstance(line, GenieUnitLineGroup):
            patches.extend(AoCUgradeAbilitySubprocessor.move_ability(tech_group, line, diff))

        return patches
