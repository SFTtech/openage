# Copyright 2020-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-branches

"""
Creates patches for technologies.
"""
from __future__ import annotations
import typing


from .....nyan.nyan_structs import MemberOperator
from ....entity_object.conversion.aoc.genie_tech import CivTeamBonus, CivBonus
from ..aoc.tech_subprocessor import AoCTechSubprocessor
from ..aoc.upgrade_attribute_subprocessor import AoCUpgradeAttributeSubprocessor
from ..aoc.upgrade_resource_subprocessor import AoCUpgradeResourceSubprocessor
from .upgrade_attribute_subprocessor import DE2UpgradeAttributeSubprocessor
from .upgrade_resource_subprocessor import DE2UpgradeResourceSubprocessor

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.converter_object import ConverterObjectGroup
    from openage.convert.entity_object.conversion.aoc.genie_effect import GenieEffectObject
    from openage.convert.value_object.conversion.forward_ref import ForwardRef


class DE2TechSubprocessor:
    """
    Creates raw API objects and patches for techs and civ setups in DE2.
    """

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

        # TODO: These refer to different atributes in DE2
        24: AoCUpgradeAttributeSubprocessor.imperial_tech_id_upgrade,
        26: AoCUpgradeAttributeSubprocessor.attack_warning_sound_upgrade,
        42: AoCUpgradeAttributeSubprocessor.standing_wonders_upgrade,
        43: AoCUpgradeAttributeSubprocessor.train_button_upgrade,
        46: AoCUpgradeAttributeSubprocessor.tribute_inefficiency_upgrade,
        48: AoCUpgradeAttributeSubprocessor.tc_available_upgrade,
        49: AoCUpgradeAttributeSubprocessor.gold_counter_upgrade,
        57: AoCUpgradeAttributeSubprocessor.kidnap_storage_upgrade,

        30: DE2UpgradeAttributeSubprocessor.herdable_capacity_upgrade,
        51: DE2UpgradeAttributeSubprocessor.bfg_unknown_51_upgrade,
        59: DE2UpgradeAttributeSubprocessor.charge_attack_upgrade,
        60: DE2UpgradeAttributeSubprocessor.charge_regen_upgrade,
        61: DE2UpgradeAttributeSubprocessor.charge_event_upgrade,
        62: DE2UpgradeAttributeSubprocessor.charge_type_upgrade,
        63: AoCUpgradeAttributeSubprocessor.ignore_armor_upgrade,
        71: DE2UpgradeAttributeSubprocessor.bfg_unknown_71_upgrade,
        73: DE2UpgradeAttributeSubprocessor.bfg_unknown_73_upgrade,
        100: AoCUpgradeAttributeSubprocessor.resource_cost_upgrade,
        101: AoCUpgradeAttributeSubprocessor.creation_time_upgrade,
        102: AoCUpgradeAttributeSubprocessor.min_projectiles_upgrade,
        103: AoCUpgradeAttributeSubprocessor.cost_food_upgrade,
        104: AoCUpgradeAttributeSubprocessor.cost_wood_upgrade,
        105: AoCUpgradeAttributeSubprocessor.cost_gold_upgrade,
        106: AoCUpgradeAttributeSubprocessor.cost_stone_upgrade,
        107: AoCUpgradeAttributeSubprocessor.max_projectiles_upgrade,
        108: AoCUpgradeAttributeSubprocessor.garrison_heal_upgrade,
        109: DE2UpgradeAttributeSubprocessor.regeneration_rate_upgrade,
        110: DE2UpgradeAttributeSubprocessor.villager_pop_space_upgrade,
        111: DE2UpgradeAttributeSubprocessor.min_convert_upgrade,
        112: DE2UpgradeAttributeSubprocessor.max_convert_upgrade,
        113: DE2UpgradeAttributeSubprocessor.convert_chance_upgrade,
    }

    upgrade_resource_funcs = {
        0: DE2UpgradeResourceSubprocessor.current_food_amount_upgrade,
        1: DE2UpgradeResourceSubprocessor.current_wood_amount_upgrade,
        2: DE2UpgradeResourceSubprocessor.current_stone_amount_upgrade,
        3: DE2UpgradeResourceSubprocessor.current_gold_amount_upgrade,
        4: AoCUpgradeResourceSubprocessor.starting_population_space_upgrade,
        27: AoCUpgradeResourceSubprocessor.monk_conversion_upgrade,
        28: AoCUpgradeResourceSubprocessor.building_conversion_upgrade,
        29: AoCUpgradeResourceSubprocessor.siege_conversion_upgrade,
        32: AoCUpgradeResourceSubprocessor.bonus_population_upgrade,
        35: AoCUpgradeResourceSubprocessor.faith_recharge_rate_upgrade,
        36: AoCUpgradeResourceSubprocessor.farm_food_upgrade,
        46: AoCUpgradeResourceSubprocessor.tribute_inefficiency_upgrade,
        47: AoCUpgradeResourceSubprocessor.gather_gold_efficiency_upgrade,
        50: AoCUpgradeResourceSubprocessor.reveal_ally_upgrade,
        69: DE2UpgradeResourceSubprocessor.bfg_unknown_69_upgrade,
        77: AoCUpgradeResourceSubprocessor.conversion_resistance_upgrade,
        78: AoCUpgradeResourceSubprocessor.trade_penalty_upgrade,
        79: AoCUpgradeResourceSubprocessor.gather_stone_efficiency_upgrade,
        84: AoCUpgradeResourceSubprocessor.starting_villagers_upgrade,
        85: AoCUpgradeResourceSubprocessor.chinese_tech_discount_upgrade,
        86: AoCUpgradeResourceSubprocessor.research_time_upgrade,
        88: DE2UpgradeResourceSubprocessor.fish_trap_food_amount_upgrade,
        89: AoCUpgradeResourceSubprocessor.heal_rate_upgrade,
        90: AoCUpgradeResourceSubprocessor.heal_range_upgrade,
        91: AoCUpgradeResourceSubprocessor.starting_food_upgrade,
        92: AoCUpgradeResourceSubprocessor.starting_wood_upgrade,
        93: AoCUpgradeResourceSubprocessor.starting_stone_upgrade,
        94: AoCUpgradeResourceSubprocessor.starting_gold_upgrade,
        96: AoCUpgradeResourceSubprocessor.berserk_heal_rate_upgrade,
        97: AoCUpgradeResourceSubprocessor.herding_dominance_upgrade,
        176: DE2UpgradeResourceSubprocessor.conversion_min_adjustment_upgrade,
        177: DE2UpgradeResourceSubprocessor.conversion_max_adjustment_upgrade,
        178: AoCUpgradeResourceSubprocessor.conversion_resistance_min_rounds_upgrade,
        179: AoCUpgradeResourceSubprocessor.conversion_resistance_max_rounds_upgrade,
        180: DE2UpgradeResourceSubprocessor.conversion_min_building_upgrade,
        181: DE2UpgradeResourceSubprocessor.conversion_max_building_upgrade,
        182: DE2UpgradeResourceSubprocessor.conversion_building_chance_upgrade,
        183: AoCUpgradeResourceSubprocessor.reveal_enemy_upgrade,
        189: AoCUpgradeResourceSubprocessor.gather_wood_efficiency_upgrade,
        190: AoCUpgradeResourceSubprocessor.gather_food_efficiency_upgrade,
        191: AoCUpgradeResourceSubprocessor.relic_gold_bonus_upgrade,
        192: AoCUpgradeResourceSubprocessor.heresy_upgrade,
        193: AoCUpgradeResourceSubprocessor.theocracy_upgrade,
        194: AoCUpgradeResourceSubprocessor.crenellations_upgrade,
        196: AoCUpgradeResourceSubprocessor.wonder_time_increase_upgrade,
        197: AoCUpgradeResourceSubprocessor.spies_discount_upgrade,
        208: DE2UpgradeResourceSubprocessor.feitoria_gold_upgrade,
        209: DE2UpgradeResourceSubprocessor.reveal_enemy_tcs_upgrade,
        211: DE2UpgradeResourceSubprocessor.elevation_attack_upgrade,
        212: DE2UpgradeResourceSubprocessor.cliff_attack_upgrade,
        214: DE2UpgradeResourceSubprocessor.free_kipchaks_upgrade,
        216: DE2UpgradeResourceSubprocessor.sheep_food_amount_upgrade,
        218: DE2UpgradeResourceSubprocessor.cuman_tc_upgrade,
        219: DE2UpgradeResourceSubprocessor.bfg_unknown_219_upgrade,
        220: DE2UpgradeResourceSubprocessor.relic_food_production_upgrade,
        234: DE2UpgradeResourceSubprocessor.first_crusade_upgrade,
        236: DE2UpgradeResourceSubprocessor.burgundian_vineyards_upgrade,
        237: DE2UpgradeResourceSubprocessor.folwark_collect_upgrade,
        238: DE2UpgradeResourceSubprocessor.folwark_flag_upgrade,
        239: DE2UpgradeResourceSubprocessor.folwark_mill_id_upgrade,
        241: DE2UpgradeResourceSubprocessor.stone_gold_gen_upgrade,
        242: DE2UpgradeResourceSubprocessor.workshop_food_gen_upgrade,
        243: DE2UpgradeResourceSubprocessor.workshop_wood_gen_upgrade,
        244: DE2UpgradeResourceSubprocessor.workshop_stone_gen_upgrade,
        245: DE2UpgradeResourceSubprocessor.workshop_gold_gen_upgrade,
        251: DE2UpgradeResourceSubprocessor.trade_food_bonus_upgrade,
        254: DE2UpgradeResourceSubprocessor.herdable_garrison_upgrade,
        262: DE2UpgradeResourceSubprocessor.bengali_conversion_resistance_upgrade,
        266: DE2UpgradeResourceSubprocessor.doi_paper_money_upgrade,
        267: DE2UpgradeResourceSubprocessor.forager_wood_gather_upgrade,
        268: DE2UpgradeResourceSubprocessor.resource_decay_upgrade,
        269: DE2UpgradeResourceSubprocessor.tech_reward_upgrade,
        272: DE2UpgradeResourceSubprocessor.cliff_defense_upgrade,
        273: DE2UpgradeResourceSubprocessor.elevation_defense_upgrade,
        274: DE2UpgradeResourceSubprocessor.chieftains_upgrade,
        280: DE2UpgradeResourceSubprocessor.conversion_range_upgrade,
        282: DE2UpgradeResourceSubprocessor.unknown_recharge_rate_upgrade,
        502: DE2UpgradeResourceSubprocessor.bfg_unknown_502_upgrade,
        507: DE2UpgradeResourceSubprocessor.bfg_unknown_507_upgrade,
        521: DE2UpgradeResourceSubprocessor.bfg_unknown_521_upgrade,
        551: DE2UpgradeResourceSubprocessor.bfg_unknown_551_upgrade,
    }

    @classmethod
    def get_patches(cls, converter_group: ConverterObjectGroup) -> list[ForwardRef]:
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
            converter_group = dataset.civ_groups[converter_group.get_civilization_id()]
            team_bonus = True

        elif isinstance(converter_group, CivBonus):
            effects = converter_group.get_effects()

            # Change converter group here, so that the Civ object gets the patches
            converter_group = dataset.civ_groups[converter_group.get_civilization_id()]

        else:
            effects = converter_group.get_effects()

        team_effect = False
        for effect in effects:
            type_id = effect.get_type()

            if team_bonus or type_id in (10, 11, 12, 13, 14, 15, 16):
                team_effect = True
                type_id -= 10

            if type_id in (0, 4, 5):
                patches.extend(cls.attribute_modify_effect(converter_group,
                                                           effect,
                                                           team=team_effect))

            elif type_id in (1, 6):
                patches.extend(cls.resource_modify_effect(converter_group,
                                                          effect,
                                                          team=team_effect))

            elif type_id == 2:
                # Enabling/disabling units: Handled in creatable conditions
                pass

            elif type_id == 3:
                patches.extend(AoCTechSubprocessor.upgrade_unit_effect(converter_group,
                                                                       effect))

            elif type_id == 101:
                patches.extend(AoCTechSubprocessor.tech_cost_modify_effect(converter_group,
                                                                           effect,
                                                                           team=team_effect))

            elif type_id == 102:
                # Tech disable: Only used for civ tech tree
                pass

            elif type_id == 103:
                patches.extend(AoCTechSubprocessor.tech_time_modify_effect(converter_group,
                                                                           effect,
                                                                           team=team_effect))

            team_effect = False

        return patches

    @staticmethod
    def attribute_modify_effect(
        converter_group: ConverterObjectGroup,
        effect: GenieEffectObject,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates the patches for modifying attributes of entities.
        """
        patches = []
        dataset = converter_group.data

        effect_type = effect.get_type()
        operator = None
        if effect_type in (0, 10):
            operator = MemberOperator.ASSIGN

        elif effect_type in (4, 14):
            operator = MemberOperator.ADD

        elif effect_type in (5, 15):
            operator = MemberOperator.MULTIPLY

        else:
            raise TypeError(f"Effect type {effect_type} is not a valid attribute effect")

        unit_id = effect["attr_a"].value
        class_id = effect["attr_b"].value
        attribute_type = effect["attr_c"].value
        value = effect["attr_d"].value

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

        upgrade_func = DE2TechSubprocessor.upgrade_attribute_funcs[attribute_type]
        for affected_entity in affected_entities:
            patches.extend(upgrade_func(converter_group, affected_entity, value, operator, team))

        return patches

    @staticmethod
    def resource_modify_effect(
        converter_group: ConverterObjectGroup,
        effect: GenieEffectObject,
        team: bool = False
    ) -> list[ForwardRef]:
        """
        Creates the patches for modifying resources.
        """
        patches = []

        effect_type = effect.get_type()
        operator = None
        if effect_type in (1, 11):
            mode = effect["attr_b"].value

            if mode == 0:
                operator = MemberOperator.ASSIGN

            else:
                operator = MemberOperator.ADD

        elif effect_type in (6, 16):
            operator = MemberOperator.MULTIPLY

        else:
            raise TypeError(f"Effect type {effect_type} is not a valid attribute effect")

        resource_id = effect["attr_a"].value
        value = effect["attr_d"].value

        if resource_id in (-1, 6, 21):
            # -1 = invalid ID
            # 6  = set current age (unused)
            # 21 = tech count (unused)
            return patches

        upgrade_func = DE2TechSubprocessor.upgrade_resource_funcs[resource_id]
        patches.extend(upgrade_func(converter_group, value, operator, team))

        return patches
