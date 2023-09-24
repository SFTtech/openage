# Copyright 2020-2023 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-branches
#
# TODO:
# pylint: disable=line-too-long

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
from .upgrade_attribute_subprocessor import SWGBCCUpgradeAttributeSubprocessor
from .upgrade_resource_subprocessor import SWGBCCUpgradeResourceSubprocessor

if typing.TYPE_CHECKING:
    from engine.convert.entity_object.conversion.converter_object import ConverterObjectGroup
    from engine.convert.entity_object.conversion.aoc.genie_effect import GenieEffectObject
    from engine.convert.value_object.conversion.forward_ref import ForwardRef


class SWGBCCTechSubprocessor:
    """
    Creates raw API objects and patches for techs and civ setups in SWGB.
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
        100: SWGBCCUpgradeAttributeSubprocessor.resource_cost_upgrade,
        101: AoCUpgradeAttributeSubprocessor.creation_time_upgrade,
        102: AoCUpgradeAttributeSubprocessor.min_projectiles_upgrade,
        103: AoCUpgradeAttributeSubprocessor.cost_food_upgrade,
        104: SWGBCCUpgradeAttributeSubprocessor.cost_carbon_upgrade,
        105: SWGBCCUpgradeAttributeSubprocessor.cost_nova_upgrade,
        106: SWGBCCUpgradeAttributeSubprocessor.cost_ore_upgrade,
        107: AoCUpgradeAttributeSubprocessor.max_projectiles_upgrade,
        108: AoCUpgradeAttributeSubprocessor.garrison_heal_upgrade,
    }

    upgrade_resource_funcs = {
        4: AoCUpgradeResourceSubprocessor.starting_population_space_upgrade,
        5: SWGBCCUpgradeResourceSubprocessor.conversion_range_upgrade,
        10: SWGBCCUpgradeResourceSubprocessor.shield_recharge_rate_upgrade,
        23: SWGBCCUpgradeResourceSubprocessor.submarine_detect_upgrade,
        26: SWGBCCUpgradeResourceSubprocessor.shield_dropoff_time_upgrade,
        27: SWGBCCUpgradeResourceSubprocessor.monk_conversion_upgrade,
        28: SWGBCCUpgradeResourceSubprocessor.building_conversion_upgrade,
        31: SWGBCCUpgradeResourceSubprocessor.assault_mech_anti_air_upgrade,
        32: AoCUpgradeResourceSubprocessor.bonus_population_upgrade,
        33: SWGBCCUpgradeResourceSubprocessor.shield_power_core_upgrade,
        35: SWGBCCUpgradeResourceSubprocessor.faith_recharge_rate_upgrade,
        36: AoCUpgradeResourceSubprocessor.farm_food_upgrade,
        38: SWGBCCUpgradeResourceSubprocessor.shield_air_units_upgrade,
        46: AoCUpgradeResourceSubprocessor.tribute_inefficiency_upgrade,
        47: AoCUpgradeResourceSubprocessor.gather_gold_efficiency_upgrade,
        50: AoCUpgradeResourceSubprocessor.reveal_ally_upgrade,
        56: SWGBCCUpgradeResourceSubprocessor.cloak_upgrade,
        58: SWGBCCUpgradeResourceSubprocessor.detect_cloak_upgrade,
        77: AoCUpgradeResourceSubprocessor.conversion_resistance_upgrade,
        78: AoCUpgradeResourceSubprocessor.trade_penalty_upgrade,
        79: AoCUpgradeResourceSubprocessor.gather_stone_efficiency_upgrade,
        84: AoCUpgradeResourceSubprocessor.starting_villagers_upgrade,
        85: AoCUpgradeResourceSubprocessor.chinese_tech_discount_upgrade,
        87: SWGBCCUpgradeResourceSubprocessor.concentration_upgrade,
        89: AoCUpgradeResourceSubprocessor.heal_rate_upgrade,
        90: SWGBCCUpgradeResourceSubprocessor.heal_range_upgrade,
        91: AoCUpgradeResourceSubprocessor.starting_food_upgrade,
        92: AoCUpgradeResourceSubprocessor.starting_wood_upgrade,
        96: SWGBCCUpgradeResourceSubprocessor.berserk_heal_rate_upgrade,
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
                # Same effect as (type_id - 1) but gets applied to all allies
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

            elif type_id == 7:
                # Spawn units: This should be a script
                pass

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
        if effect_type == 0:
            operator = MemberOperator.ASSIGN

        elif effect_type == 4:
            operator = MemberOperator.ADD

        elif effect_type == 5:
            operator = MemberOperator.MULTIPLY

        else:
            raise TypeError(f"Effect type {effect_type} is not a valid resource effect")

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

        upgrade_func = SWGBCCTechSubprocessor.upgrade_attribute_funcs[attribute_type]
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
        if effect_type == 1:
            mode = effect["attr_b"].value

            if mode == 0:
                operator = MemberOperator.ASSIGN

            else:
                operator = MemberOperator.ADD

        elif effect_type == 6:
            operator = MemberOperator.MULTIPLY

        else:
            raise TypeError(f"Effect type {effect_type} is not a valid resource effect")

        resource_id = effect["attr_a"].value
        value = effect["attr_d"].value

        if resource_id in (-1, 6, 21):
            # -1 = invalid ID
            # 6  = set current age (unused)
            # 21 = tech count (unused)
            return patches

        upgrade_func = SWGBCCTechSubprocessor.upgrade_resource_funcs[resource_id]
        patches.extend(upgrade_func(converter_group, value, operator, team))

        return patches
